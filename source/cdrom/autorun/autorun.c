/// Windows Auto Run Program
/// PE x86 application

#include "autorun.h"

/// Macros

#define IDB_BACKDROP_16                  200

#define RESBUTTON_INVALID                -1

#define RESBUTTON_INTERACTIVE_CD         100
#define RESBUTTON_COOL_VIDEOS            110
#define RESBUTTON_BROWSE_CD              120
#define RESBUTTON_WINDOWS_SETUP          130

#define IDBUTTON_WINDOWS_SETUP           3

#define BUTTON_START_Y                   126

#define BUTTON_WIDTH                     334
#define BUTTON_HEIGHT                    59

#define BUTTON_COLOR_4BIT                RGB(128, 128, 128)
#define BUTTON_HIGHLIGHT_COLOR_4BIT      RGB(0, 0, 0)
#define BUTTON_DISABLED_COLOR_4BIT       RGB(64, 64, 64)
#define BUTTON_DESCRIPTION_COLOR_4BIT    RGB(64, 64, 64)

#define BUTTON_COLOR                     RGB(75, 90, 129)
#define BUTTON_HIGHLIGHT_COLOR           RGB(0, 0, 0)
#define BUTTON_DISABLED_COLOR            RGB(107, 136, 185)
#define BUTTON_DESCRIPTION_COLOR         RGB(0, 0, 0)

#define BUTTON_1_COLOR                   RGB(255, 49, 0)
#define BUTTON_2_COLOR                   RGB(255, 206, 8)
#define BUTTON_3_COLOR                   RGB(132, 214, 82)
#define BUTTON_4_COLOR                   RGB(0, 156, 255)

#define ROOT(app, parms, dir)            ((app << 6) | (parms << 3) | dir)

#define EXEC_ROOT(item)                  ((item) >> 6)
#define PARAMS_ROOT(item)                (((item) >> 3) & RootMax)
#define DIR_ROOT(item)                   (item & RootMax)

#define GetName(x)                       x + 1
#define GetDesc(x)                       x + 2
#define GetExec(x)                       x + 3
#define GetParam(x)                      x + 4
#define GetDir(x)                        x + 5

/// Enums and structures

enum {
    RootNone = 0, /* app doesnt need relative directory */
    RootCD, /* cd path relative */
    RootWin, /* windows path relative */
    RootSys, /* system path relative */
    RootMax = RootSys /* for bitwise ops */
};

typedef struct {
    HWND hWnd;

    HDC hDc;
    HBITMAP hBackBmp;
    HBITMAP hBtnNormal;
    HBITMAP hBtnHigh;

    HPALETTE hPal;

    HFONT hFont;

    int wndHeight;

    COLORREF colNormal, colHigh, colDisabled, colDesc;

    BOOL useKeyboard;
} AutoRunUserData;

typedef struct {
    int resource;
    DWORD root;

    int x, y;
    RECT btnLoc, textLoc;

    DWORD unk0; // unused
    DWORD unk1; // unused

    HWND hWnd;
    WNDPROC prevProc;

    char text[64];
    char description[256];
} AutoRunButton;

/// Globals

HCURSOR g_mainCursor = NULL;    // AUTORUN.EXE:0x00403000

HINSTANCE g_hInst = NULL;       // AUTORUN.EXE:0x00403004

BOOL g_has4BitDisplay = FALSE;  // AUTORUN.EXE:0x00403008
BOOL g_hasPalette = FALSE;      // AUTORUN.EXE:0x0040300c
BOOL g_mouseAvailable = FALSE;  // AUTORUN.EXE:0x00403010
BOOL g_enableSetup = FALSE;     // AUTORUN.EXE:0x00403014
BOOL g_appDisabled = TRUE       // AUTORUN.EXE:0x00403018

HHOOK g_hMouseHook = NULL;      // AUTORUN.EXE:0x0040301c

HWND g_hMainWindow = NULL;      // AUTORUN.EXE:0x00403020

int g_activeButton = -2;        // AUTORUN.EXE:0x00403024

// AUTORUN.EXE:0x00403028
AutoRunButton g_autoRunButtons[] = {
    { RESBUTTON_INTERACTIVE_CD,    ROOT(RootCD, RootNone, RootCD),        0, 0, 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, 0, 0, 0, 0, 0, 0, NULL, NULL}, // interactive cd sampler
    { RESBUTTON_COOL_VIDEOS,       ROOT(RootNone, RootCD, RootCD),        0, 0, 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, 0, 0, 0, 0, 0, 0, NULL, NULL}, // cool video clips
    { RESBUTTON_BROWSE_CD,         ROOT(RootNone, RootCD, RootCD),        0, 0, 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, 0, 0, 0, 0, 0, 0, NULL, NULL}, // browse cd
    { RESBUTTON_WINDOWS_SETUP,     ROOT(RootNone, RootNone, RootSys),     0, 0, 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, 0, 0, 0, 0, 0, 0, NULL, NULL}, // add/remove software
};

#define AUTORUN_MAX_BUTTONS ARRAY_SIZE(g_autoRunButtons)

// AUTORUN.EXE:0x00403628
COLORREF g_buttonColors[] = {
    BUTTON_1_COLOR,
    BUTTON_2_COLOR,
    BUTTON_3_COLOR,
    BUTTON_4_COLOR,
};

// AUTORUN.EXE:0x00403638
HPEN g_buttonPens[] = {
    NULL,
    NULL,
    NULL,
    NULL,
};

/// Read only data (merged into .text)

static const char szAutoRunPrevention[] = "__Win95SetupDiskQuery";
static const char szAutoRunClass[] = "AutoRunMain";
static const char szDefFontName[] = "Arial";

static const int iWsStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
static const int iDefaultFontSize = 19;

/// Functions

// AUTORUN.EXE:0x004010b8
// should've been in utils.c, microsoft
LONG AutoRunStrToLong(LPCSTR str) {
    LONG l = 0;

    if (*str == '-')
        str++;

    while (*str >= '0' && *str <= '9')
        l = l * 10 + (*str++ - '0');

    if (*str == '-')
        l = -l;

    return l;
}

// AUTORUN.EXE:0x004010f8
LRESULT CALLBACK AutoRunButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    int index = GetWindowLong(hWnd, GWL_ID);
    PAINTSTRUCT blankPs;

    if (msg == WM_ERASEBKGND)
        return FALSE;

    if (index >= 0 && index < AUTORUN_MAX_BUTTONS) {
        if (msg == WM_PAINT) {
            BeginPaint(hWnd, &blankPs);
            EndPaint(hWnd, &blankPs);
        }
        else if (msg == WM_KEYDOWN) {
            PostMessage(GetParent(hWnd), msg, wParam, lParam);
        }

        return CallWindowProc(g_autoRunButtons[index].prevProc, hWnd, msg, wParam, lParam);
    }
}

// AUTORUN.EXE:0x00401181
HWND AutoRunCreateButton(AutoRunUserData* data) {
    HWND hWnd;

    hWnd = CreateWindow("Button", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, 0, 0, 0, data->hWnd, NULL, g_hInst, NULL);
    if (hWnd) {
        SetClassLong(hWnd, GCL_HCURSOR, (LONG)g_mainCursor);
    }

    return hWnd;
}

// AUTORUN.EXE:0x004011c9
void AutoRunInitButtons(AutoRunUserData* data) {
    HWND hWnd;
    int i;
    int lineY = 170;

    for (i = 0; i < AUTORUN_MAX_BUTTONS; i++) {
        hWnd = NULL;

        // why does it create buttons when "g_has4BitDisplay" is false???
        // I'm starting to believe Windows 98's autorun program is rushed.
        if (g_autoRunButtons[i].resource != RESBUTTON_INVALID && !g_has4BitDisplay) {
            hWnd = AutoRunCreateButton(data);
        }

        if (hWnd) {
            g_autoRunButtons[i].hWnd = hWnd;
            SetWindowLong(hWnd, GWL_ID, i);

            g_autoRunButtons[i].prevProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)AutoRunButtonProc);

            SetWindowPos(hWnd, NULL, g_autoRunButtons[i].x, g_autoRunButtons[i].y, BUTTON_WIDTH, (BUTTON_HEIGHT - 15), SWP_NOZORDER | SWP_NOACTIVATE);

            LoadString(g_hInst, GetName(g_autoRunButtons[i].resource), g_autoRunButtons[i].text, sizeof(g_autoRunButtons[i].text));
            LoadString(g_hInst, GetDesc(g_autoRunButtons[i].resource), g_autoRunButtons[i].description, sizeof(g_autoRunButtons[i].description));

            SetRect(&g_autoRunButtons[i].textLoc, 0, lineY - 32, 286, lineY);
            InvalidateRect(data->hWnd, &g_autoRunButtons[i].textLoc, FALSE);

            g_buttonPens[i] = CreatePen(PS_SOLID, 2, g_buttonColors[i]);
        }
        lineY += BUTTON_HEIGHT;
    }
}

// AUTORUN.EXE:0x004012d9
void AutoRunClean(AutoRunUserData* data) {
    int i = 0;

    if (g_activeButton >= 0) {
        data->useKeyboard = FALSE;
        SendMessage(data->hWnd, WM_APP, TRUE, (LPARAM)-1);
    }

    if (data->hDc) {
        if (data->hBackBmp) {
            SelectBitmap(data->hDc, data->hBackBmp);
            data->hBackBmp = NULL;
        }
        DeleteDC(data->hDc);
        data->hDc = NULL;
    }

    if (data->hPal) {
        DeletePalette(data->hPal);
        data->hPal = NULL;
    }

    if (data->hFont) {
        DeleteFont(data->hFont);
        data->hFont = NULL;
    }

    for (i = 0; i < AUTORUN_MAX_BUTTONS; i++) {
        if (g_buttonPens[i]) {
            DeletePen(g_buttonPens[i]);
        }
    }
}

// AUTORUN.EXE:0x00401372
BOOL AutoRunBuildPath(char* path, int resource, DWORD root) {
    CHAR directory[MAX_PATH];

    if (resource == RESBUTTON_INVALID) {
        *path = 0; // a hack that makes empty strings.
    }
    else {
        if (!LoadString(g_hInst, resource, path, MAX_PATH)) {
            return FALSE;
        }
    }
    if (*path == ' ' && !path[1]) {
        *path = 0;
    }

    *directory = 0;
    switch (root) {
        case RootCD: {
            GetModuleFileName(g_hInst, directory, sizeof(directory));
            _PathStripToRoot(directory);
            break;
        }

        case RootWin: {
            GetRealWindowsDirectory(directory, sizeof(directory));
            break;
        }

        case RootSys: {
            GetSystemDirectory(directory, sizeof(directory));
        }
    }

    if (*directory) {
        if (*path) {
            PathAppend(directory, path);
        }
        lstrcpy(path, directory);
    }

    return TRUE;
}

// AUTORUN.EXE:0x00401452
BOOL AutoRunInit(HWND hWnd, AutoRunUserData* data, LPCREATESTRUCT cs) {
    char command[MAX_PATH];
    int i;
    int btnY;
    BITMAP unk0;
    char weightBuf[32];

    data->hWnd = hWnd;

    data->hDc = CreateCompatibleDC(NULL);
    if (!data->hDc) {
        goto exit;
    }

    data->hBackBmp = SelectBitmap(data->hDc, (HBITMAP)cs->lpCreateParams);
    if (!data->hBackBmp) {
        goto exit;
    }

    if (g_hasPalette) {
        data->hPal = PaletteFromHDC(data->hDc);
        if (!data->hPal) {
            goto exit;
        }
    }

    if (AutoRunBuildPath(command, GetExec(g_autoRunButtons[0].resource), EXEC_ROOT(g_autoRunButtons[0].resource)) && PathFileExists(command)) {
        // right here is an if statement but has nothing
    }

    for (i = 0; i < AUTORUN_MAX_BUTTONS; i++) {
        // right here is a for loop but has nothing
    }

    GetObject(cs->lpCreateParams, sizeof(unk0), &unk0); // does nothing whatsoever

    btnY = BUTTON_START_Y;
    for (i = 0; i < AUTORUN_MAX_BUTTONS; i++) {
        if (g_autoRunButtons[i].resource != RESBUTTON_INVALID) {
            g_autoRunButtons[i].x = 0;
            g_autoRunButtons[i].y = btnY;
        }

        btnY += BUTTON_HEIGHT;
    }

    {
        HDC hDc = GetDC(NULL);
        LOGFONT logFont = {
            iDefaultFontSize,
            0,
            0,
            0,
            FW_BOLD,
            FALSE,
            FALSE,
            FALSE,
            (hDc ? GetTextCharset(hDc) : DEFAULT_CHARSET),
            OUT_STROKE_PRECIS,
            CLIP_DEFAULT_PRECIS,
            PROOF_QUALITY | NONANTIALIASED_QUALITY,
            VARIABLE_PITCH | FF_DONTCARE,
            0,
        };

        if (!LoadString(g_hInst, IDS_DEFAULTFONTNAME, logFont.lfFaceName, sizeof(logFont.lfFaceName))) {
            lstrcpy(logFont.lfFaceName, szDefFontName);
        }

        if (LoadString(g_hInst, IDS_DEFAULTFONTSIZE, weightBuf, sizeof(weightBuf))) {
            logFont.lfHeight = AutoRunStrToLong(weightBuf);
        }

        data->hFont = CreateFontIndirect(&logFont);
        if (!data->hFont) {
            goto exit;
        }
    }

    if (g_has4BitDisplay) {
        data->colHigh = BUTTON_HIGHLIGHT_COLOR_4BIT;
        data->colDisabled = BUTTON_DISABLED_COLOR_4BIT;
        data->colNormal = BUTTON_COLOR_4BIT;
        data->colDesc = BUTTON_DESCRIPTION_COLOR_4BIT;
    }
    else {
        data->colHigh = BUTTON_HIGHLIGHT_COLOR;
        data->colDisabled = BUTTON_DISABLED_COLOR;
        data->colNormal = BUTTON_COLOR;
        data->colDesc = BUTTON_DESCRIPTION_COLOR;

        // Get button bitmap data.
        {
            HRSRC hResId = FindResource(g_hInst, MAKEINTRESOURCE(IDB_BUTTONS_HOVER), RT_BITMAP);
            HGLOBAL hRes = LoadResource(g_hInst, hResId);

            if (hResId && hRes) {
                data->hBtnNormal = LockResource(hRes);
            }

            if (data->hBtnNormal) {
                hResId = FindResource(g_hInst, MAKEINTRESOURCE(IDB_BUTTONS_NOHOVER), RT_BITMAP);
                hRes = LoadResource(g_hInst, hResId);

                if (hResId && hRes) {
                    data->hBtnHigh = LockResource(hRes);
                }

                if (data->hBtnHigh) {
                    g_mainCursor = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_CURSOR_HAND));
                }
                else {
                    goto exit;
                }
            }
        }
    }

    PostMessage(g_hMainWindow, WM_APP, TRUE, -1);
    return TRUE;

exit:
    AutoRunClean(data);
    return FALSE;
}

// AUTORUN.EXE:0x004016e0
void AutoRunResize(AutoRunUserData* data) {
    RECT rect;

    GetClientRect(data->hWnd, &rect);
    data->wndHeight = rect.bottom - rect.top;
}

// AUTORUN.EXE:0x00401706
void AutoRunRealizePalette(HWND hWnd, AutoRunUserData* data, HDC hDc) {
    HDC hwndHDc;

    if (data->hPal) {
        hwndHDc = hDc ? hDc : GetDC(hWnd);

        if (hwndHDc) {
            SelectPalette(hwndHDc, data->hPal, FALSE);

            if (!hDc) {
                ReleaseDC(hWnd, hwndHDc);
            }

            if (RealizePalette(hwndHDc) > 0) {
                RedrawWindow(hWnd, NULL, NULL, RDW_ALLCHILDREN | RDW_ERASE | RDW_INVALIDATE);
            }
        }
    }
}

// AUTORUN.EXE:0x00401777
void AutoRunErase(AutoRunUserData* data, HDC hDc) {
    RECT rect;

    GetClientRect(data->hWnd, &rect);

    AutoRunRealizePalette(data->hWnd, data, hDc);
    BitBlt(hDc, 0, 0, rect.right, rect.bottom, data->hDc, 0, 0, SRCCOPY);
}

// AUTORUN.EXE:0x004017bc
void AutoRunPaint(AutoRunUserData* data) {
    COLORREF color;
    PAINTSTRUCT ps;
    int bmpPosSrc;
    HDC hDc = BeginPaint(data->hWnd, &ps);
    COLORREF textColor = GetTextColor(hDc);
    HFONT textFont = NULL;
    HGDIOBJ colorObj;
    BITMAPINFO* bmInfo;
    RECT newTextRect, rect;
    POINT client;
    int i;

    AutoRunRealizePalette(data->hWnd, data, hDc);
    SetBkMode(hDc, TRANSPARENT);

    if (data->hFont) {
        textFont = SelectFont(hDc, data->hFont);
    }

    bmpPosSrc = 0;

    for (i = 0; i < AUTORUN_MAX_BUTTONS; i++) {
        if (g_autoRunButtons[i].hWnd) {
            color = (g_activeButton == i) ? data->colHigh : (IsWindowEnabled(g_autoRunButtons[i].hWnd) ? data->colNormal : data->colDisabled);
        }

        if (textColor != color) {
            SetTextColor(hDc, color);
            textColor = color;
        }

        DrawText(hDc, g_autoRunButtons[i].text, -1, &g_autoRunButtons[i].textLoc, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
        colorObj = SelectObject(hDc, (HGDIOBJ)g_buttonPens[i]);

        MoveToEx(hDc, 0, g_autoRunButtons[i].textLoc.bottom - 1, NULL);
        LineTo(hDc, g_autoRunButtons[i].textLoc.right + 37, g_autoRunButtons[i].textLoc.bottom - 1);

        SelectObject(hDc, colorObj);

        bmInfo = (BITMAPINFO*)data->hBtnHigh;
        SetDIBitsToDevice(hDc, g_autoRunButtons[i].textLoc.right + 6, g_autoRunButtons[i].textLoc.top - 2, 32, 32, bmpPosSrc, 0, 0, 32, bmInfo->bmiColors + (1 << (bmInfo->bmiHeader.biBitCount & 0x1f)), bmInfo, DIB_RGB_COLORS);

        bmpPosSrc += 32;
    }

    if (g_activeButton >= 0) {
        client.x = client.y = 0;

        if (data->colDesc != textColor) {
            SetTextColor(hDc, data->colDesc);
        }

        SetRect(&newTextRect, g_autoRunButtons[g_activeButton].textLoc.right, 0, g_autoRunButtons[g_activeButton].textLoc.right + 250, 0);
        OffsetRect(&newTextRect, 48, 0);
        DrawText(hDc, g_autoRunButtons[g_activeButton].description, -1, &newTextRect, DT_CALCRECT | DT_WORDBREAK);

        ClientToScreen(data->hWnd, &client);
        GetWindowRect(g_autoRunButtons[g_activeButton].hWnd, &rect);
        OffsetRect(&rect, -client.x, -client.y);

        client.x = newTextRect.bottom - newTextRect.top;
        newTextRect.top = rect.bottom - 35;
        newTextRect.bottom = client.x + newTextRect.top;

        DrawText(hDc, g_autoRunButtons[g_activeButton].description, -1, &newTextRect, DT_WORDBREAK);

        bmInfo = (BITMAPINFO*)data->hBtnNormal;
        SetDIBitsToDevice(hDc, g_autoRunButtons[g_activeButton].textLoc.right + 6, g_autoRunButtons[g_activeButton].textLoc.top - 2, 32, 32, g_activeButton << 5, 0, 0, 32, bmInfo->bmiColors + (1 << (bmInfo->bmiHeader.biBitCount & 0x1f)), bmInfo, DIB_RGB_COLORS);
    }

    if (textFont) {
        SelectFont(hDc, textFont);
    }

    EndPaint(data->hWnd, &ps);
}

// AUTORUN.EXE:0x00401a57
void AutoRunActivateButton(AutoRunUserData* data, int index) {
#define AUTORUN_MOUSE_EVENT 0
    AutoRunButton* newBtn;
    AutoRunButton* oldBtn;

    if (index >= 0) {
        if (!g_autoRunButtons[index].hWnd || !IsWindowEnabled(g_autoRunButtons[index].hWnd)) {
            index = -1;
        }
    }

    if (index != g_activeButton) {
        newBtn = (index >= 0) ? &g_autoRunButtons[index] : NULL;
        oldBtn = (g_activeButton >= 0) ? &g_autoRunButtons[g_activeButton] : NULL;

        if (oldBtn) {
            RECT rect = oldBtn->textLoc;
            InflateRect(&rect, 298, 30);
            InvalidateRect(data->hWnd, &rect, TRUE);
        }

        g_activeButton = index;

        if (newBtn) {
            RECT rect = newBtn->textLoc;
            InflateRect(&rect, 298, 30);
            InvalidateRect(data->hWnd, &rect, FALSE);

            SetFocus(newBtn->hWnd);

            if (g_mouseAvailable && !data->useKeyboard) {
                SetTimer(data->hWnd, AUTORUN_MOUSE_EVENT, 333, NULL);
            }
        }
        else {
            SetFocus(data->hWnd);

            if (g_mouseAvailable) {
                KillTimer(data->hWnd, AUTORUN_MOUSE_EVENT);
            }
        }

        UpdateWindow(data->hWnd);
    }
#undef AUTORUN_MOUSE_EVENT
}

// AUTORUN.EXE:0x00401b9d
void AutoRunButtonOnMouse(AutoRunUserData* data, int index, BOOL force) {
    if (index >= 0 || !data->useKeyboard || force) {
        data->useKeyboard = !g_mouseAvailable;
        AutoRunActivateButton(data, index);
    }
}

// AUTORUN.EXE:0x00401bd0
int AutoRunProcessButton(HWND hWnd, LPPOINT location) {
    int index;
    POINT point;

    if (GetAsyncKeyState(VK_LBUTTON) < 0) {
        return g_activeButton;
    }

    if (hWnd && IsWindowEnabled(hWnd) && GetParent(hWnd) == g_hMainWindow) {
        index = GetWindowLong(hWnd, GWL_ID);
        if (index >= 0 && index < AUTORUN_MAX_BUTTONS) {
            ScreenToClient(hWnd, location);

            point.x = location->x;
            point.y = location->y;

            if (PtInRect(&g_autoRunButtons[index].btnLoc, point)) {
                return index;
            }
        }
    }
    return -1;
}

// AUTORUN.EXE:0x00401c6a
void AutoRunDetectButton(AutoRunUserData* data) {
    int index;
    POINT curPoint;
    HWND hWnd;

    if (!data->useKeyboard) {
        index = -1;

        if (!g_appDisabled) {
            GetCursorPos(&curPoint);

            hWnd = WindowFromPoint(curPoint);
            if (hWnd) {
                index = AutoRunProcessButton(hWnd, &curPoint);
            }
        }
        if (index != g_activeButton) {
            AutoRunButtonOnMouse(data, index, FALSE);
        }
    }
}

// AUTORUN.EXE:0x00401cc8
void AutoRunToggleButton(AutoRunUserData* data, UINT btnId, BOOL enable) {
    if (btnId >= 0 && btnId < AUTORUN_MAX_BUTTONS) {
        if (g_autoRunButtons[btnId].hWnd && IsWindow(g_autoRunButtons[btnId].hWnd)) {
            EnableWindow(g_autoRunButtons[btnId].hWnd, enable);
            InvalidateRect(data->hWnd, &g_autoRunButtons[btnId].textLoc, FALSE);

            AutoRunDetectButton(data);
        }
    }
}

// AUTORUN.EXE:0x00401d20
BOOL AutoRunCDInDrive(HWND hWnd) {
    CHAR fileName[MAX_PATH];

    GetModuleFileName(g_hInst, fileName, sizeof(fileName));

    while (!PathFileExists(fileName)) {
        if (ShellMessageBoxA(g_hInst, hWnd, MAKEINTRESOURCE(IDS_NOCD), MAKEINTRESOURCE(IDS_TITLE), MB_OKCANCEL | MB_ICONERROR) == IDCANCEL) {
            return FALSE;
        }
    }

    return TRUE;
}

// AUTORUN.EXE:0x00401d7d
void AutoRunLaunchSetup(HWND hWnd) {
    CHAR command[MAX_PATH];
    CHAR params[MAX_PATH];
    CHAR dir[MAX_PATH];

    if (AutoRunBuildPath(command, 9, RootCD)) {
        if (AutoRunBuildPath(params, -1, RootNone)) {
            AutoRunBuildPath(dir, -1, RootCD);
        }
    }

    ShellExecute(hWnd, NULL, command, params, dir, SW_SHOWNORMAL);
}

// AUTORUN.EXE:0x00401de6
void AutoRunClickButton(AutoRunUserData* data, int btnId) {
    CHAR command[MAX_PATH];
    CHAR params[MAX_PATH];
    CHAR dir[MAX_PATH];

    if (btnId >= 0 && btnId < AUTORUN_MAX_BUTTONS) {
        PlaySound(MAKEINTRESOURCE(IDW_BLIP), g_hInst, SND_RESOURCE | SND_NODEFAULT | SND_ASYNC);
        if (AutoRunCDInDrive(data->hWnd)) {
            if (AutoRunBuildPath(command, GetExec(g_autoRunButtons[btnId].resource), EXEC_ROOT(g_autoRunButtons[btnId].root))) {
                if (AutoRunBuildPath(params, GetParam(g_autoRunButtons[btnId].resource), PARAMS_ROOT(g_autoRunButtons[btnId].root))) {
                    AutoRunBuildPath(dir, GetDir(g_autoRunButtons[btnId].resource), DIR_ROOT(g_autoRunButtons[btnId].root));
                }
            }
            if (g_autoRunButtons[btnId].resource == RESBUTTON_WINDOWS_SETUP && g_enableSetup) {
                AutoRunLaunchSetup(data->hWnd);
            }
            else {
                ShellExecute(data->hWnd, NULL, command, params, dir, SW_SHOWNORMAL);
            }
        }
    }
}

// AUTORUN.EXE:0x00401ed7
void AutoRunHandleKeys(AutoRunUserData* data, TCHAR key) {
    int button = g_activeButton;
    int position = 0;
    int i;

    switch (key) {
        case VK_TAB: {
            position = (GetKeyState(VK_SHIFT) < 0) ? -1 : 1;
            break;
        }
        case VK_RETURN: {
            if (g_activeButton >= 0) {
                AutoRunClickButton(data, g_activeButton);
            }
        }
        case VK_ESCAPE: {
            button = -1;
            break;
        }
        case VK_END: {
            button = 4;
        }
        case VK_LEFT:
        case VK_UP: {
            position = -1;
            break;
        }
        case VK_HOME: {
            button = -1;
        }
        case VK_RIGHT:
        case VK_DOWN: {
            position = 1;
            break;
        }
        default: {
            return;
        }
    }

    if (position != 0) {
        for (i = 0; i < AUTORUN_MAX_BUTTONS; i++) {
            button += position;

            if (button >= AUTORUN_MAX_BUTTONS) {
                button = -1;
            }
            else if (button < 0) {
                button = AUTORUN_MAX_BUTTONS;
            }
            else if (g_autoRunButtons[button].hWnd && IsWindowEnabled(g_autoRunButtons[button].hWnd)) {
                break;
            }
        }
    }

    if (button >= 0) {
        SetCursor(NULL);
        data->useKeyboard = TRUE;
    }
    else {
        data->useKeyboard = !g_mouseAvailable;
    }

    AutoRunActivateButton(data, button);
}

// AUTORUN.EXE:0x00402002
/* deobfuscate this ! */
BOOL AutoRunCheckVersion(AutoRunUserData* data) {
    OSVERSIONINFO osver = { sizeof(osver), 0, 0, 0, 0, 0 };
    BOOL disableSetup = FALSE;
    int message = 0;
    int msgtype = 0;
    int msgButton;

    if (!GetVersionEx(&osver)) {
        message = IDS_VERSIONFAIL;
        msgtype = MB_OK | MB_ICONSTOP;
        goto returnPoint;
    }

    if (osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
        g_enableSetup = FALSE;
        if (((osver.dwMajorVersion <= VER_MAJOR_NUMBER) &&
            ((osver.dwMajorVersion != VER_MAJOR_NUMBER || (osver.dwMinorVersion <= VER_MINOR_NUMBER)))) &&
            ((osver.dwMajorVersion != VER_MAJOR_NUMBER ||
                ((osver.dwMinorVersion != VER_MINOR_NUMBER || (osver.dwBuildNumber <= VER_BUILD_NUMBER)))))) {
            if ((osver.dwMajorVersion < VER_MAJOR_NUMBER) ||
                ((osver.dwMajorVersion == VER_MAJOR_NUMBER &&
                    ((osver.dwMinorVersion < VER_MINOR_NUMBER ||
                        ((osver.dwMinorVersion == VER_MINOR_NUMBER && (osver.dwBuildNumber < VER_BUILD_NUMBER)))))))) {
                g_enableSetup = TRUE;
            }
            if (g_enableSetup) {
                message = IDS_VERSIONNEW;
                msgtype = MB_ICONQUESTION | MB_YESNOCANCEL;
            }
            goto returnPoint;
        }
        message = IDS_VERSIONOLD;
        msgtype = MB_ICONWARNING;
    }
    disableSetup = TRUE;

returnPoint:
    if (disableSetup) {
        AutoRunToggleButton(data, IDBUTTON_WINDOWS_SETUP, FALSE);
    }
    if (msgtype != 0) {
        ShowWindow(data->hWnd, SW_SHOWNORMAL);

        msgButton = ShellMessageBoxA(g_hInst, data->hWnd, MAKEINTRESOURCE(message), MAKEINTRESOURCE(IDS_TITLE), msgtype);
        if (msgButton == IDYES) {
            AutoRunLaunchSetup(data->hWnd);
        }
        else if (msgButton == IDCANCEL) {
            return FALSE;
        }
    }

    return TRUE;
}

// AUTORUN.EXE:0x00402168
LRESULT CALLBACK AutoRunMouseHook(int code, WPARAM wParam, LPARAM lParam) {
    int id;

    if (code >= 0) {
        id = g_appDisabled ? -1 : AutoRunProcessButton(((MOUSEHOOKSTRUCT*)lParam)->hwnd, &((MOUSEHOOKSTRUCT*)lParam)->pt);

        if (id != g_activeButton) {
            PostMessage(g_hMainWindow, WM_APP, FALSE, id);
        }
    }

    return CallNextHookEx(g_hMouseHook, code, wParam, lParam);
}

// AUTORUN.EXE:0x004021c3
LRESULT CALLBACK AutoRunWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    AutoRunUserData* data;
    HWND hCurrentWnd;

    data = (AutoRunUserData*)GetWindowLong(hWnd, GWL_USERDATA);
    hCurrentWnd = g_hMainWindow;

    switch (msg) {
        case WM_PAINT: {
            AutoRunPaint(data);
            break;
        }
        case WM_CREATE: {
            PlaySound(MAKEINTRESOURCE(IDW_STARTUP), g_hInst, SND_RESOURCE | SND_NODEFAULT | SND_ASYNC);
            AutoRunInitButtons(data);
            ShowWindow(hWnd, SW_SHOWNORMAL);

            if (!AutoRunCheckVersion(data)) {
                return -1;
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE: {
            AutoRunResize(data);
            break;
        }
        case WM_ACTIVATE: {
            g_appDisabled = (LOWORD(wParam) == WA_INACTIVE || HIWORD(wParam));
            AutoRunDetectButton(data);

            hCurrentWnd = g_hMainWindow;
            goto defaultRet;
        }
        case WM_NCCREATE: {
            data = (AutoRunUserData*)LocalAlloc(LPTR, sizeof(AutoRunUserData));
            if (data && !AutoRunInit(hWnd, data, (LPCREATESTRUCT)lParam)) {
                LocalFree((HANDLE)data);
                data = NULL;
            }

            SetWindowLong(hWnd, GWL_USERDATA, (LONG)data);

            hCurrentWnd = hWnd;

            if (!data) {
                return FALSE;
            }

            goto defaultRet;
        }
        case WM_NCDESTROY: {
            if (data) {
                AutoRunClean(data);
                LocalFree(data);
            }
            g_hMainWindow = NULL;

            hCurrentWnd = g_hMainWindow;
            goto defaultRet;
        }
        case WM_ERASEBKGND: {
            AutoRunErase(data, (HDC)wParam);
            break;
        }
        case WM_KEYDOWN: {
            AutoRunHandleKeys(data, (TCHAR)wParam);
            break;
        }
        case WM_COMMAND: {
            if (HIWORD(wParam) == BN_CLICKED) {
                AutoRunClickButton(data, LOWORD(wParam));
            }
            break;
        }
        case WM_TIMER: {
            AutoRunDetectButton(data);
            break;
        }
        case WM_QUERYNEWPALETTE: {
            AutoRunRealizePalette(hWnd, data, NULL);
            break;
        }
        case WM_PALETTECHANGED: {
            if (wParam == (WPARAM)hWnd) {
                break;
            }
        }
        case WM_APP: {
            AutoRunButtonOnMouse(data, lParam, wParam);
            break;
        }
        default:
defaultRet:
            g_hMainWindow = hCurrentWnd;
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return TRUE;
}

// AUTORUN.EXE:0x004023da
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hWnd;
    HDC hDcMonitor;
    int ret = -1;
    HBITMAP hbBackground = NULL;
    CHAR szAppTitle[MAX_PATH];
    MSG msg;
    BITMAP bBackground;
    WNDCLASS wc;
    RECT rect;

    g_hInst = hInstance;

    hWnd = FindWindow(szAutoRunPrevention, szAutoRunPrevention);
    if (hWnd) {
        ret = 0;
        goto exit;
    }

    if (!LoadString(g_hInst, IDS_TITLE, szAppTitle, sizeof(szAppTitle))) {
        ret = 0;
        goto exit;
    }

    hWnd = FindWindow(szAutoRunClass, szAppTitle);
    if (hWnd) {
        ret = 0;
        SetForegroundWindow(hWnd);
        goto exit;
    }

    if (!GetClassInfo(g_hInst, szAutoRunClass, &wc)) {
        wc.style = 0;
        wc.cbWndExtra = 0;
        wc.cbClsExtra = 0;
        wc.hInstance = hInstance;
        wc.lpfnWndProc = AutoRunWndProc;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = NULL; // in windows 9x the icon would be seen as the windows logo
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = szAutoRunClass;

        if (!RegisterClass(&wc))
            goto exit;
    }

    hDcMonitor = GetDC(NULL);
    // Windows 98's autorun program uses Windows 95's autorun as a base.
    // Originally this would check the monitor to see if it is running on 16 colors so it can display a different background.
    // But because no such 16 color background exists in Windows 98's autorun, this will always be FALSE resulting the 256 color background being used all the time.
    g_has4BitDisplay = FALSE;
    g_hasPalette = (BOOL)((GetDeviceCaps(hDcMonitor, RASTERCAPS) & RC_PALETTE) != 0);

    ReleaseDC(NULL, hDcMonitor);

    hbBackground = LoadImage(g_hInst, MAKEINTRESOURCE(g_has4BitDisplay ? IDB_BACKDROP_16 /* unused */ : IDB_BACKDROP_256), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    if (!hbBackground) {
        goto exit;
    }

    if ((g_mouseAvailable = (BOOL)(GetSystemMetrics(SM_MOUSEPRESENT) != FALSE)) != FALSE) {
        g_hMouseHook = SetWindowsHookEx(WH_MOUSE, AutoRunMouseHook, g_hInst, GetCurrentThreadId());
    }

    GetObject(hbBackground, sizeof(bBackground), &bBackground);

    rect.left = (GetSystemMetrics(SM_CXSCREEN) - bBackground.bmWidth) / 2;
#ifdef CENTER_WINDOW
    rect.top = (GetSystemMetrics(SM_CYSCREEN) - bBackground.bmHeight) / 2;
#else
    rect.top = (GetSystemMetrics(SM_CYSCREEN) - bBackground.bmHeight) / 3;
#endif
    rect.right = rect.left + bBackground.bmWidth;
    rect.bottom = rect.top + bBackground.bmHeight;

    AdjustWindowRect(&rect, iWsStyle, FALSE);
    g_hMainWindow = CreateWindow(szAutoRunClass, szAppTitle, iWsStyle, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, g_hInst, hbBackground);

    if (g_hMainWindow) {
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        ret = (int)msg.wParam;
    }
exit:
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = NULL;
    }

    if (hbBackground) {
        DeleteObject(hbBackground);
    }

    return ret;
}

// AUTORUN.EXE:0x0040265c
// Autorun program uses a different entry function
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
    ExitProcess(ret);
    return ret; // never reached here
}


