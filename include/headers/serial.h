// https://learn.microsoft.com/en-us/previous-versions/ff802693(v=msdn.10)?redirectedfrom=MSDN
#ifndef __SERIAL_H__
#define __SERIAL_H__
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#define RX_TX_BUF_SIZE 1024

typedef void (*funcHandler)(void **mess, size_t messSize);
typedef int (*funcCloseSerial)(void);
typedef struct serialParam
{
    char *portName;
    funcHandler txHandler;
    funcHandler rxHandler;
    funcCloseSerial seraialClose;
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