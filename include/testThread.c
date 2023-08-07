#include "headers/testThread.h"

void *printThread(void *args)
{
    queueHandel_t *xQueuePrint = (queueHandel_t *)args;
    int ret = 0;
    struct timespec ts = {0, 0};
    ts.tv_sec = 1;
    ts.tv_nsec = 500000000;
    printItem_t needPrint;
    needPrint.str = (char *)malloc(10);
    needPrint.str[4] = 0;

    while (1)
    {
        pthread_testcancel();
        char *tmp = needPrint.str;
        getItemQueue(xQueuePrint, (void **)&(tmp), &needPrint.size);
        if (tmp)
        {
            printf("%s\tsize::%llu\n", needPrint.str, needPrint.size);
        }
        ret = pthread_delay_np(&ts);
        if (ret)
        {
            printf("Err\n");
        }
    }
    free(needPrint.str);
    pthread_exit(0);
    return NULL;
}