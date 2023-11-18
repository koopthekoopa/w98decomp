#include "autorun.h"

// AUTORUN.EXE:0x004026e0
HPALETTE PaletteFromHDC(HDC hdc) {
    DWORD pal[257];
    int i, colCount;

    colCount = GetDIBColorTable(hdc, 0, 256, (LPRGBQUAD)&pal[1]);

    for (i = 1; i <= colCount; i++)
        pal[i] = RGB(GetBValue(pal[i]), GetGValue(pal[i]), GetRValue(pal[i]));

	pal[0] = MAKELONG(0x300, colCount);

	return CreatePalette((LPLOGPALETTE)&pal[0]);
}

// AUTORUN.EXE:0x00402755
void GetRealWindowsDirectory(char *directory, int length) {
	char realWinDir[MAX_PATH];
	HKEY regKey;
	
	if (!*realWinDir) {
		LONG len = sizeof(realWinDir);
		regKey = NULL;
		
		if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Setup", &regKey)) {
			if (RegQueryValueEx(regKey, "SharedDir", NULL, NULL, (LPBYTE)realWinDir, &len)) {
				*realWinDir = 0;
			}
			
			RegCloseKey(regKey);
		}
		
		if (!*realWinDir) {
			GetWindowsDirectory(realWinDir, sizeof(realWinDir));
		}
	}
	
	if (length >= sizeof(realWinDir)) {
		length = sizeof(realWinDir);
	}
	
	lstrcpyn(directory, realWinDir, length);
}

// AUTORUN.EXE:0x004027f2
BOOL _PathStripToRoot(LPSTR path) {
	while (!PathIsRoot(path)) {
		if (!PathRemoveFileSpec(path)) {
			return FALSE;
		}
	}
	return TRUE;
}


