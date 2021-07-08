#include "XThreadPool.h"
#include "XThread.h"
#include <thread>
#include <chrono>
#include <iostream>
#include "XTask.h"
using namespace std;

void XThreadPool::Dispatch(XTask * task) {
	// �ַ��̲߳�����ѯ�Ļ���
	if (!task)return;
	int tid = (lastThread + 1) % threadCount; // lastThread��ʼ��Ϊ-1
	lastThread = tid;
	XThread * t = threads[tid];

	t->AddTask(task);

	// �����߳�
	t->Activate();

}

// ��ʼ�������̲߳������߳�
void XThreadPool::Init(int threadCount) {
	this->threadCount = threadCount;
	this->lastThread = -1;
	for (int i = 0; i < threadCount; i++) {
		XThread * t = new XThread(); // �½�һ���̣߳����̳߳ظ������
		t->id = i + 1;
		cout << "Create thread " << i << endl;
		// �����߳�
		t->Start();
		threads.push_back(t);
		this_thread::sleep_for(chrono::milliseconds(10)); // �ӳ�10ms
	}
}