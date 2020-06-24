#include"FileDialog.h"


//²Î¿¼ÎÄÏ×£ºhttps://blog.csdn.net/Murphy_CoolCoder/article/details/89380888
bool FileDialog::OpenWindowsDlg(bool IsOpen, bool IsPickFolder, TCHAR* pFilePath)
{
	WCHAR suffixsource[SUFFIXMAXLEN] = { '\0' };
	LPWSTR suffix = suffixsource;
	CoInitialize(nullptr);

	IFileDialog* pfd = NULL;
	HRESULT hr = NULL;
	if (IsOpen)
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	else
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		DWORD dwFlags;
		hr = pfd->GetOptions(&dwFlags);
		if (IsPickFolder)
			hr = pfd->SetOptions(dwFlags | FOS_PICKFOLDERS);
		else
			hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);


		COMDLG_FILTERSPEC fileType[] =
		{
			{ L"Source files",   L"*.cpp" },
			{ L"Header files",   L"*.h" },
			{ L"Dict files", L"*.bin" },
			{ L"All files", L"*.*" },
		};
		hr = pfd->SetFileTypes(ARRAYSIZE(fileType), fileType);


		hr = pfd->Show(NULL); //Show dialog
		if (SUCCEEDED(hr))
		{

			if (!IsOpen)       //Capture user change when select differen file extension.
			{

				UINT  unFileIndex(0);
				hr = pfd->GetFileTypeIndex(&unFileIndex);

				switch (unFileIndex)
				{
				case 1:
					hr = pfd->SetDefaultExtension(L"cpp");
					lstrcpy(suffix, L".cpp");
					break;
				case 2:
					hr = pfd->SetDefaultExtension(L"h");
					lstrcpy(suffix, L".h");
					break;
				case 3:
					hr = pfd->SetDefaultExtension(L"bin");
					lstrcpy(suffix, L".bin");
					break;
				default:
					hr = pfd->SetDefaultExtension(L"");
					lstrcpy(suffix, L"");
					break;
				}

			}
		}

		if (SUCCEEDED(hr))
		{
			IShellItem* pSelItem;
			hr = pfd->GetResult(&pSelItem);
			if (SUCCEEDED(hr))
			{
				LPWSTR pszFilePath = NULL;

				hr = pSelItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath);

				LPWSTR filepath = (LPWSTR)calloc(sizeof(WCHAR), lstrlen(pszFilePath));
				lstrcpy(filepath, pszFilePath);
				lstrcat(filepath, suffix);

				lstrcpy(pFilePath, filepath);
				//free(filepath);
				CoTaskMemFree(pszFilePath);


			}
			pSelItem->Release();
		}
	}
	pfd->Release();

	return true;
}

TCHAR* FileDialog::getfilepath(bool isopen)
{
	TCHAR pFilePath[PATHMAXLEN];
	OpenWindowsDlg(isopen, false, pFilePath);
	return pFilePath;
}
