#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

HPALETTE PaletteFromHDC(HDC hdc);
void GetRealWindowsDirectory(char *directory, int length);
BOOL _PathStripToRoot(LPSTR path);

#endif // UTILS_H


