#include "ConsoleDesk.h"

ConsoleDesk::ConsoleDesk(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	InitConnect();
	InitTimer();
	LoadProgramLink();
	inputCheckStoper = 0;
	GetSign(INTERNETSIGNAPI);
	InitUi();
	InitKeyListener();
	
	
	//create a link in Dir:StartUp to run at startup
	qDebug() <<STARTUPDIR + QString::fromStdString("\\ConsoleDesk.lnk");
	QFile::link(QCoreApplication::applicationFilePath(), STARTUPDIR + QString::fromStdString("\\ConsoleDesk.lnk"));
}

//initialize connections
void ConsoleDesk::InitConnect() {

	connect(ui.textEditInput, SIGNAL(textChanged()), this, SLOT(CheckInput()));

}

//initialize UI
void ConsoleDesk::InitUi() {

	ui.listWidgetLog->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.listWidgetHint->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//ui.textEditInput->activateWindow();
	ui.textEditInput->setFocus();

}

//initialize and start timer
void ConsoleDesk::InitTimer() {

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(HandleTimerEvent()));
	timer->start(1000);

	lftimer = new QTimer(this);
	connect(lftimer, SIGNAL(timeout()), this, SLOT(HandleLFTimerEvent()));
	lftimer->start(LFTIMERTIME);

}

//initialize key listener
void ConsoleDesk::InitKeyListener() {
	keyListener = new KeyListener();
	connect(keyListener, SIGNAL(KeyDown()), this, SLOT(ShowForeground()));
	keyListener->start();
}

//show foreground when win+D pressed
void ConsoleDesk::ShowForeground() {
	qDebug() << "Win+D Pressed";
	Sleep(10);
	if (!isActiveWindow()) {
		activateWindow();
	}
}

//search program links
void ConsoleDesk::LoadProgramLink() {
	
	if (FindProgramLink(PROGRAMLINKDIR, programList, programListPath)) {
		PrintLog("* Failed to read program list.");
	}
	if (FindProgramLink(USERPROGRAMLINKDIR, userProgramList, userProgramListPath)) {
		PrintLog("* Failed to read user program list.");
	}
	ReadCustomLink();

	MergeProgramLink(programList, programListPath, userProgramList, userProgramListPath, true);
	MergeProgramLink(programList, programListPath, customProgramList, customProgramListPath, false);
	
	qDebug() << programList;

	for (int i = 0; i < programListPath.count(); ++i) {
		qDebug() << programListPath.at(i);
	}

}

//read custom links
void ConsoleDesk::ReadCustomLink() {

	QFile customLinkFile(CUSTOMLINKFILE);
	if (!customLinkFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		PrintLog("* Failed to read custom links.");
		return;
	}
	QString strName, strPath;
	while (!customLinkFile.atEnd()) {
		strName = customLinkFile.readLine();
		strPath = customLinkFile.readLine();
		customProgramList << strName.left(strName.length() - 1);
		customProgramListPath << strPath.left(strPath.length() - 1);
	}
	customLinkFile.close();
	qDebug() << customProgramList;

}

//get sign sentence from the Internet
void ConsoleDesk::GetSign(QString url) {

	qDebug() << "OpenSSL support:" << QSslSocket::supportsSsl();
	//get
	QNetworkRequest request;
	QDate date = date.currentDate();
	request.setUrl(QUrl(QString::fromStdString(INTERNETSIGNAPI) + date.toString("yyyy-MM-dd")));
	manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(HandleNetworkReply(QNetworkReply*)));
	manager->get(request);

}

//Handle Internet reply (depending on the API used)
void ConsoleDesk::HandleNetworkReply(QNetworkReply *reply) {

	QTextCodec *codec = QTextCodec::codecForName("utf8");
	QString str = codec->toUnicode(reply->readAll());
	qDebug() << str;
	qDebug() << str.contains("\"content\":\"");
	str = str.mid(str.indexOf("\"content\":\"") + 11, str.indexOf("\"", str.indexOf("\"content\":\"") + 11) - str.indexOf("\"content\":\"") - 11);
	ui.labelSign->setText(str);
	//clean up
	reply->deleteLater();
	manager->deleteLater();

}

//check input when the content in textEditInput changed
void ConsoleDesk::CheckInput() {
	
	//do not do anything when there is a check-skipping flag
	//to prevent infinite loop when changing textEditInput through this slot function
	if (inputCheckStoper > 0) {
		inputCheckStoper--;
		return;
	}

	QString inStr = ui.textEditInput->toPlainText();

	//if the last character is tab, delete that tab and switch to next row in listWidgetHint
	if (inStr.endsWith("\t", Qt::CaseSensitive)) {
		if (ui.listWidgetHint->count() > 0) {
			ui.listWidgetHint->setCurrentRow((ui.listWidgetHint->currentRow() + 1) % ui.listWidgetHint->count());
		}
		inStr = inStr.left(inStr.length() - 1);//remove tab
		inputCheckStoper ++;//To prevent infinite loop. Do it before changing textEditInput
		ui.textEditInput->setText(inStr);
		ui.textEditInput->moveCursor(QTextCursor::End);//refill textEditInput with original content (with out tab)
		return;
	}

	//if the last character is enter, handle the command and delete all the contents
	if (inStr.endsWith("\n", Qt::CaseSensitive)) {
		inStr = inStr.left(inStr.length() - 1);//remove enter
		int sel = ui.listWidgetHint->currentRow();

		if (sel >= 0) {
			HandleCommand(inStr, candidateList.seq[sel]);
		}
		else {
			HandleCommand(inStr, sel);
		}

		inputCheckStoper++;//To prevent infinite loop. Do it before clearing textEditInput
		ui.textEditInput->clear();
		ui.listWidgetHint->clear();
		return;
	}

	//normal text change. search the list and print hints again
	ui.listWidgetHint->clear();
	if (inStr.count() > 0) {
		candidateList.name.clear();
		candidateList = FindString(inStr, programList);
		ui.listWidgetHint->addItems(candidateList.name);
		if (ui.listWidgetHint->count() > 0) {
			ui.listWidgetHint->setCurrentRow(0);
		}
	}
	
}

//handle command 
void ConsoleDesk::HandleCommand(QString cmd, int seq) {
	qDebug() << "Command:";
	qDebug() << cmd;
	qDebug() << "Seq:";
	qDebug() << seq;

	//please add customized rules here
	//return after each rule

	//specific commands
	if (cmd.startsWith("-", Qt::CaseSensitive)) {
		cmd = cmd.right(cmd.length() - 1);
		QStringList keyWord = cmd.split(" ");
		for (int i = 0; i < keyWord.count(); ++i) {
			if (keyWord.at(i).isEmpty()) {
				keyWord.takeAt(i);
				i--;
			}
		}
		if (keyWord.at(0) == "command" || keyWord.at(0) == "commands") {
			qDebug() << ShellExecuteA(NULL, "open", CUSTOMLINKFILE, NULL, NULL, SW_SHOWMAXIMIZED);
		}
		return;
	}
	

	//cmd mode
	if (cmd.startsWith(">", Qt::CaseSensitive)) {
		system("start cmd /k " + cmd.right(cmd.length() - 1).toLocal8Bit());
		PrintLog(cmd);
		return;
	}

	//exit, quit
	if (cmd == "exit" || cmd == "quit" || cmd == "esc" || cmd == "escape") {
		SendMessageA(FindWindowA(NULL,"ConsoleDesk"), WM_CLOSE, 0, 0);
		//this solution is quite stupid but it can close the framelessWindow (background window)
		return;
	}

	//Default Operation: run the selected program
	if (seq >= 0) {
		PrintLog(programList.at(seq));
		if ((int)ShellExecuteA(GetDesktopWindow(), "open", programListPath.at(seq).toLocal8Bit(), NULL, NULL, SW_SHOWMAXIMIZED) <= 32) {
			PrintLog("* Failed to run the program.");
		}
		return;
	}
	//change the SW_SHOWMAXIMIZED (to SW_SHOWNORMAL) if you want

	PrintLog("* Invalid Command.");
}

//print log to listWidgetLog
void ConsoleDesk::PrintLog(QString log) {

	ui.listWidgetLog->addItem(log);
	ui.listWidgetLog->setCurrentRow(ui.listWidgetLog->count() - 1);
	//select the last log
	//-1 is legal (meaning not selected)
	
}

//search a dir and get link names and paths
bool ConsoleDesk::FindProgramLink(const QString &filePath, QStringList &link, QStringList &path) {
	
	QDir dir(filePath);

	if (!dir.exists()) {
		return 1;
	}

	dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
	dir.setSorting(QDir::DirsFirst);

	QFileInfoList list = dir.entryInfoList();
	if (list.size() < 1) {
		return 1;
	}

	int i = 0;
	do {
		QFileInfo fileInfo = list.at(i);
		bool isDir = fileInfo.isDir();
		if (isDir) {
			FindProgramLink(fileInfo.filePath(), link, path);//recursion
		}
		else {
			link << fileInfo.fileName().left(fileInfo.fileName().length() - 4);//remove ".lnk"
			path << fileInfo.filePath();//including the file name
		}
		i++;
	} while (i < list.size());

	return 0;
}

//merge 2 lists (A&B) into one and remove the repeats
bool ConsoleDesk::MergeProgramLink(QStringList &listA, QStringList &pathA, QStringList listB, QStringList pathB, bool frontFirst) {
	
	if (frontFirst) {
		for (int i = 0; i < listB.count(); ++i) {
			if (!listA.contains(listB.at(i))) {
				qDebug() << "Merge:";
				qDebug() << listB.at(i);
				listA << listB.at(i);
				pathA << pathB.at(i);
			}
		}
	}
	else {
		QStringList tmpList, tmpPath;
		for (int i = 0; i < listB.count(); ++i) {
			if (!listA.contains(listB.at(i))) {
				qDebug() << "Merge:";
				qDebug() << listB.at(i);
				tmpList << listB.at(i);
				tmpPath << pathB.at(i);
			}
		}
		qDebug() << "tmpList:";
		qDebug() << tmpList;
		qDebug() << "tmpPath:";
		qDebug() << tmpPath;
		tmpList.append(listA);
		tmpPath.append(pathA);
		listA = tmpList;
		pathA = tmpPath;
	}

	//rewrited

	return 0;
}

//find the strings containing the words in the command
NAMESEQ ConsoleDesk::FindString(QString str, QStringList list) {

	QStringList input = str.split(" ");//split command into key words
	NAMESEQ currentProgram;//name and sequence in programList
	bool fContain = false;

	for (int i = 0; i < list.count(); ++i) {
		fContain = true;
		for (int j = 0; j < input.count(); ++j) {
			if (!list.at(i).contains(input.at(j), Qt::CaseInsensitive)) {
				fContain = false;
				//if the string does not contain all the key words, do not add it to the hint list
			}
		}
		if (fContain) {
			currentProgram.seq[currentProgram.name.count()] = i;
			currentProgram.name << list.at(i);
		}
	}

	return currentProgram;
}

//handle timer.timeout event (once per second)
void ConsoleDesk::HandleTimerEvent() {

	//update date
	QDate date;
	date = date.currentDate();
	QString dayOfWeek;
	const QStringList dayOfWeekName
		= { "Mon","Tue","Wed","Thu","Fri","Sat","Sun" };
	dayOfWeek = dayOfWeekName.at(date.dayOfWeek() - 1);
	ui.labelDate->setText(date.toString("yyyy/MM/dd	") + dayOfWeek + "	");

	//update time
	QTime time;
	time = time.currentTime();
	ui.labelTime->setText(time.toString("hh:mm:ss"));

}

void ConsoleDesk::HandleLFTimerEvent() {

	ReadCustomLink();
	MergeProgramLink(programList, programListPath, customProgramList, customProgramListPath, false);
	qDebug() << "LFTimer";

}