#include "GrpIcon.h"

// GrpIcon requires ICS.DLL Version 1.0 or greater
#define DWMINDLLVERSIONHI   0x0001
#define DWMINDLLVERSIONLO   0x0001

// Local functions
BOOL    CALLBACK  __export Options(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL    Options_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
void    Options_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
void    Options_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR* lpDrawItem);
BOOL    Options_DrawSelection(const DRAWITEMSTRUCT FAR* lpDrawItem, HBRUSH * hbr);
BOOL    Options_DrawFocus(const DRAWITEMSTRUCT FAR* lpDrawItem);
void    Options_OnMeasureItem(HWND hDlg, const MEASUREITEMSTRUCT FAR* lpMis);
BOOL	InstallGrpIcon(HWND hWndProgman);


APP     _App;           // APP Object for app.
UINT    ICSM_PIRATED;   // Custom message used to inform other ICS apps of piracy. //Darrin
char    _szAppName[SHORT_STRING_LENGTH];    // Application/Class name.
char    _szProgman[SHORT_STRING_LENGTH];    // Program Manager class name
char    _szString[MAX_MENUITEMTEXTLENGTH];
LPICSREGISTERAPPSTRUCT  _lpICSras;
OPTIONS _options;
DWORD   _dwLastNag;
FARPROC _pFunk[] = {KillApp, Nothing, Nothing};

int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lpszCmdLine, int nCmdShow)
	{
//	BYTE    bSecurity;
	int     nExitCode = -1;

	// Initialize the generic items of the APP structure
	_App.hInst       = hInst;
	_App.hInstPrev   = hInstPrev;
	_App.lpszCmdLine = lpszCmdLine;
	_App.nCmdShow    = nCmdShow;

	// Initialize, run, and terminate the application
	if (App_Initialize(&_App))
		{
		HWND    hWnd = GetActiveWindow();

		ShowWindow(_App.hWndFrame, SW_HIDE);
		SetActiveWindow(hWnd);
/*      if (_lpICSras->lpRegistered[_lpICSras->bRegOffSet] == 'K' ||  //dARRIN eRROR CHECK HERE?
			InnerCityAboutBox(_App.hWndFrame, _lpICSras) != -1)
			nExitCode = App_Main(&_App);

		bSecurity = _lpICSras->lpRegistered[255 - _lpICSras->bRegOffSet];

		if ((_lpICSras->lpRegistered[_lpICSras->bRegOffSet] == 'K') ||
			((InnerCityAboutBox(_App.hWndFrame, _lpICSras) != -1) &&
			(bSecurity == (BYTE) ~_lpICSras->lpRegistered[255 - _lpICSras->bRegOffSet])))
*/
			nExitCode = App_Main(&_App);
		}
	App_Terminate(&_App, FALSE);
	return(nExitCode);
	}

BOOL App_Initialize (APP* pApp)//save value here //Darrin
	{
	char        szAppCaption[GENERICSTRLENGTH];
	char        szConfig[SHORT_STRING_LENGTH];
	char        szKey[SHORT_STRING_LENGTH];
	BYTE        bSecurity;
	LPICSREGISTERAPPSTRUCT  lpICSras = NULL;
#ifdef  FIRSTREV
	char        szVersion[] = "";
#else
	char        szVersion[] = "2.01";
#endif
	LOGBRUSH    lb = {BS_PATTERN, 0, 0};

	LoadString(pApp->hInst, IDS_APPNAME, _szAppName, SHORT_STRING_LENGTH - 1);

	// Check to see if we even want to initialize
	if (pApp->hInstPrev)    // If second instance, activate first and punt.
		{
		HWND hWnd = FindWindow(_szAppName, NULL);

		// if second instance, tell user about the first instance.
		if (SendMessage(hWnd, WM_ISINSTALLED, 0, 0L))
			{
			ShowWindow(hWnd, SW_SHOWMINIMIZED);
			BringWindowToTop(hWnd);
			}
		return(FALSE);
		}

	// Try to register window class. If not successful, punt.
	if (!Frame_Initialize(pApp))
		return(FALSE);

	// Initialize rest of struct
	pApp->hInstLib = LoadLibrary("ICS.DLL");
	LoadString(pApp->hInst, IDS_PROGMAN, _szProgman, SHORT_STRING_LENGTH - 1);
	wsprintf(pApp->szINI, "%s.INI", (LPSTR) _szAppName);
	LoadString(pApp->hInst, IDS_CONFIG, szConfig, SHORT_STRING_LENGTH - 1);
	LoadString(pApp->hInst, IDS_PREVIEW, szKey, SHORT_STRING_LENGTH - 1);
	pApp->options.fPreview = GetPrivateProfileInt(szConfig, szKey, PREVIEW_DEFAULT, pApp->szINI);
	LoadString(pApp->hInst, IDS_ICONSIZE, szKey, SHORT_STRING_LENGTH - 1);
	pApp->options.nIconState = GetPrivateProfileInt(szConfig, szKey, ICONSIZE_DEFAULT, pApp->szINI);
	LoadString(pApp->hInst, IDS_MENUFONT, szKey, SHORT_STRING_LENGTH - 1);
	GetPrivateProfileString(szConfig, szKey, MENUFONT_DEFAULT, szAppCaption, GENERICSTRLENGTH, pApp->szINI);
	if (IsWin31())
		lstrcpy(pApp->options.lf.lfFaceName, "Arial");
	ParseFontString(&pApp->options.lf, szAppCaption, 14);

	pApp->hWndFrame  = NULL;
	pApp->sizeIcon.cx    = GetSystemMetrics(SM_CXICON);
	pApp->sizeIcon.cy    = GetSystemMetrics(SM_CYICON);
	pApp->sizeicon.cx = 3 * pApp->sizeIcon.cx / 4;
	pApp->sizeicon.cy = 3 * pApp->sizeIcon.cy / 4;

	pApp->hbr[0]     = CreateSolidBrush(GetSysColor(COLOR_MENU));
	pApp->hbr[1]     = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
	pApp->options.hFont = CreateFontIndirect(&pApp->options.lf);

	// Build the additions to the system menu
	pApp->hGrpPopup  = CreateGrpPopupMenu();

	// Build app caption
	wsprintf(szAppCaption, "%s %s",
		(LPSTR) _szAppName, (LPSTR) szVersion);

	lb.lbHatch = (short int) LoadBitmap(pApp->hInst, MAKEINTRESOURCE(BMP_BACKGROUND));
	pApp->hbrDlgBkGnd = CreateBrushIndirect(&lb);
	DeleteBitmap(lb.lbHatch);
	pApp->hWndFrame = Frame_CreateWindow(szAppCaption,
		CW_USEDEFAULT, CW_USEDEFAULT, 32, 32, pApp->hInst);

	if (pApp->hWndFrame == NULL)
		return(FALSE);

	// Register app with ICS.DLL
	lpICSras = (LPICSREGISTERAPPSTRUCT) GlobalAllocPtr(GHND, sizeof(ICSREGISTERAPPSTRUCT));
	lpICSras->nStructSize = sizeof(ICSREGISTERAPPSTRUCT);
	lpICSras->hWnd = pApp->hWndFrame;
	lpICSras->hTask = GetCurrentTask();
	lpICSras->hInst = pApp->hInst;
	lpICSras->hIcon = GetClassIcon(pApp->hWndFrame);
	lpICSras->fCustCtl = TRUE;
	lpICSras->fDDE = FALSE;
	lpICSras->fOLE = FALSE;
	lpICSras->fDragon = FALSE;
	lpICSras->OptionsDlgProc = (DLGPROC) MakeProcInstance((FARPROC) Options, pApp->hInst);
	lpICSras->lpszOptionsDlgTemplate = (LPSTR) MAKEINTRESOURCE(IDD_OPTIONS);
	LoadString(pApp->hInst, IDS_DLGAPPNAME, lpICSras->szAppName, sizeof(lpICSras->szAppName) - 1);
	LoadString(pApp->hInst, IDS_DLGAPPDESC, lpICSras->szAppDesc, sizeof(lpICSras->szAppDesc) - 1);
	lstrcpy(lpICSras->szINI, pApp->szINI);
	LoadString(pApp->hInst, IDS_HELPFILE, lpICSras->szHelpFile, sizeof(lpICSras->szHelpFile) - 1);
	lpICSras->dwHelpRegFormID = 40L;				// Print Reg Form ID
	lstrcpy(lpICSras->szMagic, "TheMackDaddy");
	lpICSras->lpRegistered = (LPBYTE) GlobalAllocPtr(GMEM_MOVEABLE, Reg_1_buffer_length);   // Leave memory filled with garbage
	lpICSras->bRegOffSet =  lpICSras->lpRegistered[LPBYTE_REG_INDEX];  // The offset will be whatever the byte is at position 29 of the buffer.
	lpICSras->lpRegistered[lpICSras->bRegOffSet] = 'G';// Doit here!!!!!!!//Darrin
	bSecurity = lpICSras->lpRegistered[255 - lpICSras->bRegOffSet];
	_lpICSras = ICSRegisterApp(lpICSras);
	GlobalFreePtr(lpICSras);
	if (!_lpICSras || bSecurity != (BYTE) ~_lpICSras->lpRegistered[255 - _lpICSras->bRegOffSet])
		return(FALSE);

	LoadString(pApp->hInst, IDS_VERSIONTAG, szKey, MAXCONFIGLENGTH);
	LoadString(pApp->hInst, IDS_VERSION, szAppCaption, MAXCONFIGLENGTH);
	// write version #
	WritePrivateProfileString(szConfig, szKey, szAppCaption, _lpICSras->szINI);   // write new version #

	_dwLastNag = GetCurrentTime();             //Darrin
	// Show the About Box here if not registered.
//    return(_lpICSras->lpRegistered[_lpICSras->bRegOffSet] == 'K' ||
//      1 + InnerCityAboutBox(pApp->hWndFrame, _lpICSras));
	return(TRUE);
	}

int App_Main (APP* pApp)
	{
    HWND    hWndProgman = NULL;
    MSG     msg = {NULL, WM_NULL, 0, 0L, 0L, {0, 0}};

    // Message loop #1: Executed before app is installed.
    while (TRUE)
        {
        while (hWndProgman == NULL)
            {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))   // If a message exists in the queue, translate and dispatch it.
                {
                if (msg.message == WM_QUIT) // If it's time to quit, return exit code
                    return((int) msg.wParam);
                else
                    {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    }
                }
            else
            	{
            	// Norton Desktop fix
            	if (GetModuleHandle(_szProgman))
                	hWndProgman = FindWindow(_szProgman, NULL);
                }
            }

        // To get here, Progman was detected.  If the user quit before Progman is detected,
        // the return statement was executed and we aint gonna get here...

        // Install app.
		if (_App.fInstalled = InstallGrpIcon(hWndProgman))  // Install GrpIcon
			{
	        // Now enter a more traditional message loop.
    	    while (GetMessage(&msg, NULL, 0, 0))
        	    {
            	TranslateMessage(&msg);
	            DispatchMessage(&msg);
    	        }

        	// WM_QUIT received - UnInstall app.
        	if ((hWndProgman = FindWindow(_szProgman, NULL)) != NULL)   // Progman is still there - close GI.
            	{
				PMGroup_SubClassClass(hWndProgman, FALSE); // Uninstall GrpIcon
            	return((int) msg.wParam);   // return the exit code to WinMain.
            	}
        	// Else Progman was closed before we were - they must be running another Shell
        	// Keep GrpIcon active and waiting for Progman to come around again...
        	_App.fInstalled = FALSE;
        	}
        else
        	{
        	MessageBeep(MB_ICONEXCLAMATION);
        	MessageBox(GetFocus(), "GrpIcon could not be initialized.\nPlease call Inner-City Software Technical Support.",
        		"GrpIcon Fatal Error!", MB_OK | MB_ICONEXCLAMATION);
           	return((int) msg.wParam);   // return the exit code to WinMain.
        	}
        }
	}

BOOL	InstallGrpIcon (HWND hWndProgman)
	{
	BYTE    bSecurity = _lpICSras->lpRegistered[255 - _lpICSras->bRegOffSet];

	if ((_lpICSras->lpRegistered[_lpICSras->bRegOffSet] == 'K') ||
		((InnerCityAboutBox(hWndProgman, _lpICSras) != -1) &&
		(bSecurity == (BYTE) ~_lpICSras->lpRegistered[255 - _lpICSras->bRegOffSet])))
		return(PMGroup_SubClassClass(hWndProgman, TRUE));
	else
		return(FALSE);
	}

void App_Terminate (APP* pApp, BOOL fEndSession)
	{
	// Flush any caches and buffers to disk, if necessary.
	// If fEndSession is TRUE, it is not necessary to
	// destroy windows, GDI objects, or free memory.
	if (!fEndSession)
		{
		if (pApp->hWndFrame)
			{
			pApp->fInstalled = FALSE;
			DestroyWindow(pApp->hWndFrame);
			if (pApp->hbrDlgBkGnd)
				DeleteBrush(pApp->hbrDlgBkGnd);
			if (pApp->hbr[0])
				DeleteBrush(pApp->hbr[0]);
			if (pApp->hbr[1])
				DeleteBrush(pApp->hbr[1]);
			if (pApp->hGrpPopup)
				DestroyMenu(pApp->hGrpPopup);
			if (pApp->options.hFont)
				DeleteFont(pApp->options.hFont);
			pApp->hWndFrame = NULL;
			Frame_Terminate(pApp);
			}
		if (_lpICSras)
			{
			GlobalFreePtr(_lpICSras->lpRegistered); // DO NOT do this after the UnReg call, as the pointer could be invalidated!!!
			ICSUnRegisterApp(_lpICSras);
			}
		if (pApp->hInstLib)
			{
			FreeLibrary(pApp->hInstLib);
			pApp->hInstLib = NULL;
			}
		}
	}

int     ErrorMsg (HINSTANCE hInst, int nID1, int nID2, WORD wFlags)
	{
	int     nResult;
	char    szTitle[GENERICSTRLENGTH];
	char    szMsg[GENERICSTRLENGTH];

	LoadString(hInst, nID1, szMsg, GENERICSTRLENGTH);
	LoadString(hInst, nID2, szTitle, GENERICSTRLENGTH);
	nResult = MessageBox(GetFocus(), szMsg, szTitle, wFlags);
	return(nResult);
	}


// The child dialog contains all the buttons.  This is its message handler.
BOOL    CALLBACK  __export Options (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	switch (msg)
		{
		HANDLE_DLG_MSG(hDlg, WM_INITDIALOG, Options_OnInitDialog);
		HANDLE_DLG_MSG(hDlg, WM_COMMAND, Options_OnCommand);
		HANDLE_DLG_MSG(hDlg, WM_MEASUREITEM, Options_OnMeasureItem);
		HANDLE_DLG_MSG(hDlg, WM_DRAWITEM, Options_OnDrawItem);
		HANDLE_DLG_MSG(hDlg, WM_CTLCOLOR, ICSDlg_OnCtlColor);
		}
	return(FALSE);
	}

// The WM_INITDIALOG handler for the child dialog.  It also aligns its controls,
// then re-enables its parent, as the call to DialogBoxParam disables the window from input.
BOOL    Options_OnInitDialog (HWND hDlg, HWND hWndFocus, LPARAM lParam)
	{
	RECT    rcClient, rcDlg, rcChild;
	HWND    hWndParent;
//    BITMAP  bm;
//    BYTE    bTime;
	int     nItem, n = 3;
	POINT   pt = {0, 0};
	HWND    hWndCombo = GetDlgItem(hDlg, IDC_ICONSIZE);
	LPICSREGISTERAPPSTRUCT  lp = (LPICSREGISTERAPPSTRUCT) lParam;

	// Set up Options "stack frame".
	_App.options.fStartup = InstallInWinIni(_szAppName, INI_ISINSTALLED);
	_options = _App.options;

	SetWindowText(GetParent(hDlg), "GrpIcon Options");

	// Put items in owner-draw combobox.
	nItem = ComboBox_AddString(hWndCombo, "Normal Icons");
	ComboBox_SetItemData(hWndCombo, nItem, (DWORD) (UINT) LoadIcon(_App.hInstLib, MAKEINTRESOURCE(IDI_ICSLOGO)));
	nItem = ComboBox_AddString(hWndCombo, "Small Icons");
	ComboBox_SetItemData(hWndCombo, nItem, (DWORD) (UINT) LoadIcon(_App.hInstLib, MAKEINTRESOURCE(IDI_SMALLICON)));
	nItem = ComboBox_AddString(hWndCombo, "No Icons");
	ComboBox_SetItemData(hWndCombo, nItem, NULL);
	ComboBox_SetCurSel(hWndCombo, _options.nIconState);

	// Init Edit text.
	SetDlgItemText(hDlg, IDC_CURRENTFONT, "Sample Text");
	SendDlgItemMessage(hDlg, IDC_CURRENTFONT, WM_SETFONT, (WPARAM) _options.hFont, 0L);

	// Init checkboxes.
	CheckDlgButton(hDlg, IDC_AUTOSTART, _options.fStartup);
	CheckDlgButton(hDlg, IDC_PREVIEW, _options.fPreview);

	// Move child dialog to right side of parent window
	hWndParent = GetParent(hDlg);
	GetWindowRect(GetDlgItem(hWndParent, IDC_LOGOBKGND), &rcClient);  // Get coords of canvas
	pt.x = 0; pt.y = 0;
	ClientToScreen(hWndParent, &pt);                                  // Get screen coords of dlg client corner
	OffsetRect(&rcClient, -pt.x, -pt.y);                            // Adjust rect to window coords

	GetClientRect(hWndParent, &rcDlg);
	GetClientRect(hDlg, &rcChild);
	SetWindowPos(hDlg, NULL, rcDlg.right - rcClient.left - rcChild.right, rcClient.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	EnableWindow(hWndParent, TRUE);     // Re-enable parent window.
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	UpdateWindow(hWndParent);
	return(TRUE);
	}

// WM_COMMAND handler for child dialog.
void    Options_OnCommand (HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
	{
	switch (id)
		{
		case IDOK:
			{
			char    szConfig[SHORT_STRING_LENGTH];
			char    szKey[SHORT_STRING_LENGTH];
			char    szNum[GENERICSTRLENGTH];
			// Write any changed options.

			LoadString(_App.hInst, IDS_CONFIG, szConfig, SHORT_STRING_LENGTH - 1);

			if (_App.options.fPreview != _options.fPreview)
				{
				LoadString(_App.hInst, IDS_PREVIEW, szKey, SHORT_STRING_LENGTH - 1);
				wsprintf(szNum, "%d", _options.fPreview);
				WritePrivateProfileString(szConfig, szKey, szNum, _App.szINI);
				}

			if (_App.options.nIconState != _options.nIconState)
				{
				LoadString(_App.hInst, IDS_ICONSIZE, szKey, SHORT_STRING_LENGTH - 1);
				wsprintf(szNum, "%d", _options.nIconState);
				WritePrivateProfileString(szConfig, szKey, szNum, _App.szINI);
				}

			if (_App.options.fStartup != _options.fStartup)
				{
				LPSTR   lpszFileName = GlobalAllocPtr(GHND, MAXFILENAMELENGTH);

				GetModuleFileName(GetModuleHandle(_szAppName), lpszFileName, MAXFILENAMELENGTH);
				InstallInWinIni(lpszFileName, _options.fStartup ? INI_INSTALL : INI_UNINSTALL);
				GlobalFreePtr(lpszFileName);
				}

			// It's too much trouble to determine equivalence of logfonts - just write it all the time.
			LoadString(_App.hInst, IDS_MENUFONT, szKey, SHORT_STRING_LENGTH - 1);
			wsprintf(szNum, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s",
				_options.lf.lfHeight, _options.lf.lfWidth, _options.lf.lfEscapement,
				_options.lf.lfOrientation, _options.lf.lfWeight, _options.lf.lfItalic,
				_options.lf.lfUnderline, _options.lf.lfStrikeOut, _options.lf.lfCharSet,
				_options.lf.lfOutPrecision, _options.lf.lfClipPrecision, _options.lf.lfQuality,
				_options.lf.lfPitchAndFamily, (LPSTR) _options.lf.lfFaceName);
			WritePrivateProfileString(szConfig, szKey, szNum, _App.szINI);
			// Destroy and rebuild PMGroup Menus.
			RebuildPMGroupMenus(FindWindow(_szProgman, NULL));

			// Copy temp options to real ones.
			_App.options = _options;
			// Fall through to IDCANCEL.
			}

		case IDCANCEL:
			EndDialog(hDlg, id);            // Close this dialog
			break;

		case IDB_HELP:
			WinHelp(_App.hWndFrame, _lpICSras->szHelpFile, HELP_CONTEXT, 15);
			break;

		case IDC_PREVIEW:
			_options.fPreview = (BOOL) IsDlgButtonChecked(hDlg, IDC_PREVIEW);
			break;

		case IDC_AUTOSTART:
			_options.fStartup = (BOOL) IsDlgButtonChecked(hDlg, IDC_AUTOSTART);
			break;

		case IDC_ICONSIZE:
			if (codeNotify == CBN_SELCHANGE)    // The combo box was changed!
				_options.nIconState = ComboBox_GetCurSel(hwndCtl);
			break;

		case IDB_FONTS:
			{
			CHOOSEFONT chf;

			chf.hDC = NULL;
			chf.lStructSize = sizeof(CHOOSEFONT);
			chf.hwndOwner = hDlg;
			chf.lpLogFont = &_options.lf;
			chf.Flags = CF_ANSIONLY | CF_LIMITSIZE | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
			chf.rgbColors = RGB(0, 0, 0);
			chf.lCustData = 0L;
			chf.lpfnHook = NULL;
			chf.lpTemplateName = (LPSTR) NULL;
			chf.hInstance = (HANDLE) NULL;
			chf.lpszStyle = (LPSTR) NULL;
			chf.nFontType = SCREEN_FONTTYPE;
			chf.nSizeMin = 6;
			chf.nSizeMax = 24;
			if (ChooseFont(&chf))
				{
				HWND    hWndText = GetDlgItem(hDlg, IDC_CURRENTFONT);

				if (_options.hFont != NULL)
					DeleteFont(_options.hFont);
				_options.hFont = CreateFontIndirect(&_options.lf);
				SetWindowFont(hWndText, (WPARAM) _options.hFont, TRUE);
				UpdateWindow(hWndText);
				}
			SetFocus(hDlg);
			}
			break;

		default:
			break;
		}
	return;
	}

void    Options_OnDrawItem (HWND hDlg, const DRAWITEMSTRUCT FAR* lpDrawItem)
	{
	if (lpDrawItem->itemID == -1)
		{
		Options_DrawFocus(lpDrawItem);    // function below
		return;
		}   //  lpDrawItem->itemID == -1

	if (lpDrawItem->itemAction & ODA_DRAWENTIRE)
		{
		Options_DrawSelection(lpDrawItem, _App.hbr);
		if (lpDrawItem->itemState & ODS_FOCUS)
			Options_DrawFocus(lpDrawItem);
		return;
		} //    lpDrawItem->itemAction & ODA_DRAWENTIRE

	if (lpDrawItem->itemAction & ODA_SELECT)
		{
		Options_DrawSelection(lpDrawItem, _App.hbr);
		return;
		}

	if (lpDrawItem->itemAction & ODA_FOCUS)
		Options_DrawFocus(lpDrawItem);
	return;
	}

BOOL    Options_DrawSelection (const DRAWITEMSTRUCT FAR* lpDrawItem, HBRUSH * hbr)
	{
	if (lpDrawItem->CtlType == ODT_BUTTON)  // If an owner-draw button
		ICSDlg_DrawSelection(lpDrawItem, hbr);
	else
		{
		HICON   hIcon;
		HFONT   hFont;
		RECT    rcText;
		char    szText[20];

		FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hbr[(BOOL) (lpDrawItem->itemState & ODS_SELECTED)]);
		hIcon = (HICON) lpDrawItem->itemData;
		if (hIcon)
			DrawIcon(lpDrawItem->hDC, lpDrawItem->rcItem.left + 2, lpDrawItem->rcItem.top + 2, hIcon);
		SetBkMode(lpDrawItem->hDC, TRANSPARENT);
		CopyRect(&rcText, &lpDrawItem->rcItem);
		rcText.left += (_App.sizeIcon.cx + 8);
		hFont = SelectFont(lpDrawItem->hDC, _App.options.hFont);
		SetTextColor(lpDrawItem->hDC,
			GetSysColor((lpDrawItem->itemState & ODS_SELECTED) ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT));
		ComboBox_GetLBText(lpDrawItem->hwndItem, lpDrawItem->itemID, szText);
		DrawText(lpDrawItem->hDC, szText, -1, &rcText, DT_VCENTER | DT_SINGLELINE);
		SelectFont(lpDrawItem->hDC, hFont);
		}
	return(TRUE);
	}

BOOL    Options_DrawFocus (const DRAWITEMSTRUCT FAR* lpDrawItem)
	{
	DrawFocusRect(lpDrawItem->hDC, (LPRECT) &lpDrawItem->rcItem);
	return(TRUE);
	}

void    Options_OnMeasureItem (HWND hDlg, const MEASUREITEMSTRUCT FAR* lpMeasureItem)
	{
	if (lpMeasureItem->CtlType == ODT_BUTTON)
		ICSDlg_OnMeasureItem(hDlg, lpMeasureItem);
	else if (lpMeasureItem->CtlType == ODT_COMBOBOX)
		{
		HDC     hDC;
		HFONT   hFont;
		RECT    rcText;
		char    szText[20];
		MEASUREITEMSTRUCT FAR* lpMis = (MEASUREITEMSTRUCT FAR*) lpMeasureItem;

		SetRect(&rcText, _App.sizeIcon.cx + 8, 0, 50, lpMis->itemHeight = _App.sizeIcon.cy + 4);
		ComboBox_GetText(GetDlgItem(hDlg, lpMis->CtlID), szText, 20);
		hDC = GetDC(hDlg);
		hFont = SelectFont(hDC, _App.options.hFont);
		DrawText(hDC, szText, -1, &rcText, DT_CALCRECT | DT_VCENTER | DT_SINGLELINE);
		lpMis->itemWidth = rcText.right;
		SelectFont(hDC, hFont);
		ReleaseDC(hDlg, hDC);
		}
	}

void    CALLBACK    Nothing (void)
	{
	}

void    CALLBACK    KillApp (void)
	{
	PostMessage(_App.hWndFrame, WM_CLOSE, 0, 0L);
	}
