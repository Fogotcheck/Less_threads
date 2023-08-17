#include "headers/queue.h"

/*internal functions--------------------*/
int checkQueueHandelInMain(queueHandel_t *itemQhandel);
void delAllQueueItems(queue_t **head);
void addQueueItem(queue_t **head, void *data, size_t dataSize);

void getQueueItem(queue_t **head, void **data, size_t *dataSize);
queue_t *findPrevItem(queue_t *head, queue_t *item);
queue_t *findLastItem(queue_t *head);
/*end internal functions--------------------*/

queueHandel_t *mainQueueHandelList = NULL;

void AddQueueHandel(queueHandel_t **qHandel)
{
    queueHandel_t *newItem = (queueHandel_t *)malloc(sizeof(queueHandel_t));
    newItem->handelID = (size_t)newItem;
    newItem->head = NULL;
    newItem->tail = NULL;
    newItem->countItem = 0;
    newItem->totalSizeMem = 0;
    pthread_mutex_init(&newItem->mutex, NULL);
    newItem->next = mainQueueHandelList;
    *qHandel = newItem;
    mainQueueHandelList = newItem;
}

void delAllQueueHandels(void)
{
    if (mainQueueHandelList == NULL)
    {
        return;
    }
    queueHandel_t *prev = NULL;
    while (mainQueueHandelList->next)
    {
        prev = mainQueueHandelList;
        mainQueueHandelList = mainQueueHandelList->next;
        //        printf("del::%p\n", prev);
        delAllQueueItems(&prev->head);
        pthread_mutex_destroy(&prev->mutex);
        free(prev);
    }
    delAllQueueItems(&mainQueueHandelList->head);
    pthread_mutex_destroy(&mainQueueHandelList->mutex);
    free(mainQueueHandelList);
    //    printf("del::%p\n", mainQueueHandelList);
    mainQueueHandelList = NULL;
}

int checkQueueHandelInMain(queueHandel_t *itemQhandel)
{
    queueHandel_t *head = mainQueueHandelList;
    while (head)
    {
        if (head == itemQhandel)
        {
            break;
        }
        head = head->next;
    }
    return head == NULL ? 0 : 1;
}

void delItemQueueHandel(queueHandel_t **itemQhandel)
{
    queueHandel_t *head = mainQueueHandelList, *tmp = NULL;
    if (checkQueueHandelInMain(*itemQhandel))
    {
        if (head == (*itemQhandel))
        {
            tmp = head;
            head = head->next;
            mainQueueHandelList = head;
        }
        else
        {
            while (head)
            {
                if (head->next == (*itemQhandel))
                {
                    tmp = (*itemQhandel);
                    head->next = (*itemQhandel)->next;
                    break;
                }
                head = head->next;
            }
        }
        delAllQueueItems(&tmp->head);
        pthread_mutex_destroy(&tmp->mutex);
        free(tmp);
    }
    *itemQhandel = NULL;
}

void addQueueItem(queue_t **head, void *data, size_t dataSize)
{
    queue_t *newItem = (queue_t *)malloc(sizeof(queue_t));
    newItem->data = data;
    newItem->size = dataSize;
    newItem->next = (*head);
    *head = newItem;
    // printf("in queue::%p\t%p\n", newItem, newItem->data);
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

queue_t *findPrevItem(queue_t *head, queue_t *item)
{
    queue_t *prev = head;
    while (head->next)
    {
        if (head == item)
        {
            break;
        }
        prev = head;
        head = head->next;
    }
    return prev;
}

void getQueueItem(queue_t **head, void **data, size_t *dataSize)
{
    if ((*head) == NULL)
    {
        *data = NULL;
        *dataSize = 0;
        return;
    }
    queue_t *tmp = NULL;
    queue_t *prev = NULL;
    queue_t *last = findLastItem((*head));
    if ((*head)->next == NULL)
    {
        tmp = (*head);
        *head = NULL;
    }
    else
    {
        prev = findPrevItem((*head), last);
        prev->next = NULL;
        tmp = last;
    }
    memcpy((*data), tmp->data, tmp->size);
    if (tmp->data)
    {
        free(tmp->data);
    }
    free(tmp);
}

void delAllQueueItems(queue_t **head)
{
    if ((*head) == NULL)
    {
        return;
    }
    queue_t *prev = NULL;
    while ((*head)->next)
    {
        prev = (*head);
        (*head) = (*head)->next;
        if (prev->data != NULL)
        {
            // printf("del data::0x%p\t0x%p\n", prev, prev->data);
            free(prev->data);
        }
        free(prev);
    }
    if ((*head)->data != NULL)
    {
        // printf("del data::0x%p\t0x%p\n", (*head), (*head)->data);
        free((*head)->data);
    }
    free(*head);
}

void sendQueue(queueHandel_t *qHandel, void *data, size_t dataSize)
{
    if (qHandel == NULL)
    {
        return;
    }
    pthread_mutex_lock(&qHandel->mutex);
    void *tmp = malloc(dataSize);
    memcpy(tmp, data, dataSize);
    addQueueItem(&qHandel->head, tmp, dataSize);
    qHandel->countItem++;
    qHandel->totalSizeMem += dataSize;
    pthread_mutex_unlock(&qHandel->mutex);
}

void receivQueue(queueHandel_t *qHandel, void **dataNull, size_t *dataSize)
{
    if (qHandel == NULL)
    {
        return;
    }
    pthread_mutex_lock(&qHandel->mutex);
    getQueueItem(&qHandel->head, dataNull, dataSize);
    if (*dataNull != NULL)
    {
        qHandel->countItem--;
        qHandel->totalSizeMem -= *dataSize;
    }
    pthread_mutex_unlock(&qHandel->mutex);
}