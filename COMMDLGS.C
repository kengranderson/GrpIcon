#include "grpicon.h"
#include <colordlg.h>

extern	char    _szIcon[];

/****************************************************************************************************************
*   Function:   BOOL	WINAPI	_export	SetColorHookProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)	*
*                                                                                               				*
*	Hook function for ChooseColor Common Dialog.																*
*	Changes caption of Color Dialog to indicate the Group, changes background color and adds custom buttons.	*
*                                                                                               				*
****************************************************************************************************************/
BOOL	WINAPI	_export	SetColorHookProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	static	LPCHOOSECOLOR	lpCs;
	static	RECT			rcLumScroll, rc;


	switch (msg)
		{
		case WM_INITDIALOG:
			lpCs = (LPCHOOSECOLOR) lParam;
			CustomizeCommonDialog(hDlg, IDS_CLRDLGTITLE, lParam);
			// Get coordinates of Lum Scroll control relative to parent.
			GetWindowRect(hDlg, &rc);
			GetWindowRect(GetDlgItem(hDlg, COLOR_LUMSCROLL), &rcLumScroll);
			OffsetRect(&rcLumScroll, -rc.left, -rc.top);
			// Set rc to the rect including rcLumScroll and
			GetClientRect(hDlg, &rc);
			SetRect(&rc, rcLumScroll.left, rcLumScroll.top, rc.right, rcLumScroll.bottom);
			return(TRUE);

		case WM_CTLCOLOR:
			switch (HIWORD(lParam))
				{
				case CTLCOLOR_DLG:
				case CTLCOLOR_STATIC:
					SetBkMode((HDC) wParam, TRANSPARENT);
					return((UINT) _App.hbrDlgBkGnd);

				case CTLCOLOR_EDIT:
				default:
					break;
				}
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
				case IDOK:
					Static_DeassignIcon(GetDlgItem(hDlg, IDI_GRP30));
					break;

				case IDC_RESET:
					lpCs->lCustData = IDC_RESET;	// Set status to IDC_RESET to indicate reset button was pressed.
					PostMessage(hDlg, WM_COMMAND, IDOK, MAKELONG(GetDlgItem(hDlg, IDOK), BN_CLICKED));	// Send IDOK msg to close dialog.
					break;

				case pshHelp:
					WinHelp(_App.hWndFrame, _lpICSras->szHelpFile, HELP_CONTEXT, 11);
					break;
				}
			break;

		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			{
			// See if the mouse is to the right of the Lum Scroll.  If so, invalidate that area.

			}


		default:
			break;
		}
	return(FALSE);
	}


BOOL WINAPI _export SetWallPaperHookProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	static  LPOPENFILENAME lpOfn;
	static	char    	szFileName[13];
	static	HWND		hWndOK;
	static	BOOL		fInitTile = TRUE;

	switch (msg)
		{
		case WM_INITDIALOG:
			lpOfn = (LPOPENFILENAME) lParam;
			CustomizeCommonDialog(hDlg, IDS_BMPDLGTITLE, lParam);
			CheckDlgButton(hDlg, IDC_TILESTUFF, ((LPBITMAPEX) lpOfn->lCustData)->fTile = fInitTile);
			return(TRUE);

		case WM_CTLCOLOR:
			switch (HIWORD(lParam))
				{
				case CTLCOLOR_DLG:
				case CTLCOLOR_STATIC:
				case CTLCOLOR_BTN:
				case CTLCOLOR_SCROLLBAR:
					SetBkMode((HDC) wParam, TRANSPARENT);
					return((UINT) _App.hbrDlgBkGnd);

				case CTLCOLOR_EDIT:
				case CTLCOLOR_LISTBOX:
				default:
					break;
				}
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case lst1:
					{
					HWND    hWndlst1 = GetDlgItem(hDlg, lst1);

					switch (HIWORD(lParam))
						{
						case LBN_SELCHANGE:     // File List box selection has changed
							{
							static	UINT	nSel;
							static	HBITMAP hBMP;
							static	HWND	hWndBMP;
							static	char	szNull[] = "";

							hWndBMP = GetDlgItem(hDlg, IDC_BITMAP);
							SetWindowText(hWndBMP, szNull);	// release old bitmap, if any.
							nSel = ListBox_GetCurSel(hWndlst1);
							ListBox_GetText(hWndlst1, nSel, (LPARAM) (LPSTR) szFileName);
							SetWindowText(hWndBMP, szFileName);
							hBMP = (HBITMAP) SendMessage(hWndBMP, WM_GETBITMAP, 0, 0L);
							if (hBMP != NULL)
								{
								static	char	szColors[10];
								static	BITMAPINFOHEADER	bmi;

								// Fill in the numbers.
								SendMessage(hWndBMP, WM_GETBITMAPINFOHEADER, 0, (DWORD) (LPBITMAPINFOHEADER) &bmi);
								SetDlgItemInt(hDlg, IDC_WIDTH, (int) bmi.biWidth, FALSE);
								SetDlgItemInt(hDlg, IDC_HEIGHT, (int) bmi.biHeight, FALSE);
								wsprintf(szColors, "%ld", (DWORD) bmi.biPlanes << (DWORD) bmi.biBitCount);
								SetDlgItemText(hDlg, IDC_COLORS, szColors);
								}
							else
								{
								// Blank the size stuff.
								SetDlgItemText(hDlg, IDC_WIDTH, szNull);
								SetDlgItemText(hDlg, IDC_HEIGHT, szNull);
								SetDlgItemText(hDlg, IDC_COLORS, szNull);
								}

							EnableWindow(hWndOK, (BOOL) TRUE);
							}
							break;
						}
					}
					break;

				case IDCANCEL:
					SetDlgItemText(hDlg, IDC_BITMAP, "");	// release old bitmap, if any.
					break;

				case IDOK:
					// bitmap handle, palette are passed back in lCustData of OPENFILENAME struct.
					((LPBITMAPEX) lpOfn->lCustData)->hBitmap = (HBITMAP) SendDlgItemMessage(hDlg, IDC_BITMAP, WM_GETBITMAP, 0, 0L);
					((LPBITMAPEX) lpOfn->lCustData)->hPalette = (HPALETTE) SendDlgItemMessage(hDlg, IDC_BITMAP, WM_GETPALETTE, 0, 0L);
					break;

				case IDC_RESET:
					((LPBITMAPEX) lpOfn->lCustData)->hBitmap = (HBITMAP) IDC_RESET;	// Set status to IDC_RESET to indicate reset button was pressed.
					PostMessage(hDlg, WM_COMMAND, IDCANCEL, MAKELONG(GetDlgItem(hDlg, IDCANCEL), BN_CLICKED));	// Send IDCANCEL msg to close dialog.
					break;

				case IDC_TILESTUFF:
					((LPBITMAPEX) lpOfn->lCustData)->fTile = (BOOL) IsDlgButtonChecked(hDlg, IDC_TILESTUFF);
					fInitTile = ((LPBITMAPEX) lpOfn->lCustData)->fTile;
					break;

				case pshHelp:
					WinHelp(_App.hWndFrame, _lpICSras->szHelpFile, HELP_CONTEXT, 10);
					break;

				default:
					break;
				}
			break;

		case WM_DESTROY:
			Static_DeassignIcon(GetDlgItem(hDlg, IDI_GRP30));
			break;

		default:
			break;
		}
	return(FALSE);
	}



BOOL WINAPI _export AssignIconHookProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	#define	lpmis	((LPMEASUREITEMSTRUCT) lParam)
	#define	lpdis	((const DRAWITEMSTRUCT FAR*) lParam)
	static  LPOPENFILENAME lpOfn;
	static  LPICONOBJ   IconObject; // Icon object
	static	char    	szFileName[13];
	static	HWND		hWndOK;
	static	UINT		nIconsAcross;
	static	RECT		rect;
	static	BOOL		fInFetch;

	switch (msg)
		{
		case WM_INITDIALOG:
			lpOfn = (LPOPENFILENAME) lParam;
			CustomizeCommonDialog(hDlg, IDS_ICONDLGTITLE, lParam);
			IconObject = NULL;
			GetClientRect(GetDlgItem(hDlg, IDC_ICONLISTBOX), &rect);
			nIconsAcross = rect.right / (GetSystemMetrics(SM_CXICON) + 4);
			return(TRUE);

		case WM_CTLCOLOR:
			switch (HIWORD(lParam))
				{
				case CTLCOLOR_DLG:
				case CTLCOLOR_STATIC:
				case CTLCOLOR_BTN:
				case CTLCOLOR_SCROLLBAR:
					SetBkMode((HDC) wParam, TRANSPARENT);
					return((UINT) _App.hbrDlgBkGnd);

				case CTLCOLOR_EDIT:
				case CTLCOLOR_LISTBOX:
				default:
					break;
				}
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case lst1:
					{
					HWND    hWndlst1 = GetDlgItem(hDlg, lst1);

					switch (HIWORD(lParam))
						{
						case LBN_DBLCLK:
							return(TRUE);		// Ignore this, as it has no real use.

						case LBN_SELCHANGE:     // File List box selection has changed
							{
							static	int     nSel = -1;			// This is the selection number
							int				nsel = ListBox_GetCurSel(hWndlst1);

							if (nsel == nSel)		// We have received a second LBN_SELCHANGE message - ignore it!
								break;
							else
								nSel = nsel;

							ListBox_GetText(hWndlst1, nSel, (LPARAM) (LPSTR) szFileName);
							// Create new IconObject (old one is destroyed if necessary - no need to check value - function is NULL-safe)
							IconObject = IconObject_New(IconObject, szFileName, GetDlgItem(hDlg, IDC_ICONLISTBOX), _PMGroup.hXIcon, _App.hInst);
							ListBox_ResetContent(IconObject->hWndListBox);		// Clear listbox contents
						    if (IconObject->nErrorCode == ICONOBJ_OK)		// If all is OK
						    	{
    							IconObject_GetIcons(IconObject, 0, nIconsAcross - 1);	// Get enough icons to fill LB
								IconObject_Fill(IconObject, 0, nIconsAcross, TRUE);	// Fill listbox with enough icons to fill LB
					    		ListBox_SetCurSel(IconObject->hWndListBox, 0);	// Select 1st icon
					    		while (IconObject->nIconMax < (IconObject->nNumIcons - 1))		// While there are more icons than meets the eye...
									{
									static	MSG	msg;
									static	UINT	nIconMax;

									fInFetch = TRUE;	// Set fInFetch flag so we can accidentally cancel til we're done.
									// See if there any messages for this window.
        							if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        								{
        								// if it is an LBN_SELCHANGE message from this listbox, quit and process the message.
        								if ((msg.message == WM_COMMAND) && (msg.hwnd == hDlg))
        									{
        									if ((wParam == IDOK) || (wParam == IDCANCEL) ||
        										((wParam == lst1) && (HIWORD(lParam) == LBN_SELCHANGE)))
	                						return(TRUE);	// We clicked another filename before this process finished - abort
	                						}
	                					else				// Process the message.
	                						{
	                						GetMessage(&msg, NULL, 0, 0);
	            							TranslateMessage(&msg);
    	        							DispatchMessage(&msg);
	                						}
									    }
/*
									// Do a PeekMessage loop to allow other apps to work
        							if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            							{
            							TranslateMessage(&msg);
            							DispatchMessage(&msg);
            							}
*/
									// Get next page of icons.
									if (IconObject)
										{
										nIconMax = IconObject->nIconMax;
               							if (!IconObject_GetIcons(IconObject, nIconMax + 1, nIconMax + nIconsAcross))
               								break;	// break if an error occurs.
										IconObject_Fill(IconObject, nIconMax + 1, IconObject->nIconMax - nIconMax, FALSE);	// Add new icons to listbox
										}
               						}
               					fInFetch = FALSE; // Clear fInFetch flag - OK to Cancel now.
								}
							if (IconObject)
								EnableWindow(hWndOK, (BOOL) (IconObject->nErrorCode == ICONOBJ_OK));
							nSel = -1;				// Re-initialize nSel to -1
							}
							break;
						}
					}
					break;

				case IDOK:
					if (fInFetch == TRUE)
						return(TRUE);			// Ignore this message if we are in the middle of a fetch!
					if (IconObject &&
						IconObject->hIcons)
						{
						IconObject->nUsedIcon = IconObject->nSelectedIcon;	// Tag icon as used so it won't be destroyed.
						// icon handle, number are passed back in OPENFILENAME struct.
						lpOfn->hwndOwner = (HWND) IconObject->hIcons[IconObject->nUsedIcon];
						lpOfn->lCustData = IconObject->nUsedIcon;
						}
					else						// See if they have done the Rick Bredy (they typed in a file name and hit OK)
						{
						static	OFSTRUCT	OfStruct;

						Edit_GetText(GetDlgItem(hDlg, edt1), szFileName, 13);	// Get whatever text is in the edit control.
						if (OpenFile(szFileName, &OfStruct, OF_EXIST) != HFILE_ERROR)	// If a legit file is in there...
							{
							// icon handle, number are passed back in OPENFILENAME struct.
							lpOfn->hwndOwner = (HWND) IconObject_GetOne(szFileName, 0, _App.hInst);
							lpOfn->lCustData = 0L;
							}
						}
				case IDCANCEL:
					// Get handle of selected icon
					//IconObj->hSelectedIcon
					// Destroy IconObject.
					if (fInFetch == TRUE)
						return(TRUE);			// Ignore this message if we are in the middle of a fetch!
					IconObject = IconObject_Destroy(IconObject);	// This sets IconObject to NULL
					Static_DeassignIcon(GetDlgItem(hDlg, IDI_GRP30));
					break;

				case IDC_RESET:
					lpOfn->lCustData = (DWORD) IDC_RESET;	// Set status to IDC_RESET to indicate reset button was pressed.
					PostMessage(hDlg, WM_COMMAND, IDCANCEL, MAKELONG(GetDlgItem(hDlg, IDCANCEL), BN_CLICKED));	// Send IDCANCEL msg to close dialog.
					break;

				case IDC_ICONLISTBOX:       // Icon Listbox was clicked
					switch (HIWORD(lParam))
						{
						case LBN_DBLCLK:
							SendMessage(hDlg, WM_COMMAND, IDOK, MAKELONG(0, BN_CLICKED));
							return(TRUE);
						case LBN_SELCHANGE:     // Icon List box selection has changed
							if (IconObject &&
								IconObject->hIcons)
								IconObject->nSelectedIcon = ListBox_GetCurSel(IconObject->hWndListBox);
							return(TRUE);
						default:
							break;
						}
					break;

				case pshHelp:
					WinHelp(_App.hWndFrame, _lpICSras->szHelpFile, HELP_CONTEXT, 8);
					break;

				default:
					break;
				}
			break;

		// NOTE:	THIS MESSAGE COMES BEFORE WM_INITDIALOG!!!
		case WM_MEASUREITEM:
			if (lpmis->CtlID == IDC_ICONLISTBOX)
				{
				lpmis->itemHeight = _App.sizeIcon.cy + 4;
				lpmis->itemWidth  = _App.sizeIcon.cx + 4;
				return(TRUE);
				}
			break;

		case WM_DRAWITEM:
			if (lpdis->CtlID == IDC_ICONLISTBOX)
				{
/*
The following commented-out code is used in the event that a user pulls the listbox thumb to a point that
would bring into view an icon that did not exist yet.  It is possible that not all of the icons are available,
as I am reading the icons in chunks and allowing message traffic between chunks.  Currently, however, as the listbox
does not get ADDITEM messages until the icon has been extracted, the above situation can never happen.
If a user pulls the thumb all the way to the right, it will bring into view the highest present index, which will
not be the highest when the next chunk is read.  Thus, the thumb will move to the left.  If this behavior proves
to be inadequate to users, this code should be entered, and the listbox should be filled with enough items to account for
all of the icons.  Then the items can be changed as they are read in during the chunk reads.
				// Skip next code if empty listbox
				if (lpdis->itemID != 0xFFFF)
					{
					// Make sure that requested icon images are ready; stop and extract them if necessary.
					if (lpdis->itemID > IconObject->nIconMax)
						{
						// Get enough icons to satisfy display request.
						static	UINT	nIconMax;
						static	HCURSOR	hCursor;

						nIconMax = IconObject->nIconMax;
						hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
                		IconObject_GetIcons(IconObject, nIconMax + 1, lpdis->itemID);
						IconObject_Fill(IconObject, nIconMax + 1, IconObject->nIconMax - nIconMax, FALSE);	// Add new icons to listbox
						SetCursor(hCursor);
						}
					}
*/
				PMGroup_OnDrawItem(hDlg, lpdis);
       			return(TRUE);
				}
			break;

		default:
				break;
		}
	return(FALSE);
	}


void	CustomizeCommonDialog (HWND hDlg, UINT nID, LPARAM lParam)
	{
	#define lpCs	((LPCHOOSECOLOR) lParam)
	#define	lpOfn	((LPOPENFILENAME) lParam)
	extern	char	_szBrush[];
	extern	char    _szWallPaper[];
	static	char	szReset[] = "R&eset";
#ifdef	FIRSTREV
	static	char	szCopyRight1[] = "GrpIcon © 1992\nInner-City Software";
	static	char	szCopyRight2[] = "GrpIcon\n© 1992 Inner-City Software";
#else
	static	char	szCopyRight1[] = "GrpIcon 2.0 © 1992\nInner-City Software";
	static	char	szCopyRight2[] = "GrpIcon 2.0\n© 1992 Inner-City Software";
#endif
	static	char	szButton[] = "button";
	static	char	szStatic[] = "static";
	static	char	szIcoStat[] = "&Icon Images:";
	static	char	szBmpStat[] = "&Bitmap Images:";
	static	UINT	cxIcoStat;
	static	UINT	cxBmpStat;
	static	char	szDlgTitle[1 + GENERICSTRLENGTH / 2];
	static	RECT	rcHelp, rcOK, rcCancel, rcReset, rcIcon, rcCopyRight, rcDlg;
	static	HWND	hWnd;
	static	POINT	ptIcon;
	static	UINT	cyChar;
	static	HDC		hDC;

	if (!ptIcon.x)
		{
		ptIcon.x = GetSystemMetrics(SM_CXICON);
		ptIcon.y = GetSystemMetrics(SM_CYICON);
		}

	hDC = GetDC(hDlg);
	cyChar = HIWORD(GetTextExtent(hDC, "X", 1));
	cxIcoStat = LOWORD(GetTextExtent(hDC, szIcoStat, lstrlen(szIcoStat)));
	cxBmpStat = LOWORD(GetTextExtent(hDC, szBmpStat, lstrlen(szBmpStat)));
	ReleaseDC(hDlg, hDC);

	// Append the PMGroup title to the dlg caption NOTE - casting to an OPENFILENAME works cuz
	// the CHOOSECOLOR struct has its hWndOwner member at the same offset.
	LoadString(_App.hInst, nID, szDlgTitle, GENERICSTRLENGTH / 2);
	GetWindowText(((LPOPENFILENAME) lParam)->hwndOwner, szDlgTitle + lstrlen(szDlgTitle), GENERICSTRLENGTH / 2);
	lstrcat(szDlgTitle, "'");
	SetWindowText(hDlg, szDlgTitle);

	// Create Reset button.
	GetWindowRect(GetDlgItem(hDlg, pshHelp), &rcHelp);		// get abs coords of Help button
	rcHelp.right -= rcHelp.left;
	rcHelp.bottom -= rcHelp.top;							// cvt second point to button dimensions
	ScreenToClient(hDlg, (LPPOINT) &rcHelp);				// Map start coords to hDlg coord system

	if (nID == IDS_CLRDLGTITLE)
		{
		#define	rcMix	rcOK
		// Nix 'Define Custom Colors' button and put our 'Reset' button in its place.
		GetWindowRect(hWnd = GetDlgItem(hDlg, COLOR_MIX), &rcMix);	// get abs coords of Mix button
		ScreenToClient(hDlg, (LPPOINT) &rcMix);					// Map start coords to hDlg coord system
		DestroyWindow(hWnd);									// Terminate Mix button

		// Set up rectangle for Reset button.
		SetRect(&rcReset, rcHelp.left, rcMix.top, rcHelp.right, rcHelp.bottom);
		// Set up rectangle for Copyright info.
		SetRect(&rcCopyRight, rcMix.left + ptIcon.x + 4, rcMix.top + rcHelp.bottom - ptIcon.y,
			rcHelp.left - (rcMix.left + ptIcon.x + 4), ptIcon.y);
		// Set up rectangle for GrpIcon icon.
		SetRect(&rcIcon, rcMix.left, rcMix.top + rcHelp.bottom - ptIcon.y, ptIcon.x, ptIcon.y);
		}
	else
		{
		// I want to position the Reset button the same distance below the Help button that
		// the Cancel button is below the OK button.
		GetWindowRect(GetDlgItem(hDlg, IDOK), &rcOK);			// get abs coords of OK button
		GetWindowRect(GetDlgItem(hDlg, IDCANCEL), &rcCancel);	// get abs coords of Cancel button

		// Set up rectangle for Reset button.
		SetRect(&rcReset, rcHelp.left, rcHelp.top + rcCancel.top - rcOK.top, rcHelp.right, rcHelp.bottom);
		ScreenToClient(hDlg, (LPPOINT) &rcOK);					// Map start coords to hDlg coord system
		ScreenToClient(hDlg, (LPPOINT) &rcOK.right);			// Map start coords to hDlg coord system
		GetClientRect(hDlg, &rcDlg);

		// Get coordinates of 'drives' combo box
		GetWindowRect(GetDlgItem(hDlg, cmb2), &rcCancel);
		ScreenToClient(hDlg, (LPPOINT) &rcCancel);
		ScreenToClient(hDlg, (LPPOINT) &rcCancel.right);

		// Set up rectangle for Copyright info.
		SetRect(&rcCopyRight, rcOK.left - 2, rcCancel.top, rcOK.right - rcOK.left + 2, 5 * cyChar);

		// Set up rectangle for GrpIcon icon.
		SetRect(&rcIcon, (rcOK.right + rcOK.left - ptIcon.x) / 2, rcCopyRight.top - ptIcon.y - 8, ptIcon.x, ptIcon.y);
		// Get x - coordinate of 'List files of type' combo box
		GetWindowRect(GetDlgItem(hDlg, cmb1), &rcOK);
		rcOK.right -= rcOK.left;
		rcOK.bottom -= rcOK.top;							// cvt second point to button dimensions
		ScreenToClient(hDlg, (LPPOINT) &rcOK);
		}

	hWnd = CreateWindow(szButton, szReset, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
		rcReset.left, rcReset.top, rcReset.right, rcReset.bottom,
		hDlg, (HMENU) IDC_RESET, _App.hInst, NULL);

	// Create copyright message.
	CreateWindow(szStatic, (nID == IDS_CLRDLGTITLE) ? szCopyRight1 : szCopyRight2,
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		rcCopyRight.left, rcCopyRight.top, rcCopyRight.right, rcCopyRight.bottom,
		hDlg, (HMENU) 0xFFFF, _App.hInst, NULL);

	// Create static to display GrpIcon icon
	Static_AssignIcon(CreateWindow(szStatic, "", WS_CHILD | WS_VISIBLE | SS_ICON,
		rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom,
		hDlg, (HMENU) IDI_GRP30, _App.hInst, NULL), GetClassWord(_App.hWndFrame, GCW_HICON));

	AdjustWindowRect(&rcDlg, GetWindowStyle(hDlg), FALSE);
	OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);

	switch (nID)
		{
		case IDS_CLRDLGTITLE:
			EnableWindow(hWnd, BrushOf(lpCs->hwndOwner) != _PMGroup.hbrBackground);
			break;

		case IDS_BMPDLGTITLE:
			EnableWindow(hWnd, WallPaperOf(lpOfn->hwndOwner) != NULL);
			CreateWindow(szStatic, szBmpStat, WS_CHILD | WS_VISIBLE | SS_LEFT,
				rcOK.left, rcOK.top + 2 * cyChar, cxBmpStat, cyChar,
				hDlg, (HMENU) 0xFFFF, _App.hInst, NULL);

			CreateWindow(szButton, "T&ile Bitmap", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
				rcCancel.left, rcCancel.top + 4 * cyChar, cxBmpStat, cyChar,
				hDlg, (HMENU) IDC_TILESTUFF, _App.hInst, NULL);

			cxBmpStat = cxBmpStat / 2;
			CreateWindow(szStatic, "Width:", WS_CHILD | WS_VISIBLE | SS_LEFT,
				rcCancel.left, rcCancel.top + 6 * cyChar, cxBmpStat, cyChar,
				hDlg, (HMENU) 0xFFFF, _App.hInst, NULL);

			CreateWindow(szStatic, "Height:", WS_CHILD | WS_VISIBLE | SS_LEFT,
				rcCancel.left, rcCancel.top + 8 * cyChar, cxBmpStat, cyChar,
				hDlg, (HMENU) 0xFFFF, _App.hInst, NULL);

			CreateWindow(szStatic, "Colors:", WS_CHILD | WS_VISIBLE | SS_LEFT,
				rcCancel.left, rcCancel.top + 10 * cyChar, cxBmpStat, cyChar,
				hDlg, (HMENU) 0xFFFF, _App.hInst, NULL);

			CreateWindow(szStatic, "", WS_CHILD | WS_VISIBLE | SS_LEFT,
				rcCancel.left + cxBmpStat + 4, rcCancel.top + 6 * cyChar, cxBmpStat, cyChar,
				hDlg, (HMENU) IDC_WIDTH, _App.hInst, NULL);

			CreateWindow(szStatic, "", WS_CHILD | WS_VISIBLE | SS_LEFT,
				rcCancel.left + cxBmpStat + 4, rcCancel.top + 8 * cyChar, cxBmpStat, cyChar,
				hDlg, (HMENU) IDC_HEIGHT, _App.hInst, NULL);

			CreateWindow(szStatic, "", WS_CHILD | WS_VISIBLE | SS_LEFT,
				rcCancel.left + cxBmpStat + 4, rcCancel.top + 10 * cyChar, cxBmpStat, cyChar,
				hDlg, (HMENU) IDC_COLORS, _App.hInst, NULL);

			rcOK.bottom = rcOK.right * 3 / 4;
			CreateWindow("ICSBitmap", "", WS_CHILD | WS_VISIBLE | WS_BORDER | BMS_STRETCH,
				rcOK.left, rcOK.top + 4 * cyChar, rcOK.right, rcOK.bottom,
				hDlg, (HMENU) IDC_BITMAP, _App.hInst, NULL);
			// Resize dialog to fit new controls.
			rcDlg.bottom += (rcOK.bottom + 3 * cyChar);
			SetWindowPos(hDlg, NULL, 0, 0, rcDlg.right, rcDlg.bottom, SWP_NOZORDER | SWP_NOMOVE);
			break;

		case IDS_ICONDLGTITLE:
			EnableWindow(hWnd, IconOf(lpOfn->hwndOwner) != _PMGroup.hIcon);
			CreateWindow(szStatic, szIcoStat, WS_CHILD | WS_VISIBLE | SS_LEFT,
				rcOK.left, rcOK.top + 2 * cyChar, cxIcoStat, cyChar,
				hDlg, (HMENU) 0xFFFF, _App.hInst, NULL);

			hWnd = CreateWindow("listbox", "",
				WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_BORDER | WS_HSCROLL |
				LBS_NOINTEGRALHEIGHT | LBS_MULTICOLUMN | LBS_DISABLENOSCROLL | WS_TABSTOP,
				rcOK.left, rcOK.top + 4 * cyChar, rcCancel.right - rcOK.left, ptIcon.y + 4 + GetSystemMetrics(SM_CYHSCROLL),
				hDlg, (HMENU) IDC_ICONLISTBOX, _App.hInst, NULL);

/*			nID = ptIcon.x + 4;
			GetClientRect(hWnd, &rcOK);
			SetWindowPos(hWnd, NULL, 0, 0, nID * (rcOK.right / nID),
				rcOK.bottom, SWP_NOZORDER | SWP_NOMOVE);
*/
			ListBox_SetColumnWidth(hWnd, ptIcon.x + 4);
			// Resize dialog to fit new controls.
			rcDlg.bottom += (rcOK.bottom + 5 * cyChar);
			SetWindowPos(hDlg, NULL, 0, 0, rcDlg.right, rcDlg.bottom, SWP_NOZORDER | SWP_NOMOVE);
			break;
		}
	return;
	}

