#include "XImagePro.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// 命名空间一般在cpp里使用，在hpp里直接前面加  空间名:: ,以防冲突
using namespace cv;

XImagePro::XImagePro() {}
XImagePro::~XImagePro() {}

// 设置原图，会清理处理结果
void XImagePro::Set(cv::Mat mat1, cv::Mat mat2) {
	if (mat1.empty())return;
	this->src1 = mat1;
	this->src2 = mat2;
	this->src1.copyTo(des); // 先复制一份到des
}

// 设置亮度和对比度
// @para bright double 亮度 0~100
// @para contrast int 对比度 1.0~3.0
void XImagePro::Gain(double bright, double contrast) {
	if (des.empty()) return;
	des.convertTo(des, -1, contrast, bright);// -1表示转换后的与转换前的数据格式一致
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
	rotate(des, des, ROTATE_90_COUNTERCLOCKWISE); // 逆时钟旋转90度
}

// 左右镜像
void XImagePro::FlipX() {
	if (des.empty())return;
	flip(des, des, 0);
}
// 上下镜像
void XImagePro::FlipY() {
	if (des.empty())return;
	flip(des, des, 1);
}
// 上下左右镜像
void XImagePro::FlipXY() {
	if (des.empty())return;
	flip(des, des, -1); // 参数为1表示上下左右旋转
}

// 图像尺寸
void XImagePro::Resize(int width, int height) {
	if (des.empty())return;
	resize(des, des, Size(width, height));
}

// 图像金字塔
void XImagePro::PyDown(int count) {
	if (des.empty())return;
	for (int i = 0; i < count; i++) {
		pyrDown(des, des); // 高斯金字塔次数
	}
}
void XImagePro::PyUp(int count) {
	if (des.empty())return;
	for (int i = 0; i < count; i++) {
		pyrUp(des, des); // 拉普拉斯金字塔次数
	}
}

// 视频画面裁剪
void XImagePro::Clip(int x, int y, int w, int h) {
	if (des.empty())return;
	if (x < 0 || y < 0 || w <= 0 || h <= 0)return; // x y 可以等于0
	if (x > des.cols || y > des.rows)return;
	des = des(Rect(x, y, w, h));

}

// 转为灰度图
void XImagePro::Gray() {
	if (des.empty())return;
	cvtColor(des, des, COLOR_BGR2GRAY);
}

// 水印，a为透明度
void XImagePro::Mark(int x, int y, double a) {
	if (des.empty())return;
	if (src2.empty())return;
	Mat rol = des(Rect(x, y, src2.cols, src2.rows));
	addWeighted(src2, a, rol, 1 - a, 0, rol);
}

// 融合  透明度 (设置第一个视频的透明度)
void XImagePro::Blending(double a) {
	if (des.empty())return;
	if (src2.empty())return;
	if (src2.size() != des.size()) { // 调整视频帧大小
		resize(src2, src2, des.size());
	}
	addWeighted(src2, a, des, 1 - a, 0, des); // 融合，+水印
}

// 合并
void XImagePro::Merge() {
	if (des.empty())return;
	if (src2.empty())return;
	if (src2.size() != des.size()) { // 调整视频帧大小
		int w = src2.cols * ((double)src2.rows / (double)des.rows);
		resize(src2, src2, Size(w, des.rows)); // 宽度一致
	}

	int dw = des.cols + src2.cols;
	int dh = des.rows;
	des = Mat(Size(dw, dh), src1.type());
	Mat r1 = des(Rect(0, 0, src1.cols, dh));
	Mat r2 = des(Rect(src1.cols, 0, src2.cols, dh));
	src1.copyTo(r1);
	src2.copyTo(r2);
}