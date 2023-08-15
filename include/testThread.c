#include "headers/testThread.h"

void *printThread(void *args)
{
    size_t arg = *((size_t *)args);
    free(args);
    pthread_t id = pthread_self();
    int ret = 0;
    struct timespec ts = {0, 0};
    ts.tv_sec = 3;
    ts.tv_nsec = 500000000;
    pthread_testcancel();
    ret = pthread_delay_np(&ts);
    if (ret)
    {
        printf("Err\n");
    }

    printf("id::%llu\targ::%llu\n", id, arg);
    pthread_exit(0);
    return NULL;
}