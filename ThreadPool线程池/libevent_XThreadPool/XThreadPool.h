#pragma once
#include <vector>
// ͷ�ļ���Ҳ������ʹ�������ռ�
class XThread; // ����һ���������ͣ�������������ͷ�ļ����ô����Ǿ�����ͷ�ļ����������ļ�
class XTask;
// ����ģʽ��Ψһһ��
class XThreadPool
{
public:
	static XThreadPool * Get() {
		static XThreadPool p;
		return &p;
	}
	
	// �̳߳��߳���������ʼ�������̲߳������߳�
	void Init(int threadCount);

	// ����ַ��߳�
	void Dispatch(XTask * task);
private:
	// �߳�����
	int threadCount = 0;
	int lastThread = -1;
	// �̳߳��߳�
	std::vector<XThread *> threads;
	// ����ģʽ�����캯����Ϊ˽�е�
	XThreadPool() { };
	// ��д������Ĭ��ΪĬ�Ϲ��캯������������
};

