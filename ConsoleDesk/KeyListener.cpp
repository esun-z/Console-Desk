#include "KeyListener.h"


#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
//status check only

KeyListener::KeyListener(QObject *parent) : QThread(parent) {
	memset(keyState, false, sizeof(keyState));
}

//triger, only return true when the key is just pressed
bool KeyListener::KEYDOWN(long long vk) {
	if (HIWORD(GetAsyncKeyState(vk)) > 0) {
		if (!keyState[vk]) {
			qDebug() << "Key Pressed: ";
			qDebug() << vk;
			keyState[vk] = true;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		if (keyState[vk]) {
			qDebug() << "Key Released: ";
			qDebug() << vk;
			keyState[vk] = false;
		}
		return false;
	}
}

void KeyListener::run() {

	while (1) {
		if ((KEY_DOWN(VK_LWIN) || KEY_DOWN(VK_RWIN)) && KEYDOWN('D')) {
			emit KeyDown(SPKEY_WIND);
			//Sleep(STANDARDINTERVAL);
			continue;
		}
		if (KEYDOWN(VK_UP)) {
			emit KeyDown(SPKEY_UP);
			//Sleep(SHORTINTERVAL);
			continue;
		}
		if (KEYDOWN(VK_DOWN)) {
			emit KeyDown(SPKEY_DOWN);
			//Sleep(SHORTINTERVAL);
			continue;
		}
		Sleep(10);
	}

}