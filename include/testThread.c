#include "headers/testThread.h"

void *sendThread(void *args)
{
    pthread_t id = pthread_self();
    queueHandel_t **q = (queueHandel_t **)args;
    // for (size_t i = 0; i < 4; i++)
    // {
    //     printf("id::%llu\tq[%llu]::%p\n", id, i, q[i]);
    // }

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
        sendQueue(q[1], buf, sizeof(buf));
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
    pthread_t id = pthread_self();
    queueHandel_t **q = (queueHandel_t **)args;
    // for (size_t i = 0; i < 4; i++)
    // {
    //     printf("id::%llu\tq[%llu]::%p\n", id, i, q[i]);
    // }
    char *buf = (char *)malloc(1024);
    size_t dataSize = 0;
    while (1)
    {
        for (size_t i = 0; i < 4; i++)
        {
            char *tmp = buf;
            while (tmp)
            {
                receivQueue(q[i], (void **)&tmp, &dataSize);
                if (tmp)
                {
                    printf("th::%llu\trec::q[%llu]::%s\n", id, i, buf);
                }
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