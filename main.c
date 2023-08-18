#include "include/headers/main.h"
queueHandel_t *q[4];
char myExit = 0;
void serialTXHandler(void **mess, size_t *messSize);
void serialRXHandler(void **mess, size_t *messSize);
int closeSerial(void);

int main(void)
{
    // int ret = 0;

    for (size_t i = 0; i < 4; i++)
    {
        q[i] = NULL;
        AddQueueHandel(&q[i]);
        printf("q[%llu]::%p\n", i, q[i]);
    }
    pthread_t id[3];
    pthread_attr_t att;
    pthread_attr_init(&att);

    serialParam_t serialParam = {{"\\\\.\\COM24", {0}},
                                 serialTXHandler,
                                 serialRXHandler,
                                 closeSerial};
                                 
    pthread_create(&id[0], &att, SerialThread, &serialParam);

    pthread_create(&id[1], &att, sendThread, q);
    pthread_create(&id[2], &att, recievThread, q);
    pthread_create(&id[3], &att, countThread, NULL);

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
    pthread_join(id[2], (void **)&res);
    pthread_join(id[3], (void **)&res);
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