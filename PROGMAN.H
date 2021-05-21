#ifndef _INC_PROGMAN
#define _INC_PROGMAN

// Window procedures
LRESULT CALLBACK _export Progman_NewWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void	Progman_OnPaletteChanged(HWND hWndPM, HWND hwndPaletteChange);
BOOL	Progman_OnQueryNewPalette(HWND hWndPM);
void	Progman_OnSysColorChange(HWND hWnd);
void	Progman_OnMeasureItem(HWND hWnd, LPMEASUREITEMSTRUCT lpMis);
void	Progman_OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT FAR* lpDis);
//void	Progman_OnSysCommand(HWND hWnd, UINT cmd, int x, int y);
void	Progman_OnCommand(HWND hWndPM, int id, HWND hwndCtl, UINT codeNotify);
DWORD   Progman_OnMenuChar(HWND hWnd, UINT ch, UINT flags, HMENU hMenu);

void	SendMessageToAllChildren(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif	//	_INC_PROGMAN
