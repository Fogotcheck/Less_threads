#include "headers/serial.h"
#define READ_TIMEOUT 10 // milliseconds
HANDLE hComm;

int waitAnswerRequestRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader);
int requestForRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader);

int openAndSettingSerial(serialParam_t *param)
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
        return 1;
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
        }
        else
        {
            return SERIAL_WAIT_ANSWER_READ;
        }
    }
    else
    {
        printf("immediately buf::%s\ndwRead::%ld\n", buf, dwRead);
        memset(buf, 0, bufSize);
    }
    return SERIAL_READ_REQUEST;
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
        }
        else
        { // Read completed successfully.
            printf("successfully buf::%s\ndwRead::%ld\n", buf, dwRead);
            memset(buf, 0, bufSize);
        } //  Reset flag so that another opertion can be issued.
        fWaitingOnRead = SERIAL_READ_REQUEST;
        break;

    case WAIT_TIMEOUT:
        // Operation isn't complete yet. fWaitingOnRead flag isn't
        // changed since I'll loop back around, and I don't want
        // to issue another read until the first one finishes.
        //
        // This is a good time to do some background work.
        break;

    default:
        // Error in the WaitForSingleObject; abort.
        // This indicates a problem with the OVERLAPPED structure's
        // event handle.
        break;
    }
    return fWaitingOnRead;
}

void SerialThread(void *args)
{
    serialParam_t *param = (serialParam_t *)args;

    int fWaitingOnRead = SERIAL_READ_REQUEST;
    OVERLAPPED osReader = {0};

    // Create the overlapped event. Must be closed before exiting
    // to avoid a handle leak.
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osReader.hEvent == NULL)
    // Error creating overlapped event; abort.
    {
        printf("Error creating overlapped event; abort\n");
    }
    for (;;)
    {
        switch (fWaitingOnRead)
        {
        case SERIAL_READ_REQUEST:
        {
            fWaitingOnRead = requestForRead(param->buf, param->bufSize, &osReader);
            break;
        }
        case SERIAL_WAIT_ANSWER_READ:
        {
            fWaitingOnRead = waitAnswerRequestRead(param->buf, param->bufSize, &osReader);
            break;
        }
        default:
            break;
        }
    }
    CloseHandle(osReader.hEvent);
    CloseHandle(hComm);
}
