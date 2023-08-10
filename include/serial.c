#include "headers/serial.h"
#define READ_TIMEOUT 500 // milliseconds
HANDLE hComm;

int openAndSettingSerial(void);
int waitAnswerRequestRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader);
int requestForRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader);
int requestForWrite(uint8_t *buf, size_t messSize, OVERLAPPED *osWrite);
void toggleSerialState(int *serialState);

void serialRXHandler(void *mess, size_t messSize);
void serialTXHandler(void *mess, size_t messSize);

serialParam_t *param;

int openAndSettingSerial(void)
{
    hComm = CreateFile(param->portName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       0,
                       OPEN_EXISTING,
                       FILE_FLAG_OVERLAPPED,
                       0);
    if (hComm == INVALID_HANDLE_VALUE)
    {
        serialErrorsHandler((void *)__func__);
        return 1;
    }
    if (param->rxHandler == NULL)
    {
        param->rxHandler = serialRXHandler;
    }

    return 0;
}

int requestForRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader)
{
    DWORD dwRead;
    // Issue read operation.
    if (!ReadFile(hComm, buf, bufSize, &dwRead, osReader))
    {
        if (GetLastError() != ERROR_IO_PENDING) // read not delayed?
        {
            // Error in communications; report it.
            serialErrorsHandler((void *)__func__);
        }
        else
        {
            return SERIAL_WAIT_ANSWER_READ;
        }
    }
    else
    {
        if (dwRead)
        {
            param->rxHandler(buf, dwRead);
        }
        memset(buf, 0, bufSize);
    }
    return SERIAL_DEFAULT;
}

int waitAnswerRequestRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader)
{
    DWORD dwRes, dwRead;
    int fWaitingOnRead = SERIAL_WAIT_ANSWER_READ;
    dwRes = WaitForSingleObject(osReader->hEvent, READ_TIMEOUT);
    switch (dwRes)
    {
    // Read completed.
    case WAIT_OBJECT_0:
        if (!GetOverlappedResult(hComm, osReader, &dwRead, FALSE))
        { // Error in communications; report it.
            serialErrorsHandler((void *)__func__);
        }
        else
        { // Read completed successfully.
            if (dwRead)
            {
                param->rxHandler(buf, dwRead);
            }
            memset(buf, 0, bufSize);
        } //  Reset flag so that another opertion can be issued.
        fWaitingOnRead = SERIAL_DEFAULT;
        break;

    case WAIT_TIMEOUT:
        // Operation isn't complete yet. fWaitingOnRead flag isn't
        // changed since I'll loop back around, and I don't want
        // to issue another read until the first one finishes.
        //
        // This is a good time to do some background work.
        break;

    default:
        serialErrorsHandler((void *)__func__);
        break;
    }
    return fWaitingOnRead;
}

int requestForWrite(uint8_t *buf, size_t messSize, OVERLAPPED *osWrite)
{
    DWORD dwWritten;
    DWORD dwRes;
    BOOL fRes;
    // Issue write.
    if (!WriteFile(hComm, buf, messSize, &dwWritten, osWrite))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            // WriteFile failed, but isn't delayed. Report error and abort.
            serialErrorsHandler((void *)__func__);
            fRes = SERIAL_DEFAULT;
        }
        else
        {
            // Write is pending.
            dwRes = WaitForSingleObject(osWrite->hEvent, INFINITE);
        }
        switch (dwRes)
        {
        // OVERLAPPED structure's event has been signaled.
        case WAIT_OBJECT_0:
        {
            if (!GetOverlappedResult(hComm, osWrite, &dwWritten, FALSE))
            {
                fRes = SERIAL_WRITE_REQUEST;
            }
            else
            { // Write operation completed successfully.
                fRes = SERIAL_DEFAULT;
            }
            break;
        }
        default:
        { // An error has occurred in WaitForSingleObject.
            // This usually indicates a problem with the
            // OVERLAPPED structure's event handle.
            serialErrorsHandler((void *)__func__);
            fRes = SERIAL_DEFAULT;
            break;
        }
        }
    }
    else
    { // WriteFile completed immediately.
        fRes = SERIAL_DEFAULT;
    }
    return fRes;
}

void toggleSerialState(int *serialState)
{
    static int8_t count = 5;

    if (count > 0)
    {
        *serialState = SERIAL_WRITE_REQUEST;
    }
    else
    {
        *serialState = SERIAL_READ_REQUEST;
    }
    count--;
    if (count <= -5)
    {
        count = 5;
    }
}

void *SerialThread(void *args)
{
    param = (serialParam_t *)args;
    if (openAndSettingSerial())
    {
        serialErrorsHandler((void *)__func__);
        return NULL;
    }
    // int serialState = SERIAL_DEFAULT;
    int serialState = SERIAL_READ_REQUEST;
    OVERLAPPED osReader = {0};
    OVERLAPPED osWrite = {0};
    // Create the overlapped event. Must be closed before exiting
    // to avoid a handle leak.
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if ((osReader.hEvent == NULL) || (osWrite.hEvent == NULL))
    // Error creating overlapped event; abort.
    {
        serialErrorsHandler((void *)__func__);
    }

    for (;;)
    {
        switch (serialState)
        {
        case SERIAL_READ_REQUEST:
        {
            serialState = requestForRead(param->bufRX, param->bufRXSize, &osReader);
            break;
        }
        case SERIAL_WAIT_ANSWER_READ:
        {
            serialState = waitAnswerRequestRead(param->bufRX, param->bufRXSize, &osReader);
            break;
        }
        case SERIAL_WRITE_REQUEST:
        {
            serialState = requestForWrite(param->bufRX, param->bufRXSize, &osWrite);
            break;
        }
        case SERIAL_FREE:
        {
            break;
        }
        default:
        {
            pthread_testcancel();
            toggleSerialState(&serialState);
            break;
        }
        }
    }
    CloseHandle(osWrite.hEvent);
    CloseHandle(osReader.hEvent);
    CloseHandle(hComm);
    pthread_exit(0);
    return NULL;
}

__attribute__((weak)) void serialErrorsHandler(void *err)
{
    fprintf(stderr, "Err func::%s\n", (char *)err);
}

__attribute__((weak)) void serialRXHandler(void *mess, size_t messSize)
{
    fprintf(stderr, "mess size::%llu\nmess::%s\n", messSize, (char *)mess);
}
