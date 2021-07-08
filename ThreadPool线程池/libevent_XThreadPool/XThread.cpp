#include "XThread.h"
#include <thread>
#include <iostream>
#include <event2/event.h>
#include <event2/util.h>
#include "XTask.h"
#ifdef _WIN32
#else
#include <unistd.h>
#endif // _WIN32


using namespace std;

// static 使用了隐藏属性，仅在文件内可见，所以命名可重复
static void NotifyCB(evutil_socket_t fd, short which, void *arg) {
	XThread * t = static_cast<XThread *>(arg); // 传递的参数是this
	t->Notify(fd, which);
}

// 收到主线程发出的激活消息（线程池的分发）
void XThread::Notify(evutil_socket_t fd, short which) {
	// 水平触发  只要没有接收完成，会再次进来
	char buf[2] = { 0 };
#ifdef _WIN32
	int re = recv(fd, buf, 1, 0);
#else
	// linux 下是管道，不能用recv，得用read
	int re = read(fd, buf, 1);
#endif
	if (re <= 0)
		return;
	cout << id << " thread " << buf << endl; // 打印 线程id 和 激活发送收到的消息

	XTask * task = NULL;
	// 获取任务，并初始化任务
	tasks_mutex.lock();
	if (tasks.empty()) {
		tasks_mutex.unlock();
		return;
	}
	task = tasks.front(); // 先进先出，双向链表（所以有front、back）
	tasks.pop_front();
	tasks_mutex.unlock();
	task->Init(); // 初始化任务 打印"XFtpServerCMD::Init()"，设置ReadCB和EventCB
}

// 线程激活
void XThread::Activate() {
#ifdef _WIN32
	int re = send(this->notify_send_fd, "C", 1, 0);
#else
	// linux 下是管道，不能用recv，得用read
	int re = write(this->notify_send_fd, "c", 1);
#endif
	if (re <= 0) {
		cerr << "XThread::Activate() failed! " << endl;
	}
}

// 添加处理的任务，一个线程同时可以处理多个任务，共用一个event_base
void XThread::AddTask(XTask *t) {
	if (!t)return;
	t->base = this->base;
	tasks_mutex.lock();
	tasks.push_back(t); // 往task链表中加入链表
	tasks_mutex.unlock();
}

// 启动线程
void XThread::Start() {
	// 安装线程，初始化event_base和管道监听事件用于激活线程NotifyCB
	Setup();
	// 启动线程，设置回调函数
	thread th(&XThread::Main, this);

	// 断开与主线程联系
	th.detach();
}

// 线程入口函数
void XThread::Main() {
	cout << id << "XThread::Main() begin " << endl;
	event_base_dispatch(base); // 事件分发，类似于while()，一直调用多路复用，多个线程都卡在这里分发事件
	event_base_free(base);
	cout << id << "XThread::Main() end " << endl;
}

// 安装线程，初始化event_base和管道监听事件用于激活线程
bool XThread::Setup() {
	// windows用配对socket   linux用管道
#ifdef _WIN32
	// 创建一个socketpair 可以互相通信，  fds[0] 读   fds[1] 写
	evutil_socket_t fds[2];
	if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0) {
		cout << "evutil_socketpair failed!" << endl;
		return false;
	}
	// 将socket设置成非阻塞，提升性能
	evutil_make_socket_nonblocking(fds[0]);
	evutil_make_socket_nonblocking(fds[1]);
#else
	// Linux 下 使用管道
	// 创建的管道 不能用send recv读取  read  write
	int fds[2];
	if (pipe(fds)) {
		cerr << "pipe failed!" << endl;
		return false;
	}
#endif

	// 读取绑定到event事件中，写入要保存
	notify_send_fd = fds[1];

	// 创建libevent上下文（无锁）
	event_config * ev_conf = event_config_new();
	event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
	this->base = event_base_new_with_config(ev_conf); // 根据config配置初始化创建base事件集上下文
	event_config_free(ev_conf);
	if (!base) {
		cerr << "event_base_new_with_config failed in thread" << endl;
		return false;
	}
	// 添加管道监听事件，用于激活线程执行任务
	event *ev = event_new(base, fds[0], EV_READ | EV_PERSIST, NotifyCB, this); // 读 fds[0] 持久事件
	event_add(ev, 0);

	return true;
}

XThread::XThread() {}
XThread::~XThread() {}
