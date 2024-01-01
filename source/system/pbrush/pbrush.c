/// Microsoft Paint Forwarder
/// PE x86 application

// This app is the same as the Wordpad forwarder except it closes the forwarder after executing the app.

#include <windows.h>

/// Read only data (merged into .text)

static const char szMspaintFile[] = "mspaint.exe";

/// Functions

// PBRUSH.EXE:0x0040100c
// Pbrush program uses a different entry function (same as the autorun program but without ExitProcess)
int _stdcall WinEntry() {
    int ret;
    STARTUPINFO start;
    LPSTR pszCmdLine = GetCommandLine();

    if (*pszCmdLine == '\"') {
        while (*++pszCmdLine && *pszCmdLine != '\"') {}
        
        if (*pszCmdLine == '\"') {
            pszCmdLine++;
        }
    }
    else {
        while (*pszCmdLine++ > ' ') {}
    }

    while (*pszCmdLine && *pszCmdLine++ <= ' ') {}

    start.dwFlags = 0;
    GetStartupInfo(&start);

    ret = WinMain(GetModuleHandle(NULL), NULL, pszCmdLine, (start.dwFlags & STARTF_USESHOWWINDOW) ? start.wShowWindow : SW_SHOWDEFAULT);
    return ret;
}

// PBRUSH.EXE:0x00401085
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ShellExecute(NULL, NULL, szMspaintFile, lpCmdLine, NULL, nCmdShow);
    ExitProcess(0);
    return 0;
}


