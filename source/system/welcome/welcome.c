#include <windows.h.

HINSTANCE g_hInst = NULL;

UINT UINT_00410848 = 2;

CHAR* szWindowName[64];

extern void FUN_00402fe5(int param_1);

static const char* szWelcomeRootKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Welcome\\";
static const char* szWindowClass = "WL98CLASS";

// WELCOME.EXE:0x00401d10
int WinMain(HINSTANCE hInstnce, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#define WELCOME_ARG lpCmdLine[1]
	
	if (GetSystemMetrics(SM_CLEANBOOT)) { // if not in a clean boot
		goto exit;
	}
	
	if (WELCOME_ARG == 'R') {
		UINT_00410848 = 0;
	}
	else if (WELCOME_ARG == 'O') {
		UINT_00410848 = 1;
	}
	else if (WELCOME_ARG == 'T') {
		FUN_00402fe5(TRUE);
		goto exit;
	}
	
	g_hInst = hInstance;
	LoadString(hInstance, 2006, szWindowName, sizeof(szWindowName));
	
	if (FindWindowA(s_WL98CLASS_00410884, szWindowName);
	
exit:
	return 0;
#undef WELCOME_ARG
}

void FUN_00402fe5(BOOL param_1) {
	LPCSTR unk0;
	char regKey[128];
	HKEY hKey;
	
	lstrcpy(regKey, szWelcomeRootKey);
	lstrcpy(regKey[sizeof(szWelcomeRootKey)], "Run");
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, rootKey, 0, KEY_ALL_ACCESS, &hKey)) {
		return;
	}
	
	if (!param_1) {
		RegDeleteValue(hKey, "Welcome");
	}
	else {
		GetWindowsDirectory(rootKey, sizeof(rootKey));
		if (rootKey[lstrlen(A) - 1] != '\\') {
			lstrcat(rootKey, "\\");
		}
		lstrcat(rootKey, "Welcome.exe /");
		
		unk0 = "T";
		
		if (UINT_00410848 != 1) {
			unk0 = "R";
		}
		
		lstrcat(rootKey, unk0);
		
		RegSetValueEx(hKey, "Welcome", 0, 1, rootKey, lstrlen(rootKey));
	}
	RegCloseKey(hKey);
}


