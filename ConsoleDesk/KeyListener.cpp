#include "KeyListener.h"


#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

KeyListener::KeyListener(QObject *parent) : QThread(parent) {

}

void KeyListener::run() {

	while (1) {
		if ((KEY_DOWN(VK_LWIN) || KEY_DOWN(VK_RWIN)) && KEY_DOWN('D')) {
			emit KeyDown(SPKEY_WIND);
			Sleep(STANDARDINTERVAL);
			continue;
		}
		if (KEY_DOWN(VK_UP)) {
			emit KeyDown(SPKEY_UP);
			Sleep(SHORTINTERVAL);
			continue;
		}
		if (KEY_DOWN(VK_DOWN)) {
			emit KeyDown(SPKEY_DOWN);
			Sleep(SHORTINTERVAL);
			continue;
		}
		Sleep(10);
	}

}