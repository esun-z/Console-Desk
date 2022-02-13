# Console Desk

**English** | **[中文](https://www.github.com/esun-z/Console-Desk/blob/master/README_ZH-CN.md)**

A **console-like** customized desktop.

## Preview

### Screenshots

![Screenshot (Empty)](https://s2.loli.net/2022/02/02/38NVqnamgdWjpsD.png)

![Screenshot (Using)](https://s2.loli.net/2022/02/02/Z2PUiGRtCMOKfw3.png)

### Features

- Run installed programs.
- Search local files.
- Run CMD commands.
- Open user-specified files and programs.
- Open websites.
- Show current date and time.
- Show an everyday sentence when Internet connection available.

### UI Components

- Upper: 
  - List Widget showing **logs**.
- Mid: 
  - **Everyday sentence** label.
  - **Date** label and **time** label.
  - TextEdit waiting for **user input**.
- Lower:
  - List Widget showing **candidates**. Updating as user input changes.

## How to Use

### Basic

1. Download the executable file in a [release](https://www.github.com/esun-z/Console-Desk/releases).
2. Run it.

### Commands

| Command                                   | Description                                                  |
| ----------------------------------------- | ------------------------------------------------------------ |
| "- command" or "- commands"               | Open CustomLinks.txt.                                        |
| "- exit", "- quit", "- esc" or "- escape" | Close Console Desk.                                          |
| "- runatboot " + ["true" or "false"]      | Set whether to run Console Desk at system startup. False by default. |
| "-f " + [File Name]                       | Search local files (depending on Everything.exe running).    |
| "> " + [CMD Command]                      | Run CMD command (reserving CMD window).                      |
| [Program Name]                            | Run the program selected in the list widget below.           |
| [Website Link]                            | Open a website with default browser.                         |

### Hot Keys

| Key       | Description                                                  |
| --------- | ------------------------------------------------------------ |
| Enter     | Run current command.                                         |
| Tab       | Switch between the candidates in the list widget below.      |
| Up / Down | Select the next / last candidate in the list widget below.   |
| Win + D   | Switch to Console Desk if Console Desk window is not activated; Switch to system desktop if Console Desk window is activated. |

### Add Custom Link

**Either approach** is available.

- Right click a program and pin it to the start menu (Easiest).

- Add .lnk files into **Start Menu** folder (Easier).

- Add custom links in "**CustomLinks.txt**" and save it **beside ConsoleDesk.exe**.

  Write your links like this:

  ```
  <name1>
  <absolute path1>
  <name2>
  <absolute path2>
  ...
  ```

  Here is an example:

  ```
  computer
  C:\Users\a\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\System Tools\Computer.lnk
  explorer
  C:\Users\a\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\System Tools\File Explorer.lnk
  terminal
  C:\Users\a\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Windows Terminal.lnk
  cmd
  C:\Windows\System32\cmd.exe
  run
  C:\Users\a\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\System Tools\Run.lnk
  control panel
  C:\Users\a\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\System Tools\Control Panel.lnk
  ```
  
  Change the user name if you want to copy this.

Customized links in "CustomLinks.txt" have **higher priorities** among the candidates.

Adding customized links starting with "-" is not recommended since it might be ignored or lead to unexpected errors.

### Customize Functions

Clone this project and write your own function in:

```
void ConsoleDesk::HandleCommand(QString cmd, int seq){
	//to do
}
```

Pay attention to the **annotation** there and the **priority** of each function.

## Environment

### Develop Tools

Visual Studio 2015 with Qt 5.15.1 MSVC 2015 x64.

Please add OpenSSL support to Qt manually.

### Target Platform

Windows 10 or higher.

## Using Frameworks & Libraries

[Qt](https://www.qt.io/)

[Open SSL](https://www.github.com/openssl/openssl) (Optional)

[Qt Frameless Window DarkStyle](https://www.github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle)

## Licence

GNU GPL V3.0