#include "XVideoWidget.h"
#include <QPainter>
#include <opencv2/imgproc.hpp>

using namespace cv;

XVideoWidget::XVideoWidget(QWidget * p) : QOpenGLWidget(p) {}
XVideoWidget::~XVideoWidget() {}

// �ۺ���
void XVideoWidget::SetImage(cv::Mat mat) {
	QImage::Format fmt = QImage::Format_RGB888; // Ĭ����RGB�����ﲻ�����ڻҶ�ͼ�������dump
	int pixSize = 3; // Ĭ����RGB��Ϊ3
	if (mat.type() == CV_8UC1) { // �Ҷ�ͼ��ʽ��8λ1��
		fmt = QImage::Format_Grayscale8;
		pixSize = 1; // �Ҷ�ͼΪ1���������ܣ������˷��ڴ�ռ�
	}
	// ���ڴ�С��ߵ���4�ı���,��Ȼ��Ĭ�϶��룬��ɻ�����б
	if (img.isNull()|| img.format() != fmt ) { // �����任����Ҫ���´���
		delete img.bits();
		uchar * buf = new uchar[width()*height() * pixSize];
		img = QImage(buf, width(), height(), fmt);
	}

	// �ı��С����Ӧ����
	Mat des;
	cv::resize(mat, des, Size(img.size().width(), img.size().height()));
	if(pixSize > 1)
		cv::cvtColor(des, des, COLOR_BGR2RGB); // ��ɫ�ռ�ɫ�ʲ���Ӧ BGR �� RGB 

	// �ڴ��ڴ����û��棬des
	memcpy(img.bits(), des.data, des.cols*des.rows*des.elemSize());
	update(); // ���»�һ֡������ XVideoWidget::paintEvent(QPaintEvent * e)
}


void XVideoWidget::paintEvent(QPaintEvent * e) {
	QPainter p;
	p.begin(this); // ���»���
	p.drawImage(QPoint(0, 0), img);
	p.end();
}

