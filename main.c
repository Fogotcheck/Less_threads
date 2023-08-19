#include "include/headers/main.h"
queueHandel_t *q[4];
char myExit = 0;
void serialTXHandler(void **mess, size_t *messSize);
void serialRXHandler(void **mess, size_t *messSize);
int closeSerial(void);

serialParam_t serialParam = {{"\\\\.\\COM3", {0}},
                             serialTXHandler,
                             serialRXHandler,
                             closeSerial};

int main(int argc, char *argv[])
{
    // int ret = 0;
    argumentsHandler(argc, argv);
    for (size_t i = 0; i < 4; i++)
    {
        q[i] = NULL;
        AddQueueHandel(&q[i]);
    }
    pthread_t id[3];
    pthread_attr_t att;
    pthread_attr_init(&att);

    pthread_create(&id[0], &att, SerialThread, &serialParam);

    pthread_create(&id[1], &att, sendThread, q);
    pthread_create(&id[2], &att, recievThread, q);
    /*for tests*/
    //    pthread_create(&id[3], &att, countThread, NULL);

    while (1)
    {
        scanf("%c", &myExit);
        if (myExit == 'q')
        {
            pthread_cancel(id[3]);
            pthread_cancel(id[2]);
            break;
        }
    }
    uint8_t *res;
    pthread_join(id[0], (void **)&res);
    pthread_join(id[2], (void **)&res);
//    pthread_join(id[3], (void **)&res);
    delAllQueueHandels();
    printf("Hello world\n");
    return 0;
}

void serialRXHandler(void **mess, size_t *messSize)
{
    sendQueue(q[1], *mess, *messSize);
}

void serialTXHandler(void **mess, size_t *messSize)
{
    receivQueue(q[2], mess, messSize);
}

int closeSerial(void)
{
    return myExit == 0 ? 0 : 1;
}

void setBaudRate(char *optarg)
{
    serialParam.portSettings.dcb.BaudRate = atoi(optarg);
    printf("baud rate::%ld\n", serialParam.portSettings.dcb.BaudRate);
}
void setByteSize(char *optarg)
{
    serialParam.portSettings.dcb.ByteSize = atoi(optarg);
    printf("byte size::%u\n", serialParam.portSettings.dcb.ByteSize);
}
void setParity(char *optarg)
{
    serialParam.portSettings.dcb.Parity = atoi(optarg);
    printf("Parity::%u\n", serialParam.portSettings.dcb.Parity);
}
void setEndBits(char *optarg)
{
    serialParam.portSettings.dcb.StopBits = atoi(optarg);
    printf("End bits::%u\n", serialParam.portSettings.dcb.StopBits);
}
void setPortName(char *optarg)
{
    serialParam.portSettings.portName = optarg;
    printf("Port name::%s\n", serialParam.portSettings.portName);
}