#pragma once
class XTask
{
public:
	struct event_base * base = 0; // event_base 事件集
	int sock = 0; // socket 文件描述符
	int thread_id = 0; // 线程 编号
	// 初始化任务
	virtual bool Init() = 0; // 定义为纯虚函数，如果子类接口不实现，则无法实例化
};

