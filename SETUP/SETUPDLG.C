#include "Setup.h"

void    HandleSelectionState (HWND hWnd, const DRAWITEMSTRUCT FAR*  lpDis);
void	HandleWMDrawItem(HWND hDlg, WPARAM wParam, LPARAM lParam);
void	HandleWMMeasureItem(LPARAM lParam);


#define YES_COLUMN          0
#define NO_COLUMN           1
#define CANCEL_COLUMN       2

HGDIOBJ		_hGDIObj[7];

INITDIALOGCTLS  idcs[] =
	{
	{IDC_LOGOBMP, (HFONT*) &_hGDIObj[2]},
	{IDI_WCK, (HFONT*) &_hGDIObj[3]},
	{IDC_COPYRIGHT, (HFONT*) &_hGDIObj[6]},
	{IDI_MODICON, (HFONT*) &_hGDIObj[4]},
	{IDC_PRESENTS, (HFONT*) &_hGDIObj[0]},
	{IDC_WCK1, (HFONT*) &_hGDIObj[1]},
	{IDC_WCK2, (HFONT*) &_hGDIObj[1]},
	{IDC_MODULE, (HFONT*) &_hGDIObj[0]},
	{IDC_MODNAME, (HFONT*) &_hGDIObj[1]},
	{0, NULL}
	};

BITMAP	_bmButtons;


// Setup's initial sign on screen.  Asks user for destination directory.
BOOL FAR PASCAL WelcomeDlgProc (HWND hDlg, WORD wMsg, WORD wParam, LONG lParam)
    {
    static HBRUSH hDlgBkGnd;
//    static	RECT	rcButtons;
    BOOL fProcessed = TRUE;
    char szBuf[MAXDIR];
    OFSTRUCT ofStruct;
    RECT 	rc;
    BITMAP	bm;
    POINT	pt = {0, 0};
    int nBMP = -1;
    LOGBRUSH lb = {BS_PATTERN, 0, 0};
	REQUESTFONTSTRUCT rqf = {sizeof(REQUESTFONTSTRUCT), "MS Sans Serif", 14, FALSE, 700};

    switch (wMsg)
        {
        case WM_INITDIALOG:
            lb.lbHatch = (short int) LoadBitmap((HINSTANCE) lParam, MAKEINTRESOURCE(BMP_BACKGROUND));
            hDlgBkGnd = CreateBrushIndirect(&lb);
           	DeleteBitmap(lb.lbHatch);
            _hGDIObj[2] = (HGDIOBJ) LoadBitmap((HINSTANCE) lParam, MAKEINTRESOURCE(BMP_GLOBE));
			_hGDIObj[3] = (HGDIOBJ) LoadIcon((HINSTANCE) lParam, MAKEINTRESOURCE(IDI_WCK));
			_hGDIObj[4] = (HGDIOBJ) LoadIcon((HINSTANCE) lParam, MAKEINTRESOURCE(IDI_MODICON));

			// Load dialog box fonts.
			_hGDIObj[0] = RequestFont(&rqf);
			rqf.fItalic = TRUE;
			_hGDIObj[1] = RequestFont(&rqf);
			rqf.fItalic = FALSE;
			rqf.nPtSize = 8;
			rqf.nWeight = 400;
			_hGDIObj[6] = RequestFont(&rqf);

			while (idcs[++nBMP].nID)
				SendDlgItemMessage(hDlg, idcs[nBMP].nID, WM_ICSSETFONT, (WPARAM) *idcs[nBMP].phFont, FALSE);

			// Center Logo image inside canvas.
			GetWindowRect(GetDlgItem(hDlg, IDC_LOGOCANVAS), &rc);	// Get coords of canvas
			ClientToScreen(hDlg, &pt);                             	// Get screen coords of dlg client corner
			OffsetRect(&rc, -pt.x, -pt.y);

			GetBitmap(_hGDIObj[2], &bm);
			SetWindowPos(GetDlgItem(hDlg, IDC_LOGOBMP), NULL,
				(rc.left + rc.right - bm.bmWidth) / 2,
				(rc.top + rc.bottom - bm.bmHeight) / 2,
				bm.bmWidth, bm.bmHeight, SWP_NOZORDER);

/*
			SetWindowPos(GetDlgItem(hDlg, IDC_LOGOBMP), NULL,
				rc.left + (((rc.right - rc.left) - bm.bmWidth) / 2),
				rc.top + (((rc.bottom - rc.top) - bm.bmHeight) / 2),
				bm.bmWidth, bm.bmHeight, SWP_NOZORDER);
*/

            SetupInfoSys(SIM_GETAPPNAME, 0, szBuf);
            SetWindowText(hDlg, szBuf);
            GetWindowRect(hDlg, &rc);
            SetWindowPos(hDlg, NULL,
                (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 3,
                0, 0, SWP_NOSIZE | SWP_NOZORDER);

            SetupInfoSys(SIM_GETDEFDIR, 0, szBuf);
            SetDlgItemText(hDlg, IDC_DESTPATH, szBuf);
            SetWindowPos(GetDlgItem(hDlg, IDOK), NULL, 0, 0, _bmButtons.bmWidth, _bmButtons.bmHeight, SWP_NOMOVE | SWP_NOZORDER);
            SetWindowPos(GetDlgItem(hDlg, IDCANCEL), NULL, 0, 0, _bmButtons.bmWidth, _bmButtons.bmHeight, SWP_NOMOVE | SWP_NOZORDER);
            break;

        case WM_CTLCOLOR:
			if (HIWORD(lParam) == CTLCOLOR_DLG)
                {
                RECT rc;

                // Color the dialogbox background with the specified RGB color
                GetClientRect((HWND) LOWORD(lParam), &rc);
                FillRect((HDC) wParam, &rc, hDlgBkGnd);
                return((UINT) hDlgBkGnd);
                }
			else if (HIWORD(lParam) == CTLCOLOR_STATIC)
                {
				return((UINT) GetStockBrush(LTGRAY_BRUSH));
                }
            break;

        case WM_COMMAND:
            switch (wParam)
                {
                case IDC_DESTPATH:
                    EnableWindow(GetDlgItem(hDlg, IDOK),
                        (BOOL) SendMessage((HWND) LOWORD(lParam), EM_LINELENGTH, 0, 0));
                    break;

                case IDOK:
                    GetDlgItemText(hDlg, IDC_DESTPATH, szBuf, sizeof(szBuf));
                        OpenFile(szBuf, &ofStruct, OF_PARSE);
                        lstrcpy(_szDstDir, (LPSTR) ofStruct.szPathName);
                // Do IDCANCEL case
                case IDCANCEL:
                    EndDialog(hDlg, wParam);
					DeleteFont(_hGDIObj[0]);
					DeleteFont(_hGDIObj[1]);
					DeleteFont(_hGDIObj[6]);
					DeleteBitmap(_hGDIObj[2]);
                    break;
                }
            break;

		case WM_MEASUREITEM:
			HandleWMMeasureItem(lParam);
            break;

		case WM_DRAWITEM:
			HandleWMDrawItem(hDlg, wParam, lParam);
			break;

        default:
            fProcessed = FALSE;
            break;
        }
    return(fProcessed);
    }


void	HandleWMDrawItem (HWND hDlg, WPARAM wParam, LPARAM lParam)
	{
	LPDRAWITEMSTRUCT lpDis = (LPDRAWITEMSTRUCT) lParam;

	if (lpDis->itemID == -1)
		{
		DrawFocusRect(lpDis->hDC, &lpDis->rcItem);
		return;
		}   //  lpDis->itemID == -1

	if (lpDis->itemAction & ODA_DRAWENTIRE)
		{
		HandleSelectionState(hDlg, lpDis);

		if (lpDis->itemState & ODS_FOCUS)
			DrawFocusRect(lpDis->hDC, &lpDis->rcItem);
		return;
		} //    lpDis->itemAction & ODA_DRAWENTIRE

	if (lpDis->itemAction & ODA_SELECT)
		{
		HandleSelectionState(hDlg, lpDis);
		return;
		}

	if (lpDis->itemAction & ODA_FOCUS)
		DrawFocusRect(lpDis->hDC, &lpDis->rcItem);
	}

void    HandleSelectionState (HWND hWnd, const DRAWITEMSTRUCT FAR*  lpDis)
	{
	int		nState;
	HDC		hDCMem = CreateCompatibleDC(lpDis->hDC);
	HBITMAP	hBitmap = SelectBitmap(hDCMem, _hGDIObj[5]);

	if (lpDis->itemState & ODS_SELECTED)
		nState = 1;
	else if (lpDis->itemState & ODS_FOCUS)
		nState = 2;
	else
		nState = 0;

	BitBlt(lpDis->hDC, 0, 0, _bmButtons.bmWidth, _bmButtons.bmHeight, hDCMem,
		(lpDis->CtlID - 1) * _bmButtons.bmWidth, nState * _bmButtons.bmHeight, SRCCOPY);
	SelectBitmap(hDCMem, hBitmap);
	DeleteDC(hDCMem);
	return;
	}

void	HandleWMMeasureItem (LPARAM lParam)
	{
	((LPMEASUREITEMSTRUCT) lParam)->itemWidth = _bmButtons.bmWidth;
	((LPMEASUREITEMSTRUCT) lParam)->itemHeight = _bmButtons.bmHeight;
	}


// Displays copying status.  Allows user to cancel the installation.
BOOL FAR PASCAL StatusDlgProc (HWND hDlg, WORD wMsg, WORD wParam, LONG lParam)
    {
    static HBRUSH hDlgBkGnd;
    BOOL fProcessed = TRUE;
    int nResult;
    char szBuf[100];
    RECT rc;
    LOGBRUSH lb = {BS_PATTERN, 0, 0};

    switch (wMsg)
        {
        case WM_INITDIALOG:
            lb.lbHatch = (short int) LoadBitmap((HINSTANCE) GetWindowWord(hDlg, GWW_HINSTANCE), MAKEINTRESOURCE(BMP_BACKGROUND));
            hDlgBkGnd = CreateBrushIndirect(&lb);
            DeleteBitmap(lb.lbHatch);
            SetupInfoSys(SIM_GETAPPNAME, 0, szBuf);
            SetWindowText(hDlg, szBuf);
            GetWindowRect(hDlg, &rc);
            SetWindowPos(hDlg, NULL,
                (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 3,
                0, 0, SWP_NOSIZE | SWP_NOZORDER);
            SetWindowPos(GetDlgItem(hDlg, IDCANCEL), NULL, 0, 0, _bmButtons.bmWidth, _bmButtons.bmHeight, SWP_NOMOVE | SWP_NOZORDER);
            break;

        case WM_SHOWWINDOW:
            fProcessed = FALSE;
            if (!wParam)
                break;
            EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);
            SetDlgItemText(hDlg, IDC_STATLINE1, "");
            SetDlgItemText(hDlg, IDC_STATLINE2, "");
            SendDlgItemMessage(hDlg, IDC_METER, WM_SETCOMPLETECOLOR, 0, GREEN);
            SendDlgItemMessage(hDlg, IDC_METER, WM_SETINCOMPLETECOLOR, 0, RED);
            SendDlgItemMessage(hDlg, IDC_METER, WM_SETPARTSCOMPLETE, 0, 0);
            SendDlgItemMessage(hDlg, IDC_METER, WM_SETPARTSINJOB, 0, 0);
            break;

        case WM_COMMAND:
            switch (wParam)
                {
                case IDOK:
                // User presses ENTER.  DO IDCANCEL case.
                case IDCANCEL:
                    nResult = MsgBox(_hInstance, hDlg, IDS_QUERYABORT, _szAppName,
                        MB_ICONQUESTION | MB_YESNO);
                    if (nResult == IDYES)
                        EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
                    DeleteObject(hDlgBkGnd);
                    break;
                }
            break;

        case WM_CTLCOLOR:
            if (HIWORD(lParam) == CTLCOLOR_DLG)
                {
                RECT rc;

                /* Color the dialogbox background with the specified RGB color */
                GetClientRect((HWND) LOWORD(lParam), &rc);
                FillRect((HDC) wParam, &rc, hDlgBkGnd);
                return((UINT) hDlgBkGnd);
                }
            else
                SetBkMode((HDC) wParam, TRANSPARENT);
                return((UINT) hDlgBkGnd);
            break;

		case WM_MEASUREITEM:
			HandleWMMeasureItem(lParam);
            break;

		case WM_DRAWITEM:
			HandleWMDrawItem(hDlg, wParam, lParam);
			break;

        default:
            fProcessed = FALSE;
            break;
        }
    return(fProcessed);
    }

// Prompt's user to insert a different diskette.
BOOL FAR PASCAL InsertDiskDlgProc (HWND hDlg, WORD wMsg, WORD wParam, LONG lParam)
    {
    BOOL fProcessed = TRUE;
    char szBuf[100];
    RECT rc;
    static HBRUSH hDlgBkGnd;
    LOGBRUSH lb = {BS_PATTERN, 0, 0};

    switch (wMsg)
        {
        case WM_INITDIALOG:
            lb.lbHatch = (short int) LoadBitmap((HINSTANCE) GetWindowWord(hDlg, GWW_HINSTANCE), MAKEINTRESOURCE(BMP_BACKGROUND));
            hDlgBkGnd = CreateBrushIndirect(&lb);
            DeleteBitmap(lb.lbHatch);
            // lParam is address of diskette description.
            SetupInfoSys(SIM_GETAPPNAME, 0, szBuf);
            SetWindowText(hDlg, szBuf);
            GetWindowRect(hDlg, &rc);
/*            SetWindowPos(hDlg, NULL,
                (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2,
                (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 3,
                0, 0, SWP_NOSIZE | SWP_NOZORDER);
*/
            // Throw away the data segment and use the new one.
            // This is in case the data segment has moved.
            SetDlgItemText(hDlg, IDC_DISKNAME, (LPSTR) (char NEAR *) lParam);
            SetDlgItemText(hDlg, IDC_SRCPATH, _szSrcDir);
            SendDlgItemMessage(hDlg, IDC_SRCPATH, EM_LIMITTEXT, sizeof(_szSrcDir), 0);
            MessageBeep(0);
            break;

        case WM_COMMAND:
            switch (wParam)
                {
                case IDC_SRCPATH:
                    EnableWindow(GetDlgItem(hDlg, IDOK),
                        (BOOL) SendMessage((HWND) LOWORD(lParam), EM_LINELENGTH, 0, 0));
                    break;

                case IDOK:
                    GetDlgItemText(hDlg, IDC_SRCPATH, _szSrcDir, sizeof(_szSrcDir));
                    // Do IDCANCEL case

                case IDCANCEL:
                    EndDialog(hDlg, wParam);
                    DeleteObject(hDlgBkGnd);
                    break;
                }
                break;

        case WM_CTLCOLOR:
            if (HIWORD(lParam) == CTLCOLOR_DLG)
                {
                RECT rc;

                /* Color the dialogbox background with the specified RGB color */
                GetClientRect((HWND) LOWORD(lParam), &rc);
                FillRect((HDC) wParam, &rc, hDlgBkGnd);
                return((UINT) hDlgBkGnd);
                }
            else
                SetBkMode((HDC) wParam, TRANSPARENT);
                return((UINT) hDlgBkGnd);
            break;

        default:
            fProcessed = FALSE;
            break;
        }
    return(fProcessed);
    }

/********************************************************************
*   Creates a logical font from a variable number of parameters.    *
*   The REQUESTFONTSTRUCT has a first element specifying the length *
*   of the structure - thus, it can be extendedas we go along.      *
********************************************************************/
HFONT   RequestFont (LPREQUESTFONTSTRUCT lpRequestFont)
	{
	static  LPLOGFONT   lpLf;
	static  HFONT       hFont;
	static  HDC         hDC;

	// This field MUST be specified!
	if (!lpRequestFont->lpszFaceName ||
		!*lpRequestFont->lpszFaceName)
		return(NULL);

	lpLf = (LPLOGFONT) GlobalAllocPtr(GHND, sizeof(LOGFONT));
	lstrcpy(lpLf->lfFaceName, lpRequestFont->lpszFaceName);

	hDC = CreateIC("DISPLAY", NULL, NULL, NULL);
	lpLf->lfHeight = ((lpRequestFont->nStructSize > offsetof(REQUESTFONTSTRUCT, nPtSize)) ? lpRequestFont->nPtSize : 10)
		* GetDeviceCaps(hDC, LOGPIXELSY) / 72;
	DeleteDC(hDC);

	lpLf->lfWeight = (lpRequestFont->nStructSize > offsetof(REQUESTFONTSTRUCT, nWeight)) ? lpRequestFont->nWeight : 400;
	lpLf->lfItalic = (BYTE) ((lpRequestFont->nStructSize > offsetof(REQUESTFONTSTRUCT, fItalic)) ? lpRequestFont->fItalic : FALSE);
	// Fill in defaults not specified in struct yet.
	lpLf->lfCharSet = DEFAULT_CHARSET;
	lpLf->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lpLf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lpLf->lfQuality = PROOF_QUALITY;
	lpLf->lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;

	hFont = CreateFontIndirect(lpLf);
	GlobalFreePtr(lpLf);
	return(hFont);
	}

