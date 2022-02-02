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
	framelessWindow.showFullScreen();
    return a.exec();

}
