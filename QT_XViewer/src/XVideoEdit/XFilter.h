#pragma once
#include <opencv2/core.hpp>
#include <vector>

enum XTaskType {
	XTASK_NONE,
	XTASK_GAIN, // 亮度  对比度调整
	XTASK_ROTATE90, // 图像旋转
	XTASK_ROTATE180,
	XTASK_ROTATE270,
	XTASK_FLIPX, // 图像镜像
	XTASK_FLIPY,
	XTASK_FLIPXY,
	XTASK_RESIZE, // 改变尺寸
	XTASK_PYDOWN, // 高斯金字塔
	XTASK_PYUP,   // 拉普拉斯金字塔
	XTASK_CLIP,		// 图像裁剪
	XTASK_GRAY,		// 转成灰度图像
	XTASK_MARK,		// 添加水印
	XTASK_BLEND,	// 图像融合
	XTASK_MERGE,	// 图像合并
};

struct XTask {
	XTaskType type; // 任务种类
	std::vector<double> para; // 任务所对应的参数
};

class XFilter
{
public:
	virtual cv::Mat Filter(cv::Mat mat1, cv::Mat mat2) = 0;
	virtual void Add(XTask task) = 0; // 添加一个任务，设置为纯虚函数，接口，不实现
	virtual void Clear() = 0;	// 清理一个任务

	// 单例模式
	static XFilter * Get();

	virtual ~XFilter(); // 设置为虚析构函数，不然后不造成内存切割

protected:
	XFilter();
};

