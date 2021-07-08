#include "XFtpServerCMD.h"
#include <iostream>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <string.h>
using namespace std;

// ���߳�XThread event �¼��ַ�
static void ReadCB(bufferevent *bev, void *ctx) {
	XFtpServerCMD * cmd = (XFtpServerCMD *)ctx;
	char data[1024] = { 0 };
	for (;;) {
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0)break;
		data[len] = '\0';
		cout << data << flush; // ��ӡ�ӿͻ����յ�����Ϣ������

		// ���Դ��룬Ҫ�����
		if (strstr(data, "quit")) {
			bufferevent_free(bev);
			delete cmd;
			break;
		}
	}
}

static void EventCB(bufferevent *bev, short what, void *ctx) {
	XFtpServerCMD * cmd = (XFtpServerCMD *)ctx;
	// ����Է�����ϵ������߻��������п����ղ���BEV_EVENT_EOF��Ϣ
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)) {
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		bufferevent_free(bev);
		delete cmd;
	}
}

// ��ʼ������ ���������߳���
bool XFtpServerCMD::Init() {
	cout << "XFtpServerCMD::Init()" << endl;
	// ����socket bufferevent
	// base socket
	bufferevent * bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, ReadCB, 0, EventCB, this); // bufferevent���ûص�����
	bufferevent_enable(bev, EV_READ | EV_WRITE); // ����buffereventȨ��

	// ��ӳ�ʱ
	timeval rt = { 10, 0 };
	bufferevent_set_timeouts(bev, &rt, 0);
	return true;
}
XFtpServerCMD::XFtpServerCMD() {}
XFtpServerCMD::~XFtpServerCMD() {}