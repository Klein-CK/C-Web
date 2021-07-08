#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

using namespace std;

template<typename T>
class ThreadPool
{
	bool terminal_flag;
	int m_threadNumber;
	int m_maxTasks;
	queue<T*> m_TaskQueue;
	mutex m_mtx;
	condition_variable m_cv;

	static void *thread_callback(void *arg);
	void run();

public:
	explicit ThreadPool(int threadNumber = 10, int maxTasks = 20);
	~ThreadPool();

	bool append_task(T *task);
	
};

template<typename T>
ThreadPool<T>::ThreadPool(int threadNumber, int maxTasks) :
	m_threadNumber(threadNumber),
	m_maxTasks(maxTasks),
	terminal_flag(false)
{
	if(m_threadNumber < 1) m_threadNumber = 1;

	for (int i = 0; i < m_threadNumber; ++i)
	{
		pthread_t pid;
		if(pthread_create(&pid, nullptr, thread_callback, static_cast<void*>(this)) == 0)
		{
			cout << "ThreadPool: create " << i+1 << " thread" << endl;
			pthread_detach(pid); // 分离线程，若这里不分离则需要析构的时候调用join操作
		}
	}
}

template<typename T>
ThreadPool<T>::~ThreadPool() {
	{
		unique_lock<mutex> lock(m_mtx);
		terminal_flag = true;
	}
	m_cv.notify_all(); //通知所有wait状态的线程竞争对象的控制权，唤醒所有线程执行
}

template<typename T>
bool ThreadPool<T>::append_task(T * task) {
	if(m_TaskQueue.size() > m_maxTasks) {
		cout <<"ThreadPool: WorkQueue is Full " << endl;
		return false;
	}
	{
		unique_lock<mutex> lock(m_mtx);
		m_TaskQueue.emplace(task);
	}
	m_cv.notify_one(); // 通知线程
	return true;
}

template<typename T>
void *ThreadPool<T>::thread_callback(void * arg) {
	ThreadPool * pool = static_cast<ThreadPool*>(arg);
	pool->run();
	return nullptr;
}

template<typename T>
void ThreadPool<T>::run() {
	unique_lock<mutex> lock(m_mtx);
	while(!terminal_flag) {
		m_cv.wait(lock); // 没有资源：释放m_mtx，加cv锁；  有资源：释放cv锁，加m_mtx
		if(!m_TaskQueue.empty()) {
			T * task = m_TaskQueue.front();
			m_TaskQueue.pop();
			if(task) task->process();
		}
	}
}

class Counter
{
	int m_val;
public:
	Counter(int val) : m_val(val) {}
	~Counter();
	void process() {
		cout << "counter-> " << m_val << endl;
	}
};

#if 1
int main(int argc, char const *argv[])
{
	cout << argv[0] << endl; // print path 
	int threadN = stoi(argv[1]), tasksM = stoi(argv[2]);
	ThreadPool<Counter> pool(threadN, tasksM);

	for (int i = 0; i < tasksM; ++i)
	{
		Counter * task = new Counter(i);
		pool.append_task(task);
	}
	getchar();
	cout << endl;
	return 0;
}
#endif
