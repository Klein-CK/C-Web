#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

using namespace std;

#define LL_ADD(item, list) do { \
	item->pre = NULL; \
	item->next = list; \
	if (list != NULL) list->pre = item; \
	list = item; \
} while (0) 

#define LL_REMOVE(item, list) do { \
	if(item->pre) item->pre->next = item->next; \
	if(item->next) item->next->pre = item->pre; \
	if(item == list) list = item->next; \
	item->pre = item->next = NULL; \
} while (0) 

typedef struct NTASK
{
	void (*func)(struct NTASK *arg);
	void *user_data;

	struct NTASK *pre, *next;
}ntask;

typedef struct NWORKER
{
	pthread_t threadid;
	bool terminal_flag;

	struct NWORKER *pre, *next;
	struct NMANAGER *pool;
}nworker;

typedef struct NMANAGER
{
	ntask   *tasks;
	nworker *workers;

	pthread_mutex_t mutex;
	pthread_cond_t  cond;
}npool;

static void *thread_callback(void *arg) {
	nworker * worker = (nworker*)arg;

	while(true) {
		// 执行队列，也是公共资源，加锁
		pthread_mutex_lock(&worker->pool->mutex);

		// 1、等待是否有任务
		while(worker->pool->tasks == NULL) {
			if(worker->terminal_flag) break;

			pthread_cond_wait(&worker->pool->cond, &worker->pool->mutex);
		}

		if(worker->terminal_flag) {
			pthread_mutex_unlock(&worker->pool->mutex);
			break;
		}

		// 2、有任务则获取一个任务
		ntask * task = worker->pool->tasks; // 任务链表，链表头结点任务
		if(task) LL_REMOVE(task, worker->pool->tasks);

		pthread_mutex_unlock(&worker->pool->mutex);

		if(!task) continue;

		// 3、执行一个任务
		task->func(task);
	}

	free(worker);
	pthread_exit(NULL);
}

int thread_pool_create(npool * pool, int num) {
	if(num < 1) num = 1;
	if(pool == NULL) return -1;
	memset(pool, 0, sizeof(npool));

	pthread_mutex_t blank_mutex = PTHREAD_MUTEX_INITIALIZER;
	memcpy(&pool->mutex, &blank_mutex, sizeof(pthread_mutex_t));

	pthread_cond_t blank_cond   = PTHREAD_COND_INITIALIZER;
	memcpy(&pool->cond,  &blank_cond,  sizeof(pthread_cond_t));

	int i = 0;
	for(; i < num; i++) {
		nworker * worker = (nworker*)malloc(sizeof(nworker));
		if(!worker) { cout << "worker malloc" << endl; return -2; }
		memset(worker, 0, sizeof(nworker));

		worker->pool = pool;
		worker->terminal_flag = false;

		printf("pthread_create\n");
		//cout << "pthread_create " << endl;
		int ret = pthread_create(&worker->threadid, NULL, thread_callback, (void*)worker);
		if(ret) { cout << "pthread_create error" << endl; free(worker); return -3;}

		LL_ADD(worker, worker->pool->workers);
	}
	return 0;
}

int thread_pool_push_task(npool * pool, ntask * task) {
	pthread_mutex_lock(&pool->mutex);

	LL_ADD(task, pool->tasks);

	pthread_cond_signal(&pool->cond); // 通知一个wait条件锁，因为执行队列在等待中

	pthread_mutex_unlock(&pool->mutex);
}

void thread_pool_destory(npool * pool) {
	nworker * worker = pool->workers;

	for(; worker != NULL; worker = worker->next) {
		worker->terminal_flag = true; // 设置为终止标识，不再参与执行
	}

	pthread_mutex_lock(&pool->mutex);
	pool->workers = NULL;
	pool->tasks = NULL;
	pthread_cond_broadcast(&pool->cond);
	pthread_mutex_unlock(&pool->mutex);
}

#if 1
void counter(ntask * task) {
	printf("counter---> %d, selfid: %lu\n", *(int*)task->user_data, pthread_self());
	//cout << "counter-->" << *(int *)task->user_data << endl;

	free(task->user_data);
	free(task);
}

int main() {
	npool pool;
	thread_pool_create(&pool, 10);
	printf("thread_pool_create \n");

	int i = 0;
	for(; i < 500; i++) {
		ntask * task = (ntask*)malloc(sizeof(ntask));
		if(!task) { cout << "malloc" << endl; exit(1); }
		memset(task, 0, sizeof(ntask));

		task->func = counter;
		task->user_data = malloc(sizeof(int)); //本身就是void *，所以先无需转换
		*(int *)task->user_data = i;

		printf("thread_pool_push_task\n");
		thread_pool_push_task(&pool, task);
	}

	getchar();
	printf("\n");
}
#endif
