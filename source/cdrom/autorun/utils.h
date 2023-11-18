#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

extern HPALETTE PaletteFromHDC(HDC hdc);
extern void GetRealWindowsDirectory(char *directory, int length);
extern BOOL _PathStripToRoot(LPSTR path);

#endif // UTILS_H


