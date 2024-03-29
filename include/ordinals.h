#ifndef WIN32_ORDINAL_EXPORTS_H
#define WIN32_ORDINAL_EXPORTS_H

/// SHELL32

WINSHELLAPI BOOL WINAPI PathIsRoot(LPCSTR pszPath); // 29
WINSHELLAPI BOOL WINAPI PathRemoveFileSpec(LPSTR pszPath); // 35
WINSHELLAPI BOOL WINAPI PathAppend(LPSTR pszPath, LPCSTR pszMore); // 36
WINSHELLAPI BOOL WINAPI PathFileExists(LPCSTR pszPath); // 45
WINSHELLAPI LRESULT WINAPI RunDll_CallEntry16(DWORD proc, HWND hwnd, HINSTANCE inst, LPCSTR cmdline, int cmdshow); // 122
WINSHELLAPI int WINAPI ShellMessageBoxA(HINSTANCE hAppInst, HWND hWnd, LPCSTR pszText, LPCSTR pszTitle, UINT fuStyle); // 183

/// KERNEL32

WINBASEAPI HMODULE LoadLibrary16(LPCSTR pszPath); // 35
WINBASEAPI FARPROC GetProcAddress16(HMODULE hDll, LPCSTR pszPath); // 36
WINBASEAPI void FreeLibrary16(HMODULE hDll); // 37

#endif // WIN32_ORDINAL_EXPORTS_H


