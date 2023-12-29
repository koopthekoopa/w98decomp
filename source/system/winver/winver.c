#include "resource.h"
#include <windows.h>

LRESULT CALLBACK _export APPDLGPROC(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_INITDIALOG: {
            SetDlgItemText(hWnd, IDD_ABOUT_DIALOG, (LPCSTR)lParam);
            return TRUE;
            break;
        }
        case WM_COMMAND: {
            EndDialog(hWnd, 1);
            break;
        }
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    char wndTitle[128];
    
    if (hPrevInstance == NULL) {
        LoadString(hInstance, IDS_ABOUT_TITLE, wndTitle, sizeof(wndTitle));
        // unsure if this part has been decompiled properly. arguments are handled weirdly in 16 bit apps and are sometimes parsed weirdly when decompiled.
        DialogBoxParam((HINSTANCE)MakeProcInstance(NULL, hInstance) /* ???? */, MAKEINTRESOURCE(IDD_ABOUT_DIALOG), NULL, (DLGPROC)APPDLGPROC /* ???? */, (LPARAM)wndTitle);
    }
    
    return hPrevInstance == 0;
}


