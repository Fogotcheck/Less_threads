#include "headers/testThread.h"

void *printThread(void *args)
{
    /*
    8 байт - атомарная операция для моей ОС ??
    вариант на си:                      на assembler:
    __asm__ volatile("NOP");            14000263c:	90              nop
    size_t arg = *((size_t *)args);     14000263d:	48 8b 45 10     mov    0x10(%rbp),%rax
                                        140002641:	48 8b 00     	mov    (%rax),%rax
                                        140002644:	48 89 45 f8     mov    %rax,-0x8(%rbp)
    __asm__ volatile("NOP");            140002648:	90              nop

    или нет?
    __asm__ volatile("NOP");            14000263c:	90              nop
                                 14000263d:	48 8b 45 f8     mov    -0x8(%rbp),%rax
                                        140002641:	48 89 45 f0     mov    %rax,-0x10(%rbp)
    __asm__ volatile("NOP");            140002645:	90              nop
    */
    uint32_t a, b;
    __asm__ volatile("NOP");
    a = b;
    __asm__ volatile("NOP");
    // size_t arg = *((size_t *)args);
    // free(args);
    // pthread_t id = pthread_self();
    // int ret = 0;
    // struct timespec ts = {0, 0};
    // /*
    // наглядный пример почему так делать нельзя
    // одно устройство - один thread иначе всё путается
    // for (size_t i = 0; i < 10; i++)
    // {
    //     printf("id[%llu]::%llu\t", i, id);
    //     if (((i + 1) % 2) == 0)
    //     {
    //         printf("\n");
    //     }
    // }
    // */
    // ts.tv_sec = 3;
    // ts.tv_nsec = 500000000;
    // pthread_testcancel();
    // ret = pthread_delay_np(&ts);
    // if (ret)
    // {
    //     printf("Err\n");
    // }
    // size_t *tmp = (size_t *)malloc(sizeof(size_t));
    // printf("id::%llu\targ::%llu\n", id, arg);
    // *tmp = arg;
    // /*аналогичные операции return*/
    // // pthread_exit(tmp);
    // return tmp;
}