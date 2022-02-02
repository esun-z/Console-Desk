# Console Desk

A **console-like** customized desktop application.

## Preview

### Screenshot

![Screenshot (Empty)](https://s2.loli.net/2022/02/02/38NVqnamgdWjpsD.png)

![Screenshot (Using)](https://s2.loli.net/2022/02/02/Z2PUiGRtCMOKfw3.png)

### Components

- Upper: 
  - List Widget showing **logs**.
- Mid: 
  - **Everyday sentence** label.
  - **Date and time** labels.
  - Text Edit expecting **user input**.
- Lower:
  - List Widget showing **candidates**. Updating as user input changes.

## How to Use

### Basic

1. Download the executable file in a release.
2. Run it.

### Commands

| Command                           | Destination                                       |
| --------------------------------- | ------------------------------------------------- |
| "- command" or "- commands"       | Open CustomLinks.txt                              |
| "> " + [CMD command]              | Run CMD command (reserve CMD window)              |
| "exit", "quit", "esc" or "escape" | Close this application                            |
| [Program Name]                    | Run the program selected in the list widget below |

### Hot Keys

| Key     | Destination                                                  |
| ------- | ------------------------------------------------------------ |
| Enter   | Run current command                                          |
| Tab     | Switch between the candidates in the list widget below       |
| Win + D | Switch to Console Desk when it is not activated; Switch back to system desktop when activated. |

### Add Custom Link

**Either approach** is available.

- Add .lnk files into **Start Menu folder** (Easier)

- **Add custom links in "CustomLinks.txt"** and **save it beside ConsoleDesk.exe**

  Write your links like this:

  ```
  <name>
  <absolute path>
  <name>
  <absolute path>
  ...
  ```

  **Add an extra Enter at the end of the file.**

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

### Customize Functions

Clone this project and write your own function in

```
void ConsoleDesk::HandleCommand(QString cmd, int seq){
	//to do
}
```

Pay attention to the **annotation** there and the **priority** of each function.

## Environment

### Develop

Visual Studio 2015 with Qt 5.15.1 msvc 2015 x64

Please add OpenSSL support to Qt manually.

### Target Platform

Windows 10 and upper

## Using Frameworks & Libraries

[Qt](https://www.qt.io/)

[Open SSL](https://github.com/openssl/openssl) (Optional)

[Qt Frameless Window DarkStyle](https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle)

## Licence

GNU GPL V3.0