#include "XVideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "XFilter.h"

using namespace cv;
using namespace std;


// 1号视频源
static VideoCapture cap1;
// 2号视频源
static VideoCapture cap2;

static bool isexit = false;

// 保存视频
static VideoWriter vw;


void XVideoThread::SetBegin(double p) {
	mutex.lock(); 
	// 获得视频的总帧数
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	// 根据比例把浮点数转换成帧数
	int frame = p * count;
	begin = frame;
	mutex.unlock(); 
}

void XVideoThread::SetEnd(double p) {
	mutex.lock();   
	// 获得视频的总帧数
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	// 根据比例把浮点数转换成帧数
	int frame = p * count;
	end = frame;
	mutex.unlock(); 
}

// 重载Seek函数
bool XVideoThread::Seek(double pos) {
	// 获得视频的总帧数
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	// 根据比例把浮点数转换成帧数
	int frame = pos * count;
	return Seek(frame);
}

// 设置进度条
bool XVideoThread::Seek(int frame) {
	mutex.lock();

	if (!cap1.isOpened()) {
		mutex.unlock();
		return false;
	}
	int re = cap1.set(CAP_PROP_POS_FRAMES, frame); // 设置视频的图像帧
	if (cap2.isOpened())
		cap2.set(CAP_PROP_POS_FRAMES, frame);

	mutex.unlock();
	return re;
}

// 返回当前播放的位置
double XVideoThread::GetPos() {
	double pos = 0;
	mutex.lock();

	// 视频打开情况下
	if (!cap1.isOpened()) {
		mutex.unlock();
		return pos;
	}

	// 获得视频的总帧数
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	// 获取当前的播放位置对应的帧
	double cur   = cap1.get(CAP_PROP_POS_FRAMES);

	if (count > 0.001) {
		pos = cur / count;
	}

	mutex.unlock();
	return pos;
}

// 打开一号视频源文件
bool XVideoThread::Open(const std::string file) {
	cout << "open:" << file << endl;
	Seek(0); // 打开文件后锁定到初始位置

	mutex.lock(); // 加互斥锁
	bool re = cap1.open(file);
	mutex.unlock(); // 解互斥锁

	cout << re << endl;
	if(!re) // 打开视频失败
		return re;
	fps    = cap1.get(CAP_PROP_FPS); // 打开成功后
	width  = cap1.get(CAP_PROP_FRAME_WIDTH); // 获取视频图像的宽
	height = cap1.get(CAP_PROP_FRAME_HEIGHT); // 获取视频图像的高
	if (fps <= 0) fps = 25;
	src1file = file;
	// 获得视频的总帧数
	double count = cap1.get(CAP_PROP_FRAME_COUNT);
	totalMs = (count / (double)fps) * 1000;
	return true;
}

// 打开二号视频源文件
bool XVideoThread::Open2(const std::string file) {
	cout << "open2:" << file << endl;
	Seek(0);

	mutex.lock(); // 加互斥锁
	bool re = cap2.open(file);
	mutex.unlock(); // 解互斥锁

	cout << re << endl;
	if (!re) // 打开视频失败
		return re;
	/*fps = cap1.get(CAP_PROP_FPS); // 打开成功后*/
	width2  = cap2.get(CAP_PROP_FRAME_WIDTH);  // 获取视频图像的宽
	height2 = cap2.get(CAP_PROP_FRAME_HEIGHT); // 获取视频图像的高
	//if (fps <= 0) fps = 25;
	return true;
}

void XVideoThread::run() { // 重载了run，继承自QThread类
	Mat mat1;
	while (!isexit) { // 不退出情况下
		mutex.lock();

		if (isexit) {
			mutex.unlock();
			break;
		}
		// 判断视频是否打开
		if (!cap1.isOpened()) {
			mutex.unlock(); // 进晚， 退晚， 省CPU资源
			msleep(5);
			continue;
		}

		if (!isPlay) {
			mutex.unlock(); // 进晚， 退晚， 省CPU资源
			msleep(5);
			continue;
		}

		int cur = cap1.get(CAP_PROP_POS_FRAMES);
		// 读取一帧视频，解码并颜色转换
		if ((end > 0 && cur >= end) || !cap1.read(mat1) || mat1.empty()) { // 读失败

			mutex.unlock();
			// 读到最后一帧，自动调用结束。导出到结尾位置
			if (isWrite) {
				StopSave();
				SaveEnd();
			}

			msleep(5);
			continue;
		}

		// 通过过滤器处理视频
		Mat mat2 = mark;
		if (cap2.isOpened()) {
			cap2.read(mat2);
		}

		// 显示图像， 之前是使用OpenCV的imshow显示，现在选用QT的信号槽（观察者模式）来调用
		if (!isWrite) {
			ViewImage1(mat1);
			if(!mat2.empty())
				ViewImage2(mat2);
		}//  一旦开始导出，就先不刷新视频了。不写入时刷新
			

		Mat des = XFilter::Get()->Filter(mat1, mat2);

		// 显示目标生成后的图像
		if(!isWrite)
			ViewDes(des);

		// 这里根据fps来播放视频
		int s = 0;
		s = 1000 / fps;
		// 导出写入
		if (isWrite) {
			s = 1; // 如果在写视频时
			vw.write(des);
		}

		msleep(s); // fps来播放视频

		mutex.unlock();

	}
}

// 重载start, 启动run线程
XVideoThread::XVideoThread() { start(); }

XVideoThread::~XVideoThread() {
	mutex.lock();
	isexit = true; // 这里退出标志 是为了关闭视频后读取视频仍在进行，造成线程dump掉
	mutex.unlock();
	// 这里需要等待，不然又会线程dump掉
	wait();
}

// 开始保存视频
bool XVideoThread::StartSave(const std::string filename, int width , int height, bool isColor) {
	cout << "开始导出" << endl;
	// 进度条到开始
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

	if (!vw.isOpened()) {// 多线程编程下一旦准备return返回时，
		mutex.unlock();  // 需要想想有没有什么资源需要释放，比如这里的mutex
		cout << "start save failed!" << endl;
		return false; 
	}

	this->isWrite = true;
	desFile = filename;

	mutex.unlock();
	return true;
}

// 停止保存视频，写入视频帧的索引
void XVideoThread::StopSave() {
	cout << "停止导出" << endl;
	mutex.lock();
	vw.release(); // 释放VideoWriter
	isWrite = false;
	mutex.unlock();
}