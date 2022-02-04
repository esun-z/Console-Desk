#include "KeyListener.h"


#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

KeyListener::KeyListener(QObject *parent) : QThread(parent) {

}

void KeyListener::run() {

	while (1) {
		if ((KEY_DOWN(VK_LWIN) || KEY_DOWN(VK_RWIN)) && KEY_DOWN('D')) {
			emit KeyDown();
			Sleep(400);
		}
		Sleep(10);
	}

}