#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
//TODO:remake for win+Unix
//#include <windows.h>


#define PASS_README "I:\\Evgeny\\Evgeny_vsc\\Work\\LessGB\\LessGB_control\\include\\headers\\readrme.md"
#define BUF_SIZE 1024

typedef struct locateExe
{
    char *fileExePass;
    size_t sizeExePass;
} locateExe_t;

extern locateExe_t exePass;

void argumentsHandler(int argc, char *argv[]);

/*CALLBACK*/

void setBaudRate(char *optarg);
void setByteSize(char *optarg);
void setParity(char *optarg);
void setEndBits(char *optarg);
void setPortName(char *optarg);
void filePassErrCallback(char *exePassAndName);
void setNewEpsilonCallback(double newValEpsilon);
#endif //__ARGUMENTS_H__