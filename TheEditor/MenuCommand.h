#pragma once
#include"TheEditorcommon.h"

#ifndef MENUCOMMAND_H
#define MENUCOMMAND_H

#define PATHMAXLEN 640
#define MAXLEN 64
#include "BloomClass.h"
#include "FileDialog.h"



class MenuCommand {
private:
	//void setColor(HWND richEditCtrl, COLORREF textColor, COLORREF bgColor);
	
	TCHAR GLOBALFILENAME[PATHMAXLEN];
	TCHAR GLOBALRUNNINGPATH[PATHMAXLEN];
	TCHAR GLOBALDICTFILENAME[MAX_PATH] = L"";
	TCHAR publicbuffer[MAXLEN];

	

public:
	
	MenuCommand() {
		//hashfunction_num = 8;
		lstrcpy(GLOBALFILENAME, L"");		
		GetCurrentDirectory(PATHMAXLEN, GLOBALRUNNINGPATH);	
	}
	int OpenFilef();
	int SaveFilef();
	int FormatFilef();
	int OpenDictFilef();
	int NewFilef();
	int CloseFilef();
	int CompilerFilef();
	int GeneraterFilef();
	int RunningCodef();
	int RefreshRunningCodef();
	void savedictfile();
	int openexistdictfile();
	int opennewdictfile();

	void sethashfunction_num(int num);
	

};

#endif