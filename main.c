#include "include/headers/main.h"
uint8_t buf[1024];
char comPortName[] = "\\\\.\\COM3";
int main(void)
{
    serialParam_t comPortParam;
    comPortParam.buf = buf;
    comPortParam.bufSize = sizeof(buf);
    comPortParam.portName = comPortName;
    openAndSettingSerial(&comPortParam);
    SerialThread(&comPortParam);
    printf("Hello world\n");
    return 0;
}
