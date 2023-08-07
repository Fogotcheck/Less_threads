#include "include/headers/main.h"

int main(void)
{
    char threadName[] = "___a";

    initQueueManagement(0);

    int ret = 0;
    pthread_t tid;
    pthread_attr_t att;

    ret = pthread_attr_init(&att);
    if (ret)
    {
        printf("Err\n");
    }
    queueHandel_t *printQueue = NULL;
    queueHandel_t *firstQueue = NULL;
    queueHandel_t *secondQueue = NULL;
    for (size_t i = 0; i < 10; i++)
    {
        threadName[0] = '1';
        addItemQueue(&firstQueue, threadName, strlen(threadName));
        threadName[0] = '2';
        addItemQueue(&secondQueue, threadName, strlen(threadName));
        threadName[0] = '3';
        addItemQueue(&printQueue, threadName, strlen(threadName));
        threadName[3]++;
    }

    att.param.sched_priority = 2;
    ret = pthread_create(&tid, &att, printThread, printQueue);
    if (ret)
    {
        printf("Err\n");
    }
    char exit = 0;
    while (1)
    {
        scanf("%c", &exit);
        if (exit == 'y')
        {
            ret = pthread_cancel(tid);
            if (ret == 0)
            {
                break;
            }
        }
        else
        {
            addItemQueue(&printQueue, threadName, strlen(threadName));
            threadName[3]++;
        }
    }
    delAllQueueHandels();
    ret = pthread_join(tid, NULL);
    printf("hello worllld\n");
    return 0;
}
