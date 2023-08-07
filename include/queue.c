#include "headers/queue.h"

/*internal functions--------------------*/
queue_t *findLastItem(queue_t *head);
void addNewQueue(queueHandel_t **xQueueHandel);
void delAllList(queue_t **head);
/*end internal functions--------------------*/
queueHandel_t *arrayQueueHandels = NULL;

size_t currentSize = 0;
void initQueueManagement(size_t countQueue)
{

    if (countQueue)
    {
        currentSize = sizeof(queueHandel_t) * countQueue;
    }
    else
    {
        countQueue = START_SIZE_QUEUE_HANDELS;
    }
    currentSize = sizeof(queueHandel_t) * countQueue;
    arrayQueueHandels = (queueHandel_t *)malloc(currentSize);
    memset(arrayQueueHandels, 0, currentSize);
    currentSize = countQueue;
}

void addNewQueue(queueHandel_t **xQueueHandel)
{
    size_t count = 0;
    while (count < currentSize)
    {
        if (arrayQueueHandels[count].countId == 0)
        {
            break;
        }
        count++;
    }
    if (count >= currentSize)
    {
        MOD_NAME
        printf("Error\n");
        /*TODO:: remake this after realoc
        address::arrayQueueHandels[0]... will change
        arrayQueueHandels = (queueHandel_t *)realloc(arrayQueueHandels, (sizeof(queueHandel_t) * currentSize + sizeof(queueHandel_t) * START_SIZE_QUEUE_HANDELS));
        queueHandel_t *tmp = &arrayQueueHandels[count];
        memset(tmp, 0, sizeof(queueHandel_t) * START_SIZE_QUEUE_HANDELS);
        currentSize += START_SIZE_QUEUE_HANDELS;
        */
    }
    *xQueueHandel = &arrayQueueHandels[count];
    (*xQueueHandel)->countId = (size_t)&arrayQueueHandels[count];
    (*xQueueHandel)->head = NULL;
}

void createQueue(queueHandel_t **xQueueHandel, void *data, size_t size)
{

    if ((*xQueueHandel) == NULL)
    {
        addNewQueue(xQueueHandel);
    }

    queue_t *newItem = (queue_t *)malloc(sizeof(queue_t));
    newItem->prev = NULL;
    newItem->data = (void *)malloc(size + 1);
    newItem->size = size;
    memset(newItem->data, 0, (size + 1));
    memcpy(newItem->data, data, size);
    (*xQueueHandel)->countItem++;
    newItem->next = (*xQueueHandel)->head;
    (*xQueueHandel)->head = newItem;
}

void delAllList(queue_t **head)
{
    queue_t *tmp = NULL;
    while ((*head)->next)
    {
        tmp = (*head);
        (*head) = (*head)->next;
        free(tmp->data);
        free(tmp);
    }
    free((*head)->data);
    free((*head));
}

queue_t *findLastItem(queue_t *head)
{
    queue_t *last = head;
    while (head)
    {
        last = head;
        head = head->next;
    }
    return last;
}

void addItemQueue(queueHandel_t **xQueueHandel, void *data, size_t size)
{
    if (((*xQueueHandel) == NULL) || (*xQueueHandel)->head == NULL)
    {
        createQueue(xQueueHandel, data, size);
        return;
    }
    (*xQueueHandel)->countItem++;
    queue_t *newItem = (queue_t *)malloc(sizeof(queue_t));
    queue_t *last = findLastItem((*xQueueHandel)->head);
    newItem->size = size;
    newItem->data = (void *)malloc(size + 1);
    memset(newItem->data, 0, size + 1);
    memcpy(newItem->data, data, size);
    last->next = newItem;
    newItem->next = NULL;
    newItem->prev = last;
}

void getItemQueue(queueHandel_t *xQueueHandel, void **data, size_t *size)
{
    if ((xQueueHandel->head) == NULL)
    {
        *data = NULL;
        *size = 0;
        return;
    }
    queue_t *tmp = (xQueueHandel->head);

    memcpy((*data), tmp->data, tmp->size);
    *size = tmp->size;
    xQueueHandel->head = (xQueueHandel->head)->next;
    xQueueHandel->countItem--;
    free(tmp->data);
    free(tmp);
}

void printAllQueueHandels(void)
{
    for (size_t i = 0; i < currentSize; i++)
    {
        printf("arr[%lld]::%p\thead::%p\tcountItem::%llu\tid::0x%llx\n",
               i, &arrayQueueHandels[i],
               arrayQueueHandels[i].head,
               arrayQueueHandels[i].countItem,
               arrayQueueHandels[i].countId);

        if (arrayQueueHandels[i].head)
        {
            queue_t *tmp = arrayQueueHandels[i].head;
            while (tmp)
            {
                printf("data::%s\tsize::%llu\n", (char *)tmp->data, tmp->size);
                tmp = tmp->next;
            }
        }
    }
}

void delAllQueueHandels(void)
{

    for (size_t i = 0; i < currentSize; i++)
    {
        if (arrayQueueHandels[i].head != NULL)
        {
            delAllList(&arrayQueueHandels[i].head);
        }
    }
    free(arrayQueueHandels);
}