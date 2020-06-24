#pragma once
#include"TheEditorcommon.h"

#ifndef FILE_DIALOG_CLASS
#define FILE_DIALOG_CLASS

#define FILE_DIALOG_CMD_OPEN 1
#define FILE_DIALOG_CMD_CREATE 0


class FileDialog {
public:
	TCHAR* getfilepath(bool isopen);//return the path your need
private:
	bool OpenWindowsDlg(bool IsOpen, bool IsPickFolder, TCHAR* pFilePath);
};

#endif