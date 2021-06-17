#pragma once

#include <QtWidgets/QWidget>
#include "ui_XVideoUI.h"

class XVideoUI : public QWidget
{
    Q_OBJECT

public:
    XVideoUI(QWidget *parent = Q_NULLPTR);

	// 定时器
	void timerEvent(QTimerEvent * e);

	// 槽函数声明
public slots:
	// 打开视频源文件
	void Open();
	void Play();
	void Pause();

	// 进度条按住 和 释放
	void SliderPress();
	void SliderRelease();

	// 滑动条拖动，设置进度条
	void SetPos(int);
	void Left(int pos);
	void Right(int pos);

	// 设置过滤器
	void Set();

	// 导出视频
	void Export();

	// 导出结束
	void ExportEnd();

	// 添加水印
	void Mark();

	// 融合
	void Blend();

	// 合并
	void Merge();

private:
    Ui::XVideoUIClass ui;
};
