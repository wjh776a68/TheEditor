#include"MenuCommand.h"

/*参考文献： https://blog.csdn.net/zuishikonghuan/article/details/47441163; https://blog.walterlv.com/post/how-to-write-win32-file-dialog-filter.html */

Bloomclass BloomCoref(0);
FileDialog FileDialogcoref;
textcontent textcontentcore;

int MenuCommand::OpenFilef()
{
	lstrcpy(GLOBALFILENAME, FileDialogcoref.getfilepath(FILE_DIALOG_CMD_OPEN));
	if (lstrcmp(GLOBALFILENAME, L"") != 0)
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

		//SetWindowText(re, tcharbuffer);
		textcontentcore.settext(tcharbuffer);
		return 1;

	}
	return 0;
}


/*设置richtextbox的样式*//*
void MenuCommand::setColor(HWND richEditCtrl, COLORREF textColor, COLORREF bgColor)
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
}*/

int MenuCommand::SaveFilef()
{
	lstrcpy(GLOBALFILENAME, FileDialogcoref.getfilepath(FILE_DIALOG_CMD_CREATE));

	if (lstrcmp(GLOBALFILENAME, L"") != 0)
	{

		int re_text_length = textcontentcore.getlength();
		TCHAR* tcharbuffer = (TCHAR*)calloc(re_text_length + 1, sizeof(TCHAR));
		textcontentcore.gettext(tcharbuffer, re_text_length);

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
int MenuCommand::FormatFilef()
{

	while (BloomCoref.GetBloomlen() == 0)
	{
		if (OpenDictFilef() == 0)
		{
			return -1;
		}



	}

	long len, li = 0, ni = 0;
	len = textcontentcore.getlengthEX();

	do
	{

		ni = textcontentcore.findnextbreak(li);

		//do something that find the word between two porition
		if (ni < len)
		{
			_textrange textrangevar;
			_charrange rangevar;
			char wordcontent[MAXLEN * MAXLEN] = { 0 };
			//TCHAR tcharwordcontent[MAXLEN];
			LPSTR wordcontentpointer = wordcontent;
			rangevar.cpMin = li;
			rangevar.cpMax = ni;
			textrangevar.chrg = rangevar;
			textrangevar.lpstrText = wordcontentpointer;
			//SendMessage(re, EM_GETTEXTRANGE, 0, (LPARAM)&textrangevar);
			int returncount = textcontentcore.getwordrange(textrangevar);

			//size_t maxlen = MAXLEN;
			int i_withignore = 2;
			int i = 2;
			while (i_withignore < 2 * (rangevar.cpMax - rangevar.cpMin)/*textrangevar.lpstrText[i] != '\0'  &&(isalpha(textrangevar.lpstrText[i]) || isdigit(textrangevar.lpstrText[i]))*/)
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

			if (returncount < MAXLEN){
				if (BloomCoref.checkstring(textrangevar.lpstrText)){
					textcontentcore.turnblue(rangevar);
					//SendMessage(re, EM_EXSETSEL, 0, (LPARAM)&rangevar);
					//SendMessage(re, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&specialstyle);

				}
				else{
					textcontentcore.turnnormal(rangevar);
					//SendMessage(re, EM_EXSETSEL, 0, (LPARAM)&rangevar);
					//SendMessage(re, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&normalstyle);
				}
			}
			
			li = ni;
		}

	} while (ni < len);
	
	return 0;
}

int MenuCommand::OpenDictFilef()
{
	lstrcpy(GLOBALDICTFILENAME, FileDialogcoref.getfilepath(FILE_DIALOG_CMD_OPEN));
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


int MenuCommand::NewFilef()
{
	lstrcpy(GLOBALFILENAME, L"");
	//SetWindowText(re, L"");
	textcontentcore.cleartext();
	return 0;
}

int MenuCommand::CloseFilef()
{
	int ret = MessageBox(NULL, L"是否保存该文件?", L"是否保存", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (ret)
	{
	case IDYES:return SaveFilef();
	case IDNO:return NewFilef();
	default:return 0;

	}
	lstrcpy(GLOBALFILENAME, L"");
	//SetWindowText(re, L"");
	textcontentcore.cleartext();
}

int MenuCommand::CompilerFilef()//TO_DO
{
	MenuCommand::GeneraterFilef();
	return 0;
}

/*参考文献：https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa； https://forum.xojo.com/19555-error-123-in-api-call-createprocessa/0； https://stackoverflow.com/questions/17703721/how-to-execute-another-exe-from-a-c-program-in-windows */
int MenuCommand::GeneraterFilef()
{
	if (lstrcmp(GLOBALFILENAME, L"") == 0)
	{
		MessageBox(NULL, L"请首先打开一个源码文件!", NULL, NULL);
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

	TCHAR* GENERATENAME = (TCHAR*)calloc(lstrlen(GLOBALFILENAME) + 1, sizeof(TCHAR));
	lstrcpy(GENERATENAME, GLOBALFILENAME);

//start https://blog.csdn.net/gao199108/article/details/77965182

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
//end

	TCHAR argv[PATHMAXLEN];
	lstrcpy(argv, L" \"");

	lstrcat(argv, GLOBALFILENAME);
	lstrcat(argv, L"\" -o \"");
	lstrcat(argv, GENERATENAME);
	lstrcat(argv, L"\"");
	lstrcat(pathbuffer, argv);

	if (CreateProcess(NULL, pathbuffer, NULL, NULL, TRUE, HIDE_WINDOW, NULL, NULL, &si, &pi))
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

int MenuCommand::RunningCodef()
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
	TCHAR CodeExecuteprogpath[] = L"\\ConsoleAppExecuter.exe";
	lstrcpy(pathbuffer, GLOBALRUNNINGPATH);
	lstrcat(pathbuffer, CodeExecuteprogpath);
#else
	TCHAR CodeExecuteprogpath[] = L"\\ConsoleAppExecuter.exe";
#endif

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	TCHAR* GENERATENAME = (TCHAR*)calloc(lstrlen(GLOBALFILENAME) + 1, sizeof(TCHAR));
	lstrcpy(GENERATENAME, GLOBALFILENAME);

//start https://blog.csdn.net/gao199108/article/details/77965182

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

//end

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

int MenuCommand::RefreshRunningCodef()
{
	if (!GeneraterFilef())
	{
		return RunningCodef();
	}
	return -1;

}

int MenuCommand::opennewdictfile() {
	lstrcpy(GLOBALDICTFILENAME, FileDialogcoref.getfilepath(FILE_DIALOG_CMD_CREATE));

	if (lstrcmp(GLOBALDICTFILENAME, L"") == 0)
	{
		
		return 1;
	}
	else
	{
		BloomCoref.SetBloomlen(0);
		return 0;
	}
}

int MenuCommand::openexistdictfile() {
	lstrcpy(GLOBALDICTFILENAME, FileDialogcoref.getfilepath(FILE_DIALOG_CMD_OPEN));
	if (lstrcmp(GLOBALDICTFILENAME, L"") == 0)
	{	
		return 1;
	}
	else
	{
		FILE* fp;
		_wfopen_s(&fp, GLOBALDICTFILENAME, L"r");
		if (fp == NULL)
		{
			MessageBox(NULL, L"Error finding the dict file.", NULL, NULL);
			//fclose(fp);
			return 1;
		}
		char checklegal[5] = { 0 };
		fread(checklegal, sizeof(char), 4, fp);
		if (strcmp(checklegal, "bfd"))
		{
			MessageBox(NULL, L"Please choose the dict file.", L"Error", NULL);
			fclose(fp);			
			return 1;

		}
		int intbuffer;
		fread(&intbuffer, sizeof(int), 1, fp);
		settrackbarvalue(intbuffer);
		
		intbuffer = 0;
		fclose(fp);

		TCHAR filenamebuffer[260];
		lstrcpy(filenamebuffer, GLOBALDICTFILENAME);

		lstrcat(filenamebuffer, L"_config");
		_wfopen_s(&fp, filenamebuffer, L"r");
		if (fp == NULL)
		{
			MessageBox(NULL, L"Error finding the _config file.", NULL, NULL);	
			//fclose(fp);
			return 1;
		}
		//char checklegal2[5] = { 0 };
		fread(checklegal, sizeof(char), 5, fp);
		if (strcmp(checklegal, "bfdc"))
		{
			MessageBox(NULL, L"Please choose the dict config file,or the dict file could only be used to read", L"Error", NULL);
			fclose(fp);		
			return 1;
		}

		int fp_config_length;
		fread(&fp_config_length, sizeof(int), 1, fp);

		while (!feof(fp))
		//for(int i=0;i<fp_config_length;i++)
		{
			char charbuffer[MAXLEN];
			size_t convertedChars = 0;
			TCHAR tcharbuffer[MAXLEN];
			int readlen = fread(charbuffer, sizeof(char), MAXLEN, fp);
			//if (readlen != MAXLEN)
			//{
				//break;
			//}
			mbstowcs_s(&convertedChars, tcharbuffer, charbuffer, MAXLEN);
			if (!feof(fp)) {
				textcontentcore.adddictword(tcharbuffer);
			}

		}

		fclose(fp);
		return 0;
	}
}

void MenuCommand::savedictfile() {
	if (lstrcmp(GLOBALDICTFILENAME, L"") == 0 || GLOBALDICTFILENAME[0]==52428)
	{
		return;
	}
	else {
		int i;
		FILE* fp_config;
		int intbuffer;
		TCHAR filenamebuffer[PATHMAXLEN];
		lstrcpy(filenamebuffer, GLOBALDICTFILENAME);
		_wfopen_s(&fp_config, lstrcat(filenamebuffer, L"_config"), L"w+");

		size_t tmp;
		int childre_list_count = textcontentcore.getdictwordnumber();
		int childre_trackbar_pos = textcontentcore.gettrackbarvalue();
		intbuffer = childre_trackbar_pos;


		BloomCoref.initspellchecker(intbuffer);
		if (fp_config != NULL)
		{
			fwrite("bfdc", sizeof(char), 5, fp_config);

			fwrite(&childre_trackbar_pos, sizeof(int), 1, fp_config);

			for (i = 0; i < childre_list_count; i++)
			{
				//TCHAR* tbuffer = (TCHAR*)malloc(sizeof(TCHAR) * textcontentcore.getdictwordlen(i));
				TCHAR* tbuffer = (TCHAR*)malloc(sizeof(TCHAR) * MAXLEN);
				textcontentcore.getdictword(i, tbuffer);
				//char* buffer = (char*)malloc(sizeof(char) * (wcslen(tbuffer) + 1));
				char* buffer = (char*)malloc(sizeof(char) * MAXLEN);
				//wcstombs_s(&tmp, buffer, wcslen(tbuffer) + 1, tbuffer, wcslen(tbuffer) + 1);
				wcstombs_s(&tmp, buffer, MAXLEN, tbuffer, MAXLEN);
				fwrite(buffer, sizeof(char), MAXLEN, fp_config);
				BloomCoref.AddString(buffer);
			}
			fclose(fp_config);
		}
		BloomCoref.exportdict(GLOBALDICTFILENAME);

		BloomCoref.releasedict();
		lstrcpy(GLOBALDICTFILENAME, L"");

		return;
	}

	
}

void MenuCommand::sethashfunction_num(int num) {
	BloomCoref.sethashfunction_num(num);
}