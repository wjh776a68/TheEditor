#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <Richedit.h>
#include<iostream> //file pointer,class
#include <Commctrl.h>
#include <malloc.h>  //calloc
#include<Commdlg.h>
#include<math.h>   //sqrt
#include<Shlobj.h> //选择文件夹对话框
//#include<afxwin.h>
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib, "comctl32.lib")
#include "resource.h"
#include "TheEditorcommon.h"




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

#define IDM_STATUSBAR           9000
#define IDM_CHILDBUTTON			10000
#define IDM_CHILDTRACKBAR		10001

#endif




static TCHAR szWindowClass[] = _T("win32app");
static TCHAR childszWindowClass[] = _T("childwin32app");
static TCHAR szWindowTitle[] = _T("TheEditor (BloomFilter)");

//HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);
HWND mainwindow;
HWND re, underbar_hWnd,toolbar_hWnd;
HWND editdictwindow;
HWND childre, childre_button, childre_list, childre_trackbar;

WNDCLASSEX childwcex;
HMENU hRoot;

TCHAR GLOBALDICTFILENAME[MAX_PATH] = { 0 };
TCHAR publicbuffer[MAXLEN];
TCHAR GLOBALFILENAME[PATHMAXLEN];
TCHAR GLOBALRUNNINGPATH[PATHMAXLEN];
//TCHAR FILENAME[PATHMAXLEN];

int Bloomclass::GetBloomlen(){
	return bloomfilter.BLOOMFILTERBITCOUNT;
}

int Bloomclass::SetBloomlen(int len) {
	Bloomclass::releasedict();
	return Bloomclass::initspellchecker(len);
}

void CreateMyMenu();//创建菜单
//int create(LPCTSTR pszFile);
int open(LPCTSTR pszFile);
int save(LPCTSTR pszFile);
int OpenDictFilef();
int CreateDictFile();
Bloomclass BloomCoref;

/*设置richtextbox的样式*/
void setColor(HWND richEditCtrl, COLORREF textColor, COLORREF bgColor)
{

	CHARFORMAT2 cf;

	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_EFFECTS2; // I'm setting only the style information
	
	cf.crTextColor = textColor;
	cf.crBackColor = bgColor;
	cf.dwEffects = CFE_BOLD;
	

	SendMessage(richEditCtrl, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);


	PVOID res = 0;
	AddFontResourceEx(L"Font\\Roboto-Regular.ttf", FR_PRIVATE, res);

	//CFont m_font;
	LOGFONT lf; // Used to create the CFont.

	memset(&lf, 0, sizeof(LOGFONT));   // Clear out structure.
	lf.lfHeight = 20;                  // Request a 20-pixel-high font
	lstrcpy(lf.lfFaceName, L"Roboto");    //    with face name "Arial".
//	m_font.CreateFontIndirect(&lf);    // Create the font.

	// Use the font to paint a control. This code assumes
	// a control named IDC_TEXT1 in the dialog box.
	//re->SetFont(&m_font);
	SendMessage(re, WM_SETFONT, (WPARAM)CreateFontIndirect(&lf), false);
}

/*参考文献： https://blog.csdn.net/zuishikonghuan/article/details/47441163; https://blog.walterlv.com/post/how-to-write-win32-file-dialog-filter.html */

int OpenFilef_old(HWND hWnd)
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
		
		if (file_size >= 4294967295)
		{
			MessageBox(NULL, L"Sorry but the file cannot bigger than 4294967295\n,Please choose another file to try.", NULL, NULL);
			return -1;
		}

		char* charbuffer = (CHAR*)calloc(file_size + 1, sizeof(CHAR));
		TCHAR* tcharbuffer = (TCHAR*)calloc(file_size + 1, sizeof(TCHAR));

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

int OpenFilef()
{
	lstrcpy(GLOBALFILENAME, getfilepath(1));
	if (lstrcmp(GLOBALFILENAME,L"")!=0)
	{

		FILE* fp;
		_wfopen_s(&fp, GLOBALFILENAME, L"r");

		if (fp == NULL)
		{
			MessageBox(NULL, L"Error file path", NULL, NULL);
			return -1;
		}

		//lstrcpy(GLOBALFILENAME, filename);

		fseek(fp, 0L, SEEK_END);
		int file_size = ftell(fp);
		rewind(fp);

		if (file_size >= 4294967295)
		{
			MessageBox(NULL, L"Sorry but the file cannot bigger than 4294967295\n,Please choose another file to try.", NULL, NULL);
			return -1;
		}

		char* charbuffer = (CHAR*)calloc(file_size + 1, sizeof(CHAR));
		TCHAR* tcharbuffer = (TCHAR*)calloc(file_size + 1, sizeof(TCHAR));

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



int SaveFilef_old(HWND hWnd)
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

		fwrite(charbuffer, sizeof(char), re_text_length + 1, fp);
		fclose(fp);

		return 1;
	}
	return 0;
}

int SaveFilef()
{
	lstrcpy(GLOBALFILENAME, getfilepath(0));
	
	if (lstrcmp(GLOBALFILENAME,L"")!=0)
	{

		int re_text_length = GetWindowTextLength(re);
		TCHAR* tcharbuffer = (TCHAR*)calloc(re_text_length + 1, sizeof(TCHAR));
		GetWindowText(re, tcharbuffer, re_text_length + 1);

		char* charbuffer = (CHAR*)calloc(re_text_length + 1, sizeof(CHAR));

		size_t convertedChars = 0;
		wcstombs_s(&convertedChars, charbuffer, re_text_length + 1, tcharbuffer, re_text_length + 1);

		FILE* fp;
		_wfopen_s(&fp, GLOBALFILENAME, L"w+");

		if (fp == NULL)
		{
			MessageBox(NULL, L"Error file path", NULL, NULL);
			return -1;
		}

		//lstrcpy(GLOBALFILENAME, FILENAME);

		fwrite(charbuffer, sizeof(char), re_text_length + 1, fp);
		fclose(fp);

		return 1;
	}
	return 0;
}

/*参考文献：https://docs.microsoft.com/en-us/windows/win32/controls/rich-edit-controls */
int FormatFilef()
{
	
	while (BloomCoref.GetBloomlen() == 0)
	{
		if (OpenDictFilef() == 0)
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
	specialstyle.crTextColor = RGB(0, 0, 255);
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

			if (BloomCoref.checkstring(textrangevar.lpstrText))
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

int OpenDictFilef_old(HWND hWnd)
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

		BloomCoref.importdict(filename);
		return 1;
	}
	return 0;
}

int OpenDictFilef()
{
	lstrcpy(GLOBALDICTFILENAME, getfilepath(1));
	if (lstrcmp(GLOBALDICTFILENAME, L"") == 0)
	{
		MessageBox(NULL, L"Cannot find the file", NULL, NULL);
	}
	else
	{
		BloomCoref.importdict(GLOBALDICTFILENAME);
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//设置鼠标样式
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//设置画刷背景色
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, _T("call to RegisterClassEX failed."), _T("win32"), NULL);
		return 1;
	}

	//hInst = hInstance;
	//g_hInst = hInstance;
	//HWND hWnd
		mainwindow= CreateWindow(
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
	if (!mainwindow) {//创建窗口失败
		MessageBox(NULL, _T("call to CreateWindow failed"), _T("win32"), NULL);//调用消息弹出框展示错误消息
		return 1;
	}
	

		re = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT("Hi!  Click to open or create file on the \"file\" bar above.\n"),
		WS_VISIBLE | WS_CHILD | WS_VSCROLL |
		ES_MULTILINE | ES_LEFT | ES_NOHIDESEL | ES_AUTOVSCROLL /*| ES_READONLY*/,
		0, 0, 480, 400,
			mainwindow, NULL, hInstance, NULL);
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
		childwcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));//参考文献: https://blog.csdn.net/baidu_24190413/article/details/47430551;
		childwcex.hCursor = LoadCursor(NULL, IDC_ARROW);//设置鼠标样式
		childwcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//设置画刷背景色
		childwcex.lpszMenuName = NULL;
		childwcex.lpszClassName = childszWindowClass;
		childwcex.hIconSm = LoadIcon(childwcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
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
	
	underbar_hWnd = CreateWindowEx(
		0,                       // no extended styles
		STATUSCLASSNAME,         // name of status bar class
		(PCTSTR)NULL,           // no text when first created
		SBARS_SIZEGRIP |         // includes a sizing grip
		WS_CHILD | WS_VISIBLE,   // creates a visible child window
		0, 0, 0, 0,              // ignores size and position
		mainwindow,              // handle to parent window
		(HMENU)IDM_STATUSBAR,       // child window identifier
		hInstance,                   // handle to application instance
		NULL);                   // no window creation data

	if (!underbar_hWnd)
	{
		MessageBox(NULL, _T("create underbar failed."), _T("TheEditor"), NULL);
		return 1;
	}
	int paParts[6] = { 100,-1 };
	SendMessage(underbar_hWnd, SB_SETPARTS, 6, (LPARAM)paParts);

	HIMAGELIST g_hImageList = NULL;
	const int ImageListID = 0;
	const int numButtons = 3;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_BUTTON;


	toolbar_hWnd = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | WS_VISIBLE, 0, 100, 100, 30,
		underbar_hWnd, NULL, hInstance, NULL);

	if (!toolbar_hWnd)
	{
		MessageBox(NULL, _T("create toolbar failed."), _T("TheEditor"), NULL);
		return 1;
	}

	// Create the image list.
	g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
		numButtons, 0);

	// Set the image list.
	SendMessage(toolbar_hWnd, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)g_hImageList);

	// Load the button images.
	SendMessage(toolbar_hWnd, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Initialize button info.
	// IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.

	TBBUTTON tbButtons[numButtons] =
	{
		{ MAKELONG(STD_UNDO,  ImageListID), IDM_NEWFILE,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Undo" },
		{ MAKELONG(STD_REDOW, ImageListID), IDM_OPENFILE, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Redo"},
		{ MAKELONG(STD_HELP, ImageListID), IDM_SAVEFILE, 0,               buttonStyles, {0}, 0, (INT_PTR)L"Format"}
	};

	// Add buttons.
	SendMessage(toolbar_hWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(toolbar_hWnd, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Resize the toolbar, and then show it.
	//SendMessage(toolbar_hWnd, TB_AUTOSIZE, 0, 0);
	ShowWindow(toolbar_hWnd, TRUE);



	ShowWindow(mainwindow, nCmdShow);//显示窗口
	ShowWindow(re, nCmdShow);

	UpdateWindow(mainwindow);//修改窗口
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
	case IDYES:return SaveFilef();
	case IDNO:return NewFilef();
	default:return 0;

	}
	lstrcpy(GLOBALFILENAME, L"");
	SetWindowText(re, L"");
}

int CompilerFilef()
{
	return 0;
}

/*参考文献：https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa； https://forum.xojo.com/19555-error-123-in-api-call-createprocessa/0； https://stackoverflow.com/questions/17703721/how-to-execute-another-exe-from-a-c-program-in-windows */
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

	if (CreateProcess(NULL,pathbuffer, NULL,NULL,TRUE,HIDE_WINDOW,NULL,NULL,&si,&pi))
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
		case IDM_OPENFILE:OpenFilef(); break;
		case IDM_SAVEFILE:SaveFilef(); break;
		case IDM_FORMATFILE:FormatFilef(); break;
		case IDM_CLOSEFILE:CloseFilef(); break;
		case IDM_EXITPROG:PostQuitMessage(0); break;
		case IDM_OPENDICTFILE:OpenDictFilef(); break;
		case IDM_REFRESHRUNNINGCODE:RefreshRunningCodef(); break;
		case IDM_COMPILER:CompilerFilef(); break;
		case IDM_GENERATOR:GeneraterFilef(); break;
		case IDM_RUNNINGCODE:RunningCodef(); break;

		case IDM_CREATEDICTFILE:
		{
			
			/*参考文献： https://blog.csdn.net/feixiaoxing/article/details/78869359； https://www.cnblogs.com/iBinary/p/9581252.html */
			

			int ret;
			ret = MessageBox(NULL, L"Do you want to open an exist file?", L"Open or Create", MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);
			if (ret !=IDCANCEL)
			{
				lstrcpy(GLOBALDICTFILENAME, L"");       //clean path in case close open file choose

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

				/*参考文献： https://bbs.csdn.net/topics/110139909； https://stackoom.com/question/97cp/%E7%AE%80%E5%8D%95%E7%9A%84Win-%E8%BD%A8%E8%BF%B9%E6%A0%8F； https://docs.microsoft.com/zh-cn/windows/win32/controls/trackbar-control-reference?redirectedfrom=MSDN*/
				childre_trackbar = CreateWindowEx(
					0,                               // no extended styles 
					TRACKBAR_CLASS,                  // class name 
					L"Trackbar Control",              // title (caption) 
					WS_CHILD |
					WS_VISIBLE |
					TBS_AUTOTICKS |
					TBS_ENABLESELRANGE,              // style 
					150, 220                         // position 
					, 250, 50,                        // size 
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
					editdictwindow, NULL, NULL/* GetModuleHandle(NULL)*/, NULL);
				if (!childre_list)
				{
					FatalAppExitA(0, "Couldn't create the list box,program exit!");
					return 1;
				}

				if (ret == IDYES)
				{
					lstrcpy(GLOBALFILENAME, getfilepath(1));
					if (lstrcmp(GLOBALFILENAME, L"") != 0)
					{
						DestroyWindow(editdictwindow);
						return -1;
					}
					else
					{
						FILE* fp;
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
						int intbuffer;
						fread(&intbuffer, sizeof(int), 1, fp);
						//BloomCoref.SetBloomlen(intbuffer);
						SendMessage(childre_trackbar, TBM_SETPOS,
							(WPARAM)TRUE,                   // redraw flag 
							(LPARAM)intbuffer);
						//BloomCoref.SetBloomlen(0);
						intbuffer = 0;
						fclose(fp);

						TCHAR filenamebuffer[260];
						lstrcpy(filenamebuffer, GLOBALDICTFILENAME);

						lstrcat(filenamebuffer, L"_config");
						_wfopen_s(&fp, filenamebuffer, L"r");
						if (fp == NULL)
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
							int readlen = fread(charbuffer, sizeof(char), MAXLEN, fp);
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
					lstrcpy(GLOBALFILENAME, getfilepath(0));

					if (lstrcmp(GLOBALFILENAME,L"")!=0)
					{
						DestroyWindow(editdictwindow);
						return -1;
					}
					else
					{
						BloomCoref.SetBloomlen(0);

					}
				}

			}
			else
			{
				return 0;
			}

			break;
		}
		case IDM_ABOUTPROG: {    //https ://blog.csdn.net/kingsollyu/article/details/7492948

			MSGBOXPARAMS msg_info;
			memset(&msg_info, 0, sizeof(MSGBOXPARAMS));

			// 初始化结构体大小
			msg_info.cbSize = sizeof(MSGBOXPARAMS);

			// 拥有的窗口句柄，指定一个值
			msg_info.hwndOwner = NULL;

			// 如果你不使用的图标资源，并不需要明确。
			msg_info.hInstance = GetModuleHandle(NULL);

			// 指定的文本将显示在消息框
			msg_info.lpszText = L"The Editor\nVersion 1.0.0.0\nCopyright 2020 wjh776a68";

			// 消息框显示在标题指定标题的人
			msg_info.lpszCaption = L"About The Editor";

			// 消息框样式指定
			msg_info.dwStyle = MB_OK | MB_USERICON;

			// 使用图标资源的人表示，为了使用这种说法，是一定要因素dwStyle
			// MB_USERICON 你必须得到明确。
			msg_info.lpszIcon = MAKEINTRESOURCE(IDI_ICON1);

			// 定义消息框，根据打印信息。
			int result = MessageBoxIndirect(&msg_info);

			// 在选定按钮的消息框的ID返回一个值。 
			//return result;


			//MessageBox(NULL, L"The Editor\nVersion 1.0.0.0\nCopyright 2019 wjh776a68", L"About The Editor", NULL); break;
			}
		}
	}
	case WM_SIZE:
	{
		/*参考文献：https://bbs.csdn.net/topics/290055553 */
		if (hWnd == mainwindow)
		{
			LPRECT rect = (LPRECT)malloc(sizeof(tagRECT));
			GetClientRect(hWnd, rect);
			SetWindowPos(re, HWND_TOP, 0, 0, rect->right, rect->bottom - 30, SWP_NOMOVE |  SWP_NOZORDER);
			
			SetWindowPos(underbar_hWnd, HWND_TOP, 100, rect->bottom - 30, rect->right-100, 30, SWP_NOMOVE | SWP_NOZORDER);
			
			SetWindowPos(toolbar_hWnd, HWND_TOP, 0, rect->bottom - 30, 100, 30, SWP_NOMOVE | SWP_NOZORDER);
			//SendMessage(underbar_hWnd, SB_SETICON, 0, (LPARAM)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_REDO), IMAGE_ICON, 20, 20,LR_SHARED | LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
			//SendMessage(underbar_hWnd, SB_SETICON, 1, (LPARAM)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_UNDO), IMAGE_ICON, 20, 20, LR_SHARED | LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
			//SendMessage(underbar_hWnd, SB_SETICON, 0, (LPARAM)LoadImage(NULL, L"Images\\undo.png", IMAGE_BITMAP, 30, 30, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE));
			//SendMessage(underbar_hWnd, SB_SETICON, 1, (LPARAM)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_REDO), IMAGE_BITMAP, 30, 30, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
			//HANDLE tmp=  LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_REDO), IMAGE_BITMAP, 30, 30, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
		//	int ret=GetLastError();
		//	Sleep(100);
		
			//SendMessage(underbar_hWnd, SB_SETICON, 2, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
			


		}
		
	}
	//case WM_MENUSELECT:
	//{
		//if (hWnd == underbar_hWnd)
		//{
		//UINT wIDs[] = { MH_BASE ,0x1100,0,0};
		//	MenuHelp(WM_MENUSELECT, wParam, lParam, GetMenu(hWnd), GetModuleHandle(NULL), underbar_hWnd, wIDs);
		//}
	//}
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
	{

	
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 310, L"New keyword:", wcslen(L"New keyword:"));
		//char charbuffer[5];
		//TCHAR tcharbuffer[5];
		//int i = SendMessage(childre_trackbar, TBM_GETPOS, 0, 0);
		//size_t tmp;
		//sprintf_s(charbuffer, "%d", i);
		
		//mbstowcs_s(&tmp, tcharbuffer, strlen(charbuffer) + 1, charbuffer, strlen(charbuffer) + 1);
		TextOut(hdc, 20, 220, L"Bits of bloomfilter", wcslen(L"Bits of bloomfilter"));
		//TextOut(hdc, 80, 220, tcharbuffer, lstrlen(tcharbuffer));
		EndPaint(hWnd, &ps);
		break;
	}
	case  WM_CLOSE:
	{
		if (lstrcmp(GLOBALDICTFILENAME, L"") == 0)
		{
			break;
		}
			
		

		int i;
		FILE* fp_config;
		int intbuffer;
		TCHAR filenamebuffer[PATHMAXLEN];
		lstrcpy(filenamebuffer, GLOBALDICTFILENAME);
		_wfopen_s(&fp_config, lstrcat(filenamebuffer, L"_config"), L"w+");

		size_t tmp;
		int childre_list_count = SendMessage(childre_list, LB_GETCOUNT, 0, 0);
		int childre_trackbar_pos = SendMessage(childre_trackbar, TBM_GETPOS, 0, 0);
		intbuffer = childre_trackbar_pos;


		BloomCoref.initspellchecker(intbuffer);
		if (fp_config != NULL)
		{
			fwrite("bfdc", sizeof(char), 5, fp_config);

			for (i = 0; i < childre_list_count; i++)
			{
				TCHAR* tbuffer = (TCHAR*)malloc(sizeof(TCHAR) * SendMessage(childre_list, LB_GETTEXTLEN, i, 0));
				SendMessage(childre_list, LB_GETTEXT, i, (LPARAM)tbuffer);
				char* buffer = (char*)malloc(sizeof(char) * (wcslen(tbuffer) + 1));
				wcstombs_s(&tmp, buffer, wcslen(tbuffer) + 1, tbuffer, wcslen(tbuffer) + 1);
				fwrite(buffer, sizeof(char), MAXLEN, fp_config);
				BloomCoref.AddString(buffer);
			}
			fclose(fp_config);
		}
		BloomCoref.exportdict(GLOBALDICTFILENAME);

		BloomCoref.releasedict();
		DestroyWindow(editdictwindow);
		lstrcpy(GLOBALDICTFILENAME, L"");
		break;

	}	


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

/***********************************************
	函数名称: CreateMyMenu()
	函数功能: 创建程序主窗口的菜单。
	传入参数: 无
	返回值: 无
	参考文献: https://blog.csdn.net/tcjiaan/article/details/8510260；
************************************************/
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


/***********************************************
	函数名称: Bloomclass::hash1(char* buffer, int command = 0)
	函数功能: 将字符串按照ascii码变为数字，并得到其积，最后对比特长度取余，如果int参数为1则将bloomfilter对应标志位置1，否则跳过。之后查询该位置是否为1.
	传入参数: char*目标字符串，int 0：查询，1：更改
	返回值: 如果是1返回true，否则为false。
************************************************/
bool Bloomclass::hash1(char* buffer,  int command = 0) {
	unsigned int tmp = 1;
	while (*buffer != '\0')
	{
		tmp *= (int)(*buffer);
		tmp %= bloomfilter.BLOOMFILTERBITCOUNT;
		buffer++;
	}
	if (command == 1)
		*(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) = 1;
	return *(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) == 1;

}

/***********************************************
	函数名称: Bloomclass::hash2(char* buffer, int command = 0)
	函数功能: 将字符串按照ascii码变为数字，并得到其和，最后对比特长度取余，如果int参数为1则将bloomfilter对应标志位置1，否则跳过。之后查询该位置是否为1.
	传入参数: char*目标字符串，int 0：查询，1：更改
	返回值: 如果是1返回true，否则为false。
************************************************/
bool Bloomclass::hash2(char* buffer,  int command = 0) {
	unsigned int tmp = 0;
	while (*buffer != '\0')
	{
		tmp += (int)(*buffer);

		buffer++;
	}
	if (command == 1)
		*(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) = 1;
	return *(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) == 1;
}

/***********************************************
	函数名称: Bloomclass::hash3(char* buffer, int command = 0)
	函数功能: 将字符串按照ascii码变为数字，并得到平方根取整的和，最后对比特长度取余，如果int参数为1则将bloomfilter对应标志位置1，否则跳过。之后查询该位置是否为1.
	传入参数: char*目标字符串，int 0：查询，1：更改
	返回值: 如果是1返回true，否则为false。
************************************************/
bool Bloomclass::hash3(char* buffer,  int command = 0) {
	unsigned int tmp = 1;
	while (*buffer != '\0')
	{
		tmp *= (int)(sqrt((double)*buffer));

		buffer++;
	}
	if (command == 1)
		*(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) = 1;
	return *(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) == 1;
}

/***********************************************
	函数名称: Bloomclass::hash4(char* buffer, int command = 0)
	函数功能: 将字符串按照ascii码变为数字，并按+-+-。。。顺序依次加减，最后对比特长度取余，如果int参数为1则将bloomfilter对应标志位置1，否则跳过。之后查询该位置是否为1.
	传入参数: char*目标字符串，int 0：查询，1：更改
	返回值: 如果是1返回true，否则为false。
************************************************/
bool Bloomclass::hash4(char* buffer, int command = 0) {
	unsigned int tmp = 0;
	unsigned int sign = 1;
	while (*buffer != '\0')
	{
		tmp += sign * (int)(*buffer);
		sign *= -1;
		buffer++;
	}
	if (command == 1)
		*(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) = 1;
	return *(bloomfilter.dicthash + tmp % bloomfilter.BLOOMFILTERBITCOUNT) == 1;
}

/***********************************************
	函数名称: Bloomclass::AddString(char* buffer)
	函数功能: 添加目标字符串到bloomfilter的特征位为1
	传入参数: char*目标字符串
	返回值: 如果符合返回true，否则为false。
************************************************/
bool Bloomclass::AddString(char* buffer)
{
	return hash1(buffer,1) && hash2(buffer,1) && hash3(buffer,1) && hash4(buffer,1);
}

/***********************************************
	函数名称: Bloomclass::checkstring(char* buffer)
	函数功能: 检查目标字符串是否在bloomfilter的特征位为1
	传入参数: char*目标字符串
	返回值: 如果符合返回true，否则为false。
************************************************/
bool Bloomclass::checkstring(char* buffer)
{
	return hash1(buffer) && hash2(buffer) && hash3(buffer) && hash4(buffer);
}

/***********************************************
	函数名称: Bloomclass::exportdict(TCHAR* path)
	函数功能: 将bloomfilter结构体的二进制位数据导出到目标路径
	传入参数: TCHAR*文件路径
	返回值: 如果操作完成返回的bloomfilter的二进制位的个数，否则为-1。
************************************************/
int Bloomclass::exportdict(TCHAR* path)
{
	
	FILE* fp;
	_wfopen_s(&fp, GLOBALDICTFILENAME, L"w+");
	
	if (fp != NULL)
	{
		char checklegal[] = "bfd";
		fwrite(checklegal, sizeof(char), 4, fp);
		fwrite(&(bloomfilter.BLOOMFILTERBITCOUNT), sizeof(int), 1, fp);
		fwrite(bloomfilter.dicthash, sizeof(unsigned int), bloomfilter.BLOOMFILTERBITCOUNT, fp);
		fclose(fp);
		return bloomfilter.BLOOMFILTERBITCOUNT;
	}
	return -1;
}

/***********************************************
	函数名称: Bloomclass::importdict(TCHAR* path)
	函数功能: 向bloomfilter结构体导入来自指定路径文件内的二进制位数据，这个文件的开头以“bfd”开头，之后以一个int表示所占用的二进制位的个数
	传入参数: TCHAR*类型的路径
	返回值: 如果操作完成返回0，否则为-1。
************************************************/
int Bloomclass::importdict(TCHAR* path)
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
	fread(&(bloomfilter.BLOOMFILTERBITCOUNT), sizeof(int), 1, fp);
	initspellchecker(bloomfilter.BLOOMFILTERBITCOUNT);
	fread(bloomfilter.dicthash, sizeof(unsigned int), bloomfilter.BLOOMFILTERBITCOUNT, fp);

	fclose(fp);
	return 0;

}


/***********************************************
	函数名称: Bloomclass::releasedict(int bitcount)
	函数功能: 释放bloomfilter结构体所占用的内存
	传入参数: 无
	返回值: 如果操作完成返回0，否则不为0。
************************************************/
int Bloomclass::releasedict()
{
	free(bloomfilter.dicthash);
	bloomfilter.dicthash = NULL;
	bloomfilter.BLOOMFILTERBITCOUNT = 0;
	return bloomfilter.BLOOMFILTERBITCOUNT;
}


/***********************************************
	函数名称: Bloomclass::initspellchecker(int bitcount)
	函数功能: 初始化bloomfilter结构体
	传入参数: 需要的bloomfilter的二进制位个数，默认64
	返回值: 操作成功返回申请到的bloomfilter的二进制位个数。
************************************************/

int Bloomclass::initspellchecker(int bitcount=64)
{
	bloomfilter.BLOOMFILTERBITCOUNT = bitcount;
	bloomfilter.dicthash = (unsigned int*)malloc((bloomfilter.BLOOMFILTERBITCOUNT) * sizeof(unsigned int));

	int i;
	for (i = 0; i < bloomfilter.BLOOMFILTERBITCOUNT; i++)
	{
		*(bloomfilter.dicthash + i) = 0;
	}	
	return bloomfilter.BLOOMFILTERBITCOUNT;
}
