#include "XVideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "XFilter.h"

using namespace cv;
using namespace std;


// 1����ƵԴ
static VideoCapture cap1;
// 2����ƵԴ
static VideoCapture cap2;

static bool isexit = false;

// ������Ƶ
static VideoWriter vw;


void XVideoThread::SetBegin(double p) {
	mutex.lock(); 
	// �����Ƶ����֡��
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	// ���ݱ����Ѹ�����ת����֡��
	int frame = p * count;
	begin = frame;
	mutex.unlock(); 
}

void XVideoThread::SetEnd(double p) {
	mutex.lock();   
	// �����Ƶ����֡��
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	// ���ݱ����Ѹ�����ת����֡��
	int frame = p * count;
	end = frame;
	mutex.unlock(); 
}

// ����Seek����
bool XVideoThread::Seek(double pos) {
	// �����Ƶ����֡��
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	// ���ݱ����Ѹ�����ת����֡��
	int frame = pos * count;
	return Seek(frame);
}

// ���ý�����
bool XVideoThread::Seek(int frame) {
	mutex.lock();

	if (!cap1.isOpened()) {
		mutex.unlock();
		return false;
	}
	int re = cap1.set(CAP_PROP_POS_FRAMES, frame); // ������Ƶ��ͼ��֡
	if (cap2.isOpened())
		cap2.set(CAP_PROP_POS_FRAMES, frame);

	mutex.unlock();
	return re;
}

// ���ص�ǰ���ŵ�λ��
double XVideoThread::GetPos() {
	double pos = 0;
	mutex.lock();

	// ��Ƶ�������
	if (!cap1.isOpened()) {
		mutex.unlock();
		return pos;
	}

	// �����Ƶ����֡��
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	// ��ȡ��ǰ�Ĳ���λ�ö�Ӧ��֡
	double cur   = cap1.get(CAP_PROP_POS_FRAMES);

	if (count > 0.001) {
		pos = cur / count;
	}

	mutex.unlock();
	return pos;
}

// ��һ����ƵԴ�ļ�
bool XVideoThread::Open(const std::string file) {
	cout << "open:" << file << endl;
	Seek(0); // ���ļ�����������ʼλ��

	mutex.lock(); // �ӻ�����
	bool re = cap1.open(file);
	mutex.unlock(); // �⻥����

	cout << re << endl;
	if(!re) // ����Ƶʧ��
		return re;
	fps    = cap1.get(CAP_PROP_FPS); // �򿪳ɹ���
	width  = cap1.get(CAP_PROP_FRAME_WIDTH); // ��ȡ��Ƶͼ��Ŀ�
	height = cap1.get(CAP_PROP_FRAME_HEIGHT); // ��ȡ��Ƶͼ��ĸ�
	if (fps <= 0) fps = 25;
	src1file = file;
	// �����Ƶ����֡��
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	totalMs = (count / (double)fps) * 1000;
	return true;
}

// �򿪶�����ƵԴ�ļ�
bool XVideoThread::Open2(const std::string file) {
	cout << "open2:" << file << endl;
	Seek(0);

	mutex.lock(); // �ӻ�����
	bool re = cap2.open(file);
	mutex.unlock(); // �⻥����

	cout << re << endl;
	if (!re) // ����Ƶʧ��
		return re;
	/*fps = cap1.get(CAP_PROP_FPS); // �򿪳ɹ���*/
	width2  = cap2.get(CAP_PROP_FRAME_WIDTH);  // ��ȡ��Ƶͼ��Ŀ�
	height2 = cap2.get(CAP_PROP_FRAME_HEIGHT); // ��ȡ��Ƶͼ��ĸ�
	//if (fps <= 0) fps = 25;
	return true;
}

void XVideoThread::run() { // ������run���̳���QThread��
	Mat mat1;
	while (!isexit) { // ���˳������
		mutex.lock();

		if (isexit) {
			mutex.unlock();
			break;
		}
		// �ж���Ƶ�Ƿ��
		if (!cap1.isOpened()) {
			mutex.unlock(); // ���� ���� ʡCPU��Դ
			msleep(5);
			continue;
		}

		if (!isPlay) {
			mutex.unlock(); // ���� ���� ʡCPU��Դ
			msleep(5);
			continue;
		}

		int cur = cap1.get(CAP_PROP_POS_FRAMES);
		// ��ȡһ֡��Ƶ�����벢��ɫת��
		if ((end > 0 && cur >= end) || !cap1.read(mat1) || mat1.empty()) { // ��ʧ��

			mutex.unlock();
			// �������һ֡���Զ����ý�������������βλ��
			if (isWrite) {
				StopSave();
				SaveEnd();
			}

			msleep(5);
			continue;
		}

		// ͨ��������������Ƶ
		Mat mat2 = mark;
		if (cap2.isOpened()) {
			cap2.read(mat2);
		}

		// ��ʾͼ�� ֮ǰ��ʹ��OpenCV��imshow��ʾ������ѡ��QT���źŲۣ��۲���ģʽ��������
		if (!isWrite) {
			ViewImage1(mat1);
			if(!mat2.empty())
				ViewImage2(mat2);
		}//  һ����ʼ���������Ȳ�ˢ����Ƶ�ˡ���д��ʱˢ��
			

		Mat des = XFilter::Get()->Filter(mat1, mat2);

		// ��ʾĿ�����ɺ��ͼ��
		if(!isWrite)
			ViewDes(des);

		// �������fps��������Ƶ
		int s = 0;
		s = 1000 / fps;
		// ����д��
		if (isWrite) {
			s = 1; // �����д��Ƶʱ
			vw.write(des);
		}

		msleep(s); // fps��������Ƶ

		mutex.unlock();

	}
}

// ����start, ����run�߳�
XVideoThread::XVideoThread() { start(); }

XVideoThread::~XVideoThread() {
	mutex.lock();
	isexit = true; // �����˳���־ ��Ϊ�˹ر���Ƶ���ȡ��Ƶ���ڽ��У�����߳�dump��
	mutex.unlock();
	// ������Ҫ�ȴ�����Ȼ�ֻ��߳�dump��
	wait();
}

// ��ʼ������Ƶ
bool XVideoThread::StartSave(const std::string filename, int width , int height, bool isColor) {
	cout << "��ʼ����" << endl;
	// ����������ʼ
	Seek(begin);
	mutex.lock();

	if (!cap1.isOpened())
	{
		mutex.unlock();
		return false;
	}
	if (width  <= 0 ) width  = cap1.get(CAP_PROP_FRAME_WIDTH);
	if (height <= 0 ) height = cap1.get(CAP_PROP_FRAME_HEIGHT);

	
	vw.open(filename,
		VideoWriter::fourcc('X', '2', '6', '4'),
		this->fps,
		Size(width, height),
		isColor
	);

	if (!vw.isOpened()) {// ���̱߳����һ��׼��return����ʱ��
		mutex.unlock();  // ��Ҫ������û��ʲô��Դ��Ҫ�ͷţ����������mutex
		cout << "start save failed!" << endl;
		return false; 
	}

	this->isWrite = true;
	desFile = filename;

	mutex.unlock();
	return true;
}

// ֹͣ������Ƶ��д����Ƶ֡������
void XVideoThread::StopSave() {
	cout << "ֹͣ����" << endl;
	mutex.lock();
	vw.release(); // �ͷ�VideoWriter
	isWrite = false;
	mutex.unlock();
}