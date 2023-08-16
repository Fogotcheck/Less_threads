#ifndef __TETS_THREAD_H__
#define __TETS_THREAD_H__
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

typedef struct printItem
{
    char *str;
    size_t size;
} printItem_t;

#include "queue.h"
void *sendThread(void *args);
void *recievThread(void *args);
#endif //__TETS_THREAD_H__