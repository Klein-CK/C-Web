#include "XVideoUI.h"
#include <QtWidgets/QApplication>
#include "XAudio.h"



int main(int argc, char *argv[])
{
	//XAudio::Get()->ExportA("sea_waves.mp4", "sea_waves.mp3");
	//XAudio::Get()->Merge("sea_waves.avi", "sea_waves.mp3", "out.avi");

    QApplication a(argc, argv);
    XVideoUI w;
    w.show();
    return a.exec();
}
