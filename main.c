#include "include/headers/main.h"
uint8_t buf[1024];
char comPortName[] = "\\\\.\\COM24";

queueHandel_t *printQueue = NULL, *serialQueue = NULL;

void sendToWrite(void **mess, size_t *sizeMess);

int main(void)
{
    int ret = 0;
    initQueueManagement(3);
    char testMess[] = "hello from main\n\0";
    size_t testMessSize = strlen(testMess) + 2;
    // size_t sizePrint = 0, sizeSerialRX = 0, sizeSerialTX = 0;

    createQueue(&serialQueue, (void *)testMess, &testMessSize);
    createQueue(&printQueue, (void *)testMess, &testMessSize);

    serialParam_t comPortParam;
    comPortParam.portName = comPortName;
    comPortParam.txHandler = sendToWrite;
    comPortParam.rxHandler = NULL;
    comPortParam.seraialClose = NULL;

    pthread_t thSerialID, thPrintID;
    pthread_attr_t thSerialAtt, thPrintAtt;
    pthread_attr_init(&thSerialAtt);
    pthread_attr_init(&thPrintAtt);

    pthread_create(&thSerialID, &thSerialAtt, SerialThread, &comPortParam);
    pthread_create(&thPrintID, &thPrintAtt, printThread, (void *)serialQueue);
    char myExit = 0;

    while (1)
    {
        scanf("%c", &myExit);
        if (myExit == 'y')
        {
            ret = pthread_cancel(thSerialID);
            if (ret == 0)
            {
                ret = pthread_cancel(thPrintID);
                if (ret == 0)
                {
                    break;
                }
            }
        }
        addItemQueue(&printQueue, testMess, &testMessSize);
    }
    delAllQueueHandels();
    ret = pthread_join(thSerialID, NULL);
    printf("Hello world\n");
    return 0;
}

void sendToWrite(void **mess, size_t *sizeMess)
{
    char *tmp = (char *)*mess;
    printf("mess::%s\nsize::%lld", tmp, *sizeMess);
    addItemQueue(&serialQueue, (void *)*mess, sizeMess);
}
