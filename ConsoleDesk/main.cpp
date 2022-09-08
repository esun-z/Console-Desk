#include "ConsoleDesk.h"
#include <QtWidgets/QApplication>
#include "framelesswindow.h"
#include "DarkStyle.h"


int main(int argc, char *argv[])
{
	
	QApplication a(argc, argv);
    ConsoleDesk w;
	w.setWindowIcon(QIcon(":/ConsoleDesk/res/icon.ico"));
	FramelessWindow framelessWindow;
	framelessWindow.setWindowIcon(QIcon(":/ConsoleDesk/res/icon.ico"));
	framelessWindow.setContent(&w);
	framelessWindow.setWindowState(Qt::WindowFullScreen);
	framelessWindow.setWindowTitle("Console Desk");
	QApplication::setStyle(new DarkStyle);
	SetWindowLong((HWND)w.winId(), GWL_EXSTYLE, GetWindowLong((HWND)w.winId(), GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);
	SetWindowLong((HWND)framelessWindow.winId(), GWL_EXSTYLE, GetWindowLong((HWND)framelessWindow.winId(), GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);
	w.flWindow = &framelessWindow;
	framelessWindow.showFullScreen();
    return a.exec();

}