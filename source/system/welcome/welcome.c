#include <windows.h>
#include "intrin.h"

HINSTANCE g_hInst = NULL;

UINT UINT_00410848 = 2;

CHAR* szWindowName[64];

HWND g_hWnd = NULL;

BOOL BOOL_0040f0cc = FALSE;

void FUN_00402fe5(int param_1);
BOOL EflagsFunctions1();

static const char* szWelcomeRootKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Welcome\\";
static const char* szWindowClass = "WL98CLASS";

// WELCOME.EXE:0x00401d10
int WinMain(HINSTANCE hInstnce, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#define WELCOME_ARG lpCmdLine[1]
    WNDCLASS wc;
    
    if (GetSystemMetrics(SM_CLEANBOOT)) { // if not in safe mode
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
    
    if (FindWindow(szWindowClass, szWindowName)) {
        SetForegroundWindow(hWnd);
        goto exit;
    }
    
    if (FUN_004030e8() /* error - function doesn't exist yet */) {
        FUN_00402b29(0,0x20006, 0, 1); // error - function doesn't exist yet
        if (WELCOME_ARG == 'B') {
            g_hWnd = FUN_00402cd3(); // error - function doesn't exist yet
        }
        
        wc.lpszClassName = szWindowClass;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOUECE(102));
        wc.lpszMenuName = NULL;
        wc.hbrBackground = GetStockObject(BOOL_0040f0cc != 0 ? 4 : 0);
        wc.lpfnWndProc = DefDlgProc;
        wc.hInstance = hInstance;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0x1e; // find out
        
        if (RegisterClass(&wc)) {
            DialogBoxParam(g_hInst, MAKEINTRESOURCE(101), g_hWnd, FUN_00401e55, 0); // error - function FUN_00401e55 doesn't exist yet
            FUN_0040320f();  // error - function doesn't exist yet
        }
    }
    
exit:
    return 0;
#undef WELCOME_ARG
}

// WELCOME.EXE:0x00402fe5
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

// WELCOME.EXE:0x0040505d
BOOL EflagsFunctions1() {
    INT oldFlag, newFlag;
    BOOL ret;
    
    ret = TRUE;
    
    oldFlag = __readeflags();
    __writeeflags(oldFlag ^ 0x200000);
    newFlag = __readeflags();

    if (oldFlag == newFlag) {
        ret = FALSE;
    }

    return ret;
}


