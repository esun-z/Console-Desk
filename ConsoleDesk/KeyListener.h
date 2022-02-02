#pragma once

#ifndef KEYLISTENER_H
#define KEYLISTENER_H


#include <qobject.h>
#include <qthread.h>
#include <qdebug.h>
#include <Windows.h>

class KeyListener :public QThread {
	Q_OBJECT
public:
	explicit KeyListener(QObject *parent = nullptr);

protected:
	void run();

signals:
	void KeyDown();

public slots:
};

#endif //KEYLISTENER_H

