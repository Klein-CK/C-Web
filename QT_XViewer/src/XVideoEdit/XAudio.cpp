#include "XAudio.h"
#include <iostream>
#include "qmutex.h"
using namespace std;

struct XTime {
	XTime(int tms) {
		h = (tms / 1000) / 3600; // 毫秒/1000为秒，再转小时
		m = ((tms / 1000) % 3600) / 60; // 分钟
		s = (tms / 1000) % (3600 * 60);
		ms = tms % 1000;
	}
	std::string ToString() {
		char buf[16] = { 0 };
		sprintf(buf, "%d:%d:%d.%d", h, m, s, ms); // 把各种类型转换成字符串
		return buf;
	}
	int h = 0;
	int m = 0;
	int s = 0;
	int ms = 0;

};

class CXAudio :public XAudio {
public:
	QMutex mutex;
	// 导出音频文件
	// @para src string 源文件
	// @para out string 输出的音频文件(MP3)
	bool ExportA(std::string src, std::string out, int beginMs, int outMs) {
		// ffmpeg -i test.mp4 -ss 0:1:10.112 -t 0:1:10.100 -vn -y test.mp3
		string cmd = "ffmpeg.exe";
		cmd += " -i ";
		cmd += src;
		cmd += " ";
		if (beginMs > 0) {
			cmd += " -ss ";
			XTime xt(beginMs);
			cmd += xt.ToString();
		}
		if (outMs > 0) {
			cmd += " -t ";
			XTime xt(outMs);
			cmd += xt.ToString();
		} 
		cmd += " -vn -y ";
		cmd += out;
		cout << cmd << endl;
		mutex.lock();
		system(cmd.c_str());
		mutex.unlock();
		return true;
	}

	// 合并音视频
	// @para v string 视频文件(avi)
	// @para a string 音频文件(MP3)
	// @para out string 输出件(avi)
	bool Merge(std::string v, std::string a, std::string out) {
		// ffmpeg -i test.avi -i test.mp3 -c copy out.avi
		string cmd = "ffmpeg.exe";
		cmd += " -i ";
		cmd += v;
		cmd += " -i ";
		cmd += a;
		cmd += " -c copy ";
		cmd += out;
		cout << cmd << endl;
		mutex.lock();
		system(cmd.c_str());
		mutex.unlock();
		return true;
	}
};



XAudio * XAudio::Get() {
	static CXAudio ca;
	return &ca;
}

XAudio::XAudio() {}
XAudio::~XAudio() {}
