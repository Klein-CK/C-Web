#pragma once
#include <vector>
// 头文件中也尽量不使用命名空间
class XThread; // 声明一下它的类型，而不是引用其头文件，好处就是尽量在头文件中少引用文件
class XTask;
// 单例模式，唯一一个
class XThreadPool
{
public:
	static XThreadPool * Get() {
		static XThreadPool p;
		return &p;
	}
	
	// 线程池线程数量，初始化所有线程并启动线程
	void Init(int threadCount);

	// 负责分发线程
	void Dispatch(XTask * task);
private:
	// 线程数量
	int threadCount = 0;
	int lastThread = -1;
	// 线程池线程
	std::vector<XThread *> threads;
	// 单例模式，构造函数设为私有的
	XThreadPool() { };
	// 不写析构，默认为默认构造函数，内联即可
};

