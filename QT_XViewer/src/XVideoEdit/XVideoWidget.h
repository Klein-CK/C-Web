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
	// �ۺ���������Ҫʵ�ֵģ����źű���Ҫ�������źŷ������ۺ������յ�����
public slots:
	void SetImage(cv::Mat mat);
protected:
	QImage img;
};

