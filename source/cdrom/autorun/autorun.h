#ifndef AUTORUN_H
#define AUTORUN_H

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <regstr.h>

#include "resource.h"
#include "macros.h"
#include "ordinals.h"

// from utils.c
extern HPALETTE PaletteFromHDC(HDC hdc);
extern void GetRealWindowsDirectory(char *directory, int length);
extern BOOL _PathStripToRoot(LPSTR path);

// #define CENTER_WINDOW

#endif // AUTORUN_H


