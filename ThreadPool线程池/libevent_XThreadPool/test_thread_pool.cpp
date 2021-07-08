#include <event2/event.h>
#include <event2/listener.h>
#include <iostream>
#include "XThreadPool.h"
#include "XFtpServerCMD.h"
#include "XTask.h"
#ifndef _WIN32
#include <signal.h>
#include <string.h>
#endif
using namespace std;

const int SPORT = 5001;

void listen_cb(struct evconnlistener * e, evutil_socket_t s, struct sockaddr * a, int socklen, void * arg)
{
	cout << "listen_cb" << endl; // LEV_OPT_REUSEABLE，监听到新的客户端连接
	XTask * task = new XFtpServerCMD(); // XFtpServerCMD 继承 XTask
	task->sock = s; // socket fd 文件描述符
	XThreadPool::Get()->Dispatch(task); // 来一个任务就分发任务
};

int main()
{
#ifdef _WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#else // _WIN32
	// 忽略管道信号，发送数据给已关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return 1;
#endif
    //1. 初始化线程池
	XThreadPool::Get()->Init(10); // 打印，并把10个线程构建好

	std::cout << "test thread pool!\n" << endl;
	event_base * base = event_base_new();
	if (base) {
		cout << "event_base_new success!" << endl;
	}
	// 监听端口
	// socket 创建 bind, listen
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET; // IPv4协议
	sin.sin_port = htons(SPORT); // 转换字节序，变成网络端字节序大端


	evconnlistener * ev = evconnlistener_new_bind(base,  //libevent的上下文
		listen_cb,				//接收到连接的回调函数
		base,					//回调函数获取的参数arg
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, // 地址重用， evconnlistener关闭同时关闭socket
		10,						// 连接队列大小，对应listen函数
		(sockaddr*)&sin,        // 绑定的地址和端口
		sizeof(sin)
		);
	
	// 事件分发处理
	if(base)
		event_base_dispatch(base);
	if(base)
		event_base_free(base);
	if (ev)
		evconnlistener_free(ev);

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}


