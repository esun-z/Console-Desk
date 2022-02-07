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

	keyFreezer = new QTimer(this);
	connect(keyFreezer, SIGNAL(timeout()), this, SLOT(KeyFreezerStop()));
	
}

//Stop key freezer after one timeout
void ConsoleDesk::KeyFreezerStop() {

	keyFreezer->stop();

}

//initialize key listener
void ConsoleDesk::InitKeyListener() {
	keyListener = new KeyListener();
	connect(keyListener, SIGNAL(KeyDown(int)), this, SLOT(HandleKeyListener(int)));
	keyListener->start();
}

//Handle KeyListener signals
void ConsoleDesk::HandleKeyListener(int spkey) {

	switch (spkey) {
	case SPKEY_WIND:
		ShowForeground();
		break;
	case SPKEY_UP:
		if (ui.textEditInput->toPlainText().isEmpty() && ui.listWidgetLog->count() > 0) {
			ui.textEditInput->setText(lastCommand);
			ui.textEditInput->moveCursor(QTextCursor::End);
			return;
		}
		if (ui.listWidgetHint->count() > 1) {
			if (ui.listWidgetHint->currentRow() == -1) {
				ui.listWidgetHint->setCurrentRow(ui.listWidgetHint->count() - 1);
			}
			else {
				ui.listWidgetHint->setCurrentRow(((ui.listWidgetHint->currentRow() - 1) % ui.listWidgetHint->count() + ui.listWidgetHint->count()) % ui.listWidgetHint->count());
			}
		}
		break;
	case SPKEY_DOWN:
		if (ui.listWidgetHint->count() > 1) {
			if (ui.listWidgetHint->currentRow() == -1) {
				ui.listWidgetHint->setCurrentRow(0);
			}
			else {
				ui.listWidgetHint->setCurrentRow((ui.listWidgetHint->currentRow() + 1) % ui.listWidgetHint->count());
			}
		}
		break;
	default:
		break;
	}

}

//show foreground when win+D pressed
void ConsoleDesk::ShowForeground() {
	qDebug() << "Win+D Pressed";
	qDebug() << keyFreezer->isActive();
	if (keyFreezer->isActive()) {
		return;
	}
	if (!isActiveWindow()) {
		Sleep(100);
		activateWindow();
	}
	else {
		showMinimized();
	}
	delete keyFreezer;
	keyFreezer = new QTimer(this);
	connect(keyFreezer, SIGNAL(timeout()), this, SLOT(KeyFreezerStop()));
	keyFreezer->start(400);
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
		lftimer->stop();
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
		lastCommand = inStr;
		int sel = ui.listWidgetHint->currentRow();

		HandleCommand(inStr, candidateList.seq[sel]);

		inputCheckStoper++;//To prevent infinite loop. Do it before clearing textEditInput
		ui.textEditInput->clear();
		ui.listWidgetHint->clear();
		return;
	}
	
	//please add customized rules here
	//return after each rule

	candidateList.name.clear();
	ui.listWidgetHint->clear();

	//skip searching if this is a special command
	if (inStr.startsWith("-") || inStr.startsWith(">")) {
		ui.listWidgetHint->addItem(inStr);
		ui.listWidgetHint->setCurrentRow(-1);
		return;
	}

	//open in browser
	if (!inStr.contains(" ") && inStr.contains(".") && !inStr.startsWith(".") && !inStr.endsWith(".")) {
		candidateList.name << inStr;
		candidateList.seq[candidateList.name.count() - 1] = SPSEQ_WEBSITE;
	}

	//normal text change. search the list and print hints again
	if (inStr.count() > 0) {
		//scandidateList.name.clear();
		//int i = candidateList.name.count();
		NAMESEQ candidateProgramList = FindString(inStr, programList);
		int numNotProgram = candidateList.name.count();

		candidateList.name << candidateProgramList.name;
		for (int i = 0; i < candidateProgramList.name.count(); ++i) {
			candidateList.seq[i + numNotProgram] = candidateProgramList.seq[i];
		}

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

	//special sequence recognize
	switch (seq) {
	case SPSEQ_WEBSITE:
		if (!cmd.startsWith("http", Qt::CaseInsensitive)) {
			cmd = "http://" + cmd;
		}
		ShellExecuteA(NULL, "open", cmd.toLocal8Bit(), NULL, NULL, SW_SHOWMAXIMIZED);
		return;
		//break;
	default:
		if (seq < 0) {
			PrintLog("* Invalid Special Sequence. An unexpected error occured.");
			return;
		}
		break;
	}

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
		
		//open command list
		if (keyWord.at(0) == "command" || keyWord.at(0) == "commands") {
			qDebug() << ShellExecuteA(NULL, "open", CUSTOMLINKFILE, NULL, NULL, SW_SHOWMAXIMIZED);
		}

		//exit or quit
		if (keyWord.at(0) == "exit" || keyWord.at(0) == "quit" || keyWord.at(0) == "esc" || cmd == "escape") {
			SendMessageA(FindWindowA(NULL, "ConsoleDesk"), WM_CLOSE, 0, 0);
			//this solution is quite stupid but it can close the framelessWindow (background window)
		}

		//about or help
		if (keyWord.at(0) == "about" || keyWord.at(0) == "help") {
			ShellExecuteA(NULL, "open", "https://www.github.com/esun-z/Console-Desk", NULL, NULL, SW_SHOWMAXIMIZED);
		}

		//shutdown
		if (keyWord.at(0) == "shutdown") {
			system("shutdown -s -t 0");
		}

		//reboot
		if (keyWord.at(0) == "reboot") {
			system("shutdown -r -t 0");
		}

		//setting
		if (keyWord.at(0) == "run-at-boot" || keyWord.at(0) == "runatboot") {
			if (keyWord.count() > 1 && !keyWord.at(1).isEmpty()/*count at first !!*/) {
				if (keyWord.at(1) == "1" || keyWord.at(1).contains("true",Qt::CaseInsensitive)) {
					//create a link in Dir:StartUp to run at startup
					qDebug() << STARTUPDIR + QString::fromStdString("\\ConsoleDesk.lnk");
					QFile::link(QCoreApplication::applicationFilePath(), STARTUPDIR + QString::fromStdString("\\ConsoleDesk.lnk"));
					PrintLog("- Setting: Run at boot -> true");
				}else if (keyWord.at(1) == "0" || keyWord.at(1).contains("false", Qt::CaseInsensitive)) {
					//remove that link file
					QFile::remove(STARTUPDIR + QString::fromStdString("\\ConsoleDesk.lnk"));
					PrintLog("- Setting: Run at boot -> false");
				}
				else {
					PrintLog("* Please attach a value (ture or false).");
				}
			}
			else {
				PrintLog("* Please attach a value (ture or false).");
			}
		}


		return;
	}
	

	//cmd mode
	if (cmd.startsWith(">", Qt::CaseSensitive)) {
		system("start cmd /k " + cmd.right(cmd.length() - 1).toLocal8Bit());
		PrintLog(cmd);
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
	//change the SW_SHOWMAXIMIZED (to SW_SHOWNORMAL for example) if you want

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