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
    pthread_t thread;             //һ���߳�
    int terminate;                //��ֹ״̬
    struct NWORKQUEUE *workqueue; //�������
    struct NWORKER *prev;         //ǰһ���߳�
    struct NWORKER *next;         //��һ���߳�

} nworker;

// Njob����ڵ�
typedef struct NJOB
{
    void (*job_function)(struct NJOB *job); //����ָ��
    void *user_data;                        //�û�����
    struct NJOB *prev;                      //ǰһ������
    struct NJOB *next;                      //��һ������
} nJob;

// Nworkqueue ��������
typedef struct NWORKQUEUE
{
    struct NWORKER *workers;   //������Ա
    struct NJOB *waiting_jobs; //����
    pthread_mutex_t jobs_mtx;  //������
    pthread_cond_t jobs_cond;  //��������
} nWorkQueue;

//�����������Ϊ�̳߳�
typedef nWorkQueue nThreadPool;

//���̣߳����̳߳��������
static void *ntyWorkerThread(void *ptr)
{
    //
    nworker *worker = (nworker *)ptr;

    while (1)
    {
        pthread_mutex_lock(&worker->workqueue->jobs_mtx);

        //������������еȴ�����Ϊ��
        while (worker->workqueue->waiting_jobs == NULL)
        {
            if (worker->terminate)
                break;
            //�����ȴ��������
            pthread_cond_wait(&worker->workqueue->jobs_cond, &worker->workqueue->jobs_mtx);
        }
        //�������н�����ֹ״̬
        if (worker->terminate)
        {
            pthread_mutex_unlock(&worker->workqueue->jobs_mtx);
        }
        //������ӵȴ�������ȡ����
        nJob *job = worker->workqueue->waiting_jobs;
        if (job != NULL)
        {
            LL_REMOVE(job, worker->workqueue->waiting_jobs);
        }

        pthread_mutex_unlock(&worker->workqueue->jobs_mtx);

        //���ȡ����������Ϊ��
        if (job == NULL)
            continue;

        //������ָ����������Ļص�������
        job->job_function(job);
    }

    // �˳�ѭ��
    free(worker);
    pthread_exit(NULL);
}

//�����̳߳�
int ntyThreadPoolCreate(nThreadPool *workqueue, int numWorkers)
{
    if (numWorkers < 1)
        numWorkers = 1;
    memset(workqueue, 0, sizeof(nThreadPool));
    //��ʼ��Ҫʹ�õ���������
    pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
    memset(&workqueue->jobs_cond, &blank_cond, sizeof(workqueue->jobs_cond));
    //��ʼ��������
    pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
    memset(&workqueue->jobs_mtx, &blank_mutex, sizeof(workqueue->jobs_mtx));
}

//�ر��̳߳�
void ntyThreadPoolShutdown(nThreadPool *workqueue)
{
}

//��������ӽ��ȴ�����
void ntyThreadPoolQueue(nThreadPool *workqueue, nJob *job)
{
    pthread_mutex_lock(&workqueue->jobs_mtx);

    //��������ӽ��ȴ�����
    LL_ADD(job, workqueue->waiting_jobs);
    // ����һ���źŸ�����һ�����ڴ��������ȴ�״̬���߳�,
    // ʹ����������״̬,����ִ��
    pthread_cond_signal(&workqueue->jobs_cond);

    pthread_mutex_unlock(&workqueue->jobs_mtx);
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