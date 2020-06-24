#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <Richedit.h>
#include<iostream> //file pointer
#include <Commctrl.h>
#include <malloc.h>  //calloc
#include<Commdlg.h>
#include<math.h>   //sqrt

#ifndef Theeditor_define_area
#define MAXLEN 64
//#define BLOOMFILTERBITCOUNT MAXLEN
#define DEBUG 1
#define PATHMAXLEN 10 * MAXLEN
#endif

#ifndef Theeditor_define_window

#define IDM_NEWFILE				301
#define IDM_OPENFILE			302
#define IDM_SAVEFILE			303
#define IDM_CLOSEFILE			304
#define IDM_EXITPROG			305

#define IDM_FORMATFILE			401
#define IDM_OPENDICTFILE		402
#define IDM_CREATEDICTFILE		403

#define IDM_REFRESHRUNNINGCODE	501
#define IDM_COMPILER			502
#define IDM_GENERATOR			503
#define IDM_RUNNINGCODE			504

#define IDM_SETTINGS			601
#define IDM_ABOUTPROG			602


#define IDM_EDITDICTFILE		900

#define IDM_CHILDBUTTON			10000
#define IDM_CHILDTRACKBAR		10001

#endif

//选择文件夹对话框
#include<Shlobj.h>
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib, "comctl32.lib")

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR childszWindowClass[] = _T("childwin32app");
static TCHAR szWindowTitle[] = _T("TheEditor (BloomFilter)");

//HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);
HWND mainwindow;
HWND re,childre, childre_button,childre_list, childre_trackbar;
HWND editdictwindow;
TCHAR GLOBALDICTFILENAME[MAX_PATH] = { 0 };
WNDCLASSEX childwcex;
HMENU hRoot;

unsigned int* dicthash = NULL;
TCHAR publicbuffer[MAXLEN];
int BLOOMFILTERBITCOUNT = 0;
//HINSTANCE g_hInst;
TCHAR GLOBALFILENAME[PATHMAXLEN];
TCHAR GLOBALRUNNINGPATH[PATHMAXLEN];

void CreateMyMenu();//创建菜单
//int create(LPCTSTR pszFile);
int open(LPCTSTR pszFile);
int save(LPCTSTR pszFile);
void initspellchecker(int);
int importdict(TCHAR*);
int checkstring(char*);
int WINAPI OpenDictFilef(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int CreateDictFile(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool  hash1(char*, int);
bool  hash2(char*, int);
bool  hash3(char*, int);
bool  hash4(char*, int);
int releasedict();




void setColor(HWND richEditCtrl, COLORREF textColor, COLORREF bgColor)
{
	CHARFORMAT2 cf;

	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information

	cf.crTextColor = textColor;
	cf.crBackColor = bgColor;
	cf.dwEffects = CFE_BOLD;

	SendMessage(richEditCtrl, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}


int WINAPI OpenFilef(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	TCHAR filename[MAX_PATH] = { 0 };
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = _T("Source Files(*.c)|*.c|Source Files(*.cpp)|*.cpp|Header Files(*.h)|*.h|Header Files(*.hpp)|*.hpp||");
	ofn.lpstrDefExt = _T("cpp");
	ofn.lpstrTitle = _T("Open File");
	ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofn.FlagsEx = OFN_EX_NOPLACESBAR;
	ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
	ofn.hwndOwner = hWnd;
	if (GetOpenFileName(&ofn))
	{

		FILE* fp;
		_wfopen_s(&fp, filename, L"r");

		if (fp == NULL)
		{
			MessageBox(NULL, L"Error file path", NULL, NULL);
			return -1;
		}

		lstrcpy(GLOBALFILENAME , filename);		

		fseek(fp, 0L, SEEK_END);
		int file_size = ftell(fp);
		rewind(fp);

		char* charbuffer = (CHAR*)calloc(file_size + 1, sizeof(CHAR));
		TCHAR* tcharbuffer = (TCHAR*)calloc(file_size + 1, sizeof(TCHAR));
		if (file_size >= 4294967296)
		{
			MessageBox(NULL, L"Sorry but the file cannot bigger than 4294967295\n,Please choose another file to try.", NULL, NULL);
			return -1;
		}
		fread(charbuffer, sizeof(char), file_size, fp);
		*(charbuffer + file_size + 1) = '\0';

		fclose(fp);
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, tcharbuffer, file_size + 1, charbuffer, file_size + 1);
			
		SetWindowText(re, tcharbuffer);

		return 1;



	}
	return 0;
}


int WINAPI SaveFilef(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	TCHAR filename[MAX_PATH] = { 0 };
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = _T("Source Files(*.c;*.cpp)|*.c;*.cpp|Header Files(*.h;*.hpp)|*.h;*.hpp||");
	ofn.lpstrDefExt = _T("cpp");
	ofn.lpstrTitle = _T("Save To");
	ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofn.FlagsEx = OFN_EX_NOPLACESBAR;
	ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
	ofn.hwndOwner = hWnd;
	if (GetSaveFileName(&ofn))
	{

		int re_text_length=GetWindowTextLength(re);
		TCHAR* tcharbuffer = (TCHAR*)calloc( re_text_length + 1, sizeof(TCHAR));
		GetWindowText(re, tcharbuffer, re_text_length+1);
		
		char* charbuffer = (CHAR*)calloc(re_text_length + 1, sizeof(CHAR));

		size_t convertedChars = 0;
		wcstombs_s(&convertedChars, charbuffer, re_text_length + 1, tcharbuffer, re_text_length + 1);
		
		FILE* fp;		
		_wfopen_s(&fp, filename, L"w+");

		if (fp == NULL)
		{
			MessageBox(NULL, L"Error file path", NULL, NULL);
			return -1;
		}

		lstrcpy(GLOBALFILENAME, filename);

		fwrite(charbuffer, sizeof(char), re_text_length+1, fp);
		fclose(fp);

		return 1;
	}
	return 0;
}


int WINAPI FormatFilef(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	while (BLOOMFILTERBITCOUNT == 0)
	{
		if (OpenDictFilef(hWnd, message, wParam, lParam) == 0)
		{
			return -1;
		}
			
		
		
	}

	int len,li=0,ni=0;
	_gettextlengthex currenttextlen;
	currenttextlen.codepage = CP_ACP;
	len=SendMessage(re, EM_GETTEXTLENGTHEX, (WPARAM)&currenttextlen,0);
	
	CHARFORMAT2 normalstyle;
	normalstyle.cbSize = sizeof(CHARFORMAT2);
	normalstyle.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information
	normalstyle.crTextColor = RGB(0, 0, 0);
	normalstyle.crBackColor = RGB(255, 255, 255);
	normalstyle.dwEffects = CFE_DISABLED;

	CHARFORMAT2 specialstyle;
	specialstyle.cbSize = sizeof(CHARFORMAT2);
	specialstyle.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information
	specialstyle.crTextColor = RGB(0, 0, 0);
	specialstyle.crBackColor = RGB(255, 255, 255);
	specialstyle.dwEffects = CFE_BOLD;


	do
	{

		ni=SendMessage(re, EM_FINDWORDBREAK, WB_MOVEWORDRIGHT,li);

		//do something that find the word between two porition
		if (ni < len)
		{
			_textrange textrangevar;
			_charrange rangevar;
			char wordcontent[MAXLEN] = { 0 };
			//TCHAR tcharwordcontent[MAXLEN];
			LPSTR wordcontentpointer = wordcontent;
			rangevar.cpMin = li;
			rangevar.cpMax = ni;
			textrangevar.chrg = rangevar;
			textrangevar.lpstrText = wordcontentpointer;
			SendMessage(re, EM_GETTEXTRANGE, 0, (LPARAM)&textrangevar);
			//size_t maxlen = MAXLEN;
			int i_withignore = 2;
			int i = 2;
			while (i_withignore < 2*(rangevar.cpMax- rangevar.cpMin)/*textrangevar.lpstrText[i] != '\0'  &&(isalpha(textrangevar.lpstrText[i]) || isdigit(textrangevar.lpstrText[i]))*/)
			{
				if (textrangevar.lpstrText[i_withignore] == '\0' || textrangevar.lpstrText[i_withignore] == '\r' || textrangevar.lpstrText[i_withignore] == ' ')
				{
					textrangevar.lpstrText[i_withignore] = '\0';
					i_withignore += 2;
					continue;
				}
				textrangevar.lpstrText[i_withignore / 2] = textrangevar.lpstrText[i];
				textrangevar.lpstrText[i] = '\0';
				i += 2;
				i_withignore += 2;
			}

			if (checkstring(textrangevar.lpstrText))
			{
				SendMessage(re, EM_EXSETSEL, 0, (LPARAM)&rangevar);
				SendMessage(re, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&specialstyle);

			}
			else
			{
				SendMessage(re, EM_EXSETSEL, 0, (LPARAM)&rangevar);
				SendMessage(re, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&normalstyle);
			}
			li = ni;
		}
		
	} while (ni < len);
	
	return 0;
}

int WINAPI OpenDictFilef(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	TCHAR filename[MAX_PATH] = { 0 };
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = _T("Dict Files(*.bin)||");
	ofn.lpstrDefExt = _T("bin");
	ofn.lpstrTitle = _T("Open Dict File");
	ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofn.FlagsEx = OFN_EX_NOPLACESBAR;
	ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
	ofn.hwndOwner = hWnd;
	if (GetOpenFileName(&ofn))
	{

		importdict(filename);
		return 1;
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	
	lstrcpy(GLOBALFILENAME, L"");

	InitCommonControls();
	LoadLibrary(TEXT("Msftedit.dll"));

	
	GetCurrentDirectory(PATHMAXLEN, GLOBALRUNNINGPATH);

	CreateMyMenu();

	WNDCLASSEX wcex;
	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//设置鼠标样式
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//设置画刷背景色
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, _T("call to RegisterClassEX failed."), _T("win32"), NULL);
		return 1;
	}

	//hInst = hInstance;
	//g_hInst = hInstance;
	HWND hWnd = CreateWindow(
		szWindowClass,
		szWindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		600,
		500,
		NULL,
		hRoot,
		hInstance,
		NULL
	);
	if (!hWnd) {//创建窗口失败
		MessageBox(NULL, _T("call to CreateWindow failed"), _T("win32"), NULL);//调用消息弹出框展示错误消息
		return 1;
	}
	mainwindow = hWnd;

		re = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT("Hi!  Click to open or create file on the \"file\" bar above.\n"),
		WS_VISIBLE | WS_CHILD | WS_VSCROLL |
		ES_MULTILINE | ES_LEFT | ES_NOHIDESEL | ES_AUTOVSCROLL /*| ES_READONLY*/,
		0, 0, 480, 400,
		hWnd, NULL, hInstance, NULL);
		if (!re)
		{
			FatalAppExitA(0, "Couldn't create the rich text box,program exit!");
			return 1;
		}
		
		                                                                           //editdictfilewindowclass
		childwcex.cbSize = sizeof(WNDCLASSEX);
		childwcex.style = CS_HREDRAW | CS_VREDRAW;
		childwcex.lpfnWndProc = ChildWndProc;
		childwcex.cbClsExtra = 0;
		childwcex.cbWndExtra = 0;
		childwcex.hInstance = GetModuleHandle(NULL);
		childwcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPLICATION));
		childwcex.hCursor = LoadCursor(NULL, IDC_ARROW);//设置鼠标样式
		childwcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//设置画刷背景色
		childwcex.lpszMenuName = NULL;
		childwcex.lpszClassName = childszWindowClass;
		childwcex.hIconSm = LoadIcon(childwcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
		if (!RegisterClassEx(&childwcex)) {
			MessageBox(NULL, _T("call to RegisterClassEX(child) failed."), _T("TheEditor"), NULL);
			return 1;
		}
		

	CHARFORMAT2 cf;
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information
	cf.crTextColor = RGB(0, 0, 0);
	cf.crBackColor = RGB(255, 255, 255);
	cf.dwEffects = CFE_DISABLED;
	SendMessageW(re, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
	//SendMessageW(re, EM_CANPASTE, 1, 0);
	ShowWindow(hWnd, nCmdShow);//显示窗口
	ShowWindow(re, nCmdShow);

	UpdateWindow(hWnd);//修改窗口
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);//转换消息
		DispatchMessage(&msg);//分发消息
	}

	return (int)msg.wParam;

}

int NewFilef()
{
	lstrcpy(GLOBALFILENAME, L"");
	SetWindowText(re, L"");
	return 0;
}

int CloseFilef()
{
	int ret=MessageBox(NULL, L"Do you need to save this file?", L"Save or not", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (ret)
	{
	case IDYES:return SaveFilef(NULL, NULL, NULL, NULL);
	case IDNO:return NewFilef();
	default:return 0;

	}
	lstrcpy(GLOBALFILENAME, L"");
}

int CompilerFilef()
{
	return 0;
}

int GeneraterFilef()
{
	if (lstrcmp(GLOBALFILENAME, L"") == 0)
	{
		MessageBox(NULL, L"Please open a file first!", NULL, NULL);
		return -1;
	}

	int finished = -1;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	
	TCHAR pathbuffer[PATHMAXLEN] = L"";
	TCHAR GENERATERPATH[] = L"\\MinGW64\\bin\\gcc.exe";
	lstrcpy(pathbuffer, GLOBALRUNNINGPATH);
	lstrcat(pathbuffer, GENERATERPATH);

	
	TCHAR *GENERATENAME=(TCHAR*)calloc(lstrlen(GLOBALFILENAME) + 1, sizeof(TCHAR));
	lstrcpy(GENERATENAME, GLOBALFILENAME);

#ifndef https://blog.csdn.net/gao199108/article/details/77965182

	//TCHAR* szBuffer = _T("Good morning to you and to you !");
	TCHAR* szFind=(TCHAR*)calloc(sizeof(TCHAR),MAXLEN);
	lstrcpy(szFind, L".");
	TCHAR* pLast = GENERATENAME + _tcslen(GENERATENAME);
	//TCHAR* pFind;
	for (; pLast >= GENERATENAME; pLast--)
	{
		if (*pLast == *szFind)
		{
			
			if (_tcsncmp(pLast, szFind, _tcslen(szFind)) == 0)
			{
				//pFind = pLast;
				break;
			}
			
		}
		*pLast = '\0';
	}                                                              
	lstrcat(GENERATENAME, L"exe");

#endif

	TCHAR argv[PATHMAXLEN];
	lstrcpy(argv, L" \"");
	
	lstrcat(argv, GLOBALFILENAME);
	lstrcat(argv, L"\" -o \"");
	lstrcat(argv, GENERATENAME);
	lstrcat(argv, L"\"");
	lstrcat(pathbuffer, argv);

	if (CreateProcess(NULL,pathbuffer, NULL,NULL,TRUE,0,NULL,NULL,&si,&pi))
	{
		finished = 0;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	
	if (finished != 0)
		MessageBox(NULL, L"Error generate output", NULL, NULL);
	return finished;
	
	
}

int RunningCodef()
{
	if (lstrcmp(GLOBALFILENAME, L"") == 0)
	{
		MessageBox(NULL, L"Please open a file first!", NULL, NULL);
		return -1;
	}

	int finished = -1;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
#if (DEBUG==1)
	TCHAR pathbuffer[PATHMAXLEN] = L"";
	TCHAR CodeExecuteprogpath[]=L"\\ConsoleAppExecuter.exe";
	lstrcpy(pathbuffer, GLOBALRUNNINGPATH);
	lstrcat(pathbuffer, CodeExecuteprogpath);
#else
	TCHAR CodeExecuteprogpath[] = L"\\ConsoleAppExecuter.exe";
#endif

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	TCHAR* GENERATENAME = (TCHAR*)calloc(lstrlen(GLOBALFILENAME) + 1, sizeof(TCHAR));
	lstrcpy(GENERATENAME, GLOBALFILENAME);

#ifndef https://blog.csdn.net/gao199108/article/details/77965182

	//TCHAR* szBuffer = _T("Good morning to you and to you !");
	TCHAR* szFind = (TCHAR*)calloc(sizeof(TCHAR), MAXLEN);
	lstrcpy(szFind, L".");
	TCHAR* pLast = GENERATENAME + _tcslen(GENERATENAME);
	//TCHAR* pFind;
	for (; pLast >= GENERATENAME; pLast--)
	{
		if (*pLast == *szFind)
		{

			if (_tcsncmp(pLast, szFind, _tcslen(szFind)) == 0)
			{
				//pFind = pLast;
				break;
			}

		}
		*pLast = '\0';
	}
	lstrcat(GENERATENAME, L"exe");

#endif


	if (CreateProcess(pathbuffer, GENERATENAME, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		finished = 0;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (finished != 0)
		MessageBox(NULL, L"Error running output", NULL, NULL);
	return finished;
}

int RefreshRunningCodef()
{
	if (!GeneraterFilef())
	{
		return RunningCodef();
	}
	return -1;

}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;//画图结构体
	HDC hdc;//绘图上下文句柄
	
	switch (message) {
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_NEWFILE:NewFilef(); break;
		case IDM_OPENFILE:OpenFilef(hWnd, message, wParam, lParam); break;
		case IDM_SAVEFILE:SaveFilef(hWnd,message,wParam,lParam); break;
		case IDM_FORMATFILE:FormatFilef(hWnd, message, wParam, lParam); break;
		case IDM_CLOSEFILE:CloseFilef(); break;
		case IDM_EXITPROG:PostQuitMessage(0); break;
		case IDM_OPENDICTFILE:OpenDictFilef(hWnd, message, wParam, lParam); break;
		case IDM_REFRESHRUNNINGCODE:RefreshRunningCodef(); break;
		case IDM_COMPILER:CompilerFilef(); break;
		case IDM_GENERATOR:GeneraterFilef(); break;
		case IDM_RUNNINGCODE:RunningCodef(); break;

		case IDM_CREATEDICTFILE:
		{
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(ofn));

			
			ofn.lpstrFile = GLOBALDICTFILENAME;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = _T("Dict Files(*.bin)||");
			ofn.lpstrDefExt = _T("bin");
			ofn.lpstrTitle = _T("Open Dict File");
			ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
			ofn.FlagsEx = OFN_EX_NOPLACESBAR;
			ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
			ofn.hwndOwner = hWnd;

			int ret;
			ret=MessageBox(NULL, L"Do you want to open an exist file?", L"Open or Create", MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);
			if (ret != IDCANCEL)
			{
				editdictwindow = CreateWindowExW(0, L"childwin32app", TEXT("Edit dict file window"),
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, CW_USEDEFAULT, 480, 400,
					hWnd, NULL, GetModuleHandle(NULL), NULL);

				if (!editdictwindow)
				{
					FatalAppExitA(0, "Couldn't create the dict edit window,program exit!");
					return 1;
				}
				ShowWindow(editdictwindow, SW_SHOW);

				childre = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""),
					WS_VISIBLE | WS_CHILD |
					ES_MULTILINE | ES_LEFT | ES_NOHIDESEL | WS_BORDER/*| ES_READONLY*/,
					120, 310, 200, 30,
					editdictwindow, NULL, GetModuleHandle(NULL), NULL);
				if (!childre)
				{
					FatalAppExitA(0, "Couldn't create the keyword box,program exit!");
					return 1;
				}

				childre_button = CreateWindowEx(0, L"BUTTON", L"Add",
					WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP |
					ES_NOHIDESEL /*| ES_READONLY*/,
					320, 310, 50, 30,
					editdictwindow, (HMENU)IDM_CHILDBUTTON, NULL/* GetModuleHandle(NULL)*/, NULL);
				if (!childre_button)
				{
					FatalAppExitA(0, "Couldn't create the button,program exit!");
					return 1;
				}

				
				childre_trackbar=CreateWindowEx(
					0,                               // no extended styles 
					TRACKBAR_CLASS,                  // class name 
					L"Trackbar Control",              // title (caption) 
					WS_CHILD |
					WS_VISIBLE |
					TBS_AUTOTICKS |
					TBS_ENABLESELRANGE,              // style 
					150, 220                         // position 
					, 250, 50 ,                        // size 
					editdictwindow,                         // parent window 
					(HMENU)IDM_CHILDTRACKBAR,                     // control identifier 
					NULL,                         // instance 
					NULL                             // no WM_CREATE parameter 
				);

				if (!childre_trackbar)
				{
					FatalAppExitA(0, "Couldn't create the trackbar,program exit!");
					return 1;
				}

				SendMessage(childre_trackbar, TBM_SETRANGE,
					(WPARAM)TRUE,                   // redraw flag 
					(LPARAM)MAKELONG(64, 1024));  // min. & max. positions

				SendMessage(childre_trackbar, TBM_SETPAGESIZE,
					0, (LPARAM)4);                  // new page size 

				SendMessage(childre_trackbar, TBM_SETSEL,
					(WPARAM)FALSE,                  // redraw flag 
					(LPARAM)MAKELONG(64, 1024));

				SendMessage(childre_trackbar, TBM_SETPOS,
					(WPARAM)TRUE,                   // redraw flag 
					(LPARAM)64);





				childre_list = CreateWindowEx(WS_EX_CLIENTEDGE, L"listbox", L"Keywords list",
					WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_READONLY,
					20, 10, 400, 200,
					editdictwindow, NULL,NULL/* GetModuleHandle(NULL)*/, NULL);
				if (!childre_list)
				{
					FatalAppExitA(0, "Couldn't create the list box,program exit!");
					return 1;
				}

				if (ret == IDYES)
				{
					if (!GetOpenFileName(&ofn))
					{
						return -1;
					}
					else
					{
						FILE *fp;
						_wfopen_s(&fp, GLOBALDICTFILENAME, L"r");
						if (fp == NULL)
						{
							MessageBox(NULL, L"Error finding the dict file.", NULL, NULL);
							DestroyWindow(editdictwindow);
							return -1;
						}
						char checklegal[5] = { 0 };
						fread(checklegal, sizeof(char), 4, fp);
						if (strcmp(checklegal, "bfd"))
						{
							MessageBox(NULL, L"Please choose the dict file.", L"Error", NULL);
							fclose(fp); 
							DestroyWindow(editdictwindow);
							return -1;

						}

						fread(&BLOOMFILTERBITCOUNT, sizeof(int), 1, fp);
						SendMessage(childre_trackbar, TBM_SETPOS,
							(WPARAM)TRUE,                   // redraw flag 
							(LPARAM)BLOOMFILTERBITCOUNT);
						
						BLOOMFILTERBITCOUNT = 0;
						fclose(fp);
						
						TCHAR filenamebuffer[260];
						lstrcpy(filenamebuffer, GLOBALDICTFILENAME);
						
						lstrcat(filenamebuffer, L"_config");
						_wfopen_s(&fp, filenamebuffer, L"r");
						if(fp==NULL)
						{
							MessageBox(NULL, L"Error finding the _config file.", NULL, NULL);
							DestroyWindow(editdictwindow);
							return -1;
						}
						//char checklegal2[5] = { 0 };
						fread(checklegal, sizeof(char), 5, fp);
						if (strcmp(checklegal, "bfdc"))
						{
							MessageBox(NULL, L"Please choose the dict config file,or the dict file could only be used to read", L"Error", NULL);
							fclose(fp);
							DestroyWindow(editdictwindow);
							return -1;
						}

						while (!feof(fp))
						{
							char charbuffer[MAXLEN];
							size_t convertedChars = 0;
							TCHAR tcharbuffer[MAXLEN];
							int readlen=fread(charbuffer, sizeof(char), MAXLEN, fp);
							if (readlen != MAXLEN)
							{
								break;
							}
							mbstowcs_s(&convertedChars, tcharbuffer, charbuffer, MAXLEN);
							SendMessage(childre_list, LB_ADDSTRING, 0, (LPARAM)(tcharbuffer));
						}

						fclose(fp);
						
					}
				}
				else if (ret == IDNO)
				{
					if (!GetSaveFileName(&ofn))
					{
						return -1;
					}
					else
					{
						BLOOMFILTERBITCOUNT = 0;
						
					}
				}

			}
			else
			{
				return 0;
			}

			break;
		}
		case IDM_ABOUTPROG:MessageBox(NULL, L"The Editor\nVersion 1.0.0.0\nCopyright 2019 wjh776a68", L"About The Editor", NULL); break;
		}
	}
	case WM_SIZE:
	{
		if (hWnd == mainwindow)
		{
			LPRECT rect = (LPRECT)malloc(sizeof(tagRECT));
			GetClientRect(hWnd, rect);
			SetWindowPos(re, HWND_TOP, 0, 0, rect->right, rect->bottom, SWP_NOMOVE |  SWP_NOZORDER);
		}
		
	}
		
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//TextOut(hdc, 50, 5, hello, wcslen(hello));
		
		EndPaint(hWnd, &ps);
		break;

	case  WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;//画图结构体
	HDC hdc;//绘图上下文句柄
	switch (message) {
	case WM_COMMAND:
	{
	case WM_HSCROLL:
	{

		if (HWND(lParam) == childre_trackbar)
		{

			int newPos = SendMessage(childre_trackbar, TBM_GETPOS, 0, 0);
			int selStart = SendMessage(childre_trackbar, TBM_GETSELSTART, 0, 0);
			int selEnd = SendMessage(childre_trackbar, TBM_GETSELEND, 0, 0);

			if (newPos <=64)
			{
				SendMessage(childre_trackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)64);
			}
			else if (newPos < 128)
			{
				SendMessage(childre_trackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)64);
			}
			else if (newPos < 256)
			{
				SendMessage(childre_trackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)128);
			}
			else if (newPos < 512)
			{
				SendMessage(childre_trackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)256);
			}
			else if (newPos < 1024)
			{
				SendMessage(childre_trackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)512);
			}

			else if (newPos >=1024)
			{
				SendMessage(childre_trackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)1024);
			}
			break;
		}

		
	}
	case BN_CLICKED:
	{
		if (LOWORD(wParam) == IDM_CHILDBUTTON)
		{

			GETTEXTEX gettextbuffer;
			GETTEXTLENGTHEX gettextlengthbuffer;
			gettextlengthbuffer.codepage = 1200;
			gettextlengthbuffer.flags = GTL_DEFAULT;

			
			if (SendMessage(childre, EM_GETTEXTLENGTHEX, (WPARAM)&gettextlengthbuffer, 0) >= MAXLEN)
			{
				MessageBox(NULL, L"Key word out of length, invalid operation!", NULL, NULL);   // Reason: storage these key words using static length to divide words.
				break;
			}
			gettextbuffer.cb = MAXLEN;
			gettextbuffer.codepage = 1200;
			gettextbuffer.flags = GT_DEFAULT;
			gettextbuffer.lpDefaultChar = NULL;
			gettextbuffer.lpUsedDefChar = false;
			TCHAR tcharbuffer[MAXLEN];

			SendMessage(childre,EM_GETTEXTEX,(WPARAM)&gettextbuffer,(LPARAM)&tcharbuffer);

			SendMessage(childre_list, LB_ADDSTRING, 0, (LPARAM)tcharbuffer);

			SETTEXTEX settextbuffer;
			settextbuffer.codepage = 1200;
			settextbuffer.flags = ST_DEFAULT;

			tcharbuffer[0] = tcharbuffer[1] = '\0';
			SendMessage(childre, EM_SETTEXTEX, (WPARAM)&gettextbuffer, (LPARAM)&tcharbuffer);
			break;
		}
		
		
	}
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 310, L"New keyword:", wcslen(L"New keyword:"));
		TextOut(hdc, 20, 220, L"Size of dicthash", wcslen(L"Size of dicthash"));
		EndPaint(hWnd, &ps);
		break;
		
	case  WM_CLOSE:
	{
		int i;
		size_t tmp;
		int childre_list_count=SendMessage(childre_list, LB_GETCOUNT, 0, 0);
		FILE *fp,*fp_config;
		int childre_trackbar_pos=SendMessage(childre_trackbar, TBM_GETPOS, 0, 0);
		BLOOMFILTERBITCOUNT = childre_trackbar_pos;
		TCHAR filenamebuffer[260];
		lstrcpy(filenamebuffer, GLOBALDICTFILENAME);
		_wfopen_s(&fp, GLOBALDICTFILENAME, L"w+");
		_wfopen_s(&fp_config, lstrcat(filenamebuffer, L"_config"), L"w+");
		initspellchecker(BLOOMFILTERBITCOUNT);
		if(fp!=NULL && fp_config!=NULL)
		{

			fwrite("bfdc", sizeof(char), 5, fp_config);


			for (i = 0; i < childre_list_count; i++)
			{
				TCHAR* tbuffer = (TCHAR*)malloc(sizeof(TCHAR)* SendMessage(childre_list, LB_GETTEXTLEN, i, 0));
				SendMessage(childre_list, LB_GETTEXT, i, (LPARAM)tbuffer);
				char* buffer = (char*)malloc(sizeof(char) * (wcslen(tbuffer) + 1));
				wcstombs_s(&tmp,buffer, wcslen(tbuffer) + 1, tbuffer, wcslen(tbuffer) + 1);
				
				fwrite(buffer, sizeof(char), MAXLEN, fp_config);

				hash1(buffer, 1);
				hash2(buffer, 1);
				hash3(buffer, 1);
				hash4(buffer, 1);

			}

		
			char checklegal[] = "bfd";
			fwrite(checklegal, sizeof(char), 4, fp);
			fwrite(&BLOOMFILTERBITCOUNT, sizeof(int), 1, fp);
			fwrite(dicthash, sizeof(unsigned int), BLOOMFILTERBITCOUNT, fp);

			fclose(fp);
			fclose(fp_config);
		}

		releasedict();
		BLOOMFILTERBITCOUNT = 0;
		DestroyWindow(editdictwindow);
		break;

	}	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}


void CreateMyMenu()
{
	hRoot = CreateMenu();
	if (!hRoot)
		return;
	HMENU pop1 = CreatePopupMenu();
	AppendMenu(hRoot,
		MF_POPUP,
		(UINT_PTR)pop1,
		L"文件");
	AppendMenu(pop1,
		MF_STRING,
		IDM_NEWFILE,
		L"新建");
	AppendMenu(pop1,
		MF_STRING,
		IDM_OPENFILE,
		L"打开");
	AppendMenu(pop1,
		MF_STRING,
		IDM_SAVEFILE,
		L"保存");
	AppendMenu(pop1,
		MF_STRING,
		IDM_CLOSEFILE,
		L"关闭");
	AppendMenu(pop1,
		MF_SEPARATOR,
		NULL,
		L"-");
	AppendMenu(pop1,
		MF_STRING,
		IDM_EXITPROG,
		L"退出");


	HMENU pop2 = CreatePopupMenu();
	AppendMenu(hRoot,
		MF_POPUP,
		(UINT_PTR)pop2,
		L"编辑");
	AppendMenu(pop2,
		MF_STRING,
		IDM_FORMATFILE,
		L"格式化");
	AppendMenu(pop2,
		MF_STRING,
		IDM_OPENDICTFILE,
		L"导入字典文件");
	AppendMenu(pop2,
		MF_STRING,
		IDM_CREATEDICTFILE,
		L"编辑字典文件");
	HMENU pop3 = CreatePopupMenu();
	AppendMenu(hRoot,
		MF_POPUP,
		(UINT_PTR)pop3,
		L"项目");
	AppendMenu(pop3,
		MF_STRING,
		IDM_REFRESHRUNNINGCODE,
		L"重新生成并运行");
	AppendMenu(pop3,
		MF_STRING,
		IDM_COMPILER,
		L"编译");
	AppendMenu(pop3,
		MF_STRING,
		IDM_GENERATOR,
		L"生成");
	AppendMenu(pop3, MF_STRING,
		IDM_RUNNINGCODE,
		L"调试运行");
	HMENU pop4 = CreatePopupMenu();
	AppendMenu(hRoot,
		MF_POPUP,
		(UINT_PTR)pop4,
		L"帮助");
	AppendMenu(pop4,
		MF_STRING,
		IDM_SETTINGS,
		L"设置");
	AppendMenu(pop4,
		MF_STRING,
		IDM_ABOUTPROG,
		L"关于");


}



bool hash1(char* buffer,  int command = 0) {
	unsigned int tmp = 1;
	while (*buffer != '\0')
	{
		tmp *= (int)(*buffer);
		tmp %= BLOOMFILTERBITCOUNT;
		buffer++;
	}
	if (command == 1)
		*(dicthash + tmp % BLOOMFILTERBITCOUNT) = 1;
	return *(dicthash + tmp % BLOOMFILTERBITCOUNT) == 1;

}


bool hash2(char* buffer,  int command = 0) {
	unsigned int tmp = 0;
	while (*buffer != '\0')
	{
		tmp += (int)(*buffer);

		buffer++;
	}
	if (command == 1)
		*(dicthash + tmp % BLOOMFILTERBITCOUNT) = 1;
	return *(dicthash + tmp % BLOOMFILTERBITCOUNT) == 1;
}

bool hash3(char* buffer,  int command = 0) {
	unsigned int tmp = 1;
	while (*buffer != '\0')
	{
		tmp *= (int)(sqrt((double)*buffer));

		buffer++;
	}
	if (command == 1)
		*(dicthash + tmp % BLOOMFILTERBITCOUNT) = 1;
	return *(dicthash + tmp % BLOOMFILTERBITCOUNT) == 1;
}

bool hash4(char* buffer, int command = 0) {
	unsigned int tmp = 0;
	unsigned int sign = 1;
	while (*buffer != '\0')
	{
		tmp += sign * (int)(*buffer);
		sign *= -1;
		buffer++;
	}
	if (command == 1)
		*(dicthash + tmp % BLOOMFILTERBITCOUNT) = 1;
	return *(dicthash + tmp % BLOOMFILTERBITCOUNT) == 1;
}

int checkstring(char* buffer)
{
	return hash1(buffer) && hash2(buffer) && hash3(buffer) && hash4(buffer);
}



int importdict(TCHAR* path)
{

	FILE* fp;

	 int ret=_wfopen_s(&fp,path, _T("r"));
	 if (ret == 2)
	 {
		 MessageBox(NULL, L"NO specific dict file found!", _T("Error"), NULL);
		 return -1;
	 }
	if (fp == NULL)
	{

		wprintf(publicbuffer,"No dict file found");
		MessageBox(NULL, publicbuffer, _T("Error"), NULL);
		return -1;
	}
	rewind(fp);
	char checklegal[4] = { 0 };
	fread(checklegal, sizeof(char), 4, fp);
	if (strcmp(checklegal, "bfd"))
	{
		MessageBox(NULL, L"Please choose the dict file.", L"Error", NULL);
		return -1;
	}
	fread(&BLOOMFILTERBITCOUNT, sizeof(int), 1, fp);
	initspellchecker(BLOOMFILTERBITCOUNT);
	fread(dicthash, sizeof(unsigned int), BLOOMFILTERBITCOUNT, fp);

	fclose(fp);
	return 0;

}

int releasedict()
{
	free(dicthash);
	dicthash = NULL;
	BLOOMFILTERBITCOUNT = 0;
	return 0;
}

void initspellchecker(int bitcount)
{
	BLOOMFILTERBITCOUNT = bitcount;
	dicthash = (unsigned int*)malloc((BLOOMFILTERBITCOUNT) * sizeof(unsigned int));

	int i;
	for (i = 0; i < BLOOMFILTERBITCOUNT; i++)
	{
		*(dicthash + i) = 0;	
	}	
}
