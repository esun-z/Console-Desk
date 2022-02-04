#pragma once

#include <QtWidgets/QWidget>
#include "ui_ConsoleDesk.h"

#include <Windows.h>

#include <qtimer.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <QtNetwork\qnetworkaccessmanager.h>
#include <QtNetwork\qnetworkrequest.h>
#include <QtNetwork\qnetworkreply.h>
#include <qurl.h>
#include <qtextcodec.h>
#include <qstandardpaths.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qtextedit.h>

#include <qdebug.h>

#include "KeyListener.h"

#define PROGRAMLINKDIR "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs"
#define USERPROGRAMLINKDIR QDir::homePath()+"\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs"
#define CUSTOMLINKFILE "CustomLinks.txt"
#define STARTUPDIR "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp"
#define LFTIMERTIME 10000
#define MAXPROGRAMCOUNT 1024
#define INTERNETSIGNAPI "https://sentence.iciba.com/index.php?c=dailysentence&m=getdetail&title="

#define SPSEQ_WEBSITE -2

struct NAMESEQ {
	QStringList name;
	int seq[MAXPROGRAMCOUNT];
};

class ConsoleDesk : public QWidget
{
    Q_OBJECT

public:
    ConsoleDesk(QWidget *parent = Q_NULLPTR);
	QStringList programList, userProgramList, customProgramList;
	QStringList programListPath, userProgramListPath, customProgramListPath;
	NAMESEQ candidateList;
	QNetworkAccessManager *manager;

public slots:
	void CheckInput();
	void HandleTimerEvent();
	void HandleLFTimerEvent();
	void HandleNetworkReply(QNetworkReply *reply);
	void ShowForeground();
	void KeyFreezerStop();

private:

	QTimer *timer;
	QTimer *lftimer;
	QTimer *keyFreezer;
	KeyListener *keyListener;
    Ui::ConsoleDeskClass ui;

	int inputCheckStoper;

	void InitConnect();
	void InitUi();
	void InitTimer();
	void InitKeyListener();
	void LoadProgramLink();
	void ReadCustomLink();
	void GetSign(QString url);
	void HandleCommand(QString cmd, int seq);
	void PrintLog(QString log);
	bool FindProgramLink(const QString &filePath, QStringList &link, QStringList &path);
	bool MergeProgramLink(QStringList &listA, QStringList &pathA, QStringList listB, QStringList pathB, bool frontFirst);
	NAMESEQ FindString(QString str, QStringList list);
};
