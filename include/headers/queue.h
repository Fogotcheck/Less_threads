#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define START_SIZE_QUEUE_HANDELS 5
typedef struct queue
{
    void *data;
    size_t size;
    struct queue *next;
} queue_t;

typedef struct queueHandel
{
    size_t handelID;
    queue_t *head;
    size_t totalSizeMem;
    size_t countItem;
    pthread_mutex_t mutex;
    struct queueHandel *next;
} queueHandel_t;


void AddQueueHandel(queueHandel_t **qHandel);
void delAllQueueHandels(void);
void delItemQueueHandel(queueHandel_t **itemQhandel);
void sendQueue(queueHandel_t *qHandel, void *data, size_t dataSize);
void receivQueue(queueHandel_t *qHandel, void **dataNull, size_t *dataSize);

#endif //__QUEUE_H__