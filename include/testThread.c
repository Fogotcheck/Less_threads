#include "headers/testThread.h"

void *sendThread(void *args)
{
    pthread_t id = pthread_self();
    queueHandel_t **q = (queueHandel_t **)args;
    for (size_t i = 0; i < 4; i++)
    {
        printf("id::%llu\tq[%llu]::%p\n", id, i, q[i]);
    }

    int ret = 0;
    struct timespec ts = {0, 0};
    ts.tv_sec = 1;
    ts.tv_nsec = 0; // 500000000;
    char buf[64] = "id:: \tth_Hello\ti::\0\0\0";
    buf[4] = (char)id;
    uint8_t len = strlen(buf);
    for (size_t i = 0; i < 10; i++)
    {
        buf[len + 1] = i + '0';
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
    for (size_t i = 0; i < 4; i++)
    {
        printf("id::%llu\tq[%llu]::%p\n", id, i, q[i]);
    }
    while (1)
    {
        pthread_testcancel();
    }
    return NULL;
}