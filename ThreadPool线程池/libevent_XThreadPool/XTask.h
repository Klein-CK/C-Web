#pragma once
class XTask
{
public:
	struct event_base * base = 0; // event_base �¼���
	int sock = 0; // socket �ļ�������
	int thread_id = 0; // �߳� ���
	// ��ʼ������
	virtual bool Init() = 0; // ����Ϊ���麯�����������ӿڲ�ʵ�֣����޷�ʵ����
};

