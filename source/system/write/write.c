/// Wordpad Forwarder
/// PE x86 application

#include <windows.h>

/// Read only data (merged into .text)

static const char szWordpadFile[] = "wordpad.exe";

/// Functions

// WRITE.EXE:0x0040100c
// Write program uses a different entry function (same as the autorun program but without ExitProcess)
int _stdcall WinEntry() {
    int ret;
    STARTUPINFO start;
    LPSTR pszCmdLine = GetCommandLine();

    if (*pszCmdLine == '\"') {
        while (*++pszCmdLine && (*pszCmdLine != '\"')) {}

        if (*pszCmdLine != '\"') {
            pszCmdLine++;
        }
    }
    else {
        while (*pszCmdLine > ' ') {
            pszCmdLine++;
        }
    }
    while (*pszCmdLine && (*pszCmdLine <= ' ')) {
        pszCmdLine++;
    }

    start.dwFlags = 0;
    GetStartupInfoA(&start);

    ret = WinMain(GetModuleHandle(NULL), NULL, pszCmdLine, (start.dwFlags & STARTF_USESHOWWINDOW) ? start.wShowWindow : SW_SHOWDEFAULT);
    return ret;
}

// WRITE.EXE:0x004023da
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ShellExecute(NULL, NULL, szWordpadFile, lpCmdLine, NULL, nCmdShow);
    return 0;
}


