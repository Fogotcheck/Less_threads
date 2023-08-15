#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define START_SIZE_QUEUE_HANDELS 5
typedef struct queue
{
    void *data;
    size_t size;
    struct queue *next;
    struct queue *prev;
} queue_t;

typedef struct queueHandel
{
    size_t countId;
    queue_t *head;
    size_t countItem;
}queueHandel_t;

#define MOD_NAME                                                                                    \
    {                                                                                               \
        fprintf(stderr, "\n\t%s\nFunction name:\t%s\t\tLine:%d\n\n", __FILE__, __func__, __LINE__); \
    }

/**/
void initQueueManagement(size_t countQueue);
/*FILO*/
void createQueue(queueHandel_t **xQueueHandel, void *data, size_t *size);
/*FIFO*/
void addItemQueue(queueHandel_t **xQueueHandel, void *data, size_t *size);
/*get item and del head*/
void getItemQueue(queueHandel_t *xQueueHandel, void **data, size_t *size);
void delAllQueueHandels(void);

/*TODO:: this func for debug dell after tests*/
void printAllQueueHandels(void);
#endif //__QUEUE_H__