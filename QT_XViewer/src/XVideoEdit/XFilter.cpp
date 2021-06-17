#include "XFilter.h"
#include "qmutex.h"
#include "XImagePro.h"

class CXFilter :public XFilter {
public:
	std::vector<XTask> tasks;
	QMutex mutex;

	// 核心处理代码
	cv::Mat Filter(cv::Mat mat1, cv::Mat mat2) {
		mutex.lock(); // 互斥，进入临界区
		
		XImagePro p;
		p.Set(mat1, mat2); // 设置原图，会清理处理结果
		for (int i = 0; i < tasks.size(); i++) {
			switch (tasks[i].type) // XTaskType type; 任务种类
			{
			case XTASK_GAIN:
				// 亮度对比度调整
				p.Gain(tasks[i].para[0], tasks[i].para[1]);
				break;
			case XTASK_ROTATE90:
				p.Rotate90();
				break;
			case XTASK_ROTATE180:
				p.Rotate180();
				break;
			case XTASK_ROTATE270:
				p.Rotate270();
				break;

			case XTASK_FLIPX:
				p.FlipX();
				break;
			case XTASK_FLIPY:
				p.FlipY();
				break;
			case XTASK_FLIPXY:
				p.FlipXY();
				break;
			case XTASK_RESIZE:
				p.Resize(tasks[i].para[0], tasks[i].para[1]);
				break;

			case XTASK_PYDOWN:
				p.PyDown(tasks[i].para[0]);
				break;
			case XTASK_PYUP:
				p.PyUp(tasks[i].para[0]);
				break;
			case XTASK_CLIP:
				p.Clip(tasks[i].para[0], tasks[i].para[1], tasks[i].para[2], tasks[i].para[3]);
				break;
			case XTASK_GRAY:
				p.Gray();
				break;
			case XTASK_MARK:
				p.Mark(tasks[i].para[0], tasks[i].para[1], tasks[i].para[2]);
				break;
			case XTASK_BLEND:
				p.Blending(tasks[i].para[0]);
				break;
			case XTASK_MERGE:
				p.Merge();
				break;
			default:
				break;
			}
		}
		cv::Mat re = p.Get();

		mutex.unlock();

		return re;
	}

	// 添加一个任务，（设置为纯虚函数，接口，不实现），这里具体实现
	void Add(XTask task) {
		mutex.lock();
		tasks.push_back(task);
		mutex.unlock();
	}

	// 清理所有任务
	void Clear() {
		mutex.lock();
		tasks.clear();
		mutex.unlock();
	}
};


XFilter * XFilter::Get() {
	static CXFilter cx; // 继承类必须把 基类的纯虚函数方法实现, 否则报错
	return &cx;
}


XFilter::XFilter() {}

XFilter::~XFilter() {}