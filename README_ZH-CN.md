# 控制台桌面

**[English](https://www.github.com/esun-z/Console-Desk/blob/master/README.md)** | **中文**

一个控制台风格的自定义桌面。

## 总览

### 截图

![Screenshot (Empty)](https://s2.loli.net/2022/02/02/38NVqnamgdWjpsD.png)

![Screenshot (Using)](https://s2.loli.net/2022/02/02/Z2PUiGRtCMOKfw3.png)

### 功能

- 搜索并运行已经安装的程序。
- 搜索并打开本地文件。
- 运行 CMD 命令。
- 打开用户指定的文件或程序。
- 打开网页。
- 显示当前日期和时间。
- 显示“每日一句”（需要互联网连接）。

### 用户界面

- 上方：
  - 日志列表。
- 中间：
  - 每日一句。
  - 日期与时间。
  - 用于接收指令的输入框。
- 下方：
  - 候选项列表。会随着用户输入指令而更新。

## 使用方法

### 基本

1. 下载一个 [release](https://www.github.com/esun-z/Console-Desk/releases) 中的 exe 文件。
2. 运行它。

### 命令

| 命令                                      | 功能描述                                     |
| ----------------------------------------- | -------------------------------------------- |
| "- command" 或 "- commands"               | 打开 CustomLinks.txt 以编辑自定义链接。      |
| "- exit", "- quit", "- esc" or "- escape" | 退出 Console Desk 。                         |
| "- runatboot " + ["true" or "false"]      | 设置是否开机启动。默认为否。                 |
| "-f " + [ 文件名 ]                        | 搜索本地文件（需要 Everything 正在运行中）。 |
| "> " + [ CMD 命令 ]                       | 运行 CMD 命令（结束时保留命令行窗口）。      |
| [ 应用程序名称 ]                          | 运行选中的应用程序。                         |
| [ 网址 ]                                  | 使用默认浏览器打开网址。                     |

### 热键

| 热键                                    | 功能描述                                                     |
| --------------------------------------- | ------------------------------------------------------------ |
| 回车（Enter）                           | 运行当前命令。                                               |
| 制表（Tab）                             | 在下方列表中的候选项之间切换。                               |
| 向上（Up）/ 向下（Down）                | 选择上一个 / 下一个候选项。                                  |
| 上档键 (Shift) + 大写锁定键 (Caps Lock) | 当本程序不在前台时，切换至本程序；当本程序在前台时，隐藏它。 |

### 添加自定义链接

以下**任一方法**均可行。

- 右键一个程序并选择 “固定到‘开始’屏幕”（最简单）。

- 添加 .lnk 文件到**系统开始菜单**文件夹（比较简单）。

- 添加自定义链接到 “**CustomLinks.txt**” 中，并将该文本文件保存至本程序旁。

  自定义链接的写法如下：

  ```
  <name1>
  <absolute path1>
  <name2>
  <absolute path2>
  ...
  ```

  以下是一个案例：

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

  如果您想要直接复制这个案例，请更改路径中的用户名。

在 “CustomLinks.txt” 中的用户自定义链接相比于其他候选项将拥有**更高优先级**。

不建议您使用以 “-” 开头的名称作为自定义链接名，因为它可能会被忽略或引起未知的错误。

### 添加自定义功能

克隆或下载这个项目，并在以下函数中添加自定义功能：

```
void ConsoleDesk::HandleCommand(QString cmd, int seq){
	//to do
}
```

请注意代码中的**注释**和各功能之间的**优先级排序**。

## 环境

### 开发平台

Visual Studio 2015，安装有 Qt 5.15.1 MSVC 2015 x64 。

请手动为 Qt 添加 OpenSSL 支持。

### 目标平台

Windows 10 或更高版本。

## 使用的库和开发框架

[Qt](https://www.qt.io/)

[Open SSL](https://www.github.com/openssl/openssl)（可选）

[Qt Frameless Window DarkStyle](https://www.github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle)

## 许可协议

GNU GPL V3.0