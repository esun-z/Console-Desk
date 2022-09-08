#include "KeyListener.h"


#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
//status check only

KeyListener::KeyListener(QObject *parent) : QThread(parent) {
	memset(keyState, false, sizeof(keyState));
}

//triger, only return true when the key is just pressed
bool KeyListener::KEYDOWN(long long vk) {
	if (vk < 0 || vk > 255) {
		qDebug() << "Invalid keycode.";
		return false;
	}
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
		if ((KEY_DOWN(VK_LSHIFT) || KEY_DOWN(VK_RSHIFT)) && KEYDOWN(VK_CAPITAL) && !KEY_DOWN(VK_CONTROL)) {
			if (LOWORD(GetKeyState(VK_CAPITAL)) == 65408) {
				qDebug() << "Caps Up";
				keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
				keybd_event(VK_CAPITAL, 0, 0, 0);
				keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
			}
			else {
				qDebug() << "Caps Down";
				keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
				keybd_event(VK_CAPITAL, 0, 0, 0);
				keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
				//keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			}
			qDebug() << (GetKeyState(VK_CAPITAL) & 0xffff);

			emit KeyDown(SPKEY_AWAKE);
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