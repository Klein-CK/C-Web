#pragma once
#include <opencv2/core.hpp>

class XImagePro
{
public:
	XImagePro();
	~XImagePro();
	// ����ԭͼ���������������
	void Set(cv::Mat mat1, cv::Mat mat2);

	// ��ȡ��������
	cv::Mat Get() { return des; }

	// �������ȺͶԱȶ�
	// @para bright double ���� 0~100
	// @para contrast int �Աȶ� 1.0~3.0
	void Gain(double bright, double contrast);

	// ͼ����ת
	void Rotate90();
	void Rotate180();
	void Rotate270();

	// ����
	void FlipX();
	void FlipY();
	void FlipXY();

	// ͼ��ߴ�
	void Resize(int width, int height);

	// ͼ�������
	void PyDown(int count);
	void PyUp(int count);

	// ��Ƶ����ü�
	void Clip(int x, int y, int w, int h);

	// תΪ�Ҷ�ͼ
	void Gray();

	// ˮӡ
	void Mark(int x, int y, double a);

	// �ں�
	void Blending(double a); // ͸����

	// �ϲ�
	void Merge();

private:
	// ԭͼ
	cv::Mat src1, src2;

	// Ŀ��ͼ������ԭͼ���ɱ任���ͼ
	cv::Mat des;
};
