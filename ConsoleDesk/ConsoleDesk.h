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
#include <qprocess.h>
#include <qwaitcondition.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qtextedit.h>

#include <qdebug.h>

#include "KeyListener.h"

#include "framelesswindow.h"

#define PROGRAMLINKDIR "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs"
#define USERPROGRAMLINKDIR QDir::homePath()+"\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs"
#define CUSTOMLINKFILE "CustomLinks.txt"
#define STARTUPDIR "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp"

#define LFTIMERTIME 10000
#define MAXPROGRAMCOUNT 1024

#define INTERNETSIGNAPI "https://sentence.iciba.com/index.php?c=dailysentence&m=getdetail&title="
#define SEARCHENGINEAPI_GOOGLE "https://www.google.com/search?q="
#define SEARCHENGINEAPI_BING "https://www.bing.com/search?q="
#define SEARCHENGINEAPI_YAHOO "https://search.yahoo.com/search?p="
#define SEARCHENGINEAPI_DDG "https://duckduckgo.com/?q="
#define SEARCHENGINEAPI_BAIDU "https://www.baidu.com/s?wd="
#define SEARCHENGINEAPI_YANDEX "https://www.yandex.com/search/?text="

//#define SPSEQ_UNKNOWN -1
//#define SPSEQ_WEBSITE -2
//#define SPSEQ_LOCALFILE -3
//#define SPSEQ_WEBSEARCH -4

enum SPSEQ {
	SPSEQ_UNKNOWN = -1,
	SPSEQ_WEBSITE = -2,
	SPSEQ_LOCALFILE = -3,
	SPSEQ_WEBSEARCH = -4,
	SPSEQ_COMMAND = -5
};

static QStringList WebSearchEngine = {
	"g ", "b ", "bd ", "yh ", "yd ", "ddg "
};

enum WEBSEARCHENGINE {
	WSE_GOOGLE,
	WSE_BING,
	WSE_BAIDU,
	WSE_YAHOO,
	WSE_YANDEX,
	WSE_DDG
};

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
	QString lastCommand;
	QProcess *esProc;
	FramelessWindow* flWindow;

public slots:
	
	void CheckInput();
	void HandleTimerEvent();
	void HandleLFTimerEvent();
	void HandleNetworkReply(QNetworkReply *reply);
	void HandleKeyListener(int spkey);
	void HandleFindFile(int exitCode, QProcess::ExitStatus exitStatus);
	void ShowForeground();
	void KeyFreezerStop();

private:

	QTimer *timer;
	QTimer *lftimer;
	QTimer *keyFreezer;
	QWaitCondition *wait;
	KeyListener *keyListener;
    Ui::ConsoleDeskClass ui;

	int inputCheckStoper;
	int currentSearchEngine = 0;

	void InitUi();
	void InitTimer();
	void InitKeyListener();
	void InitFindFile();
	void ReleaseSourceFile();
	void LoadProgramLink();
	void ReadCustomLink();
	void GetSign(QString url);
	void HandleCommand(QString cmd, int seq);
	void PrintLog(QString log);
	bool FindProgramLink(const QString &filePath, QStringList &link, QStringList &path);
	bool MergeProgramLink(QStringList &listA, QStringList &pathA, QStringList listB, QStringList pathB, bool frontFirst);
	void FindFile(QString text/*, QStringList &result*/);
	NAMESEQ FindString(QString str, QStringList list);
};