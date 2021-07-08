#include <iostream>
#include <vector>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class ThreadPool
{
	typedef function<void(void)> Task;

	bool terminal_flag;
	int m_threadNumber;
	int m_maxTasks;
	queue<Task> m_TaskQueue;
	mutex m_mtx;
	condition_variable m_cv;

	static void *thread_callback(void *arg);
	void run();

public:
	explicit ThreadPool(int threadNumber = 10, int maxTasks = 20);
	~ThreadPool();

	bool append_task(Task task); // 添加任务接口
	
};

ThreadPool::ThreadPool(int threadNumber, int maxTasks) :
	m_threadNumber(threadNumber),
	m_maxTasks(maxTasks),
	terminal_flag(false)
{
	if(m_threadNumber < 1) m_threadNumber = 1;

	for (int i = 0; i < m_threadNumber; ++i)
	{
		cout << "ThreadPool: Create thread" << i+1 << endl;
		thread([this]{ ThreadPool::thread_callback(this); }).detach(); // 创建并分离线程
	}
}

ThreadPool::~ThreadPool() {
	{
		unique_lock<mutex> lock(m_mtx);
		terminal_flag = true;
	}
	m_cv.notify_all(); //通知所有wait状态的线程竞争对象的控制权，唤醒所有线程执行
}

bool ThreadPool::append_task(ThreadPool::Task task) {
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

void *ThreadPool::thread_callback(void * arg) {
	ThreadPool * pool = static_cast<ThreadPool*>(arg);
	pool->run();
	return nullptr;
}

void ThreadPool::run() {
	unique_lock<mutex> lock(m_mtx);
	while(!terminal_flag) {
		m_cv.wait(lock); // 没有资源：释放m_mtx，加cv锁；  有资源：释放cv锁，加m_mtx
		if(!m_TaskQueue.empty()) {
			Task task = m_TaskQueue.front();
			m_TaskQueue.pop();
			task();
		}
	}
}



#if 1

typedef function<void(void)> Task;

int main(int argc, char const *argv[])
{
	cout << argv[0] << endl; // print path 
	int threadN = stoi(argv[1]), tasksM = stoi(argv[2]);
	ThreadPool pool(threadN, tasksM);

	for (int i = 0; i < tasksM; ++i)
	{
		Task task([i] {
			cout << "counter--> " << i << endl;
		});
		pool.append_task(task);
	}
	getchar();
	cout << endl;
	return 0;
}
#endif

