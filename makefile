TARGET = Threads

BUILD_DIR = build
TMP_DIR = tmp
INCLUDES_DIR = include
HEADERS_DIR = headers

WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused

THREAD = -lpthread

CFLAGS += -std=c11 $(WARNINGS) $(THREAD)
ASFLAGS = -Wall -g
LDFLAGS = -Wl,-rpath=$(BUILD_DIR) $(INCLUDES)

SRC = $(wildcard *.c) \
$(wildcard $(INCLUDES_DIR)/*.c)

ASM_SRC = $(wildcard *.S)

INCLUDES = \
-I $(INCLUDES_DIR)/$(HEADERS_DIR)

OBJ = $(SRC:.c=.o)
ASM_OBJ = $(ASM_SRC:.S=.o) 

OBJDUMP = objdump
OBJCOPY = objcopy
GCC_NM = gcc-nm

#ifeq ($(OS),Windows_NT)
#	DEL_COM = del
#	DIR_SEPR = \\
#	MOVE_COM = move
#	CREATE_BUILD_DIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
#	CREATE_TMP_DIR = if not exist $(TMP_DIR) mkdir $(TMP_DIR)
#else
DEL_COM = rm 
DIR_SEPR = /
MOVE_COM = mv
CREATE_BUILD_DIR = test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)
CREATE_TMP_DIR = test -d $(TMP_DIR) || mkdir $(TMP_DIR)
#endif

all: directories executable utilities move

directories:
	$(CREATE_BUILD_DIR)
	$(CREATE_TMP_DIR)

$(BUILD_DIR)/$(TARGET): $(OBJ) $(ASM_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

executable: $(BUILD_DIR)/$(TARGET)

#TODO: need remake
asm_executable: %.o

%.o: %.S
	$(AS) $(ASFLAGS) -c -o $@ $<

#only mingw, need test for Unix
utilities: $(TARGET).sym $(TARGET).lss

#TODO: 
#objdump -h -S build/LessGB_control.exe > LessGB_control.lss
#objcopy -j .text -j .data -O ihex build/LessGB_control.exe LessGB_control.hex
#objcopy: LessGB_control.hex 64-bit address 0x140001000 out of range for Intel Hex file
#objcopy: LessGB_control.hex: bad value
$(TARGET).hex: $(BUILD_DIR)/$(TARGET).exe
	$(OBJCOPY) -j .text -j .data -O ihex $< $@


$(TARGET).sym: $(BUILD_DIR)/$(TARGET).exe
	$(GCC_NM) -n $< > $@

$(TARGET).lss: $(BUILD_DIR)/$(TARGET).exe
	$(OBJDUMP) -h -S $< > $@

clean: 
	$(DEL_COM) $(BUILD_DIR)$(DIR_SEPR)*.*
	$(DEL_COM) $(TMP_DIR)$(DIR_SEPR)*.*

move:
	$(MOVE_COM) *.o $(TMP_DIR)
	$(MOVE_COM) $(INCLUDES_DIR)$(DIR_SEPR)*.o $(TMP_DIR)
	$(MOVE_COM) *.sym $(BUILD_DIR)
	$(MOVE_COM) *.lss $(BUILD_DIR)

.PHONY : all directories executable asm_executable clean move