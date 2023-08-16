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
    uint8_t *data = (uint8_t *)malloc(1024);

    size_t dataSize = 0;
    for (size_t i = 0; i < 4; i++)
    {
        uint8_t *tmp = data;
        while (tmp)
        {
            receivQueue(q[i], (void **)&tmp, &dataSize);
            if (tmp)
            {
                printf("data[%llu]::%s\n", i, data);
            }
        }
    }
    free(data);
    // printf("q1::0x%p\nq2::0x%p\nq3::0x%p\nq4::0x%p\n", q_1, q_2, q_3, q_4);
    //    delItemQueueHandel(&q_1);
    // printf("q1::0x%p\nq2::0x%p\nq3::0x%p\nq4::0x%p\n", q_1, q_2, q_3, q_4);
    delAllQueueHandels();
    // pthread_t thID[4];
    // pthread_attr_t att;
    printf("Hello world\n");
    return 0;
}
