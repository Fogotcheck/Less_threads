#include "headers/serial.h"
#define READ_TIMEOUT 500 // milliseconds
HANDLE hComm;

int openAndSettingSerial(void);
int waitAnswerRequestRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader);
int requestForRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader);
int requestForWriteAndWait(uint8_t *buf, size_t messSize, OVERLAPPED *osWrite);
void toggleSerialState(int *serialState);

void PrintCommState(DCB dcb);
void serialRXHandler(void **mess, size_t *messSize);
void serialTXHandler(void **mess, size_t *messSize);
int serialClose(void);

serialParam_t *param;

uint8_t *rxBuf;
uint8_t *txBuf;
size_t messSizeTX = 0;
/*!
Функция открывает serial port, и выделяет память для буферов.
\return NULL при успешном открытии порта
*/
int openAndSettingSerial(void)
{

    // TODO: добавить настройку скорости порта.
    hComm = CreateFile(param->portSettings.portName,
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
    if (param->txHandler == NULL)
    {
        param->txHandler = serialTXHandler;
    }
    if (param->seraialClose == NULL)
    {
        param->seraialClose = serialClose;
    }

    rxBuf = (uint8_t *)malloc(RX_TX_BUF_SIZE);
    txBuf = (uint8_t *)malloc(RX_TX_BUF_SIZE);

    DWORD tmpMask = 0;
    DCB tmpDCB = {0};
    GetCommMask(hComm, &tmpMask);
    GetCommState(hComm, &tmpDCB);
    SetCommMask(hComm, EV_DSR | EV_CTS);

    if (param->portSettings.dcb.BaudRate == 0)
    {
        param->portSettings.dcb.BaudRate = CBR_115200;   //  baud rate
        param->portSettings.dcb.ByteSize = 8;            //  data size, xmit and rcv
        param->portSettings.dcb.Parity = NOPARITY;       //  parity bit
        param->portSettings.dcb.StopBits = ONE5STOPBITS; //  stop bit
    }
    SetCommState(hComm, &param->portSettings.dcb);

    return 0;
}
/*!
Функция отправляет запрос на чтение данных.
\param[out] buf буфер для получения считанных данных;
\param[in] bufSize размер буфера для полученных данных;
\param[in] osReader структура с параметрами настроенного события win OVERLAPPED;
\return статус о необходимости ожидания данных;
*/
int requestForRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader)
{
    DWORD dwRead;
    size_t messSize = 0;
    if (!ReadFile(hComm, buf, bufSize, &dwRead, osReader))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
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
            messSize = dwRead;
            param->rxHandler((void **)&buf, &messSize);
        }
        memset(buf, 0, bufSize);
    }
    return SERIAL_DEFAULT;
}

/*!
Функция ожидает ответ на запрос чтение данных.
\param[out] buf буфер для получения считанных данных;
\param[in] bufSize размер буфера для полученных данных;
\param[in] osReader структура с параметрами настроенного события win OVERLAPPED;
\return статус о необходимости ожидания данных;
*/
int waitAnswerRequestRead(uint8_t *buf, size_t bufSize, OVERLAPPED *osReader)
{
    DWORD dwRes, dwRead;
    size_t messSize = 0;
    int fWaitingOnRead = SERIAL_WAIT_ANSWER_READ;
    dwRes = WaitForSingleObject(osReader->hEvent, READ_TIMEOUT);
    switch (dwRes)
    {
    case WAIT_OBJECT_0:
        if (!GetOverlappedResult(hComm, osReader, &dwRead, FALSE))
        {
            serialErrorsHandler((void *)__func__);
        }
        else
        {
            if (dwRead)
            {
                messSize = dwRead + 1;
                param->rxHandler((void **)&buf, &messSize);
            }
            memset(buf, 0, bufSize);
        }
        fWaitingOnRead = SERIAL_DEFAULT;
        break;

    case WAIT_TIMEOUT:
        /*!
        TODO: необходимо дописать функцию для оценки времени на отправку.
        при тестировании все данные отправлялись мгновенно.
        */
        break;

    default:
        serialErrorsHandler((void *)__func__);
        break;
    }

    return fWaitingOnRead;
}
/*!
Функция отправляет запрос на отправку данных в serial port.
\param[in] buf буфер данных для отправки;
\param[in] messSize размер данных для отправки;
\param[in] osReader структура с параметрами настроенного события win OVERLAPPED;
\return статус о необходимости ожидания данных.
Если данные мгновенно не отправлены,
вернёт статус о повторном периоде ожидания данных;
*/
int requestForWriteAndWait(uint8_t *buf, size_t messSize, OVERLAPPED *osWrite)
{
    DWORD dwWritten;
    DWORD dwRes;
    BOOL fRes;

    if (!WriteFile(hComm, buf, messSize, &dwWritten, osWrite))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            serialErrorsHandler((void *)__func__);
            fRes = SERIAL_DEFAULT;
        }
        else
        {
            dwRes = WaitForSingleObject(osWrite->hEvent, INFINITE);
        }
        switch (dwRes)
        {
        case WAIT_OBJECT_0:
        {
            if (!GetOverlappedResult(hComm, osWrite, &dwWritten, FALSE))
            {
                fRes = SERIAL_WRITE_REQUEST;
            }
            else
            {
                // printf("i send in wait\n");
                memset(txBuf, 0, RX_TX_BUF_SIZE);
                fRes = SERIAL_DEFAULT;
            }
            break;
        }
        default:
        {
            serialErrorsHandler((void *)__func__);
            fRes = SERIAL_DEFAULT;
            break;
        }
        }
    }
    else
    {
        // printf("i send imm\n");
        memset(txBuf, 0, RX_TX_BUF_SIZE);
        fRes = SERIAL_DEFAULT;
    }
    return fRes;
}
/*!
Функция переключает режимы работы асинхронного автомата управления (SerialThread).
\param[out] serialState статус работы автомата управления;
*/
void toggleSerialState(int *serialState)
{
    // TODO: необходимо переделать на отправку данных, при наличии этих данных в очереди;
    uint8_t *tmp = txBuf;
    param->txHandler((void **)&tmp, &messSizeTX);
    if (tmp)
    {
        *serialState = SERIAL_WRITE_REQUEST;
    }
    else
    {
        *serialState = SERIAL_READ_REQUEST;
    }
}

/*!
Функция-автомат управления режимами записи и чтения данных serial port
\param[in] {serialParam_t*} args на вход принимает структуру с параметрами serial port
*/
void *SerialThread(void *args)
{
    param = (serialParam_t *)args;
    if (openAndSettingSerial())
    {
        serialErrorsHandler((void *)__func__);
        return NULL;
    }
    int serialState = SERIAL_READ_REQUEST;

    OVERLAPPED osReader = {0};
    OVERLAPPED osWrite = {0};
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if ((osReader.hEvent == NULL) || (osWrite.hEvent == NULL))
    {
        serialErrorsHandler((void *)__func__);
    }

    for (;;)
    {
        switch (serialState)
        {
        case SERIAL_READ_REQUEST:
        {
            serialState = requestForRead(rxBuf, RX_TX_BUF_SIZE, &osReader);
            break;
        }
        case SERIAL_WAIT_ANSWER_READ:
        {
            serialState = waitAnswerRequestRead(rxBuf, RX_TX_BUF_SIZE, &osReader);
            break;
        }
        case SERIAL_WRITE_REQUEST:
        {
            serialState = requestForWriteAndWait(txBuf, messSizeTX, &osWrite);
            break;
        }
        case SERIAL_FREE:
        {
            // TODO: можно дабавить какой-нибудь обработчик
            break;
        }
        default:
        {
            pthread_testcancel();
            toggleSerialState(&serialState);
            break;
        }
        }

        if ((serialState == SERIAL_DEFAULT) &&
            (param->seraialClose()))
        {
            // serialErrorsHandler("serial close Handel::OK\n");
            break;
        }
    }
    free(rxBuf);
    free(txBuf);
    CloseHandle(osWrite.hEvent);
    CloseHandle(osReader.hEvent);
    CloseHandle(hComm);
    pthread_exit(0);
    return NULL;
}

/*!
функция обработчик ошибок.
__attribute__((weak)) - при наличии собственных обработчиков, можно переопределить данную функцию.
\param[in] err принимает на вход макрос: __func__
*/
__attribute__((weak)) void serialErrorsHandler(void *err)
{
    fprintf(stderr, "Err func::%s\n", (char *)err);
}
/*!
функция обработчик приема сообщений.
__attribute__((weak)) - при наличии собственных обработчиков, можно переопределить данную функцию.
\param[out] mess принятое сообщение по Serial port
\param[out] указатель на размер данных принятого сообщения
*/
__attribute__((weak)) void serialRXHandler(void **mess, size_t *messSize)
{
    char *tmp = (char *)(*mess);
    fprintf(stderr, "mess size::%llu\nmess::%s\n", *messSize, tmp);
}
/*!
функция обработчик отправки сообщений.
__attribute__((weak)) - при наличии собственных обработчиков, можно переопределить данную функцию.
\param[out] mess сообщение для отправки по Serial port,
при отсутствии данных mess может изменён на NULL
\param[out] указатель на размер данных принятого сообщения
*/
__attribute__((weak)) void serialTXHandler(void **mess, size_t *messSize)
{
    char *tmp = (char *)(*mess);
    fprintf(stderr, "mess size::%llu\nmess::%s\n", *messSize, tmp);
}
/*!
функция команда для закрытия порта и удаления выделенной памяти.
__attribute__((weak)) - при наличии собственных обработчиков, можно переопределить данную функцию.
\return если true необходимо закрыть порт.
*/
__attribute__((weak)) int serialClose(void)
{
    static uint8_t status = 100;
    status--;
    return status > 0 ? 0 : 1;
}

void PrintCommState(DCB dcb)
{
    //  Print some of the DCB structure values
    printf("\nBaudRate = %ld, ByteSize = %d, Parity = %d, StopBits = %d\n",
           dcb.BaudRate,
           dcb.ByteSize,
           dcb.Parity,
           dcb.StopBits);
}