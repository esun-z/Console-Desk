#pragma once

#include <QtWidgets/QWidget>
#include "ui_ConsoleDesk.h"

#include <Windows.h>

#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtNetwork\QNetworkAccessManager>
#include <QtNetwork\QNetworkRequest>
#include <QtNetwork\QNetworkReply>
#include <QUrl>
#include <QTextCodec>
#include <QStandardPaths>
#include <QProcess>
#include <QWaitCondition>

#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>

#include <QDebug>

#include "KeyListener.h"

#include "framelesswindow.h"

#define PROGRAMLINKDIR "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs"
#define USERPROGRAMLINKDIR QDir::homePath()+"\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs"
#define CUSTOMLINKFILE "CustomLinks.txt"
#define TODOLISTFILE "todoList.txt"
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
#define SEARCHENGINEAPI_BINGDICT "https://cn.bing.com/dict/search?q="
#define SEARCHENGINEAPI_BAIDUHANYU "https://hanyu.baidu.com/s?wd="

enum SPSEQ {
	SPSEQ_UNKNOWN = -1,
	SPSEQ_WEBSITE = -2,
	SPSEQ_LOCALFILE = -3,
	SPSEQ_WEBSEARCH = -4,
	SPSEQ_INNERCMD = -5,
	SPSEQ_SYSCMD = -6,
	SPSEQ_TODO = -7
};

static QStringList WebSearchEngine = {
	"g ", "b ", "bd ", "yh ", "yd ", "ddg ", "bid ", "bdh "
};

enum WEBSEARCHENGINE {
	WSE_GOOGLE,
	WSE_BING,
	WSE_BAIDU,
	WSE_YAHOO,
	WSE_YANDEX,
	WSE_DDG,
	WSE_BID,
	WSE_BDH
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

	QStringList todoList;
	QFile* todoFile;

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

	bool ReadTodo();
	bool WriteTodo();
	void PrintTodo();
};