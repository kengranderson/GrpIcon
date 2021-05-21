#include "GrpIcon.h"

#define hWndMDIClient (GetFirstChild(hWndPM))

void	Progman_OnNCDestroy(HWND hWnd);

/**************************************************************************************************
*   New window procedure for Progman. In GI, mostly used to do funky palette stuff                *
**************************************************************************************************/
LRESULT CALLBACK _export Progman_NewWndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	switch (msg)
		{
		HANDLE_MSG(hWnd, WM_PALETTECHANGED, Progman_OnPaletteChanged);
		HANDLE_MSG(hWnd, WM_QUERYNEWPALETTE, Progman_OnQueryNewPalette);
		HANDLE_MSG(hWnd, WM_DRAWITEM, Progman_OnDrawItem);
		HANDLE_MSG(hWnd, WM_MEASUREITEM, Progman_OnMeasureItem);
		HANDLE_MSG(hWnd, WM_SYSCOLORCHANGE, Progman_OnSysColorChange);
		HANDLE_MSG(hWnd, WM_COMMAND, Progman_OnCommand);
		HANDLE_MSG(hWnd, WM_MENUCHAR, Progman_OnMenuChar);
		HANDLE_MSG(hWnd, WM_NCDESTROY, Progman_OnNCDestroy);
		HANDLE_SUBCLASS_DEFAULT(_App.fpOldWndProc);
		}
	}


void	Progman_OnPaletteChanged (HWND hWndPM, HWND hwndPaletteChange)
	{
	// Palette changed message -- someone changed the palette
	//  somehow.  We must make sure that all the MDI child windows
	//  realize their palettes here.

	static BOOL fInPalChange = FALSE;

	if (!fInPalChange)
		{
		fInPalChange = TRUE;
		SendMessageToAllChildren(hWndMDIClient, WM_PALETTECHANGED, (WPARAM) hwndPaletteChange, 0L);
		fInPalChange = FALSE;
		}
	return;
	}

BOOL	Progman_OnQueryNewPalette (HWND hWndPM)
	{
	// We get a QUERYNEWPALETTE message when our app gets the
	//  focus.  We need to realize the currently active MDI
	//  child's palette as the foreground palette for the app.
	//  We do this by sending our MYWM_QUERYNEWPALETTE message
	//  to the currently active child's window procedure.  See
	//  the comments in CHILD.C (at the top of the file) for
	//  more info on this.

	HWND	hActive = FORWARD_WM_MDIGETACTIVE(hWndMDIClient, SendMessage);

	if (hActive)
		return((BOOL) SendMessage(hActive, ICS_WM_QUERYNEWPALETTE, (WPARAM) hWndPM, 0L));

	return(FALSE);
	}

void	Progman_OnMeasureItem (HWND hWndPM, LPMEASUREITEMSTRUCT lpMis)
	{
	DWORD	dwActive;

	dwActive = SendMessage(hWndMDIClient, WM_MDIGETACTIVE, 0, 0L);
	if (HIWORD(dwActive) == 1)			// MDI Child is maximized
		PMGroup_OnMeasureItem((HWND) LOWORD(dwActive), lpMis);
	return;
	}

void	Progman_OnDrawItem (HWND hWndPM, const DRAWITEMSTRUCT FAR* lpDis)
	{
	DWORD	dwActive;

	dwActive = SendMessage(hWndMDIClient, WM_MDIGETACTIVE, 0, 0L);
	if (HIWORD(dwActive) == 1)			// MDI Child is maximized
		PMGroup_OnDrawItem((HWND) LOWORD(dwActive), lpDis);
    return;
	}

void	Progman_OnSysColorChange (HWND hWndPM)
	{
	static BOOL fInSysColorChange = FALSE;

	// Reset default brush for PMGroup backgrounds
	_PMGroup.hbrOldBackground = _PMGroup.hbrBackground;
	DeleteBrush(_PMGroup.hbrBackground);
	_PMGroup.hbrBackground = CreateSolidBrush(GetSysColor((UINT) _PMGroup.hbrSysColor - 1));

	// Reset App menu brush colors.
	DeleteBrush(_App.hbr[0]);
	DeleteBrush(_App.hbr[1]);
    _App.hbr[0] = CreateSolidBrush(GetSysColor(COLOR_MENU));
	_App.hbr[1] = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));

	if (!fInSysColorChange)
		{
		fInSysColorChange = TRUE;
		SendMessageToAllChildren(hWndMDIClient, WM_SYSCOLORCHANGE, 0, 0L);
		fInSysColorChange = FALSE;
		}
	CallWindowProc(_App.fpOldWndProc, hWndPM, WM_SYSCOLORCHANGE, 0, 0L);
	return;
	}

/************************************************************************************************
*   Function:   void PMGroup_OnSysCommand (HWND hwnd, UINT cmd, int x, int y)                   *
*                                                                                               *
*   Handles WM_SYSCOMMAND commands.                                                             *
*                                                                                               *
************************************************************************************************/
void	Progman_OnCommand (HWND hWndPM, int id, HWND hwndCtl, UINT codeNotify)
	{
	HWND	hWndActive;

	switch (id)
		{
		case ID_ASSIGNICON:
		case ID_SETCOLOR:
		case ID_WALLPAPER:
		case ID_ABOUT:
			hWndActive = FORWARD_WM_MDIGETACTIVE(hWndMDIClient, SendMessage);
			FORWARD_WM_SYSCOMMAND(hWndActive, id, 0, 0, SendMessage);
			break;
		default:
			CallWindowProc((WNDPROC) _App.fpOldWndProc, hWndPM, WM_COMMAND, (WPARAM)(int)(id), MAKELPARAM((UINT)(hwndCtl), (codeNotify)));
		}
	return;
	}



//---------------------------------------------------------------------
//
// Function:   SetMessageToAllChildren
//
// Purpose:    Passes a message to all children of the specified window.
//
// Parms:      hWnd == Parent window.
//             message == message to pass to all children.
//             wParam  == wParam of message to pass to all children.
//             lParam  == lParam of message to pass to all children.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//
//---------------------------------------------------------------------

void	SendMessageToAllChildren (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
	HWND	hChild;

	if (hChild = GetFirstChild(hWnd))     // Get 1st child.
		do
			SendMessage(hChild, message, wParam, lParam);
		while (hChild = GetNextSibling(hChild));
	return;
	}

DWORD   Progman_OnMenuChar (HWND hWnd, UINT ch, UINT flags, HMENU hMenu)
	{
	DWORD   dwRetVal = 0L;

	// Only process at Progman level if the MF_POPUP flag is set but MF_SYSMENU is not set.
	// This means that a PMGroup is maximized.
	if ((flags & MF_POPUP) & ~(flags & MF_SYSMENU))
		dwRetVal = ProcessMenuChar(ch, flags, hMenu);

	return(dwRetVal ? dwRetVal :
		CallWindowProc(_App.fpOldWndProc, hWnd, WM_MENUCHAR, (WPARAM) (UINT) (ch), MAKELPARAM((flags), (UINT) (hMenu))));
	}

void	Progman_OnNCDestroy (HWND hWnd)
	{
	FORWARD_WM_QUIT(_App.hWndFrame, 0, PostMessage);
	CallWindowProc(_App.fpOldWndProc, hWnd, WM_NCDESTROY, 0, 0L);
	}
