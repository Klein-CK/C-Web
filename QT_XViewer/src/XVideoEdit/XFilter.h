#pragma once
#include <opencv2/core.hpp>
#include <vector>

enum XTaskType {
	XTASK_NONE,
	XTASK_GAIN, // ����  �Աȶȵ���
	XTASK_ROTATE90, // ͼ����ת
	XTASK_ROTATE180,
	XTASK_ROTATE270,
	XTASK_FLIPX, // ͼ����
	XTASK_FLIPY,
	XTASK_FLIPXY,
	XTASK_RESIZE, // �ı�ߴ�
	XTASK_PYDOWN, // ��˹������
	XTASK_PYUP,   // ������˹������
	XTASK_CLIP,		// ͼ��ü�
	XTASK_GRAY,		// ת�ɻҶ�ͼ��
	XTASK_MARK,		// ���ˮӡ
	XTASK_BLEND,	// ͼ���ں�
	XTASK_MERGE,	// ͼ��ϲ�
};

struct XTask {
	XTaskType type; // ��������
	std::vector<double> para; // ��������Ӧ�Ĳ���
};

class XFilter
{
public:
	virtual cv::Mat Filter(cv::Mat mat1, cv::Mat mat2) = 0;
	virtual void Add(XTask task) = 0; // ���һ����������Ϊ���麯�����ӿڣ���ʵ��
	virtual void Clear() = 0;	// ����һ������

	// ����ģʽ
	static XFilter * Get();

	virtual ~XFilter(); // ����Ϊ��������������Ȼ������ڴ��и�

protected:
	XFilter();
};

