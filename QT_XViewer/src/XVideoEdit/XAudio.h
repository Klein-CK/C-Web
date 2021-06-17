#pragma once
#include <string>
class XAudio
{
public:
	XAudio();
	virtual ~XAudio();

	static XAudio * Get();
	// 导出音频文件
	// @para src string 源文件
	// @para out string 输出的音频文件(MP3)
	// @para beginMs int 开始音频位置毫秒
	// @para outMs int 开始音频时长
	virtual bool ExportA(std::string src, std::string out, int beginMs = 0, int outMs = 0) = 0;
	// 合并音视频
	// @para v string 视频文件(avi)
	// @para a string 音频文件(MP3)
	// @para out string 输出件(avi)
	virtual bool Merge(std::string v, std::string a, std::string out) = 0;
};

