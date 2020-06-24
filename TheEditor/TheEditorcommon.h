#pragma once

#ifndef Theeditor_define_area
#define Theeditor_define_area
//#include <afx.h>

//#include<afxwin.h>

#include <Shlobj.h>
#include <string>
#include <windows.h>
#include <iostream>
#include<sstream>
#include <tchar.h>
#include <stdlib.h>
#include<vector>//vector

#include <string.h>
#include <Richedit.h>
#include<iostream> //file pointer,class
#include <Commctrl.h>
#include <malloc.h>  //calloc
#include<Commdlg.h>

#include<Shlobj.h> //选择文件夹对话框

#include "resource.h"

#include "MenuCommand.h"

#pragma comment(lib,"Shell32.lib")
#pragma comment(lib, "comctl32.lib")





//#define BLOOMFILTERBITCOUNT MAXLEN
#define DEBUG 1
#define MAXLEN 64
#define SUFFIXMAXLEN 10



void CreateMyMenu();//创建菜单
int createdictwindows();
void settrackbar();
void settrackbarvalue(int value);
//int create(LPCTSTR pszFile);
void createsettingswindows();







//TCHAR FILENAME[PATHMAXLEN];

class textcontent {
public:
	textcontent() {
		
		normalstyle.cbSize = sizeof(CHARFORMAT2);
		normalstyle.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information
		normalstyle.crTextColor = RGB(0, 0, 0);
		normalstyle.crBackColor = RGB(255, 255, 255);
		normalstyle.dwEffects = CFE_DISABLED;
	
		specialstyle.cbSize = sizeof(CHARFORMAT2);
		specialstyle.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information
		specialstyle.crTextColor = RGB(0, 0, 255);
		specialstyle.crBackColor = RGB(255, 255, 255);
		specialstyle.dwEffects = CFE_BOLD;

		
	}
	void settext(TCHAR* income);
	void cleartext();
	void gettext(TCHAR* tcharbuffer, int re_text_length);
	int getwordrange(_textrange& textrangevar);
	void turnblue(_charrange rangevar);
	void turnnormal(_charrange rangevar);
	
	void adddictword(TCHAR* tcharbuffer);
	void getdictword(int index, TCHAR* tbuffer);

	int getlength();
	int getlengthEX();
	int findnextbreak(int startposition);	
	int gettrackbarvalue();	
	int getdictwordnumber();
	int getdictwordlen(int index);
	
	

private:
	CHARFORMAT2 normalstyle;
	CHARFORMAT2 specialstyle;
	
};

#endif