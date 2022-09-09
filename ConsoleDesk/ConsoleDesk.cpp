#include "ConsoleDesk.h"

ConsoleDesk::ConsoleDesk(QWidget *parent)
    : QWidget(parent)
{
	
	memset(candidateList.seq, 0, sizeof(candidateList.seq));
	ReleaseSourceFile();
	ui.setupUi(this);
	InitUi();
	InitTimer();
	InitFindFile();
	LoadProgramLink();
	inputCheckStoper = 0;
	GetSign(INTERNETSIGNAPI);
	InitKeyListener();
	lastCommand = "";
	
}



//initialize UI
void ConsoleDesk::InitUi() {

	ui.listWidgetLog->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.listWidgetHint->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QPalette p;
	p.setColor(QPalette::Background, QColor(42, 42, 42, 112));
	p.setColor(QPalette::Text, Qt::white);
	ui.labelDate->setAutoFillBackground(true);
	ui.labelTime->setAutoFillBackground(true);
	ui.labelDate->setPalette(p);
	ui.labelTime->setPalette(p);
	
	//ui.textEditInput->activateWindow();
	ui.textEditInput->setFocus();
	connect(ui.textEditInput, SIGNAL(textChanged()), this, SLOT(CheckInput()));


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
	keyFreezer->setSingleShot(true);
	connect(keyFreezer, SIGNAL(timeout()), this, SLOT(KeyFreezerStop()));
	
	wait = new QWaitCondition();
}

//initialize file searching function
void ConsoleDesk::InitFindFile() {
	
	esProc = new QProcess(this);
	connect(esProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(HandleFindFile(int, QProcess::ExitStatus)));
}

//release source files in qrc file
void ConsoleDesk::ReleaseSourceFile() {
	qDebug() << "release source files:";
	QFile rfEs(":/ConsoleDesk/res/es.exe");
	QFile lfEs(QCoreApplication::applicationDirPath() + "/es.exe");
	if (!lfEs.exists()) {
		qDebug() << QFile::copy(":/ConsoleDesk/res/es.exe", QCoreApplication::applicationDirPath() + "/es.exe");
	}
	rfEs.close();
	lfEs.close();
}

//Stop key freezer after one timeout
void ConsoleDesk::KeyFreezerStop() {

	activateWindow();
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
	case SPKEY_AWAKE:
		ShowForeground();
		
		break;
	case SPKEY_UP:
		//if the input is empty, fill the textEdit with last command
		if (ui.textEditInput->toPlainText().isEmpty() && ui.listWidgetLog->count() > 0) {
			ui.textEditInput->setText(lastCommand);
			ui.textEditInput->moveCursor(QTextCursor::End);
			return;
		}
		if (ui.listWidgetHint->count() > 1) {
			if (ui.listWidgetHint->currentRow() == -1) {
				//select the very last candidate if nothing is selected
				ui.listWidgetHint->setCurrentRow(ui.listWidgetHint->count() - 1);
			}
			else {
				//select the last candidate (looping)
				ui.listWidgetHint->setCurrentRow(((ui.listWidgetHint->currentRow() - 1) % ui.listWidgetHint->count() + ui.listWidgetHint->count()) % ui.listWidgetHint->count());
			}
		}
		break;
	case SPKEY_DOWN:
		if (ui.listWidgetHint->count() > 1) {
			if (ui.listWidgetHint->currentRow() == -1) {
				//select the first candidate if nothing is selected
				ui.listWidgetHint->setCurrentRow(0);
			}
			else {
				//select the next candidate (looping)
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
	if (keyFreezer->isActive()) {
		return;
	}
	//Sleep(STANDARDINTERVAL);
	
	
	/*if (!isActiveWindow()) {
		keyFreezer->start(SHORTINTERVAL);
	}
	else {
		showMinimized();
	}*/

	if (isHidden() || !isActiveWindow()) {
		qDebug() << "Show window.";
		flWindow->show();
		show();
		flWindow->activateWindow();
		activateWindow();
	}
	else {
		qDebug() << "Hide window";
		flWindow->hide();
		hide();
	}
	//keyFreezer->start(100);
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
		customLinkFile.close();
		customLinkFile.open(QIODevice::ReadWrite);
		customLinkFile.close();
		if (customLinkFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			PrintLog("New custom links file is created. Type -command to edit it.");
		}
		else {
			lftimer->stop();
		}
		return;
	}
	QString strName, strPath;
	while (!customLinkFile.atEnd()) {
		strName = customLinkFile.readLine();
		strPath = customLinkFile.readLine();
		customProgramList << strName.left(strName.length() - 1);
		if (strPath.endsWith("\n")) {
			strPath = strPath.left(strPath.length() - 1);
		}
		customProgramListPath << strPath;
	}
	
	customLinkFile.close();
	qDebug() << customProgramList;
	
}

//get sign sentence from the Internet
void ConsoleDesk::GetSign(QString url) {

	qDebug() << "OpenSSL support:" << QSslSocket::supportsSsl();
	//if this value is false, please add OpenSSL support to Qt manually.
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

		if (candidateList.name.count() > 0) {
			HandleCommand(inStr, candidateList.seq[sel]);
		}
		else if(inStr.startsWith("-")) {
			HandleCommand(inStr, SPSEQ_INNERCMD);
		}
		else if (inStr.startsWith(">")) {
			HandleCommand(inStr, SPSEQ_SYSCMD);
		}
		else {
			HandleCommand(inStr, SPSEQ_UNKNOWN);
		}

		inputCheckStoper++;//To prevent infinite loop. Do it before clearing textEditInput
		ui.textEditInput->clear();
		ui.listWidgetHint->clear();
		return;
	}
	
	//please add customized rules here
	//return after each rule

	candidateList.name.clear();

	//search file
	if (inStr.startsWith("-f ", Qt::CaseInsensitive) && inStr.length() >= 4) {
		FindFile(inStr);
		return;
	}

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

	//WebSearch
	for (int i = 0; i < WebSearchEngine.count(); i++) {
		if (inStr.startsWith(WebSearchEngine.at(i))) {
			if (i < 2) {
				candidateList.name << inStr.right(inStr.length() - 2);
			}
			else if (i < 5) {
				candidateList.name << inStr.right(inStr.length() - 3);
			}
			else {
				candidateList.name << inStr.right(inStr.length() - 4);
			}
			currentSearchEngine = i;
			candidateList.seq[candidateList.name.count() - 1] = SPSEQ_WEBSEARCH;
		}
	}

	//normal text change. search the list and print hints again
	if (inStr.count() > 0) {

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
	QString parm;
	switch (seq) {
	case SPSEQ_WEBSITE:
		if (!cmd.startsWith("http", Qt::CaseInsensitive)) {
			cmd = "http://" + cmd;
		}
		ShellExecuteA(NULL, "open", cmd.toLocal8Bit(), NULL, NULL, SW_SHOWMAXIMIZED);
		return;
		//break;
	case SPSEQ_LOCALFILE:
		if (ui.listWidgetHint->count() > 0 && ui.listWidgetHint->currentRow() > -1 && !ui.listWidgetHint->currentItem()->text().isEmpty()) {
			ShellExecuteA(NULL, "open", ui.listWidgetHint->currentItem()->text().toLocal8Bit(), NULL, NULL, SW_SHOWMAXIMIZED);
			PrintLog("LocalFile: " + ui.listWidgetHint->currentItem()->text());
		}
		return;
	case SPSEQ_WEBSEARCH:
		switch (currentSearchEngine){
		case WSE_GOOGLE:
			parm = SEARCHENGINEAPI_GOOGLE + candidateList.name.at(0);
			break;
		case WSE_BING:
			parm = SEARCHENGINEAPI_BING + candidateList.name.at(0);
			break;
		case WSE_YAHOO:
			parm = SEARCHENGINEAPI_YAHOO + candidateList.name.at(0);
			break;
		case WSE_BAIDU:
			parm = SEARCHENGINEAPI_BAIDU + candidateList.name.at(0);
			break;
		case WSE_YANDEX:
			parm = SEARCHENGINEAPI_YANDEX + candidateList.name.at(0);
			break;
		case WSE_DDG:
			parm = SEARCHENGINEAPI_DDG + candidateList.name.at(0);
			break;
		default:
			PrintLog("* Invalid Search Engine.");
			break;
		}
		ShellExecuteA(NULL, "open", parm.toLocal8Bit(), NULL, NULL, SW_SHOWNORMAL);
		return;
	case SPSEQ_INNERCMD:
		break;
	case SPSEQ_SYSCMD:
		system("start cmd /k " + cmd.right(cmd.length() - 1).toLocal8Bit());
		PrintLog(cmd);
		return;
	default:
		if (seq < 0) {
			PrintLog("* Invalid Special Sequence. An unexpected error occured.");
			return;
		}
		break;
	}

	//specific commands
	if (cmd.startsWith("-", Qt::CaseSensitive)) {

		bool invalidCommand = true;

		cmd = cmd.right(cmd.length() - 1);
		QStringList keyWord = cmd.split(" ");
		for (int i = 0; i < keyWord.count(); ++i) {
			if (keyWord.at(i).isEmpty()) {
				keyWord.takeAt(i);
				i--;
			}
		}

		cmd = "";
		for (int i = 1; i < keyWord.count(); ++i) {
			cmd += keyWord.at(i);
			cmd += " ";
		}
		cmd = cmd.left(cmd.length() - 1);
		cmd.replace("+", "%2B");
		
		//open command list
		if (keyWord.at(0) == "command" || keyWord.at(0) == "commands") {
			if ((int)ShellExecuteA(NULL, "open", CUSTOMLINKFILE, NULL, NULL, SW_SHOWMAXIMIZED) < 32) {
				qDebug() << "Failed to open custom links file.";
				QFile customLinkFile(CUSTOMLINKFILE);
				customLinkFile.open(QIODevice::ReadWrite);
				customLinkFile.close();
				qDebug() << ShellExecuteA(NULL, "open", CUSTOMLINKFILE, NULL, NULL, SW_SHOWMAXIMIZED);
			}
			invalidCommand = false;
		}

		//exit or quit
		if (keyWord.at(0) == "exit" || keyWord.at(0) == "quit" || keyWord.at(0) == "esc" || keyWord.at(0) == "escape") {
			SendMessageA(FindWindowA(NULL, "ConsoleDesk"), WM_CLOSE, 0, 0);
			//this solution is quite stupid but it can close the framelessWindow (background window)
			invalidCommand = false;
		}

		//about or help
		if (keyWord.at(0) == "about" || keyWord.at(0) == "help") {
			ShellExecuteA(NULL, "open", "https://www.github.com/esun-z/Console-Desk", NULL, NULL, SW_SHOWMAXIMIZED);
			invalidCommand = false;
		}

		//shutdown
		if (keyWord.at(0) == "shutdown") {
			system("shutdown -s -t 0");
			invalidCommand = false;
		}

		//reboot
		if (keyWord.at(0) == "reboot") {
			system("shutdown -r -t 0");
			invalidCommand = false;
		}

		//lock
		if (keyWord.at(0) == "lock") {
			ShellExecuteA(NULL, "open", "Rundll32.exe", "user32.dll,LockWorkStation", NULL, SW_HIDE);
		}

		//setting: run at boot
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
			invalidCommand = false;
		}

		if (invalidCommand) {
			PrintLog("* Invalid Command");
		}

		return;
	}

	//Default Operation: run the selected program
	if (seq >= 0) {
		PrintLog(programList.at(seq));
		if ((long long)ShellExecuteA(GetDesktopWindow(), "open", programListPath.at(seq).toLocal8Bit(), NULL, NULL, SW_SHOWMAXIMIZED) <= 32) {
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

//find file through es.exe (need everything running)
void ConsoleDesk::FindFile(QString text/*, QStringList &result*/) {

	QStringList scmd;
	text = text.right(text.length() - 3);
	QStringList textList = text.split(" ");
	scmd << "-sort" << "dm" << "-n" << "32" << textList;

	qDebug() << "Search command:";
	qDebug() << QCoreApplication::applicationDirPath() + "/es.exe";
	qDebug() << scmd;
	
	esProc->waitForFinished();
	esProc->start(QCoreApplication::applicationDirPath() + "/es.exe", scmd);

}

//handle findfile.finished
void ConsoleDesk::HandleFindFile(int exitCode, QProcess::ExitStatus exitStatus) {

	ui.listWidgetHint->clear();

	switch (exitCode) {
	case 8:
		ui.listWidgetLog->addItem("* Fail to search files. Please make sure Everything.exe is running.");
		break;
	case 0: {
		QTextCodec *codec = QTextCodec::codecForName("utf8");
		QString tmpStr = codec->toUnicode(esProc->readAllStandardOutput());
		QStringList result = tmpStr.split("\r\n");
		result.takeLast();

		if (!result.isEmpty()) {
			ui.listWidgetHint->addItems(result);
			ui.listWidgetHint->setCurrentRow(0);
			candidateList.name << result;
			for (int i = 0; i < result.count(); ++i) {
				candidateList.seq[i] = SPSEQ_LOCALFILE;
			}
		}
		else {
			ui.listWidgetHint->addItem("* No matching file.");
		}

		break;
	}
	default:
		ui.listWidgetLog->addItem("* Error in searching files.");
		break;
	}

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

//handle lftimer.timeout event (once per 10 seconds)
void ConsoleDesk::HandleLFTimerEvent() {

	ReadCustomLink();
	MergeProgramLink(programList, programListPath, customProgramList, customProgramListPath, false);
	qDebug() << "LFTimer";

}