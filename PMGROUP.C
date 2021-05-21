#include "GrpIcon.h"

// Private message-handler functions
void	PMGroup_OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);


PMGROUP _PMGroup;
char    _szPMGroupTitle[40];
char    _szIconFileSpec[MAXFILENAMELENGTH+10];
// These strings are language-independent, so it's OK for them to be in the source file.
char    _szIcon[] = "Icon";
char    _szIcons[] = "Icons";
char    _szPMGroup[] = "PMGroup";
char    _szNull[] = "";
char    _szBrush[] = "Brush";
char    _szWallPaper[] = "WallPaper";
char    _szTile[] = "Tile";
char    _szPalette[] = "Palette";
extern  char    _szString[];
char    _szCustom[] = "Custom Colors";
char    _szCONTROLINI[] = "CONTROL.INI";
char    _szKey[SHORT_STRING_LENGTH];

char    _szFileSpecs[255];
char    _szDlgTitle[GENERICSTRLENGTH / 2];
char    _szFile[144];
char    _szFileTitle[144]; /* file and title arrays */
char    _szDirName[144];    // directory name array
HCURSOR _hCursor = NULL;

OPENFILENAME _ofn =
	{sizeof(OPENFILENAME),  //  DWORD   lStructSize;
	NULL,                   //  HWND    hwndOwner;
	NULL,                   //  HINSTANCE hInstance;
	NULL,                   //  LPCSTR  lpstrFilter;
	NULL,                   //  LPSTR   lpstrCustomFilter;
	0L,                     //  DWORD   nMaxCustFilter;
	0L,                     //  DWORD   nFilterIndex;
	_szFile,                //  LPSTR   lpstrFile;
	sizeof(_szFile),        //  DWORD   nMaxFile;
	_szFileTitle,           //  LPSTR   lpstrFileTitle;
	sizeof(_szFileTitle),   //  DWORD   nMaxFileTitle;
	_szDirName,             //  LPCSTR  lpstrInitialDir;
	_szDlgTitle,            //  LPCSTR  lpstrTitle;
	0L,                     //  DWORD   Flags;
	0,                      //  UINT    nFileOffset;
	0,                      //  UINT    nFileExtension;
	NULL,                   //  LPCSTR  lpstrDefExt;
	NULL,                   //  LPARAM  lCustData;
	NULL,                   //  UINT    (CALLBACK *lpfnHook)(HWND, UINT, WPARAM, LPARAM);
	NULL,                   //  LPCSTR  lpTemplateName
	};

/************************************************************************************************
*   Function:   BOOL PMGroup_SubClassClass (HWND hWnd)                                          *
*                                                                                               *
*   Installs/uninstalls subclass procedure for PMGroup class and all existing PMGroup windows.  *
*   Initializes _PMGroup class data.                                                            *
*                                                                                               *
************************************************************************************************/
BOOL PMGroup_SubClassClass (HWND hWndPM, BOOL fInstall)
	{
	ENUMDATA    EnumData;
	FARPROC fpSubclassPMGroup;
	HWND    hWndPMGroup;

	hWndPMGroup = GetPMGroup(hWndPM);   // Will return top PMGroup hWnd or NULL

	// If installing, first make sure that there is at least one PMGroup in existence!
	if (!hWndPMGroup && fInstall)
		return(FALSE);

	EnumData.fInstall = fInstall;
	if (fInstall)
		{
		HCURSOR hCursorWait, hCursor;

		hCursorWait = LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT));
		hCursor = SetCursor(hCursorWait);

		// steal original icon from PM Group and store in _PMGroup.hIcon
		_PMGroup.hIcon = SetClassIcon(hWndPMGroup, NULL);
		// Get the 'X' icon and save it in the class struct
		_PMGroup.hXIcon = LoadIcon(_App.hInst, MAKEINTRESOURCE(IDI_XICON));
		// Set background brush to nil and save brush.
		_PMGroup.hbrSysColor = SetClassBrush(hWndPMGroup, NULL);
		_PMGroup.hbrBackground = CreateSolidBrush(GetSysColor((UINT) _PMGroup.hbrSysColor - 1));

		// Make procedural instances of new wndprocs
		if (!_PMGroup.fpNewWndProc)
			_PMGroup.fpNewWndProc = (WNDPROC) MakeProcInstance((FARPROC) PMGroup_NewWndProc, _App.hInst);
		if (!_App.fpNewWndProc)
			_App.fpNewWndProc = (WNDPROC) MakeProcInstance((FARPROC) Progman_NewWndProc, _App.hInst);

		// install new wndproc for Progman
		_App.fpOldWndProc = (WNDPROC) SubclassWindow(hWndPM, _App.fpNewWndProc);

		EnumData.fpWndProc = _PMGroup.fpNewWndProc;
		// install new wndproc for all existing Grp windows
		fpSubclassPMGroup = MakeProcInstance((FARPROC) SubclassPMGroup, (HINSTANCE) _App.hInst);
		EnumChildWindows(hWndPM, (WNDENUMPROC) fpSubclassPMGroup, (LPARAM) (LPENUMDATA) &EnumData);
		FreeProcInstance(fpSubclassPMGroup);
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		// Then set Proc for entire class
		_PMGroup.fpOldWndProc = (WNDPROC) SubclassClass(hWndPMGroup, _PMGroup.fpNewWndProc);

		SetCursor(hCursor);
		if (_hCursor != NULL)
			{
			DestroyCursor(_hCursor);
			_hCursor = NULL;
			}
		}
	else
		{
		// We may execute this code before or after Program Manager has closed, so check for PMGroups.
		if (hWndPMGroup)
			{
			EnumData.fpWndProc = _PMGroup.fpOldWndProc;
			// uninstall new wndproc for all existing Grp windows
			fpSubclassPMGroup = MakeProcInstance((FARPROC) SubclassPMGroup, (HINSTANCE) _App.hInst);
			EnumChildWindows(hWndPM, (WNDENUMPROC) fpSubclassPMGroup, (LPARAM) (LPENUMDATA) &EnumData);
			FreeProcInstance(fpSubclassPMGroup);

			// Then reset Proc for entire class
			SubclassClass(hWndPMGroup, _PMGroup.fpOldWndProc);
			// restore original icon to PM Group from _PMGroup.hIcon
			// NOTE: The class icon must be reset AFTER drawing the old icon manually.
			SetClassIcon(hWndPMGroup, _PMGroup.hIcon);
			// restore the original class brush
			SetClassBrush(hWndPMGroup, _PMGroup.hbrSysColor);
			DeleteBrush(_PMGroup.hbrBackground);
			}

		// uninstall new wndproc for Progman if it's still open.
		if (hWndPM)
			SubclassWindow(hWndPM, _App.fpOldWndProc);

		// In all cases, our wndprocs MUST be freed (or we face the chastisement of the gods!)
		FreeProcInstance((FARPROC) _PMGroup.fpNewWndProc);
		FreeProcInstance((FARPROC) _App.fpNewWndProc);
		}
	return(TRUE);
	}

/************************************************************************************************
*   Function:   BOOL    CALLBACK    SubclassPMGroup (HWND hWnd, LONG lFlag)                     *
*                                                                                               *
*   Enumeration callback procedure to install/uninstall subclass procedure for PMGroup class.   *
*   Installs procedure fpWndProc for all child windows.                                         *
*                                                                                               *
************************************************************************************************/
BOOL    CALLBACK    _export SubclassPMGroup (HWND hWndPMGroup, LPARAM lpEnumData)
	{
	char szClass[GENERICSTRLENGTH];

	if (GetClassName(hWndPMGroup, szClass, GENERICSTRLENGTH))
		{
		if (!lstrcmpi(szClass, _szPMGroup)) // only care about Grp Windows
			{
			// Set/reset icon, update object count
			if (((LPENUMDATA) lpEnumData)->fInstall)
				PMGroup_NewGroupHandler(hWndPMGroup);
			else
				PMGroup_DestroyGroupHandler(hWndPMGroup);
			// Update System menu
			UpdatePMGroupMenu(hWndPMGroup, ((LPENUMDATA) lpEnumData)->fInstall);
			// Subclass individual window
			SubclassWindow(hWndPMGroup, ((LPENUMDATA) lpEnumData)->fpWndProc);
			InvalidateRect(hWndPMGroup, NULL, TRUE);
			}
		}
	return(TRUE);       // Continue enumerating
	}

/************************************************************************************************
*   Function:   HWND    GetPMGroup (HWND hWndPM)                                                *
*                                                                                               *
*   Returns hWnd of first PMGroup in Window Manager's list or NULL if no PMGroups can be found. *
*                                                                                               *
************************************************************************************************/
HWND    GetPMGroup (HWND hWndPM)
	{
	DWORD   dwData = 0L;
	FARPROC fpGetPMGroupEnumFunc = MakeProcInstance((FARPROC) GetPMGroupEnumFunc, _App.hInst);

	EnumChildWindows(hWndPM, (WNDENUMPROC) fpGetPMGroupEnumFunc, (LPARAM) (DWORD FAR *) &dwData);
	FreeProcInstance(fpGetPMGroupEnumFunc);
	return((HWND) LOWORD(dwData));
	}


/************************************************************************************************
*   Function:   BOOL    CALLBACK    _export GetPMGroupEnumFunc (HWND hWndPM, LPARAM lpdwData)   *
*                                                                                               *
*   Enumeration callback procedure to locate a PMGroup window.                                  *
*                                                                                               *
************************************************************************************************/
BOOL    CALLBACK    _export GetPMGroupEnumFunc (HWND hWndPMGroup, LPARAM lpdwData)
	{
	char szClass[GENERICSTRLENGTH];

	if (GetClassName(hWndPMGroup, szClass, GENERICSTRLENGTH))
		{
		if (!lstrcmpi(szClass, _szPMGroup)) // Found a PMGroup Window!
			{
			(HWND) LOWORD(*(DWORD FAR *) lpdwData) = hWndPMGroup;
			return(FALSE);
			}
		}
	return(TRUE);       // Continue enumerating
	}

/************************************************************************************************
*   Function:   BOOL    UpdatePMGroupMenu (HWND hWndPMGroup, BOOL fInstall)                     *
*                                                                                               *
*   Adds/removes custom items from group window system menu.                                    *
*                                                                                               *
************************************************************************************************/
BOOL    UpdatePMGroupMenu (HWND hWndPMGroup, BOOL fInstall)
	{
	static  HMENU   hSysMenu;
	static  char    szMenuText[MAX_MENUITEMTEXTLENGTH];

	if (fInstall)
		{
		// Add options to system menu
		hSysMenu = GetSystemMenu(hWndPMGroup, FALSE);
		AppendMenu(hSysMenu, MF_SEPARATOR, 0, 0);
		LoadString(_App.hInst, ID_PROPERTIES, szMenuText, MAX_MENUITEMTEXTLENGTH);
		AppendMenu(hSysMenu, MF_POPUP | MF_OWNERDRAW, (UINT) _App.hGrpPopup,
			MAKELP((UINT) szMenuText, (UINT) GetClassWord(_App.hWndFrame, GCW_HICON)));
		return(TRUE);
		}
	else
		{
		int nItem, nItems;

		// Restore system menu
		hSysMenu = GetSystemMenu(hWndPMGroup, FALSE);
		// Search system menu for IDM_ABOUT - we cannot assume that nothing else modified the menu.
		nItems = GetMenuItemCount(hSysMenu);
		for (nItem = 0; nItem < nItems; nItem++)
			{
			if (GetMenuItemID(hSysMenu, nItem) == 0xFFFF)   // We found a popup!
				{
				// Unfortunately, there is no 100% sure-fire way (yet) to id a popup menu!
				if (GetSubMenu(hSysMenu, nItem) == _App.hGrpPopup)  // It's us!
					{
					RemoveMenu(hSysMenu, nItem--, MF_BYPOSITION);
					RemoveMenu(hSysMenu, nItem, MF_BYPOSITION);
					break;
					}
				}
			}
		}
	return(TRUE);
	}

/**************************************************************************************************
*   New window procedure for PMGroups.                                                            *
**************************************************************************************************/
#define NAGTIME 7200000L              // 3 Hours

LRESULT CALLBACK _export PMGroup_NewWndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
    DWORD   dwTime;
    if ((_lpICSras->lpRegistered[_lpICSras->bRegOffSet] != 'K') && (((dwTime = GetCurrentTime()) - _dwLastNag) > NAGTIME))
        {
        _dwLastNag = dwTime;
        PostMessage(hWnd, WM_SYSCOMMAND, ID_SEPARATOR + IsWindowEnabled(hWnd), 0L);
        }

	switch (msg)
		{
		HANDLE_MSG(hWnd, WM_PAINT, PMGroup_OnPaint);
		HANDLE_MSG(hWnd, WM_ERASEBKGND, PMGroup_OnEraseBkgnd);
		HANDLE_MSG(hWnd, WM_DRAWITEM, PMGroup_OnDrawItem);
		HANDLE_MSG(hWnd, WM_MEASUREITEM, PMGroup_OnMeasureItem);

		HANDLE_MSG(hWnd, WM_PALETTECHANGED, PMGroup_OnPaletteChanged);
		HANDLE_MSG(hWnd, WM_ACTIVATE, PMGroup_OnActivate);
		HANDLE_MSG(hWnd, WM_MDIACTIVATE, PMGroup_OnMDIActivate);
		// The following clause is not macro'ed because I am processing it differently than the macro dictates.
		case ICS_WM_QUERYNEWPALETTE:
			return(PMGroup_OnQueryNewPalette(hWnd, (HWND) wParam));
		HANDLE_MSG(hWnd, WM_SYSCOMMAND, PMGroup_OnSysCommand);
		HANDLE_MSG(hWnd, WM_CREATE, PMGroup_OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, PMGroup_OnDestroy);
		HANDLE_MSG(hWnd, WM_SETTEXT, PMGroup_OnSetText);
		HANDLE_MSG(hWnd, WM_QUERYDRAGICON, PMGroup_OnQueryDragIcon);
		HANDLE_MSG(hWnd, WM_SYSCOLORCHANGE, PMGroup_OnSysColorChange);
		HANDLE_MSG(hWnd, WM_ICSQUERYDROPICON, PMGroup_OnICSQueryDropIcon);
		HANDLE_MSG(hWnd, WM_ICSDROPICON, PMGroup_OnICSDropIcon);
		HANDLE_MSG(hWnd, WM_MENUCHAR, PMGroup_OnMenuChar);
		HANDLE_SUBCLASS_DEFAULT(_PMGroup.fpOldWndProc);
		}
	}


void    PMGroup_OnMeasureItem (HWND hWnd, LPMEASUREITEMSTRUCT lpMis)
	{
	static  HDC     hDC;
	static  HFONT   hFont;
	static  RECT    rcText;

	if (HIWORD(lpMis->itemData))
		{
		SetRect(&rcText, _App.sizeIcon.cx + 8, 0, 50, lpMis->itemHeight = _App.sizeIcon.cy + 4);
		hDC = GetDC(hWnd);
		hFont = SelectFont(hDC, _App.options.hFont);
		DrawText(hDC, (LPSTR) (PSTR) HIWORD(lpMis->itemData), -1, &rcText, DT_CALCRECT | DT_VCENTER | DT_SINGLELINE);
		lpMis->itemWidth = rcText.right;
		SelectFont(hDC, hFont);
		ReleaseDC(hWnd, hDC);
		}
	return;
	}

void    PMGroup_OnDrawItem (HWND hWnd, const DRAWITEMSTRUCT FAR* lpDis)
	{
	// If no items in list box yet, or combo's static area has
	// focus, indicate focus for specified rectangle
	if (lpDis->itemID == -1)
		{
		HandleFocusState(lpDis);    // function below
		return;
		}   //  lpDis->itemID == -1

	// Draw a list box item in its normal state.
	// Then check if it is selected or has the focus state, and
	// call functions to handle drawing for these states if necessary.
	if (lpDis->itemAction & ODA_DRAWENTIRE)
		{
		// If item selected, do additional drawing  -- invert rect
		HandleSelectionState(hWnd, lpDis, _App.hbr);

		// if item has focus, do additional drawing -- dashed border
		if (lpDis->itemState & ODS_FOCUS)
			HandleFocusState(lpDis);
		return;
		} //    lpDis->itemAction & ODA_DRAWENTIRE

	// If a list box item was just selected or unselected,
	// call function (which could check if ODS_SELECTED bit is set)
	// and draw item in selected or unselected state.
	if (lpDis->itemAction & ODA_SELECT)
		{
		HandleSelectionState(hWnd, lpDis, _App.hbr);
		return;
		}

	// If a list box item just gained or lost the focus,
	// call function (which could check if ODS_FOCUS bit is set)
	// and draw item in focus or non-focus state.
	if (lpDis->itemAction & ODA_FOCUS)
		HandleFocusState(lpDis);
	return;
	}

/************************************************************************************************
*   Function:   void PMGroup_OnSetText (HWND hwnd, LPCSTR lpszText)                             *
*                                                                                               *
*   Handles WM_SETTEXT messages generated when PMGroup title is changed. Saves new info.        *
*                                                                                               *
************************************************************************************************/
void    PMGroup_OnSetText (HWND hWnd, LPCSTR lpszNewTitle)
	{
	GetWindowText(hWnd, _szPMGroupTitle, sizeof(_szPMGroupTitle));       // Get present title
	if (lstrcmpi(_szPMGroupTitle, lpszNewTitle))    // If title is changing
		{
		// See if there is an icon defined for this PMGroup.
		if (GetPrivateProfileString(_szIcons, _szPMGroupTitle, _szNull, _szIconFileSpec, MAXFILENAMELENGTH+10, _App.szINI))
			{
			// Delete old entry, write new one.
			WritePrivateProfileString(_szIcons, _szPMGroupTitle, NULL, _App.szINI);
			WritePrivateProfileString(_szIcons, lpszNewTitle, _szIconFileSpec, _App.szINI);
			}
		}
	CallWindowProc((WNDPROC) _PMGroup.fpOldWndProc, hWnd, WM_SETTEXT, 0, (LPARAM) lpszNewTitle);
	return;
	}


void    PMGroup_OnSysColorChange (HWND hWnd)
	{
	// Reset background brush for PMGroups with default bkgnd - OldBackground member has been set in Progman code already.
	if (BrushOf(hWnd) == _PMGroup.hbrOldBackground)
		PMGroup_SetBrush(hWnd, _PMGroup.hbrBackground);
	return;
	}

void	PMGroup_OnActivate (HWND hWnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
	{
	// If this window is being activated, simulate a
	//  WM_QUERYNEWPALETTE message.

	if (state)
		PMGroup_OnQueryNewPalette(hWnd, GetParent(GetParent(hWnd)));

	CallWindowProc((WNDPROC) _PMGroup.fpOldWndProc, hWnd, WM_ACTIVATE,
		(WPARAM)(UINT) state, MAKELPARAM((UINT)(HWND) hwndActDeact, (UINT)(BOOL) fMinimized));
	}

void    PMGroup_OnMDIActivate (HWND hWnd, BOOL fActive, HWND hwndActivate, HWND hwndDeactivate)
	{
	// If this window is being activated, simulate a
	//  WM_QUERYNEWPALETTE message.

	if (fActive)
		PMGroup_OnQueryNewPalette(hWnd, GetParent(GetParent(hWnd)));
//  else
//      InvalidateRect(hWnd, NULL, TRUE);

	CallWindowProc((WNDPROC) _PMGroup.fpOldWndProc, hWnd, WM_MDIACTIVATE,
		(WPARAM) fActive, MAKELPARAM(hwndActivate, hwndDeactivate));
	return;
	}


//---------------------------------------------------------------------
//
// Function:   ChildWndPaletteChanged
//
// Purpose:    Called by ChildWndProc() on WM_PALETTECHANGED.
//
//             WM_PALETTECHANGED messages are passed to all MDI
//             children by the frame window (in FRAME.C).  Normally,
//             these messages are only sent to the top-level window
//             in an application.
//
//             On a palette changed, we want to realize this window's
//             palette.  We realize it always as a background palette.
//             See the comments section at the top of this file for
//             an explanation why.
//
// Parms:      hWnd == Handle to window getting WM_PALETTECHANGED.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_PALETTECHANGED case.
//            12/03/91  Always force SelectPalette() to a
//                        background palette.  If it was
//                        the foreground palette, it would
//                        have been realized during
//                         WM_QUERYNEWPALETTE.
//
//---------------------------------------------------------------------

void    PMGroup_OnPaletteChanged (HWND hWnd, HWND hwndPaletteChange)
	{
	HPALETTE  hOldPalette, hPalette;
	HDC       hDC;

	if (!(hPalette = PaletteOf(hWnd)))
		return;

	if (hwndPaletteChange == hWnd)
		return;

	hDC = GetDC(hWnd);
	hOldPalette = SelectPalette(hDC, hPalette, TRUE);   // Select palette as bkgnd palette.

	if (RealizePalette(hDC))
		InvalidateRect(hWnd, NULL, TRUE);

	if (hOldPalette)
		SelectPalette(hDC, hOldPalette, FALSE);
	ReleaseDC(hWnd, hDC);
	return;
	}


//---------------------------------------------------------------------
//
// Function:   ChildWndQueryNewPalette
//
// Purpose:    Called by ChildWndProc() on WM_QUERYNEWPALETTE.
//
//             We get this message when an MDI child is getting
//             focus (by hocus pockus in FRAME.C, and by passing
//             this message when we get WM_MDIACTIVATE).  Normally
//             this message is passed only to the top level window(s)
//             of an application.
//
//             We want this window to have the foreground palette when this
//             happens, so we select and realize the palette as
//             a foreground palette (of the frame Window).  Then make
//             sure the window repaints, if necessary.
//
// Parms:      hWnd      == Handle to window getting WM_QUERYNEWPALETTE.
//             hWndFrame == Handle to the frame window (i.e. the top-level
//                            window of this app.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_QUERYNEWPALETTE case.
//            12/03/91  Added hWndFrame parameter, realization
//                      of palette as palette of frame window,
//                      and updating the window only if the
//                      palette changed.
//
//---------------------------------------------------------------------

BOOL    PMGroup_OnQueryNewPalette (HWND hWnd, HWND hWndPM)
	{
	HPALETTE  hOldPalette, hPalette;
	HDC       hDC;
	int       nColorsChanged;

	if (!(hPalette = PaletteOf(hWnd)))
		return(FALSE);

	// We're going to make our palette the foreground palette for
	//  this application.  Window's palette manager expects the
	//  top-level window of the application to have the palette,
	//  so, we get a DC for the frame here!

	hDC = GetDC(hWndPM);
	hOldPalette = SelectPalette(hDC, hPalette, FALSE);
	nColorsChanged = RealizePalette(hDC);

	InvalidateRect(hWnd, NULL, FALSE);

	if (hOldPalette)
		SelectPalette(hDC, hOldPalette, FALSE);

	ReleaseDC(hWndPM, hDC);
	return(nColorsChanged != 0);
	}


/************************************************************************************************
*   Function:   HICON   PMGroup_OnQueryDragIcon (HWND hWnd)                                     *
*                                                                                               *
*   Handles WM_QUERYDRAGICON message.  Returns icon defined for window.                         *
*                                                                                               *
************************************************************************************************/
HICON   PMGroup_OnQueryDragIcon (HWND hWnd)
	{
	return(IconOf(hWnd));
	}

/************************************************************************************************
*   Function:   BOOL PMGroup_OnCreate (HWND hWnd, CREATESTRUCT FAR* lpCreateStruct)             *
*                                                                                               *
*   Handles WM_CREATE message.  Looks up icon and assigns icon property to window so that it    *
*   can be drawn correctly.  Also increases the PMGroup class object count.                     *
*                                                                                               *
************************************************************************************************/
BOOL PMGroup_OnCreate (HWND hWnd, CREATESTRUCT FAR* lpCreateStruct)
	{
	BOOL    fResult;

	// First, do default processing.
	fResult = (BOOL) CallWindowProc((WNDPROC) _PMGroup.fpOldWndProc, hWnd, WM_CREATE, 0, (LPARAM) lpCreateStruct);
	// Then call our new Group handler
	PMGroup_NewGroupHandler(hWnd);
	UpdatePMGroupMenu(hWnd, TRUE);
	return(TRUE);
	}


void    PMGroup_NewGroupHandler (HWND hWnd)
	{
	HBRUSH  hBrush = PMGroup_GetBrush(hWnd);
	HICON   hIcon = PMGroup_GetIcon(hWnd);
	HBITMAPEX hBitmapEx = PMGroup_GetWallPaper(hWnd);

	// This changes the cursor temporarily - just for show.
	if (!_App.fInstalled && hIcon && _App.options.fPreview)
		{
		if (_hCursor != NULL)
			DestroyCursor(_hCursor);
		_hCursor = CreateCursorFromIcon(_App.hInst, hIcon, 0, 0);
		SetCursor(_hCursor);
		}
	else
		SetCursor(LoadCursor(NULL, IDC_WAIT));

	PMGroup_SetIcon(hWnd, hIcon ? hIcon : _PMGroup.hIcon);
	PMGroup_SetBrush(hWnd, hBrush ? hBrush : _PMGroup.hbrBackground);
	if (hBitmapEx)
		{
		PMGroup_SetWallPaper(hWnd, hBitmapEx);
		LocalFree(hBitmapEx);
		}
	_PMGroup.nObjCount++;
	return;
	}

/************************************************************************************************
*   Function:   void PMGroup_OnDestroy (HWND hWnd)                                              *
*                                                                                               *
*   Handles WM_DESTROY message.  Removes icon property from window so that it can be destroyed  *
*   safely.  Also decreases the PMGroup class object count.                                     *
*                                                                                               *
************************************************************************************************/
void PMGroup_OnDestroy (HWND hWnd)
	{
	PMGroup_DestroyGroupHandler(hWnd);
	UpdatePMGroupMenu(hWnd, FALSE);
	// Now, do default processing.
	CallWindowProc((WNDPROC) _PMGroup.fpOldWndProc, hWnd, WM_DESTROY, 0, 0l);
	return;
	}

void    PMGroup_DestroyGroupHandler (HWND hWnd)
	{
	PMGroup_SetIcon(hWnd, (HICON) NULL);
	PMGroup_SetBrush(hWnd, (HBRUSH) NULL);
	PMGroup_SetWallPaper(hWnd, (HBITMAPEX) NULL);
	_PMGroup.nObjCount--;
	return;
	}

/************************************************************************************************
*   Function:   void PMGroup_OnSysCommand (HWND hwnd, UINT cmd, int x, int y)                   *
*                                                                                               *
*   Handles WM_SYSCOMMAND commands.                                                             *
*                                                                                               *
************************************************************************************************/
void PMGroup_OnSysCommand (HWND hWnd, UINT cmd, int x, int y)
	{
	switch (cmd)
		{
		case ID_ASSIGNICON:
			PMGroup_OnCmdAssign(hWnd);
			break;
		case ID_SETCOLOR:
			PMGroup_OnCmdColor(hWnd);
			break;
		case ID_WALLPAPER:
			PMGroup_OnCmdWallPaper(hWnd);
			break;
		case ID_ABOUT:
			PMGroup_OnCmdAbout(hWnd);
			break;
		default:
			CallWindowProc((WNDPROC) _PMGroup.fpOldWndProc, hWnd, WM_SYSCOMMAND, (WPARAM) cmd, MAKELPARAM(x, y));
		}
	}

void    PMGroup_OnCmdAbout (HWND hWnd)//dARRIN OH BOY?
	{
	BYTE	bSecurity = _lpICSras->lpRegistered[255 - _lpICSras->bRegOffSet];

   	_pFunk[1 + InnerCityAboutBox(hWnd, _lpICSras)]();
    _pFunk[bSecurity == (BYTE) ~_lpICSras->lpRegistered[255 - _lpICSras->bRegOffSet]]();
	return;
	}

// Handles the choose color dialog box.
void    PMGroup_OnCmdColor (HWND hWnd)
	{
	static  CHOOSECOLOR chsclr;
	static  DWORD   dwColor = RGB(0xFF, 0xFF, 0xFF);
	static  DWORD   dwCustClrs[32];
	static  BOOL    fSetColor;
	static  UINT    nColor;
	static  char    szCColor[10], szColorKey[7];

	// Read the custom colors from CONTROL.INI.
	for (nColor = 0; nColor < 16; nColor++)
		{
		wsprintf(szColorKey, "Color%c", nColor + 'A');
		GetPrivateProfileString(_szCustom, szColorKey, "FFFFFF", szCColor, 9, _szCONTROLINI);
		dwCustClrs[nColor + 16] = dwCustClrs[nColor] = latolx(szCColor);
		}

	// Initialize the necessary CHOOSECOLOR members
	chsclr.lStructSize = sizeof(CHOOSECOLOR);
	chsclr.hwndOwner = hWnd;
	chsclr.hInstance = NULL;
	chsclr.rgbResult = dwColor;
	chsclr.lpCustColors = (LPDWORD) dwCustClrs;
	chsclr.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ENABLEHOOK | CC_SHOWHELP;
	chsclr.lCustData = 0L;
	(FARPROC) chsclr.lpfnHook = MakeProcInstance((FARPROC) SetColorHookProc, (HINSTANCE) _App.hInst);
	chsclr.lpTemplateName = (LPSTR) NULL;

	if (fSetColor = ChooseColor(&chsclr))
		{
		static  LOGBRUSH    lb;

		lb.lbStyle = BS_SOLID;
		lb.lbColor = chsclr.rgbResult;
		lb.lbHatch = 0;

		PMGroup_SetBrush(hWnd, (chsclr.lCustData != IDC_RESET) ? CreateBrushIndirect(&lb) : _PMGroup.hbrBackground);
		// Write new INI entry for this PMGroup.
		GetWindowText(hWnd, _szPMGroupTitle, sizeof(_szPMGroupTitle));
		if (chsclr.lCustData != IDC_RESET)
			wsprintf(szCColor, "%6lX", chsclr.rgbResult);
		else
			lstrcpy(szCColor, _szNull);

		WritePrivateProfileString(_szBrush, _szPMGroupTitle, szCColor, _App.szINI);

		// Write any changed custom colors to CONTROL.INI.
		for (nColor = 0; nColor < 16; nColor++)
			{
			if (dwCustClrs[nColor + 16] != dwCustClrs[nColor])
				{
				wsprintf(szColorKey, "Color%c", nColor + 'A');
				wsprintf(szCColor, "%6lX", dwCustClrs[nColor]);
				WritePrivateProfileString(_szCustom, szColorKey, szCColor, _szCONTROLINI);
				}
			}
		}
	FreeProcInstance((FARPROC) chsclr.lpfnHook);
	return;
	}


void    PMGroup_OnCmdWallPaper (HWND hWnd)
	{
	static  DWORD   dwFilterIndex = 1L;
	static  char    *szFilter[12];
	static  BITMAPEX    bmEx;

	// Get the file specifications for the combo box.
	GetFileSpecs(szFilter, _App.hInst, IDS_BMPFILESPECS, _szFileSpecs, sizeof(_szFileSpecs));

	KeyString(IDS_CONFIG);
	LoadString(_App.hInst, IDS_BMPDIR, _szPMGroupTitle, sizeof(_szPMGroupTitle));
	GetPrivateProfileString(_szKey, _szPMGroupTitle, _szNull, (LPSTR) _ofn.lpstrInitialDir, 143, _App.szINI);
	bmEx.hBitmap = NULL;

	// Initialize the OPENFILENAME members
	_ofn.hwndOwner = hWnd;
	_ofn.hInstance = NULL;
	_ofn.lpstrFilter = szFilter[0];
	_ofn.nFilterIndex = dwFilterIndex;
	lstrcpy(_ofn.lpstrFile, _szNull);
	lstrcpy(_ofn.lpstrFileTitle, _szNull);
	_ofn.lpstrTitle = NULL;
	_ofn.lCustData = (DWORD) (LPBITMAPEX) &bmEx;
	_ofn.Flags = OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_SHOWHELP;
	_ofn.lpstrDefExt = (LPSTR) NULL;
	(FARPROC) _ofn.lpfnHook = MakeProcInstance((FARPROC) SetWallPaperHookProc, (HINSTANCE) _App.hInst);
	GetOpenFileName(&_ofn);
	FreeProcInstance((FARPROC) _ofn.lpfnHook);
	GetWindowText(hWnd, _szPMGroupTitle, sizeof(_szPMGroupTitle));
	switch ((UINT) ((LPBITMAPEX) _ofn.lCustData)->hBitmap)
		{
		case NULL:              // Cancel button was pressed
			break;

		case IDC_RESET:         // Reset button WAS pressed!
			PMGroup_SetWallPaper(hWnd, NULL);
			// Remove INI entry for this PMGroup.
			WritePrivateProfileString(_szWallPaper, _szPMGroupTitle, NULL, _App.szINI);
			break;

		default:                // Bitmap was assigned
			{
			LPSTR   lp;

			PMGroup_SetWallPaper(hWnd, (HBITMAPEX) _ofn.lCustData);
			// Write new INI entry for this PMGroup.
			wsprintf( _szIconFileSpec, "%s %d", (LPSTR) _ofn.lpstrFile, ((HBITMAPEX) _ofn.lCustData)->fTile);
			WritePrivateProfileString(_szWallPaper, _szPMGroupTitle, _szIconFileSpec, _App.szINI);
			// Save wallpaper directory
			KeyString(IDS_CONFIG);
			LoadString(_App.hInst, IDS_BMPDIR, _szPMGroupTitle, sizeof(_szPMGroupTitle));
			// truncate end of filename to get path.
			if (lp = lstrrchr(_ofn.lpstrFile, 92))
				*lp = 0;
			else
				lstrcpy(_ofn.lpstrFile, _szNull);
			WritePrivateProfileString(_szKey, _szPMGroupTitle, _ofn.lpstrFile, _App.szINI);
			}
			break;
		}
	dwFilterIndex = _ofn.nFilterIndex;
	return;
	}

void    PMGroup_OnCmdAssign (HWND hWnd)
	{
	static  DWORD   dwFilterIndex = 1L;
	static  char    *szFilter[12];

	// Get the file specifications for the combo box.
	GetFileSpecs(szFilter, _App.hInst, IDS_ICONFILESPECS, _szFileSpecs, sizeof(_szFileSpecs));

	KeyString(IDS_CONFIG);
	LoadString(_App.hInst, IDS_ICODIR, _szPMGroupTitle, sizeof(_szPMGroupTitle));
	GetPrivateProfileString(_szKey, _szPMGroupTitle, _szNull, (LPSTR) _ofn.lpstrInitialDir, 143, _App.szINI);

	// Initialize the OPENFILENAME members
	_ofn.hwndOwner = hWnd;
	_ofn.hInstance = NULL;
	_ofn.lpstrFilter = szFilter[0] ;
	_ofn.nFilterIndex = dwFilterIndex;
	lstrcpy(_ofn.lpstrFile, _szNull);
	lstrcpy(_ofn.lpstrFileTitle, _szNull);
	_ofn.lpstrTitle = NULL;
	_ofn.lCustData = NULL;
	_ofn.Flags = OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_SHOWHELP;
	_ofn.lpstrDefExt = (LPSTR) NULL;
	(FARPROC) _ofn.lpfnHook = MakeProcInstance((FARPROC) AssignIconHookProc, (HINSTANCE) _App.hInst);
	if (GetOpenFileName(&_ofn)) // If dialog was terminated with OK, hIcon is passed in _ofn.hwndOwner
		{
		if (_ofn.hwndOwner != hWnd) // if _ofn.hwndOwner was changed, it is an icon
			{
			PMGroup_SetIcon(hWnd, (HICON) _ofn.hwndOwner);
			// Write new INI entry for this PMGroup.
			GetWindowText(hWnd, _szPMGroupTitle, sizeof(_szPMGroupTitle));
			wsprintf(_szIconFileSpec, "%s %d", (LPSTR) _ofn.lpstrFile, _ofn.lCustData);
			WritePrivateProfileString(_szIcons, _szPMGroupTitle, _szIconFileSpec, _App.szINI);

			// Save icon directory
			KeyString(IDS_CONFIG);
			LoadString(_App.hInst, IDS_ICODIR, _szPMGroupTitle, sizeof(_szPMGroupTitle));
			WritePrivateProfileString(_szKey, _szPMGroupTitle, _ofn.lpstrInitialDir, _App.szINI);
			}
		}
	else if (_ofn.lCustData == IDC_RESET)	// Reset button was pressed
		{
		PMGroup_SetIcon(hWnd, _PMGroup.hIcon);
		GetWindowText(hWnd, _szPMGroupTitle, sizeof(_szPMGroupTitle));
		WritePrivateProfileString(_szIcons, _szPMGroupTitle, NULL, _App.szINI);
		}

	dwFilterIndex = _ofn.nFilterIndex;
	FreeProcInstance((FARPROC) _ofn.lpfnHook);
	return;
	}


/********************************************************************
 *  FUNCTION   : HandleSelectionState(LPDRAWITEMSTRUCT)             *
 *                                                                  *
 *  PURPOSE    : Handles selection of list box item                 *
 ********************************************************************/
void    HandleSelectionState (HWND hWnd, const DRAWITEMSTRUCT FAR*  lpDis, HBRUSH * hbr)
	{
	static  HICON   hIcon;
	static	int	dx;

	// Fill background of item with proper brush.
	FillRect(lpDis->hDC, &lpDis->rcItem, hbr[(BOOL) (lpDis->itemState & ODS_SELECTED)]);
	// Get correct icon.
	hIcon = (HICON) LOWORD(lpDis->itemData);
	if (!hIcon)     // No icon, see if it is the Set Icon... menu item
		{
		if (lpDis->itemID == ID_ASSIGNICON)
			hIcon = IconOf(hWnd);
		}

	if (hIcon)
		{
		if (lpDis->CtlType == ODT_LISTBOX)
			DrawIcon(lpDis->hDC, lpDis->rcItem.left + 2, lpDis->rcItem.top + 2, hIcon);
		else
			{
			switch (_App.options.nIconState)
				{
				case ICONSIZE_NORMAL:
					DrawIcon(lpDis->hDC, lpDis->rcItem.left + 2, lpDis->rcItem.top + 2, hIcon);
					dx = _App.sizeIcon.cx + 8;
					break;
				case ICONSIZE_SMALL:
					{
					static	RECT	rc;
					static	HDC		hDCMem;
					static	HBITMAP	hBMP, hDefBMP;

					dx = _App.sizeIcon.cx + 8;
					// Create temp mem DC for strecthing the icon.
					hDCMem = CreateCompatibleDC(lpDis->hDC);
					// Create bitmap the size of a normal icon.
					hBMP = CreateCompatibleBitmap(lpDis->hDC, _App.sizeIcon.cx, _App.sizeIcon.cy);
					// Select this bitmap into the mem DC.
					hDefBMP = SelectBitmap(hDCMem, hBMP);
					// Create rect the same size (a normal icon)
					SetRect(&rc, 0, 0, _App.sizeIcon.cx, _App.sizeIcon.cy);
					// Paint background color into this rect in mem DC.
					FillRect(hDCMem, &rc, hbr[(BOOL) (lpDis->itemState & ODS_SELECTED)]);
					// Draw Normal Icon in mem DC
					DrawIcon(hDCMem, 0, 0, hIcon);
					// Prepare for stretching.
					SetStretchBltMode(lpDis->hDC, GetDeviceCaps(lpDis->hDC, NUMCOLORS) > 2 ? STRETCH_DELETESCANS : STRETCH_ORSCANS);
					// Stretchit!
					StretchBlt(lpDis->hDC,
						lpDis->rcItem.left + (_App.sizeIcon.cx - _App.sizeicon.cx) / 2,
						lpDis->rcItem.top + (_App.sizeIcon.cy - _App.sizeicon.cy) / 2,
						_App.sizeicon.cx, _App.sizeicon.cy,	hDCMem, 0, 0, _App.sizeIcon.cx, _App.sizeIcon.cy, SRCCOPY);
					// Trash the temp bitmap.
					DeleteBitmap(SelectBitmap(hDCMem, hDefBMP));
					// And the mem DC.
					DeleteDC(hDCMem);
					}
					break;
				case ICONSIZE_NONE:
					dx = 8;
					break;
				default:
					break;
				}

			SetBkMode(lpDis->hDC, TRANSPARENT);
			if (HIWORD(lpDis->itemData))        // There is a text string
				{
				static  HFONT   hFont;
				static  RECT    rcText;

				CopyRect(&rcText, &lpDis->rcItem);
				OffsetRect(&rcText, dx, 0);
				hFont = SelectFont(lpDis->hDC, _App.options.hFont);
				SetTextColor(lpDis->hDC,
					GetSysColor((lpDis->itemState & ODS_SELECTED) ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT));
				DrawText(lpDis->hDC, (LPSTR) (PSTR) HIWORD(lpDis->itemData), -1, &rcText, DT_VCENTER | DT_SINGLELINE);
				SelectFont(lpDis->hDC, hFont);
				}
			}
		}
	return;
	}

/********************************************************************
 *  FUNCTION   : HandleFocusState(LPDRAWITEMSTRUCT)                 *
 *                                                                  *
 *  PURPOSE    : Handles focus state of list box item               *
 ********************************************************************/
void    HandleFocusState (const DRAWITEMSTRUCT FAR* lpDis)
	{
	// Ordinarily would check for "if (lpDis->itemState & ODS_FOCUS)"
	// and do drawing for focus state, "else" draw non-focus state.
	// But second call to DrawFocusRect restores rectangle to original
	// state, so will just call function whether focus or non-focus.
	DrawFocusRect(lpDis->hDC, (LPRECT) &lpDis->rcItem);
	return;
	}


/************************************************************************************************
*   Function:   void PMGroup_OnPaint (HWND hWnd)                                                *
*                                                                                               *
*   Handles WM_PAINT message.  If window is not iconic, passes the baton to the original        *
*   window proc.  If window is iconic, icon defined in window properties is used.               *
*                                                                                               *
************************************************************************************************/
void PMGroup_OnPaint (HWND hWnd)
	{
	if (IsIconic(hWnd))     // If window is iconic, handle the repainting.
		DrawPMIcon(hWnd, IconOf(hWnd));
	else                            // Otherwise let old proc handle it.
		CallWindowProc((WNDPROC) _PMGroup.fpOldWndProc, hWnd, WM_PAINT, 0, 0L);
	return;
	}

/************************************************************************************************
*   Function:   BOOL    PMGroup_OnEraseBkgnd (HWND hWnd, HDC hDC)                               *
*                                                                                               *
*   Handles WM_ERASEBKGND message.  In all cases forwards message to original proc for handling.*
*   If window is iconic, message is first changed to WM_ICONERASEBKGND so that DefWindowProc    *
*   will paint the icon background with the default background color.                           *
*                                                                                               *
************************************************************************************************/
BOOL    PMGroup_OnEraseBkgnd (HWND hWnd, HDC hDC)
	{
	if (IsIconic(hWnd))     // Group is an icon
		return((BOOL) CallWindowProc((WNDPROC) _PMGroup.fpOldWndProc, hWnd, WM_ICONERASEBKGND, (UINT) hDC, 0));
	else                    // Group is not an icon
		{
		static  RECT    rcGroup;
		static  HBRUSH  hBrush;
		static  HBITMAP hBitmap;

		GetClientRect(hWnd, &rcGroup);
		UnrealizeObject(hBrush = BrushOf(hWnd));
		FillRect(hDC, &rcGroup, hBrush);
		// Paint wallpaper if it exists.
		if (hBitmap = WallPaperOf(hWnd))
			{
			static  RECT    rcClip, rcInterSect, rcPaint;
			static  HDC     hDCMem;
			static  HBITMAP hBMP;
			static  HPALETTE hPalette, hOldPalB, hOldPal1, hOldPal2;
			static  BITMAP  bm;
			static  int     x0, y0;
			static  int MinPos, MaxPos, nPos;

			hPalette = NULL;
			hOldPalB = NULL;
			hOldPal1 = NULL;
			hOldPal2 = NULL;
			hDCMem = CreateCompatibleDC(hDC);

			if (hPalette = PaletteOf(hWnd)) // If the window has a palette,
				{
				hOldPal1 = SelectPalette(hDCMem, hPalette, TRUE);  // Select window's palette as a FOREGROUND palette
				hOldPal2 = SelectPalette(hDC, hPalette, TRUE);     // in both DCs
				RealizePalette(hDC);                                // Realize it
				}

/*
			if (hPalette = PaletteOf(hWnd)) // If the window has a palette,
				{
				hOldPalB = SelectPalette(hDC, hPalette, TRUE);      // Select window's palette as a BACKGROUND palette
				RealizePalette(hDC);                                // Realize it
				hOldPal1 = SelectPalette(hDCMem, hPalette, FALSE);  // Select window's palette as a FOREGROUND palette
				hOldPal2 = SelectPalette(hDC, hPalette, FALSE);     // in both DCs
				}
*/

			hBMP = SelectBitmap(hDCMem, hBitmap);   // I waited til now to do the select cuz that's how DIBVIEW duz it.

			GetBitmap(hBitmap, &bm);

			GetScrollRange(hWnd, SB_VERT, &MinPos, &MaxPos);                // Get scrollbar info.
			nPos = GetScrollPos(hWnd, SB_VERT);
			x0 = 0;
			y0 = ((nPos - MinPos) % bm.bmHeight);                           // Get offset from scrollbar, if any
			SetRect(&rcPaint, 0,  0, bm.bmWidth, bm.bmHeight);              // Size of BMP
			OffsetRect(&rcPaint, 0, -y0);                                   // Adjust paint area for scrollbar

			if (GetClipBox(hDC, &rcClip) == SIMPLEREGION)                   // Just paint in update area
				{
				CopyRect(&rcGroup, &rcClip);
				OffsetRect(&rcPaint, x0 = bm.bmWidth * (rcClip.left / bm.bmWidth),
					bm.bmHeight * (rcClip.top / bm.bmHeight));
				}

			if (!IsTiled(hWnd))
				{
				GetViewportOrg(hDC);
				GetWindowOrg(hDC);
				BitBlt(hDC, 0, MinPos - nPos, bm.bmWidth, bm.bmHeight,  hDCMem, 0, 0, SRCCOPY);
				}
			else
				{
				while (IntersectRect(&rcInterSect, &rcGroup, &rcPaint))     // while we are within window
					{
					do
						{
						BitBlt(hDC, rcPaint.left, rcPaint.top, bm.bmWidth, bm.bmHeight, hDCMem, 0, 0, SRCCOPY);
						OffsetRect(&rcPaint, bm.bmWidth, 0);                // Move in X direction
						}
					while (IntersectRect(&rcInterSect, &rcGroup, &rcPaint));// while we are within window
					OffsetRect(&rcPaint, x0 - rcPaint.left, bm.bmHeight);   // Move to start of next row.
					}
				}

			SelectBitmap(hDC, hBMP);
			if (hOldPal1)
				SelectPalette(hDCMem, hOldPal1, FALSE);
			if (hOldPal2)
				SelectPalette(hDC, hOldPal2, FALSE);
/*
			if (hOldPal1)
				SelectPalette(hDCMem, hOldPal1, FALSE);
			if (hOldPal2)
				SelectPalette(hDC, hOldPal2, FALSE);
			if (hOldPalB)
				SelectPalette(hDC, hOldPalB, FALSE);
*/

			DeleteDC(hDCMem);
			}
		return(TRUE);
		}
	}

/************************************************************************************************
*   Function:   HICON   PMGroup_SetIcon (HWND hWndGrp, HICON hIcon)                             *
*                                                                                               *
*   Sets/Resets icon of a PMGroup via window properties. Function returns the same icon passed  *
*   it in the hIcon parameter.                                                   If NULL is     *
*   passed in the hIcon parameter, property is cleared.                                         *
************************************************************************************************/
HICON   PMGroup_SetIcon (HWND hWndPMGroup, HICON hIcon)
	{
	HICON   hOldIcon = GetProp(hWndPMGroup, (LPSTR) _szIcon);

	// Destroy old icon if not a system icon.
	if (hOldIcon && (hOldIcon != _PMGroup.hIcon) && (hOldIcon != _PMGroup.hXIcon))
		{
#ifdef DEBUGMSG
char    szDebugMsg[128];

wsprintf(szDebugMsg, "Icon %X destroyed.\n", hIcon);
OutputDebugString(szDebugMsg);
#endif
		DestroyIcon(hOldIcon);
		}

	InvalidateRect(hWndPMGroup, NULL, TRUE);
	if (!hIcon)     // If NULL was passed, remove icon property.
		{
		RemoveProp(hWndPMGroup, (LPSTR) _szIcon); // Remove property from window.
		return(NULL);
		}
	else
		return(SetProp(hWndPMGroup, (LPSTR) _szIcon, hIcon) ? hIcon : (HICON) NULL);
	}

/************************************************************************************************
*   Function:   HBRUSH PMGroup_SetBrush (HWND hWnd, HBRUSH hBrush)                              *
*                                                                                               *
*   Sets/Resets brush of a PMGroup via window properties. Returns the same brush passed         *
*   it in the hBrush parameter.                                                  If NULL is     *
*   passed in the hBrush parameter, property is cleared.                                        *
************************************************************************************************/
HBRUSH  PMGroup_SetBrush (HWND hWnd, HBRUSH hBrush)
	{
	HBRUSH  hOldBrush = BrushOf(hWnd);

	// Destroy existing custom brush if one exists
	if (hOldBrush && (hOldBrush != _PMGroup.hbrBackground))
		DeleteBrush(hOldBrush);

	InvalidateRect(hWnd, NULL, TRUE);
	if (!hBrush)     // If NULL was passed, remove hBrush property.
		{
		RemoveProp(hWnd, _szBrush); // Remove property from window.
		return(NULL);
		}
	else
		return(SetProp(hWnd, _szBrush, hBrush) ? hBrush : (HBRUSH) NULL);
	}

/************************************************************************************************
*   Function:   void    DrawPMIcon(HWND hWndPMGroup, HICON hIcon)                               *
*                                                                                               *
*   Manually draws icon in window.  Used in WM_PAINT routine and when closing app.              *
*                                                                                               *
************************************************************************************************/
void    DrawPMIcon(HWND hWndPMGroup, HICON hIcon)
	{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT    rc;

	GetClientRect(hWndPMGroup, &rc);
	hDC = BeginPaint(hWndPMGroup, &ps);
	DrawIcon(hDC,
		max(0, rc.right - GetSystemMetrics(SM_CXICON)) / 2,
		max(0, rc.bottom - GetSystemMetrics(SM_CYICON)) / 2,
		hIcon);
	EndPaint(hWndPMGroup, &ps);
	return;
	}


/************************************************************************************************
*   Function:   HICON   PMGroup_GetIcon (HWND hWndGrp)                                          *
*                                                                                               *
*   Looks up name of group in INI file and reads in icon, if any.  Function returns the icon if *
*   found and read, NULL if a matching icon could not be found, and the 'X' icon if error.      *
*                                                                                               *
************************************************************************************************/
HICON   PMGroup_GetIcon (HWND hWndPMGroup)
	{
	HICON   hIcon = (HICON) NULL;

	// Get title of this PMGroup.
	GetWindowText(hWndPMGroup, _szPMGroupTitle, sizeof(_szPMGroupTitle));

	// See if there is an icon defined for this PMGroup.
	if (GetPrivateProfileString(_szIcons, _szPMGroupTitle, _szNull, _szIconFileSpec, MAXFILENAMELENGTH+10, _App.szINI))
		{
		LPSTR   lp;
		int     nIconNum = 0;

		// strip icon number, if any, off end of _szIconFileSpec
		if (lp = lstrchr(_szIconFileSpec, ' '))
			{
			*lp++ = 0;
			nIconNum = latoi(lp);
			}
		hIcon = IconObject_GetOne(_szIconFileSpec, nIconNum, _App.hInst);
		if (!hIcon)
			hIcon = _PMGroup.hXIcon;
		}
	return(hIcon);
	}

/************************************************************************************************
*   Function:   HBRUSH PMGroup_GetBrush (HWND hWndGrp)                                          *
*                                                                                               *
*   Looks up name of group in INI file and reads brush color, if any.  Returns the color if     *
*   found and read, NULL otherwise.                                                             *
*                                                                                               *
************************************************************************************************/
HBRUSH  PMGroup_GetBrush (HWND hWndPMGroup)
	{
	HBRUSH  hBrush = (HBRUSH) NULL;

	// Get title of this PMGroup.
	GetWindowText(hWndPMGroup, _szPMGroupTitle, sizeof(_szPMGroupTitle));

	// See if there is a color defined for this PMGroup.
	if (GetPrivateProfileString(_szBrush, _szPMGroupTitle, _szNull, _szIconFileSpec, MAXFILENAMELENGTH+10, _App.szINI))
		{
		static  DWORD   dwColor;

		dwColor = latolx(_szIconFileSpec);
		hBrush = CreateSolidBrush(dwColor);
		}
	return(hBrush);
	}

/************************************************************************************************
*   Function:   HBITMAP PMGroup_GetWallPaper (HWND hWndGrp)                                     *
*                                                                                               *
*   Looks up name of group in INI file and reads wallpaper, if any.  Returns the bitmap         *
*   handle if found and read, NULL otherwise.                                                   *
*                                                                                               *
************************************************************************************************/
HBITMAPEX PMGroup_GetWallPaper (HWND hWndPMGroup)
	{
	HBITMAPEX   bmEX = (HBITMAPEX) LocalAlloc(LPTR, sizeof(BITMAPEX));

	// Get title of this PMGroup.
	GetWindowText(hWndPMGroup, _szPMGroupTitle, sizeof(_szPMGroupTitle));

	// See if there is wallpaper defined for this PMGroup.
	if (GetPrivateProfileString(_szWallPaper, _szPMGroupTitle, _szNull, _szIconFileSpec, MAXFILENAMELENGTH+10, _App.szINI))
		{
		static  HWND    hWnd;
		LPSTR   lp;

		bmEX->fTile = FALSE;
		// strip tile flag, if any, off end of _szIconFileSpec
		if (lp = lstrchr(_szIconFileSpec, ' '))
			{
			*lp++ = 0;
			bmEX->fTile = (BOOL) latoi(lp);
			}
		// Create ICSBitmap window to read in bitmap image from file.
		hWnd = CreateWindow("ICSBitmap", _szIconFileSpec, WS_CHILD, 0, 0, 0, 0, hWndPMGroup, (HMENU) 0xFFFF, _App.hInst, NULL);
		bmEX->hBitmap = (HBITMAP) SendMessage(hWnd, WM_GETBITMAP, 0, 0L);
		bmEX->hPalette = (HPALETTE) SendMessage(hWnd, WM_GETPALETTE, 0, 0L);

		DestroyWindow(hWnd);
		}
	if (!bmEX->hBitmap)
		{
		LocalFree(bmEX);
		bmEX = NULL;
		}
	return(bmEX);
	}

/************************************************************************************************
*   Function:   HBITMAP PMGroup_SetWallPaper (HWND hWnd, HBITMAP hBitmap)                       *
*                                                                                               *
*   Sets/Resets wallpaper of a PMGroup via window properties. Returns the same hBitmap passed   *
*   it in the hBitmap parameter.                                                                *
*   If NULL is passed in the hBitmap parameter, property is cleared.                            *
************************************************************************************************/
HBITMAPEX PMGroup_SetWallPaper (HWND hWnd, HBITMAPEX hBitmapEx)
	{
	HBITMAP     hBitmap = WallPaperOf(hWnd);
	HPALETTE    hPalette = PaletteOf(hWnd);

	InvalidateRect(hWnd, NULL, TRUE);
	// Destroy old GDI objects if they exist.
	if (hBitmap)
		DeleteBitmap(hBitmap);
	if (hPalette)
		DeletePalette(hPalette);
	if (!hBitmapEx)     // If NULL was passed, remove hBitmap property.
		{
		RemoveProp(hWnd, _szWallPaper);     // Remove property from window.
		RemoveProp(hWnd, _szPalette);       // Remove property from window.
		return(NULL);
		}
	else
		{
		SetProp(hWnd, _szWallPaper, hBitmapEx->hBitmap);
		SetProp(hWnd, _szPalette, hBitmapEx->hPalette);
		SetProp(hWnd, _szTile, (HANDLE) hBitmapEx->fTile);
		}
	return(hBitmapEx);
	}

void    GetFileSpecs (char *szFilter[], HINSTANCE hInst, UINT nID, PSTR pszFileSpecs, UINT nLen)
	{
	static  UINT    nStr;
	static  PSTR    pStr;
	static  PSTR    *pszFilter;

	pszFilter = szFilter;
	nStr = LoadString(hInst, nID, pszFileSpecs, nLen);
	for (pStr = pszFileSpecs; pStr < pszFileSpecs + nStr; pStr++)
		if (*pStr == '#')
			{
			*pStr = 0;
			*pszFilter++ = pStr + 1;
			}
	return;
	}

BOOL    PMGroup_OnICSQueryDropIcon (HWND hWnd)
	{
	return(TRUE);
	}

BOOL    PMGroup_OnICSDropIcon (HWND hWnd, UINT nIconNum, LPSTR lpszIconFileName)
	{
	HICON   hIcon;

	// Build string to write to INI file.
	wsprintf(_szIconFileSpec, "%s %d", (LPSTR) lpszIconFileName, nIconNum);
	// Write string to INI file.
	GetWindowText(hWnd, _szPMGroupTitle, sizeof(_szPMGroupTitle));       // Get present title
	WritePrivateProfileString(_szIcons, _szPMGroupTitle, _szIconFileSpec, _App.szINI);
	// Load icon from file.
	hIcon = PMGroup_GetIcon(hWnd);
	// Set icon to group.
	PMGroup_SetIcon(hWnd, hIcon);
	// Bells & Whistles - make the window flash if it is iconic!

	if (IsIconic(hWnd))
		{
		int     n;
		RECT    rcClient;
		HDC     hDC;

		UpdateWindow(hWnd);
		hDC = GetDC(hWnd);
		GetClientRect(hWnd, &rcClient);
		for (n = 0; n < 5; n++)
			{
			InvertRect(hDC, &rcClient);
			Wait(50);
			InvertRect(hDC, &rcClient);
			Wait(50);
			}
		ReleaseDC(hWnd, hDC);
		}
	return(TRUE);
	}

DWORD   PMGroup_OnMenuChar (HWND hWnd, UINT ch, UINT flags, HMENU hMenu)
	{
	DWORD   dwRetVal = 0L;

	if (flags & MF_POPUP)
		dwRetVal = ProcessMenuChar(ch, flags, hMenu);
	return(dwRetVal ? dwRetVal :
		CallWindowProc(_PMGroup.fpOldWndProc, hWnd, WM_MENUCHAR, (WPARAM) (UINT) (ch), MAKELPARAM((flags), (UINT) (hMenu))));
//	return(dwRetVal);
	}

void    Wait (DWORD dwMilliSeconds)
	{
	DWORD   dwNow = GetCurrentTime();

	while ((GetCurrentTime() - dwNow) < dwMilliSeconds)
		{
		if (GetCurrentTime() < dwNow)   // Make sure the timer didn't overflow on us!
			break;
		}
	return;
	}

DWORD   ProcessMenuChar(UINT ch, UINT flags, HMENU hMenu)
	{
	DWORD   dwRetVal = 0L;

	switch (LOWORD(AnsiUpper((LPSTR) (LONG) ch)))
		{
		case 'P':
			dwRetVal = MAKELONG(10, 2);		// 'Properties' was selected
			break;
		case 'I':
			dwRetVal = MAKELONG(0, 2);		// 'Icons' was selected
			break;
		case 'O':
			dwRetVal = MAKELONG(1, 2);		// 'Colors' was selected
			break;
		case 'W':
			dwRetVal = MAKELONG(2, 2);		// 'Wallpaper' was selected
			break;
/*		case 'D':
			dwRetVal = MAKELONG(3, 2);		// 'Password' was selected
			break;
*/
		case 'B':
			dwRetVal = MAKELONG(4, 2);		// 'About' was selected
			break;
		default:
			break;
			}
	return(dwRetVal);
	}

void	RebuildPMGroupMenus (HWND hWnd)
	{
	if (hWnd && _App.fInstalled)
		{
		WNDENUMPROC wp = (WNDENUMPROC) MakeProcInstance((FARPROC) RebuildPMGroupMenu, _App.hInst);

		// Remove Our Menu Item from All PMGroups.
		EnumChildWindows(hWnd, wp, FALSE);

		// Destroy old popup.
		DestroyMenu(_App.hGrpPopup);
		// Recreate Our Popup.
		_App.hGrpPopup = CreateGrpPopupMenu();

		// Add it to all PMGroups.
		EnumChildWindows(hWnd, wp, TRUE);
		FreeProcInstance((FARPROC) wp);
		}
	}

BOOL    CALLBACK    _export RebuildPMGroupMenu (HWND hWndPMGroup, LPARAM lParam)
	{
	char szClass[GENERICSTRLENGTH];

	if (GetClassName(hWndPMGroup, szClass, GENERICSTRLENGTH))
		{
		if (!lstrcmpi(szClass, _szPMGroup)) // only care about Grp Windows
			{
			// Update System menu
			UpdatePMGroupMenu(hWndPMGroup, (BOOL) lParam);
			}
		}
	return(TRUE);       // Continue enumerating
	}


HMENU	CreateGrpPopupMenu (void)
	{
	int		nID;
	static	char	szMenuStrings[MAX_MENUITEMTEXTLENGTH * 4];
	PSTR	pStr = szMenuStrings;
	HMENU	hMenu = CreatePopupMenu();

	szMenuStrings[0] = 0;
	for (nID = ID_ASSIGNICON; nID <= ID_ABOUT; nID++)
		{
		if (LoadString(_App.hInst, nID, pStr += (1 + lstrlen(pStr)), MAX_MENUITEMTEXTLENGTH - 1))
			{
			AppendMenu(hMenu, MF_OWNERDRAW, nID,
				MAKELP((UINT) pStr, (UINT) LoadIcon(_App.hInst, MAKEINTRESOURCE(nID))));
			if (nID == ID_SEPARATOR)
				AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
			}
        }
	return(hMenu);
	}

