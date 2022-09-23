#pragma once

#ifndef KEYLISTENER_H
#define KEYLISTENER_H

#define SHORTINTERVAL 150
#define STANDARDINTERVAL 400

#define SPKEY_AWAKE -2
#define SPKEY_UP -3
#define SPKEY_DOWN -4

#define MAXLISTENKEY 256

#include <qobject.h>
#include <qthread.h>
#include <qdebug.h>
#include <Windows.h>

class KeyListener :public QThread {
	Q_OBJECT
public:

	enum class EMITLIMITLEVEL {
		awakeOnly,
		all,
	};
	explicit KeyListener(QObject *parent = nullptr);
	EMITLIMITLEVEL emitLimitLevel;

private:
	bool keyState[MAXLISTENKEY];
	bool KEYDOWN(long long vk);

protected:
	void run();

signals:
	void KeyDown(int spkey);

public slots:
};

#endif //KEYLISTENER_H