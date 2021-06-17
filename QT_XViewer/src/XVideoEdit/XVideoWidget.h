#pragma once
#include <QOpenGLWidget>
#include <opencv2/core.hpp>

class XVideoWidget : public QOpenGLWidget
{
	Q_OBJECT
public:
	XVideoWidget(QWidget * p);	
	virtual ~XVideoWidget();

	void paintEvent(QPaintEvent * e);
	// 槽函数，是需要实现的，而信号必须要声明。信号发出，槽函数接收到调用
public slots:
	void SetImage(cv::Mat mat);
protected:
	QImage img;
};

