#include "XThreadPool.h"
#include "XThread.h"
#include <thread>
#include <chrono>
#include <iostream>
#include "XTask.h"
using namespace std;

void XThreadPool::Dispatch(XTask * task) {
	// 分发线程采用轮询的机制
	if (!task)return;
	int tid = (lastThread + 1) % threadCount; // lastThread初始化为-1
	lastThread = tid;
	XThread * t = threads[tid];

	t->AddTask(task);

	// 激活线程
	t->Activate();

}

// 初始化所有线程并启动线程
void XThreadPool::Init(int threadCount) {
	this->threadCount = threadCount;
	this->lastThread = -1;
	for (int i = 0; i < threadCount; i++) {
		XThread * t = new XThread(); // 新建一个线程，由线程池负责管理
		t->id = i + 1;
		cout << "Create thread " << i << endl;
		// 启动线程
		t->Start();
		threads.push_back(t);
		this_thread::sleep_for(chrono::milliseconds(10)); // 延迟10ms
	}
}