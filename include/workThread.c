#include "headers/workThread.h"

void *sendThread(void *args)
{
    pthread_t id = pthread_self();
    queueHandel_t **q = (queueHandel_t **)args;

    int ret = 0;
    struct timespec ts = {0, 0};
    ts.tv_sec = 1;
    ts.tv_nsec = 0; // 500000000;
    char buf[64] = "id:: \tth_Hello\ti::\0\0\0";
    buf[4] = (char)id + '0';
    uint8_t len = strlen(buf);
    for (size_t i = 0; i < 10; i++)
    {
        buf[len] = i + '0';
        sendQueue(q[2], buf, sizeof(buf));
        pthread_testcancel();
        ret = pthread_delay_np(&ts);
        if (ret)
        {
            printf("Err\n");
        }
    }

    return NULL;
}

void *recievThread(void *args)
{
    // pthread_t id = pthread_self();
    queueHandel_t **q = (queueHandel_t **)args;
    char *buf = (char *)malloc(1024);
    size_t dataSize = 0;
    while (1)
    {
        char *tmp = buf;
        while (tmp)
        {
            receivQueue(q[1], (void **)&tmp, &dataSize);
            if (tmp)
            {
                printf("rec::\t");
                for (size_t i = 0; i < dataSize; i++)
                {
                    if (buf[i] == '\r')
                    {
                        break;
                    }
                    printf("%c", buf[i]);
                }
                printf("\t\t");
                for (size_t i = 0; i < dataSize; i++)
                {
                    printf("0x%x ", (uint8_t)buf[i]);
                }
                printf("\tsize::%llu\n", dataSize);
            }
        }
        pthread_testcancel();
    }
    return NULL;
}

void *countThread(__attribute__((__unused__)) void *args)
{
    DWORD curTick = 0, prevTick = 0;
    args = NULL;
    int ret = 0;
    struct timespec ts = {0, 0};
    ts.tv_sec = 1;
    ts.tv_nsec = 0; // 500000000;
    while (1)
    {
        curTick = GetTickCount();
        printf("work th\ttick::%lu\n", (curTick - prevTick));
        prevTick = GetTickCount();
        pthread_testcancel();
        ret = pthread_delay_np(&ts);
        if (ret)
        {
            printf("Err\n");
        }
    }
    return NULL;
}