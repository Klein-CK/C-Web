#include "XImagePro.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// �����ռ�һ����cpp��ʹ�ã���hpp��ֱ��ǰ���  �ռ���:: ,�Է���ͻ
using namespace cv;

XImagePro::XImagePro() {}
XImagePro::~XImagePro() {}

// ����ԭͼ������������
void XImagePro::Set(cv::Mat mat1, cv::Mat mat2) {
	if (mat1.empty())return;
	this->src1 = mat1;
	this->src2 = mat2;
	this->src1.copyTo(des); // �ȸ���һ�ݵ�des
}

// �������ȺͶԱȶ�
// @para bright double ���� 0~100
// @para contrast int �Աȶ� 1.0~3.0
void XImagePro::Gain(double bright, double contrast) {
	if (des.empty()) return;
	des.convertTo(des, -1, contrast, bright);// -1��ʾת�������ת��ǰ�����ݸ�ʽһ��
}

void XImagePro::Rotate90() {
	if (des.empty())return;
	rotate(des, des, ROTATE_90_CLOCKWISE);
}

void XImagePro::Rotate180() {
	if (des.empty())return;
	rotate(des, des, ROTATE_180);
}

void XImagePro::Rotate270() {
	if (des.empty())return;
	rotate(des, des, ROTATE_90_COUNTERCLOCKWISE); // ��ʱ����ת90��
}

// ���Ҿ���
void XImagePro::FlipX() {
	if (des.empty())return;
	flip(des, des, 0);
}
// ���¾���
void XImagePro::FlipY() {
	if (des.empty())return;
	flip(des, des, 1);
}
// �������Ҿ���
void XImagePro::FlipXY() {
	if (des.empty())return;
	flip(des, des, -1); // ����Ϊ1��ʾ����������ת
}

// ͼ��ߴ�
void XImagePro::Resize(int width, int height) {
	if (des.empty())return;
	resize(des, des, Size(width, height));
}

// ͼ�������
void XImagePro::PyDown(int count) {
	if (des.empty())return;
	for (int i = 0; i < count; i++) {
		pyrDown(des, des); // ��˹����������
	}
}
void XImagePro::PyUp(int count) {
	if (des.empty())return;
	for (int i = 0; i < count; i++) {
		pyrUp(des, des); // ������˹����������
	}
}

// ��Ƶ����ü�
void XImagePro::Clip(int x, int y, int w, int h) {
	if (des.empty())return;
	if (x < 0 || y < 0 || w <= 0 || h <= 0)return; // x y ���Ե���0
	if (x > des.cols || y > des.rows)return;
	des = des(Rect(x, y, w, h));

}

// תΪ�Ҷ�ͼ
void XImagePro::Gray() {
	if (des.empty())return;
	cvtColor(des, des, COLOR_BGR2GRAY);
}

// ˮӡ��aΪ͸����
void XImagePro::Mark(int x, int y, double a) {
	if (des.empty())return;
	if (src2.empty())return;
	Mat rol = des(Rect(x, y, src2.cols, src2.rows));
	addWeighted(src2, a, rol, 1 - a, 0, rol);
}

// �ں�  ͸���� (���õ�һ����Ƶ��͸����)
void XImagePro::Blending(double a) {
	if (des.empty())return;
	if (src2.empty())return;
	if (src2.size() != des.size()) { // ������Ƶ֡��С
		resize(src2, src2, des.size());
	}
	addWeighted(src2, a, des, 1 - a, 0, des); // �ںϣ�+ˮӡ
}

// �ϲ�
void XImagePro::Merge() {
	if (des.empty())return;
	if (src2.empty())return;
	if (src2.size() != des.size()) { // ������Ƶ֡��С
		int w = src2.cols * ((double)src2.rows / (double)des.rows);
		resize(src2, src2, Size(w, des.rows)); // ���һ��
	}

	int dw = des.cols + src2.cols;
	int dh = des.rows;
	des = Mat(Size(dw, dh), src1.type());
	Mat r1 = des(Rect(0, 0, src1.cols, dh));
	Mat r2 = des(Rect(src1.cols, 0, src2.cols, dh));
	src1.copyTo(r1);
	src2.copyTo(r2);
}