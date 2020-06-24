#include "TheEditorcommon.h"

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

static TCHAR szWindowClass[] = _T("win32app");
static TCHAR childszWindowClass[] = _T("childwin32app");
static TCHAR szWindowTitle[] = _T("TheEditor (BloomFilter)");

//HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);
HWND mainwindow;
HWND re, underbar_hWnd, toolbar_hWnd;
HWND editdictwindow;
HWND childre, childre_button, childre_list, childre_trackbar;
HWND bits_length;

HMENU hRoot;

MenuCommand MenuCommandcoref;


int registerAllClass(HINSTANCE hInstance) {
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

	WNDCLASSEX childwcex;
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

	return 0;
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {


	InitCommonControls();
	LoadLibrary(TEXT("Msftedit.dll"));


	CreateMyMenu();

	if (registerAllClass(hInstance) == 1) {
		MessageBox(NULL, _T("Regist windows classes failed"), _T("win32"), NULL);//调用消息弹出框展示错误消息
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
	

		re = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT("点击“文件”以打开新文件.\n"),
		WS_VISIBLE | WS_CHILD | WS_VSCROLL |
		ES_MULTILINE | ES_LEFT | ES_NOHIDESEL | ES_AUTOVSCROLL /*| ES_READONLY*/,
		0, 0, 480, 400,
			mainwindow, NULL, hInstance, NULL);
		if (!re)
		{
			FatalAppExitA(0, "Couldn't create the rich text box,program exit!");
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;//画图结构体
	HDC hdc;//绘图上下文句柄
	
	switch (message) {
	
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_NEWFILE:				MenuCommandcoref.NewFilef();			break;
		case IDM_OPENFILE:				MenuCommandcoref.OpenFilef();			break;
		case IDM_SAVEFILE:				MenuCommandcoref.SaveFilef();			break;
		case IDM_FORMATFILE:			MenuCommandcoref.FormatFilef();			break;
		case IDM_CLOSEFILE:				MenuCommandcoref.CloseFilef();			break;
		case IDM_EXITPROG:				PostQuitMessage(0);						break;
		case IDM_OPENDICTFILE:			MenuCommandcoref.OpenDictFilef();		break;
		case IDM_REFRESHRUNNINGCODE:	MenuCommandcoref.RefreshRunningCodef(); break;
		case IDM_COMPILER:				MenuCommandcoref.CompilerFilef();		break;
		case IDM_GENERATOR:				MenuCommandcoref.GeneraterFilef();		break;
		case IDM_RUNNINGCODE:			MenuCommandcoref.RunningCodef();		break;

		case IDM_CREATEDICTFILE:
		{
			
			
			createdictwindows();

			break;
		}
		case IDM_ABOUTPROG: {    //https ://blog.csdn.net/kingsollyu/article/details/7492948

			MSGBOXPARAMS msg_info;
			memset(&msg_info, 0, sizeof(MSGBOXPARAMS));	
			msg_info.cbSize = sizeof(MSGBOXPARAMS);// 初始化结构体大小
			msg_info.hwndOwner = NULL;	// 拥有的窗口句柄，指定一个值
			msg_info.hInstance = GetModuleHandle(NULL);// 如果你不使用的图标资源，并不需要明确。
			msg_info.lpszText = L"数据结构课程设计--布隆过滤器\n2018218794温嘉昊";	// 指定的文本将显示在消息框
			msg_info.lpszCaption = L"关于此软件";// 消息框显示在标题指定标题的人	
			msg_info.dwStyle = MB_OK | MB_USERICON;// 消息框样式指定
			// 使用图标资源的人表示，为了使用这种说法，是一定要因素dwStyle	
			msg_info.lpszIcon = MAKEINTRESOURCE(IDI_ICON1);// MB_USERICON 你必须得到明确。
			// 定义消息框，根据打印信息。
			int result = MessageBoxIndirect(&msg_info);	// 在选定按钮的消息框的ID返回一个值。 
		
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

		}
		
	}

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);	
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
			//SendMessage(childre_trackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)SendMessage(childre_trackbar, TBM_GETPOS, 0, 0));
			settrackbar();
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

		EndPaint(hWnd, &ps);
		break;
	}

	case  WM_CLOSE:
	{
		//if (lstrcmp(GLOBALDICTFILENAME, L"") == 0)
		//{
		//	break;
		//}		
		

		MenuCommandcoref.savedictfile();

		DestroyWindow(editdictwindow);
		
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


/*参考文献： https://blog.csdn.net/feixiaoxing/article/details/78869359； https://www.cnblogs.com/iBinary/p/9581252.html */
int createdictwindows() {
	int ret;
	ret = MessageBox(NULL, L"是否利用已有字典文件?", L"打开字典文件", MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);
	if (ret != IDCANCEL)
	{
		//lstrcpy(GLOBALDICTFILENAME, L"");       //clean path in case close open file choose

		editdictwindow = CreateWindowExW(0, L"childwin32app", TEXT("编辑字典"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 480, 400,
			mainwindow, NULL, GetModuleHandle(NULL), NULL);

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

		childre_button = CreateWindowEx(0, L"BUTTON", L"添加",
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
			180, 220                         // position 
			, 220, 50,                        // size 
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
			(LPARAM)MAKELONG(64, 4096));  // min. & max. positions

		SendMessage(childre_trackbar, TBM_SETPAGESIZE,
			0, (LPARAM)4);                  // new spage size 

		SendMessage(childre_trackbar, TBM_SETSEL,
			(WPARAM)FALSE,                  // redraw flag 
			(LPARAM)MAKELONG(64, 4096));

		SendMessage(childre_trackbar, TBM_SETPOS,
			(WPARAM)TRUE,                   // redraw flag 
			(LPARAM)64);

		HWND length_label;
		length_label = CreateWindow(L"static", L"比特位数量:",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			50, 220, 100, 20,
			editdictwindow, NULL,
			(HINSTANCE)GetWindowLong(editdictwindow, GWL_HINSTANCE), NULL);
		//SetWindowText(length_label, L"BitLength:");


		bits_length = CreateWindow(L"static", L"0",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			130, 220, 40, 20,
			editdictwindow, NULL,
			(HINSTANCE)GetWindowLong(editdictwindow, GWL_HINSTANCE), NULL);
		//SetWindowText(bits_length, L"0");

		CreateWindow(L"static", L"新关键词:",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			50, 315, 70, 20,
			editdictwindow, NULL,
			(HINSTANCE)GetWindowLong(editdictwindow, GWL_HINSTANCE), NULL);




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
			if (MenuCommandcoref.openexistdictfile() == 1) {
				DestroyWindow(editdictwindow);
			}
		}
		else if (ret == IDNO)
		{
			if (MenuCommandcoref.opennewdictfile() == 1) {
				DestroyWindow(editdictwindow);
			}
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

void settrackbarvalue(int value) {

	WCHAR intbufferwhar[10];

	swprintf_s(intbufferwhar, L"%d", value);
	SetWindowText(bits_length, intbufferwhar);
	SendMessage(childre_trackbar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)value);

	return;
}

void settrackbar() {
	int newPos = SendMessage(childre_trackbar, TBM_GETPOS, 0, 0);
	int selStart = SendMessage(childre_trackbar, TBM_GETSELSTART, 0, 0);
	int selEnd = SendMessage(childre_trackbar, TBM_GETSELEND, 0, 0);

	if (newPos <= 64){
		settrackbarvalue(64);
	}
	else if (newPos < 128){
		settrackbarvalue(64);
	}
	else if (newPos < 256){
		settrackbarvalue(128);
	}
	else if (newPos < 512){
		settrackbarvalue(256);
	}
	else if (newPos < 1024){
		settrackbarvalue(512);
	}
	else if (newPos < 2048) {
		settrackbarvalue(1024);
	}
	else if (newPos < 4096) {
		settrackbarvalue(2048);
	}
	else if (newPos >= 4096) {
		settrackbarvalue(4096);
	}

	return;
}


void textcontent::settext(TCHAR* income) {
	SetWindowText(re, income);
}

int textcontent::getlength() {
	return GetWindowTextLength(re);
}

int textcontent::getlengthEX() {
	_gettextlengthex currenttextlen;
	currenttextlen.codepage = CP_ACP;
	return SendMessage(re, EM_GETTEXTLENGTHEX, (WPARAM)&currenttextlen, 0);
}

int textcontent::findnextbreak(int li) {
	return SendMessage(re, EM_FINDWORDBREAK, WB_MOVEWORDRIGHT, li);
}

void textcontent::getwordrange(_textrange &textrangevar) {
	SendMessage(re, EM_GETTEXTRANGE, 0, (LPARAM)&textrangevar);
}

void textcontent::cleartext() {
	TCHAR cleartext[] = L"";
	SetWindowText(re, cleartext);
}

void textcontent::gettext(TCHAR* tcharbuffer,int re_text_length) {
	GetWindowText(re, tcharbuffer, re_text_length + 1);
}

void textcontent::turnblue(_charrange rangevar) {
	SendMessage(re, EM_EXSETSEL, 0, (LPARAM)&rangevar);
	SendMessage(re, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&specialstyle);

}

void textcontent::turnnormal(_charrange rangevar) {
	SendMessage(re, EM_EXSETSEL, 0, (LPARAM)&rangevar);
	SendMessage(re, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&normalstyle);

}

int textcontent::gettrackbarvalue() {
	return SendMessage(childre_trackbar, TBM_GETPOS, 0, 0);
}

void textcontent::adddictword(TCHAR* tcharbuffer) {
	SendMessage(childre_list, LB_ADDSTRING, 0, (LPARAM)(tcharbuffer));
}

int textcontent::getdictwordnumber() {
	return SendMessage(childre_list, LB_GETCOUNT, 0, 0);
}

int textcontent::getdictwordlen(int index) {
	return SendMessage(childre_list, LB_GETTEXTLEN, index, 0);
}

void textcontent::getdictword(int i,TCHAR* tbuffer) {
	
	SendMessage(childre_list, LB_GETTEXT, i, (LPARAM)tbuffer);
}

