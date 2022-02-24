#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
//ʹ�ú궨�彫ĳ��������ӵ�������
#define LL_ADD(item, list) \
    do                     \
    {                      \
        item->prev = NULL; \
        item->next = list; \
        list = item;       \
    } while (0)

//ʹ�ú궨�彫ĳ�������Ӷ�����ȥ��
#define LL_REMOVE(item, list)              \
    do                                     \
    {                                      \
        if (item->prev != NULL)            \
            item->prev->next = item->next; \
        if (item->next != NULL)            \
            item->next->prev = item->prev; \
        if (list == item)                  \
            list = item->next;             \
        item->prev = item->next = NULL;    \
    } while (0)

// Nworker�߳̽ڵ�
typedef struct NWORKER
{
    pthread_t thread;
    int terminate;

} nworker;

// Njob����ڵ�
typedef struct NJOB
{
} nJob;

// Nworkqueue �̶߳���
typedef struct NWORKQUEUE
{
    struct NWORKER *workers;
} nWorkQueue;

//�����������Ϊ�̳߳�
typedef nWorkQueue nThreadPool;

//���߳����һ������
static void *ntyWorkerThread(void *ptr)
{
}

//�����̳߳�
int ntyThreadPoolCreate(nThreadPool *workqueue, int numWorkers)
{
}

//�ر��̳߳�
void ntyThreadPoolShutdown(nThreadPool *workqueue)
{
}

//��������ӽ��ȴ�����
void ntyThreadPoolQueue(nThreadPool *workqueue, nJob *job)
{

    // ����һ���źŸ�����һ�����ڴ��������ȴ�״̬���߳�,ʹ����������״̬,����ִ��
}

/************************** debug thread pool **************************/
// sdk  --> software develop kit
//  �ṩSDK������������ʹ��
#if 1

#define KING_MAX_THREAD 80
#define KING_COUNTER_SIZE 1000
void king_counter(nJob *job)
{
}

int main(int argc, char *argv[])
{
}
#endif