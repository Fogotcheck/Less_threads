#include "include/headers/main.h"

int main(void)
{
    int ret = 0;
    pthread_t thID[4];
    pthread_attr_t att;
    pthread_attr_init(&att);
    struct sched_param param;
    param.sched_priority = 0;
    for (size_t i = 0; i < 4; i++)
    {
        size_t *ptr = (size_t *)malloc(sizeof(size_t));
        *ptr = i;
        /*хз - тоже сомое что и просто att.param.sched_priority++
        возвращает тоже самое, нет возможности установить больше MAX, 
        но приоритеты не устанавливают порядок threads*/
        // pthread_attr_setschedparam(&att, &param);
        // param.sched_priority++;
        // printf("att:%d\n", att.param.sched_priority);
        ret = pthread_create(&thID[i], &att, printThread, ptr);
        if (ret != 0)
        {
            printf("th::%llu-Err\n", i);
        }
    }
    int *tmp = NULL;
    for (size_t i = 0; i < 4; i++)
    {
        ret = pthread_join(thID[i], (void **)&tmp);
        if (ret != 0)
        {
            printf("th::%llu-Err::%d\n", i, ret);
        }
    }

    printf("Hello world\n");
    return 0;
}
