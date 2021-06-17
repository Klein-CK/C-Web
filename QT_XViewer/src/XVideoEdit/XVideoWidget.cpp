#include "XVideoWidget.h"
#include <QPainter>
#include <opencv2/imgproc.hpp>

using namespace cv;

XVideoWidget::XVideoWidget(QWidget * p) : QOpenGLWidget(p) {}
XVideoWidget::~XVideoWidget() {}

// 槽函数
void XVideoWidget::SetImage(cv::Mat mat) {
	QImage::Format fmt = QImage::Format_RGB888; // 默认是RGB，这里不适用于灰度图，造成了dump
	int pixSize = 3; // 默认是RGB，为3
	if (mat.type() == CV_8UC1) { // 灰度图格式，8位1个
		fmt = QImage::Format_Grayscale8;
		pixSize = 1; // 灰度图为1，提升性能，不能浪费内存空间
	}
	// 窗口大小宽高得是4的倍数,不然会默认对齐，造成画面倾斜
	if (img.isNull()|| img.format() != fmt ) { // 经过变换后，需要重新处理
		delete img.bits();
		uchar * buf = new uchar[width()*height() * pixSize];
		img = QImage(buf, width(), height(), fmt);
	}

	// 改变大小，适应窗口
	Mat des;
	cv::resize(mat, des, Size(img.size().width(), img.size().height()));
	if(pixSize > 1)
		cv::cvtColor(des, des, COLOR_BGR2RGB); // 颜色空间色彩不对应 BGR 和 RGB 

	// 在窗口处设置画面，des
	memcpy(img.bits(), des.data, des.cols*des.rows*des.elemSize());
	update(); // 更新画一帧，调用 XVideoWidget::paintEvent(QPaintEvent * e)
}


void XVideoWidget::paintEvent(QPaintEvent * e) {
	QPainter p;
	p.begin(this); // 更新画面
	p.drawImage(QPoint(0, 0), img);
	p.end();
}

