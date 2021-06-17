#pragma once
#include <QThread>
#include "opencv2/core.hpp" // ��""��Ϊ��Linux������ֲ
#include "qmutex.h"

class XVideoThread : public QThread
{
	Q_OBJECT     // QT�źŲ�Ҫ��Ч�Ļ�����Ҫ�����

public:
	int fps = 0; // ֡��
	int width = 0;
	int height = 0;
	int width2 = 0;
	int height2 = 0;

	int totalMs = 0;

	std::string src1file;
	std::string desFile;

	int begin = 0; // ��Ƶ������ʼλ��
	int end = 0; // ����λ��

	void SetBegin(double p);
	void SetEnd(double p);

	// ����ģʽ����֤��Ψһ��
	static XVideoThread * Get() {
		static XVideoThread vt;
		return &vt;
	}

	// ��һ����ƵԴ�ļ�
	bool Open(const std::string file);	

	// �򿪶�����ƵԴ�ļ�
	bool Open2(const std::string file);

	void Play()  { mutex.lock(); isPlay = true;  mutex.unlock();}
	void Pause() { mutex.lock(); isPlay = false; mutex.unlock();}
	~XVideoThread();

	/*******���ӽ�����********/
	// ���ص�ǰ���ŵ�λ��
	double GetPos();

	// ��ת��Ƶ
	// @para frame int ֡λ��
	bool Seek(int frame);
	bool Seek(double pos);

	// ��ʼ������Ƶ
	bool StartSave(const std::string filename, int width = 0, int height = 0, bool isColor = true);

	// ֹͣ������Ƶ��д����Ƶ֡������
	void StopSave();

	// ����ˮӡ
	void SetMark(cv::Mat m){ mutex.lock(); this->mark = m; mutex.unlock(); }

	// �߳���ں���
	void run();

	// �źź���
signals:
	// ��ʾԭ��Ƶ1ͼ��
	void ViewImage1(cv::Mat mat);
	// ��ʾԭ��Ƶ2ͼ��
	void ViewImage2(cv::Mat mat);

	// ��ʾ���ɺ�ͼ��
	void ViewDes(cv::Mat mat);
	void SaveEnd();

protected:
	QMutex  mutex;
	// �Ƿ�ʼд��Ƶ
	bool isWrite = false;
	bool isPlay  = false;
	cv::Mat mark;
	XVideoThread();
};
