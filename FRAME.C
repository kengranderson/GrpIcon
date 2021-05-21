#include "GrpIcon.h"

extern	char	_szAppName[];	// Application/Class name.

BOOL Frame_Initialize (APP* pApp)
	{
	WNDCLASS cls;

	cls.hCursor         = LoadCursor(NULL, IDC_ARROW);
	cls.hIcon           = LoadIcon(pApp->hInst,
		MAKEINTRESOURCE(HIBYTE(GetVersion()) ? IDI_GRP31 : IDI_GRP30));
	cls.lpszMenuName    = NULL;
	cls.hInstance       = pApp->hInst;
	cls.lpszClassName   = _szAppName;
	cls.hbrBackground   = (HBRUSH) (COLOR_WINDOW+1);
	cls.lpfnWndProc     = Frame_WndProc;
	cls.style           = NULL;
	cls.cbWndExtra      = sizeof(FRAME*);
	cls.cbClsExtra  = 0;

	if (!RegisterClass(&cls))
		return(FALSE);

	return(TRUE);
	}

void Frame_Terminate (APP* pApp)
	{
	UnregisterClass(_szAppName, pApp->hInst);
	return;
	}

LRESULT CALLBACK _export Frame_WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	FRAME* pFrm = Frame_GetPointer(hWnd);

	HANDLE_NULLPTR(pFrm, Frame, FRAME);
	switch (msg)
		{
		HANDLE_MSG(pFrm, WM_CREATE, Frame_OnCreate);
		HANDLE_MSG(pFrm, WM_QUERYOPEN, Frame_OnQueryOpen);
		HANDLE_MSG(pFrm, WM_SYSCOMMAND, Frame_OnSysCommand);
		HANDLE_MSG(pFrm, WM_CLOSE, Frame_OnClose);
		HANDLE_MSG(pFrm, WM_DESTROY, Frame_OnDestroy);
		HANDLE_MSG(pFrm, WM_NCDESTROY, Frame_OnNCDestroy);
		HANDLE_MSG(pFrm, WM_ISINSTALLED, Frame_OnIsInstalled);
		HANDLE_DEFAULT(Frame_DefProc);
		}
	}

HWND Frame_CreateWindow (LPCSTR lpszText, int x, int y, int cx, int cy, HINSTANCE hInst)
	{
	return(CreateWindow(_szAppName, lpszText,
			WS_OVERLAPPEDWINDOW | WS_ICONIC, x, y, cx, cy,
			NULL, NULL, hInst, NULL));
	}

BOOL Frame_OnCreate (FRAME* pFrm, CREATESTRUCT FAR* lpCreateStruct)
	{
	HMENU	hSysMenu = GetSystemMenu(pFrm->hWnd, FALSE);       // Set system menu to hSysMenu

	// Update system menu of self
	InsertMenu(hSysMenu, 0, MF_BYPOSITION | MF_STRING, CMD_HIDE, "&Hide");
	RemoveMenu(hSysMenu, SC_RESTORE, MF_BYCOMMAND);
	RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
	RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
	RemoveMenu(hSysMenu, SC_MINIMIZE, MF_BYCOMMAND);

	return(TRUE);
	}

void Frame_OnSysCommand (FRAME* pFrm, UINT cmd, int x, int y)
	{
	if (cmd == CMD_HIDE)
		ShowWindow(pFrm->hWnd, SW_HIDE);
	else
		FORWARD_WM_SYSCOMMAND(pFrm->hWnd, cmd, x, y, Frame_DefProc);
	return;
	}

BOOL Frame_OnQueryOpen (FRAME* pFrm)
	{
	return(FALSE);
	}

void Frame_OnDestroy (FRAME* pFrm)
	{
	if (_lpICSras && _lpICSras->fHelpCalled)
		WinHelp(pFrm->hWnd, _lpICSras->szHelpFile, HELP_QUIT, 0L);
	return;
	}

void Frame_OnClose (FRAME* pFrm)
	{
	PostQuitMessage(0);
	}

void Frame_OnNCDestroy (FRAME* pFrm)
	{
	Frame_SetPointer(pFrm->hWnd, NULL);
	LocalFree((HLOCAL) pFrm);
	pFrm = NULL;
	return;
	}

BOOL	Frame_OnIsInstalled (FRAME* pFrm)
	{
	return(_App.fInstalled);
	}

