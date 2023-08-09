#ifndef __SERIAL_H__
#define __SERIAL_H__
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <stdint.h>
typedef struct serialParam
{
    char *portName;
    uint8_t *buf;
    size_t bufSize;
}serialParam_t;

enum STATE_SERIAL_THREAD
{
    SERIAL_READ_REQUEST,
    SERIAL_WAIT_ANSWER_READ,
    SERIAL_WRITE_REQUEST,
    SERIAL_WAIT_ANSWER_WRITE,
    SERIAL_FREE,
};

int openAndSettingSerial(serialParam_t *param);
void SerialThread(void *args);

#endif //__SERIAL_H__