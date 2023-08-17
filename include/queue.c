#include "headers/queue.h"

/*internal functions--------------------*/

int checkQueueHandelInMain(queueHandel_t *itemQhandel);
void delAllQueueItems(queue_t **head);
void pushQueueItem(queue_t **head, void *data, size_t dataSize);
void popQueueItem(queue_t **head, void **data, size_t *dataSize);
queue_t *findPrevItem(queue_t *head, queue_t *item);
queue_t *findLastItem(queue_t *head);
/*end internal functions--------------------*/

/*!
\param[] mainQueueHandelList указатель на структуру обработчиков очереди;
*/
queueHandel_t *mainQueueHandelList = NULL;

/*!
Функция добавляет обработчик очереди для взаимодействия.
\param[in] qHandel двойной указатель на обработчик.
Данный указатель будет изменён, память будет выдулена из кучи.
*/
void AddQueueHandel(queueHandel_t **qHandel)
{
    queueHandel_t *newItem = (queueHandel_t *)malloc(sizeof(queueHandel_t));
    newItem->handelID = (size_t)newItem;
    newItem->head = NULL;
    newItem->countItem = 0;
    newItem->totalSizeMem = 0;
    pthread_mutex_init(&newItem->mutex, NULL);
    newItem->next = mainQueueHandelList;
    *qHandel = newItem;
    mainQueueHandelList = newItem;
}
/*!
Функция удаляет все обработчики очередей и освобождает память.
*/
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
        delAllQueueItems(&prev->head);
        pthread_mutex_destroy(&prev->mutex);
        free(prev);
    }
    delAllQueueItems(&mainQueueHandelList->head);
    pthread_mutex_destroy(&mainQueueHandelList->mutex);
    free(mainQueueHandelList);
    mainQueueHandelList = NULL;
}
/*!
Функция проверяет наличие обработчика в главном списке.
\param[in] itemQhandel элемент который необходимо найти;
\return true, если обработчик зарегистрирован в mainQueueHandelList;
*/
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
/*!
Функция удаляет полученный обработчик и освобождает память.
\param[out] itemQhandel удаляет все элементы очереди указанного обработчика,
освобождает всю память,
изменяет указатель обработчика на NULL
*/
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
/*!
Функция добавляет данные в список очереди.
\param[in] head двойной указатель на вершину списка;
\param[in] data указатель на данные;
\param[in] dataSize размер данных;
*/
void pushQueueItem(queue_t **head, void *data, size_t dataSize)
{
    queue_t *newItem = (queue_t *)malloc(sizeof(queue_t));
    newItem->data = data;
    newItem->size = dataSize;
    newItem->next = (*head);
    *head = newItem;
}

/*!
Функция возвращает последний элемент списка.
\param[in] head указатель на вершину списка;
\return указатель на последний элемент списка
*/
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
/*!
Функция возвращает предыдущий элемент очереди.
\param[in] head указатель на вершину списка;
\param[in] item указатель на элемент списка;
\return указатель на предыдущий элемент
*/
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
/*!
Функция забирает данные из очереди.
\param[in] head двойной указатель на вершину списка;
\param[out] data двойной указатель на данные, в случае ошибки изменит указатель на NULL;
\param[out] dataSize размер данных;
*/
void popQueueItem(queue_t **head, void **data, size_t *dataSize)
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
/*!
Функция удаляет все элементы queue_t  из обработчика.
\param[in] head двойной указатель на вершину списка;
*/
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
            free(prev->data);
        }
        free(prev);
    }
    if ((*head)->data != NULL)
    {
        free((*head)->data);
    }
    free(*head);
}
/*!
Функция для отправки в очередь сообщения, данные будут скопированы.
\param[in] qHandel указатель на обработчик;
\param[in] data указатель на данные для копирования;
\param[in] dataSize размер данных;
*/
void sendQueue(queueHandel_t *qHandel, void *data, size_t dataSize)
{
    if (qHandel == NULL)
    {
        return;
    }
    pthread_mutex_lock(&qHandel->mutex);
    void *tmp = malloc(dataSize);
    memcpy(tmp, data, dataSize);
    pushQueueItem(&qHandel->head, tmp, dataSize);
    qHandel->countItem++;
    qHandel->totalSizeMem += dataSize;
    pthread_mutex_unlock(&qHandel->mutex);
}
/*!
Функция для приёма сообщения из очереди, данные будут скопированы.
\param[in] qHandel указатель на обработчик;
\param[out] data двойной указатель на данные для копирования, при отсутствии данных, изменит указатель на NULL;
\param[out] dataSize указатель на размер данных;
*/
void receivQueue(queueHandel_t *qHandel, void **dataNull, size_t *dataSize)
{
    if (qHandel == NULL)
    {
        return;
    }
    pthread_mutex_lock(&qHandel->mutex);
    popQueueItem(&qHandel->head, dataNull, dataSize);
    if (*dataNull != NULL)
    {
        qHandel->countItem--;
        qHandel->totalSizeMem -= *dataSize;
    }
    pthread_mutex_unlock(&qHandel->mutex);
}