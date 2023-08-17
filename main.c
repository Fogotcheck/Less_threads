#include "include/headers/main.h"

int main(void)
{
    // int ret = 0;
    queueHandel_t *q[4];
    for (size_t i = 0; i < 4; i++)
    {
        q[i] = NULL;
        AddQueueHandel(&q[i]);
        printf("q[%llu]::%p\n", i, q[i]);
    }
    char tq_1[] = "tq_1 hello main";
    char tq_2[] = "tq_2 hello main";
    char tq_3[] = "tq_3 hello main";
    char tq_4[] = "tq_4 hello main";
    for (size_t i = 0; i < 4; i++)
    {
        sendQueue(q[i], tq_1, sizeof(tq_1));
        sendQueue(q[i], tq_2, sizeof(tq_2));
        sendQueue(q[i], tq_3, sizeof(tq_3));
        sendQueue(q[i], tq_4, sizeof(tq_4));
    }
    pthread_t id[3];
    pthread_attr_t att;
    pthread_attr_init(&att);
    for (size_t i = 0; i < 2; i++)
    {
        pthread_create(&id[i], &att, sendThread, q);
    }
    pthread_create(&id[2], &att, recievThread, q);
    pthread_create(&id[3], &att, countThread, NULL);
    
    char myExit = 0;
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
