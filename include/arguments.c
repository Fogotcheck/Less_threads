#include "headers/arguments.h"

/*intern prot---------------------------------*/
void fileExePassHandler(char *exePassAndName);
void printKeyInfo(void);
/*end intern prot-----------------------------*/

/*Callback------------------------------------*/

__attribute__((weak)) void filePassErrCallback(char *exePassAndName)
{
    printf("Error::%s\n", exePassAndName);
}

__attribute__((weak)) void setNewEpsilonCallback(double newValEpsilon)
{
    printf("set::%f\n", newValEpsilon);
}
/*end Callback---------------------------------*/

locateExe_t exePass;

void fileExePassHandler(char *exePassAndName)
{
    exePass.fileExePass = exePassAndName;
    exePass.sizeExePass = strlen(exePassAndName);
    while (exePassAndName[exePass.sizeExePass] != '\\')
    {
        // only fo Unix
        if (exePassAndName[exePass.sizeExePass] == '/')
        {
            break;
        }
        if (exePass.sizeExePass == 0)
        {
            filePassErrCallback(exePassAndName);
            break;
        }
        exePass.sizeExePass--;
    }
    exePass.fileExePass[exePass.sizeExePass + 1] = 0;
}

void printKeyInfo(void)
{
    printf("'-h' - help\n'-b' - set Baud Rate \
    \n'-s' - set Byte Size\n'-p' - set Parity \
    \n'-e' - set End Bits\n'-N' - set Port Name\n");
}

void argumentsHandler(int argc, char *argv[])
{

    fileExePassHandler(argv[0]);
    int rez = 0;
    while ((rez = getopt(argc, argv, "hf:b:s:p:e:N:")) != -1)
    {
        switch (rez)
        {
        case 'h':
        {
            printf("https://github.com/Fogotcheck/Less_threads.git - check read.me\n");
            printKeyInfo();
            break;
        }
        case 'f':
        {
            printf("key f:: %s\n", optarg);
            break;
        }
        case 'b':
        {
            setBaudRate(optarg);
            break;
        }
        case 's':
        {
            setByteSize(optarg);
            break;
        }
        case 'p':
        {
            setParity(optarg);
            break;
        }
        case 'N':
        {
            setPortName(optarg);
            break;
        }
        case 'e':
        {
            setEndBits(optarg);
            break;
        }
        case '?':
        {
            break;
        }
        }
    }
}

__attribute__((weak)) void setBaudRate(char *optarg)
{
    printf("baud Rate::%s\n", optarg);
}

__attribute__((weak)) void setByteSize(char *optarg)
{
    printf("byte size::%s\n", optarg);
}

__attribute__((weak)) void setParity(char *optarg)
{
    printf("byte size::%s\n", optarg);
}

__attribute__((weak)) void setEndBits(char *optarg)
{
    printf("byte size::%s\n", optarg);
}

__attribute__((weak)) void setPortName(char *optarg)
{
    printf("byte size::%s\n", optarg);
}