#include<windows.h>
#include <stdio.h>
#include <tchar.h>
#include "..\TheEditorcommon.h"


//This Program is a part of TheEditor Project made by wjh776a68.
#ifndef https://blog.csdn.net/qq_40981268/article/details/82121195
int _tmain(int argc, TCHAR* argv[])
{
    TCHAR commandline[PATHMAXLEN] = L"ConsoleAppExecuter: \"";
   // char* argvpath = (char*)calloc(lstrlen(argv[0]) + 1, sizeof(char));
   // size_t num;
    //mbstowcs_s(&num, argv[0], lstrlen(argv[0])+1, argvpath, lstrlen(argv[0]) + 1);
    //strcat_s(commandline, argvpath);
   // system(commandline);
    lstrcat(commandline, argv[0]);
    lstrcat(commandline, L"\"");
    SetWindowText(GetConsoleWindow(), commandline);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD exitcode = 0;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    int ret=CreateProcess(NULL, argv[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
//  MessageBox(NULL, argv[0], NULL, NULL);
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    GetExitCodeProcess(pi.hProcess, &exitcode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    printf("\n\n----------------------------------------\n");

   
    if (ret == 0)
    {         
        printf("Running Program may occur error, the error code is %d...\n", GetLastError());
    }
    printf("Program running finished with exiting code %u\n Press Any Key to exit...",exitcode);
    getchar();
    return 0;
}
#endif
//
//
//
//int main(int argc, TCHAR* argv[])
//{
//    STARTUPINFO si;
//    PROCESS_INFORMATION pi;
//    DWORD exitcode = 0;
//    ZeroMemory(&si, sizeof(si));
//    ZeroMemory(&pi, sizeof(pi));
//
//    int ret=CreateProcess(NULL, *argv, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
//    MessageBox(NULL, *argv, NULL, NULL);
//    WaitForSingleObject(pi.hProcess, INFINITE);
//
//    GetExitCodeProcess(pi.hProcess, &exitcode);
//    CloseHandle(pi.hProcess);
//    CloseHandle(pi.hThread);
//    if (ret != 0)
//    {
//        
//        printf("Running Program occur error, the error code is %d", GetLastError());
//    }
//    printf("Program running finished with exiting code %u\n Press Any Key to exit...",exitcode);
//    getchar();
//   return 0;
//}
