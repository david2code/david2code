#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>


/*
 * thread pool
 */

#define LL_INSERT(item, list) do { \
    item->prev = NULL; \
    item->next = list; \
    if (list != NULL) \
        list->prev = item; \
    list = item;  \
} while(0)

#define LL_REMOVE(item, list) do { \
    if (item->prev != NULL) \
        item->prev->next = item->next; \
    if (item->next != NULL) \
        item->next->prev = item->prev; \
    if (list == item) \
        list = item->next; \
    item->prev = item->next = NULL; \
} while(0)

typedef struct NWORKER {
    pthread_t thread;
    int       id;

    struct NTHREADPOOL *pool;
    struct NWORKER *prev;
    struct NWORKER *next;
}nWorker;

typedef struct NJOB {
    void (*job_func_)(struct NJOB *job);
    void *user_data;
    struct NJOB *prev;
    struct NJOB *next;
}nJob;

typedef struct NTHREADPOOL {
    struct NWORKER *worker;
    struct NJOB *job;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
} nThreadPool;

static void *worker_callback(void *arg) {
    nWorker *worker = (nWorker *)arg;
    int thread_id = worker->id;

    printf("Enter thread %d\n", thread_id);
    while(1) {
        sleep(1);
        printf("thread %d is running\n", thread_id);
    }
    return 0;
}

/*
 * create thread
 */
int nThreadPoolCreate(nThreadPool *pool, int numWorkers) {
    if (numWorkers < 1) numWorkers = 1;
    if (pool == NULL) assert(0);


    memset(pool, 0, sizeof(nThreadPool));
    pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
    memcpy(&pool->mutex, &blank_mutex, sizeof(pthread_mutex_t));

    //init cond
    pthread_cond_t blank_cond = PTHREAD_COND_INITIALIZER;
    memcpy(&pool->cond, &blank_cond, sizeof(pthread_cond_t));

    int i = 0;
    for (i = 0; i < numWorkers; i++) {
        nWorker *worker = (nWorker *)malloc(sizeof(nWorker));
        if (worker == NULL) {
            perror("maloc failed");
            goto ERROR_EXIT;
        }
        memset(worker, 0, sizeof(nWorker));
        worker->pool = pool;
        worker->id = i;

        int ret = pthread_create(&worker->thread, NULL, worker_callback, (void *)worker);
        if (ret) {
            perror("pthread_create failed");
            goto ERROR_EXIT;
        }

        LL_INSERT(worker, pool->worker);
    }
    return 0;

ERROR_EXIT:
    while(pool->worker) {
        struct NWORKER *list = pool->worker;
        LL_REMOVE(list, pool->worker);
        free(list);
    }
    return -1;
}

/*
 * join thread
 */
int nThreadPoolDestory(nThreadPool *pool) {
    struct NWORKER *list; 
    for (list = pool->worker; list != NULL; list = list->next) {
        int res = pthread_join(list->thread, NULL);
        if (res != 0) {
            perror("thread join failed");
            exit(-1);
        }
        printf("thread join %d\n", list->id);
    }

    return 0;
}

nThreadPool gThreadPool;

/*
 * main
 */
int main(int argc, char **argv)
{
    nThreadPoolCreate(&gThreadPool, 4);
    nThreadPoolDestory(&gThreadPool);
    return 0;
}
