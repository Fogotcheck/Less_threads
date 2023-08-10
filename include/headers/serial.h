// https://learn.microsoft.com/en-us/previous-versions/ff802693(v=msdn.10)?redirectedfrom=MSDN
#ifndef __SERIAL_H__
#define __SERIAL_H__
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>


typedef void (*funcHandler)(void *mess, size_t messSize);

typedef struct serialParam
{
    char *portName;
    uint8_t *bufRX;
    size_t bufRXSize;
    uint8_t *bufTX;
    size_t bufTXSize;
    funcHandler rxHandler;
} serialParam_t;

enum STATE_SERIAL_THREAD
{
    SERIAL_READ_REQUEST,
    SERIAL_WAIT_ANSWER_READ,
    SERIAL_WRITE_REQUEST,
    SERIAL_FREE,
    SERIAL_DEFAULT,
};

void *SerialThread(void *args);

void serialErrorsHandler(void *err);
#endif //__SERIAL_H__