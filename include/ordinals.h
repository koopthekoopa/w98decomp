#ifndef WIN32_ORDINAL_EXPORTS_H
#define WIN32_ORDINAL_EXPORTS_H

/// SHELL32

#ifndef DEBUGGER_USE

WINSHELLAPI BOOL WINAPI PathIsRoot(LPCSTR pszPath); // 29
WINSHELLAPI BOOL WINAPI PathRemoveFileSpec(LPSTR pszPath); // 35
WINSHELLAPI BOOL WINAPI PathAppend(LPSTR pszPath, LPCSTR pszMore); // 36
WINSHELLAPI BOOL WINAPI PathFileExists(LPCSTR pszPath); // 45
WINSHELLAPI int WINAPI ShellMessageBox(HINSTANCE hAppInst, HWND hWnd, LPCSTR pszText, LPCSTR pszTitle, UINT fuStyle); // 183

/// WINMM

WINMMAPI BOOL WINAPI PlaySound(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound); // 2

#endif // DEBUGGER_USE

#endif // WIN32_ORDINAL_EXPORTS_H


