#include <windows.h>
#include <windowsx.h>
#include <iconcode.h>
#include <Strings.h>

char    _szDisplay[] = "DISPLAY";

HICON   CALLBACK        __export        IconObject_GetOne (LPSTR szFileName, int nNumIcon, HINSTANCE hInst)
	{
	static  LPICONOBJ IconObj;
    static      HDC             hDC;
    static      HICON   hIcon;

	IconObj = (LPICONOBJ) GlobalAllocPtr(GHND, sizeof(ICONOBJ));
	// Return NULL if no RAM could be allocated.
	if (!IconObj)
		return(NULL);

	// Initialize necessary members.
	hIcon = NULL;
    hDC = CreateDC(_szDisplay, NULL, NULL, NULL);
    IconObj->nColors = 1 << (GetDeviceCaps(hDC, PLANES) * min(8, GetDeviceCaps(hDC, BITSPIXEL)));

    DeleteDC(hDC);
	lstrcpy(IconObj->szFileName, szFileName);       // Copy current dir/file to IconObj.
	IconObj->hInstance = hInst;
	IconObject_GetIconInfo(IconObj);                        // Get icon info from file.
    IconObj->nIconMin = nNumIcon;
    if (IconObj->nErrorCode == ICONOBJ_OK)              // If all is OK
	{
	IconObject_GetIcons(IconObj, nNumIcon, nNumIcon);       // Get nNumIcon from file
	hIcon = IconObj->hIcons[0];
	}
	if (IconObj->lpResTable)
		GlobalFreePtr(IconObj->lpResTable);
	if (IconObj->hIcons)
		GlobalFreePtr(IconObj->hIcons);         // Free icon buffer.
	GlobalFreePtr(IconObj);         // Then free rest of struct.
	return(hIcon);
	}

/********************************************************************************************************
* LPHICON IconObject_ExtractIcons (LPICONOBJ IconObj, LPHICON lphIcon, int nFirstIcon, int nNumIcons)   *
*                                                                                                       *
* Extracts one or more icons from a file and writes handles to an icon buffer.                          *
* Assumes that relevant fields have already been populated by IconObject_GetIconInfo.                                   *
********************************************************************************************************/
LPHICON IconObject_ExtractIcons (LPICONOBJ IconObj, LPHICON lphIcon, int nFirstIcon, int nNumIcons)
	{
    static      UINT    nResID;
    static      int     nPick, nIcon, nCurrentIcon;
    static      HFILE   hFile;
    static      LPRESOURCEDESCRIPTOR ResDirDesc;
    static      LPRESOURCEDESCRIPTOR ResDesc;
    static      ICONFILEHEADER  IconFileHeader;
    static      LPEXEICOHDR             ExeIcoHdr;
    #define     ResDirGroup ((LPRESOURCEGROUP) IconObj->lpResGroup)
    #define     ResDescGroup ((LPRESOURCEGROUP) IconObj->lpResDesc)

    // check parms.
    if (!nNumIcons)
	return(lphIcon);

    hFile = _lopen(IconObj->szFileName, OF_READ);
    ResDirDesc = (LPRESOURCEDESCRIPTOR) (IconObj->lpResGroup + sizeof(RESOURCEGROUP));
    ResDesc = (LPRESOURCEDESCRIPTOR) (IconObj->lpResDesc + sizeof(RESOURCEGROUP));
    for (nCurrentIcon = nFirstIcon; nCurrentIcon < (nFirstIcon + nNumIcons); nCurrentIcon++)
	{
	// Validity check
	if (nCurrentIcon >= (int) ResDirGroup->wResCount)
		break;

	// First, jump to the offset of the resource descriptor for the selected icon
	_llseek(hFile, (LONG) ResDirDesc[nCurrentIcon].wResOffset << (LONG) IconObj->wResShift, 0);
	// Then read in the ICONFILEHEADER struct at this location to see how many DDB images are there
	_lread(hFile, (LPICONFILEHEADER) &IconFileHeader, sizeof(ICONFILEHEADER));
	// Then read in the EXEICOHDRs
	nPick = IconFileHeader.wNumIcons * sizeof(EXEICOHDR);
	_lread(hFile, ExeIcoHdr = (LPEXEICOHDR) GlobalAllocPtr(GHND, nPick), nPick);
	// select best resolution icon in dir
	nPick = IconObject_BestIcon(IconObj, ExeIcoHdr, IconFileHeader.wNumIcons);
	// Use the "offset" to get the reference # of this icon
	nResID = ExeIcoHdr[nPick].wDIBOffset;                   // Have to save it before freeing RAM
		GlobalFreePtr(ExeIcoHdr);

	// In most (if not all) cases, nResID will be the offset in ResDesc.  nResID is 1-based.
	nIcon = nResID - 1;
	if ((nResID <= ResDescGroup->wResCount) && ((ResDesc[nIcon].wResName & 0x7FFF) == nResID))      // We found the icon!
		{
#ifdef DEBUGMSG
char    szDebugMsg[184];

wsprintf(szDebugMsg, "Extracting icon #%02X from file %s\n", nCurrentIcon, IconObj->szFileName);
OutputDebugString(szDebugMsg);
#endif
			*lphIcon++ = ExtractIcon(hFile, ResDesc[nIcon].wResOffset, ResDesc[nIcon].wResLength, IconObj->wResShift, IconObj->hInstance);
			}
	else            // I guess it doesn't always work, we have to go hunting...
		{
		// Locate offset of specified icon from resource descriptor section of resTable
		for (nIcon = 0; nIcon < (int) ResDescGroup->wResCount; nIcon++)
			{
			if ((ResDesc[nIcon].wResName & 0x7FFF) == nResID)       // We found the icon!
			{
#ifdef DEBUGMSG
char    szDebugMsg[184];

wsprintf(szDebugMsg, "Extracting icon #%02X from file %s\n", nCurrentIcon, IconObj->szFileName);
OutputDebugString(szDebugMsg);
#endif
					*lphIcon++ = ExtractIcon(hFile, ResDesc[nIcon].wResOffset, ResDesc[nIcon].wResLength, IconObj->wResShift, IconObj->hInstance);
			break;
			}
		    }
		}
		}
	_lclose(hFile);
	return(lphIcon);
	}

/************************************************************************************************
*       LPICONOBJ       IconObject_New (LPSTR szFileName, HWND hWndListBox)                             *
*                                                                                               *
*       'New' method allocates new ICONOBJ, opens file, reads as many icons in as possible.                     *
*       To be invoked whenever a file list selection changes.                                                                           *
*       Returns NULL if no RAM could be allocated, any other error in nErrorCode.                                       *
************************************************************************************************/
LPICONOBJ       CALLBACK        __export        IconObject_New (LPICONOBJ IconObj, LPSTR szFileName, HWND hWndListBox, HICON hIcoDefault, HINSTANCE hInst)
	{
    static      HDC             hDC;

	// Destroy old IconObject if necessary.
	IconObject_Destroy(IconObj);
	IconObj = (LPICONOBJ) GlobalAllocPtr(GHND, sizeof(ICONOBJ));
#ifdef DEBUGMSG
{
char    szDebugMsg[128];

wsprintf(szDebugMsg, "IconObject %08lX created.\n", IconObj);
OutputDebugString(szDebugMsg);
}
#endif
	// Return NULL if no RAM could be allocated.
	if (!IconObj)
		return(IconObj);

	// Initialize given members.
    hDC = CreateDC(_szDisplay, NULL, NULL, NULL);
    IconObj->nColors = 1 << (GetDeviceCaps(hDC, PLANES) * min(8, GetDeviceCaps(hDC, BITSPIXEL)));
    DeleteDC(hDC);
	IconObj->hWndListBox = hWndListBox;                     // Handle to windows listbox object
	IconObj->hIcoDefault = hIcoDefault;
	IconObj->hInstance = hInst;
	IconObj->nUsedIcon = 0xFFFF;
	lstrcpy(IconObj->szFileName, szFileName);       // Copy current dir/file to IconObj.
	IconObject_GetIconInfo(IconObj);                        // Get icon info from file.
	return(IconObj);
	}


/************************************************************************************************
*       BOOL    IconObject_Destroy (LPICONOBJ   IconObj, HICON hKeepIcon)                           *
*                                                                                               *
*       'Destructor' method deallocates ICONOBJ, destroys all icons in object except hKeepIcon      *
*       (if specified).                                                                             *
*       To be invoked whenever a file list selection changes (before New) and when dlg closed.          *.
*       Returns NULL in all cases.                                                                                                              *
************************************************************************************************/
LPICONOBJ       CALLBACK        __export        IconObject_Destroy (LPICONOBJ IconObj)
	{
	static  int     nIcon;
	static  int     nLastIcon;

	// Return NULL if icon object is NULL.
	if (!IconObj)
		return(IconObj);

	// Free Resource table buffer if it exists.
	if (IconObj->lpResTable)
		{
		GlobalFreePtr(IconObj->lpResTable);
		IconObj->lpResTable = NULL;
		}
	// Destroy the icons.
	if (IconObj->nNumIcons)                                 // If there are any icons
		{
		nLastIcon = min(IconObj->nIconMax + 1, IconObj->nIconMin + IconObj->nNumIcons);
		for (nIcon = IconObj->nIconMin; nIcon < nLastIcon; nIcon++)
			{
			if (IconObj->hIcons &&                                                                          // If the icon exists
				IconObj->hIcons[nIcon] &&                                                               // If the icon exists
				(nIcon != IconObj->nUsedIcon) &&                                                // and it's not the Keep Icon
				(IconObj->hIcons[nIcon] != IconObj->hIcoDefault))               // or the X icon
				{
#ifdef DEBUGMSG
char    szDebugMsg[128];

wsprintf(szDebugMsg, "Destroying (%08lX)->hIcons[%02X] = %04X\n", IconObj, nIcon, IconObj->hIcons[nIcon]);
OutputDebugString(szDebugMsg);
#endif
				DestroyIcon(IconObj->hIcons[nIcon]);    // Destroy it and decrement ctr.
				IconObj->hIcons[nIcon] = NULL;
				}
	    }
	if (IconObj->hIcons)
			GlobalFreePtr(IconObj->hIcons);         // Free icon buffer.
		IconObj->hIcons = NULL;
	}
    IconObj->nNumIcons = 0;
#ifdef DEBUGMSG
{
char    szDebugMsg[128];

wsprintf(szDebugMsg, "Destroying IconObj %08lX\n", IconObj);
OutputDebugString(szDebugMsg);
}
#endif
	GlobalFreePtr(IconObj);         // Then free rest of struct.
	return(NULL);
	}


/************************************************************************************************
*       void    IconObject_Fill (LPICONOBJ      IconObj)                                                *
*                                                                                               *
*       Fills IconObj listbox with icons.                                                           *
************************************************************************************************/
void    CALLBACK        __export        IconObject_Fill (LPICONOBJ      IconObj, int nFirstIcon, int nNumIcons, BOOL fRedraw)
	{
	static  int     nIcon;
	static  int     nLastIcon;

	// Do NULL-type checking.
	if (!IconObj || !IconObj->hWndListBox)
		return;

	// Turn off listbox redraw.
	SetWindowRedraw(IconObj->hWndListBox, FALSE);

	nLastIcon = min(nFirstIcon + nNumIcons, min(IconObj->nIconMax + 1, IconObj->nNumIcons));
	for (nIcon = nFirstIcon; nIcon < nLastIcon; nIcon++)    // Add icons to LB
		ListBox_AddItemData(IconObj->hWndListBox, (int) IconObj->hIcons[nIcon - IconObj->nIconMin]);

	// Turn listbox redraw back on.
	SetWindowRedraw(IconObj->hWndListBox, TRUE);
	if (fRedraw)
		{
		InvalidateRect(IconObj->hWndListBox, NULL, TRUE);
		UpdateWindow(IconObj->hWndListBox);
		}
	return;
	}


// NOTE: This function needs to be updated to extract icons as necessary if they are not in the current buffer.
HICON   CALLBACK        __export        IconObject_IconAt (LPICONOBJ IconObj, int nIcon)
	{
	// Do parameter checking.
	if (!IconObj || !IconObj->hIcons || !IconObj->nNumIcons || (nIcon > IconObj->nNumIcons))
		return(NULL);
	return(IconObj->hIcons[nIcon]);
	}

/************************************************************************************************
*       void    IconObject_GetIcons (LPICONOBJ  IconObj, int nFirstIcon, int nLastIcon)           *
*                                                                                               *
*       Reads icons fron file specified in IconObj.  Modifies the nIconMin/Max members of IconObj       *
*       to the specified limits.  Code is smart enough to avoid redundant reads.                    *
*       Return value indicates total number of icons in buffer, or NULL if an error occurred.           *
************************************************************************************************/
int     CALLBACK        __export        IconObject_GetIcons (LPICONOBJ IconObj, int nFirstIcon, int nLastIcon)
	{
	static  LPHICON hNewIcons;

	// Do parameter checking.
	if (!IconObj || !IconObj->szFileName || !*IconObj->szFileName ||
		(nLastIcon < nFirstIcon) || (IconObj->nNumIcons == 0))
		return(0);

	// Adjust range to represent the union of the existing and requested icons.
	nFirstIcon = min(nFirstIcon, IconObj->nIconMin);
	nLastIcon =  min(IconObj->nNumIcons, max(nLastIcon, IconObj->nIconMax));

	// create a new buffer to hold all icon handles
	hNewIcons = (LPHICON) GlobalAllocPtr(GHND, (nLastIcon - nFirstIcon + 1) * sizeof(HICON));
	if (!hNewIcons)
		{
		IconObj->nErrorCode = ICONOBJ_GLOBALALLOCFAILED;
		return(0);
		}

	// Determine whether it's an ICO or EXE format file.
	if (!IconObj->fNewExe)  // If an ICO...
		{
	    static      HFILE   hFile;
	static  int     nPick;
	static  ICONFILEHEADER  IcoFileHeader;
	    #define     IcoResDesc      ((LPICONRESOURCEDESCRIPTOR) IconObj->lpResTable)

	    // Open file.
	    hFile = _lopen(IconObj->szFileName, OF_READ);
	    _lread(hFile, &IcoFileHeader, sizeof(ICONFILEHEADER));
	// select best resolution icon in dir
	nPick = IconObject_BestIcon(IconObj, (LPEXEICOHDR) IconObj->lpResTable, IcoFileHeader.wNumIcons);
#ifdef DEBUGMSG
{
char    szDebugMsg[144];

wsprintf(szDebugMsg, "Extracting icon from file %s\n", IconObj->szFileName);
OutputDebugString(szDebugMsg);
}
#endif
		*hNewIcons = ExtractIcon(hFile, (UINT) IcoResDesc[nPick].dwIcoOffset, (UINT) IcoResDesc[nPick].dwIcoSize, 0, IconObj->hInstance);
		_lclose(hFile);

		if (IconObj->hIcons)    // If already existing buffer, free it.
			GlobalFreePtr(IconObj->hIcons);
		}
	else    // If it's an EXE format file
		{
		// If there is an already existing buffer, we need to copy the existing icon handles into the new buffer.
		if (IconObj->hIcons)    // If an icon buffer already exists
			{
			static  LPHICON IconBufPtr;             // Pointer to insertion point of icon buffer

			// extract any icons numbered before the existing buffer into the new buffer.
			IconBufPtr = IconObject_ExtractIcons(IconObj, hNewIcons, nFirstIcon, IconObj->nIconMin - nFirstIcon);
			// copy existing buffer into new buffer.
			IconBufPtr = (LPHICON) lcopy(IconBufPtr, IconObj->hIcons, (IconObj->nIconMax - IconObj->nIconMin + 1) * sizeof(HICON));
			// Free old buffer.
			GlobalFreePtr(IconObj->hIcons);
			// extract any icons numbered after the existing buffer into the new buffer.
			if (nLastIcon > IconObj->nIconMax)
				IconObject_ExtractIcons(IconObj, IconBufPtr, IconObj->nIconMax + 1, nLastIcon - IconObj->nIconMax);
//      old version                     IconObject_ExtractIcons(IconObj, IconBufPtr, IconObj->nIconMax + 1, nLastIcon - IconObj->nIconMax - 1);
		}
	else                                    // (!IconObj->hIcons) No previous buffer
			IconObject_ExtractIcons(IconObj, hNewIcons, nFirstIcon, nLastIcon - nFirstIcon + 1);
		}

    // Set IconObj buffer to point to new buffer.
	IconObj->hIcons = hNewIcons;
	IconObj->nIconMin = nFirstIcon;
	IconObj->nIconMax = nLastIcon;
	return(nLastIcon - nFirstIcon + 1);
	}

/********************************************************************************************************
* void  IconObject_GetIconInfo (LPICONOBJ IconObj)                                                      *
*                                                                                                       *
* Gets icon information from a file and writes data to IconObj.                                                         *
* The DOS file name is taken from IconObj, and file is opened.  The file type is determined and saved   *
* in IconObj.  The resource directory or table is read in and a pointer to it is stored in IconObj.             *
********************************************************************************************************/
void    IconObject_GetIconInfo (LPICONOBJ IconObj)
	{
	static  OFSTRUCT        OfStruct;
	static  int                     nResult;
    static      char            szStartOfFile[70];      // Start of file
    static      HFILE           hFile;
//    static    LPICONFILEHEADER        lpIconHeader;
    static      DWORD           dwNewExeOfs;
    static      UINT            nResSize;
    static      LPSTR           lp;
	#define ResGroup        ((LPRESOURCEGROUP) lp)
	#define ResDesc         ((LPRESOURCEDESCRIPTOR) lp)
	#define lpOldExe        ((LPOLDEXEHDR) szStartOfFile)
	#define lpNewExe        ((LPNEWEXEHDR) szStartOfFile)
	#define lpIconHeader    ((LPICONFILEHEADER) szStartOfFile)

	// First, make sure the specified file exists
	hFile = OpenFile(IconObj->szFileName, &OfStruct, OF_READ);
	if (hFile == -1)                // File could not be found!
		{
		IconObj->nErrorCode = ICONOBJ_FILEOPENFAILED;
		IconObj->nNumIcons = 0;
		return;
		}

	// File exists, update szFileName to entire DOS path.
	lstrcpy(IconObj->szFileName, OfStruct.szPathName);

	// Now determine the file type.  Assume EXE, read in what would be the EXE header,
	// If it doesn't check out as an EXE, try to id it as an ICO.  If not, fail.
    nResult = _lread(hFile, (LPSTR) szStartOfFile, 70);
    if (nResult < 70)           // We reached an EOF - it's too small to be any icon file
	{
		IconObj->nErrorCode = ICONOBJ_NOTICONFILE;
		IconObj->nNumIcons = 0;
		_lclose(hFile);
	return;
	}
	// See if it is an EXE, ICO or punt.
	if ((lpIconHeader->wType == 1) && !lpIconHeader->wZero)
		{
	// It's an ICO format file
	IconObj->fNewExe = FALSE;
		// Determine how many icons are in the file.
		nResult = lpIconHeader->wNumIcons;
		if (nResult < 1 || nResult > 4)         // Not really an icon file!
			{
			IconObj->nNumIcons = 0;
			IconObj->nErrorCode = ICONOBJ_NOTICONFILE;
			_lclose(hFile);
			return;
			}
		// Checks out as icon file so far
		IconObj->nNumIcons = 1;                         // ICOs only hold 1!
		nResult *= sizeof(ICONRESOURCEDESCRIPTOR);      // Now nResult is the size of the ResDir.
		// Allocate buffer for Resource directory.
		IconObj->lpResTable = GlobalAllocPtr(GHND, nResult);
		if (!IconObj->lpResTable)
			{
			IconObj->nNumIcons = 0;
			IconObj->nErrorCode = ICONOBJ_GLOBALALLOCFAILED;
			_lclose(hFile);
			return;
			}
		// Read Resource directory and save location.
		lcopy(IconObj->lpResTable, szStartOfFile + sizeof(ICONFILEHEADER), nResult);
		IconObj->dwResTable = (DWORD) sizeof(ICONFILEHEADER);
	}
    else
	{
	    lpOldExe->wReserved2[1] = 0;                        // Zero termination byte in case it's a PKLITE file.
	if ((lpOldExe->wMagic != OLDEXE_MAGIC) ||                                       // If no Exe Header signature
		(!lstrcmp((LPBYTE) lpOldExe->wReserved1, "PKLITE")) ||  // If PKLITE file
// for stupid .NIL files (lpOldExe->wRelocOfs < 0x40) ||                                                        // or not NewEXE or
		(!lpOldExe->dwNewExeOfs))                                                               // No New EXE Header
		{
			IconObj->nErrorCode = ICONOBJ_NOTNEWEXE;
			IconObj->nNumIcons = 0;
		_lclose(hFile);
		return;
		}

	IconObj->fNewExe = TRUE;
		// Jump to locn of NewExeHdr
		if (_llseek(hFile, dwNewExeOfs = lpOldExe->dwNewExeOfs, 0) == -1)
		{
			IconObj->nErrorCode = ICONOBJ_NOTNEWEXE;
			IconObj->nNumIcons = 0;
			_lclose(hFile);
			return;
			}

		// Read new EXE header into struct
		_lread(hFile, (LPSTR) lpNewExe, sizeof(NEWEXEHDR));
		if ((lpNewExe->wMagic != NEWEXE_MAGIC) ||                                       // If not New EXE header or
			!(lpNewExe->wResidentOfs - lpNewExe->wResourceOfs))     // no resource table
		{
			IconObj->nErrorCode = ICONOBJ_NOTNEWEXE;
			IconObj->nNumIcons = 0;
			_lclose(hFile);
			return;
			}

		// Jump to locn of Resource Table
		_llseek(hFile, IconObj->dwResTable = dwNewExeOfs + lpNewExe->wResourceOfs, 0);
	// Allocate buffer and read in entire resource table.
	nResSize = (UINT) lpNewExe->wResidentOfs - lpNewExe->wResourceOfs;
		IconObj->lpResTable = GlobalAllocPtr(GHND, nResSize + 1);
		if (!IconObj->lpResTable)
			{
			IconObj->nNumIcons = 0;
			IconObj->nErrorCode = ICONOBJ_GLOBALALLOCFAILED;
			_lclose(hFile);
			return;
			}

		// Read Resource directory and save location.
		_lread(hFile, IconObj->lpResTable, nResSize);

		// Save resource shift count
		IconObj->wResShift = *(UINT FAR *) IconObj->lpResTable;
		// Increment table ptr to real start of table.
		IconObj->lpResTable += 2;
		lp = IconObj->lpResTable;

		// Count icon dirs and icons
		while (ResGroup->wResType)
		{
		if (ResGroup->wResType == RES_ICODIR)                       // found ICO dirs
			{
			IconObj->lpResGroup = lp;                                                               // Save ptr to IcoDir entries
				IconObj->nNumIcons = ResGroup->wResCount;               // Save # of IcoDir entries
				if (IconObj->lpResDesc)                                                                 // we are done if the other one is set
					break;
			}
		else if (ResGroup->wResType == RES_ICON)                        // found icons
			{
			IconObj->lpResDesc = lp;                                                                // Save ptr to Icon entries
				if (IconObj->lpResGroup)                                                                // we are done if the other one is set
					break;
			}
			lp += (sizeof(RESOURCEGROUP) + ResGroup->wResCount * sizeof(RESOURCEDESCRIPTOR));
		}
		}
	_lclose(hFile);
	IconObj->nErrorCode = (IconObj->nNumIcons > 0) ? ICONOBJ_OK : ICONOBJ_NOICONSINFILE;
	return;
	}


int IconObject_BestIcon (LPICONOBJ IconObj, LPEXEICOHDR lpExeIcoHdr, int wNumImages)
    {
    static      int     nIcon, nColors, nBestColors, nBest;
    static      UINT    nOffset, nStructSize;

    nBestColors = 0;
    nBest = 0;
    nStructSize = IconObj->fNewExe ? sizeof(EXEICOHDR) : sizeof(ICONRESOURCEDESCRIPTOR);
    for (nIcon = 0; nIcon < wNumImages; nIcon++)   // cycle thru icon images
	{
	    nOffset = nIcon * nStructSize;
		nColors = (int) ((LPEXEICOHDR) ((LPSTR) lpExeIcoHdr + nOffset))->bIcoColors;
	if ((int) IconObj->nColors == nColors)   // if exact match bet icon and display
	    {
	    nBest = nIcon;                                          // pick this icon, break out
	    break;
	    }
	else if ((int) IconObj->nColors > nColors)    // if disp better than icon
	    {                                                       // then if this icon is better than current pick
	    if (nColors > nBestColors)
		{
		nBest = nIcon;                                      // change pick to this icon
		nBestColors = nColors;
		}
	    }
	}
    return(nBest);
    }

HICON ExtractIcon (HFILE hFile, UINT wResOffset, UINT wResSize, UINT wResShift, HINSTANCE hInst)
    {
    static      int                 nIcoX;
    static      int                 nIcoY;
    static      int                 nDIBSize;               // size of DIB mask
    static      LPSTR               DIBBuffer;                  // lp to DIB bits
    static      int                 nBMPSize;               // size of BMP mask
    static      LPSTR               BMPBuffer;                  // lp to BMP bits
    static      HDC                 hDCDisp;                // hDC for System Display
    static      HDC                 hDCMem;                 // Memory hDC for creating bitmaps
    static      HDC                 hDCMemWeird;            // Memory hDC for creating off-size bitmaps
    static      HBITMAP             hBitmap;                // Generic work bitmap handle
    static      HBITMAP             hDefBitmap;             // dummy bitmap handle for SelectObject()
    static      BITMAP              Bitmap;                 // Generic work bitmap
    static      int                 nTemp;                  // Generic work int
    static      int                 nStrMode;               // StretchMode for weird sizes
    static      HICON               hIcon;                  // Icon handle to return to caller
    static      BOOL                fNormSize;              // Icon Size OK?
	static  ICONOFFSETS                     Offsets;
	static  LPBITMAPINFOHEADER      lpBmih;
    static      int                 nColors;                // # colors in icon
    static      UINT                            nResSize;
    static      UINT                            nAttempts;

    nIcoX = GetSystemMetrics(SM_CXICON);
    nIcoY = GetSystemMetrics(SM_CYICON);
    for (nAttempts = 0; nAttempts < 2; nAttempts++)     // In case of bogus icon header data
	{
	// Jump to locn of bmiHeader of icon
	if (_llseek(hFile, (LONG) wResOffset << (LONG) wResShift, 0) == -1)
		return((HICON) NULL);

	// Allocate buffer for icon and read entire icon into it.
	Offsets.lpBuf = GlobalAllocPtr(GHND, nResSize = (UINT) wResSize << wResShift);
	if (_lread(hFile, Offsets.lpBuf, nResSize) < nResSize)
		{
		GlobalFreePtr(Offsets.lpBuf);
		return((HICON) NULL);
		}
	// A lot of stuff is referenced from this point, so this makes the code more readable.
		lpBmih = (LPBITMAPINFOHEADER) Offsets.lpBuf;

	lpBmih->biHeight /= 2;

		nBMPSize = (int) (lpBmih->biWidth * lpBmih->biHeight / 8L);
	nDIBSize = (int) (lpBmih->biBitCount * lpBmih->biPlanes * (DWORD) nBMPSize);
	nColors = 1 << (lpBmih->biPlanes * lpBmih->biBitCount);

		// Get the offsets of the various components of the icon.
		Offsets.XORMask = Offsets.lpBuf + lpBmih->biSize + (nColors * sizeof(RGBQUAD));
		Offsets.ANDMask = Offsets.XORMask + nDIBSize;

		// Check for bogus icon header data resulting in a buffer too small to fit the data.
		if (nResSize < (UINT) OFFSETOF(Offsets.ANDMask) + nBMPSize)
			{
			// buffer too small - increase res size and try again.
		wResSize = (UINT) OFFSETOF(Offsets.ANDMask) + nBMPSize;
		GlobalFreePtr(Offsets.lpBuf);
		}
	    else
		break;                  // buffer OK, proceed.
		}
	if (nAttempts >= 2)             // if we got here cuz we couldn't allocate RAM, die.
		return((HICON) NULL);

    hDCDisp = CreateDC(_szDisplay, NULL, NULL, NULL);

    // Ready to process XOR, AND masks
    // Check for non-standard icon sizes (like 64x64)
    fNormSize = (BOOL) ((lpBmih->biWidth == nIcoX) && (lpBmih->biHeight == nIcoY));

    // Ready to process AND mask (have to do AND before XOR cuz I use Bitmap later...)
    // For some bizarre reason, the BMP is upside-down, so we have to flipit
    hDCMem = CreateCompatibleDC(hDCDisp);
    hBitmap = CreateBitmap(nIcoX, nIcoY, 1, 1, fNormSize ? Offsets.ANDMask : NULL);
    hDefBitmap = SelectObject(hDCMem, hBitmap);
    if (!fNormSize)
	{
	hDCMemWeird = CreateCompatibleDC(hDCDisp);
	SelectObject(hDCMemWeird, CreateBitmap((int) lpBmih->biWidth, (int) lpBmih->biHeight, 1, 1, Offsets.ANDMask));
	}

    StretchBlt(hDCMem, 0, nIcoY - 1, nIcoX, -nIcoY,
	fNormSize ? hDCMem : hDCMemWeird, 0, 0, (int) lpBmih->biWidth, (int) lpBmih->biHeight,
	SRCCOPY);

    GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &Bitmap);
    nTemp = (int)
	((LONG) Bitmap.bmWidth  * (LONG) Bitmap.bmHeight *
	 (LONG) Bitmap.bmPlanes * (LONG) Bitmap.bmBitsPixel / 8L);
    // Have to allocate new buffer only for off-size/color bitmap
    BMPBuffer = (nTemp != nBMPSize) ? GlobalAllocPtr(GHND, nTemp) : Offsets.ANDMask;
    // this copies a device-dependent form of the bits to BMPBuffer
    GetBitmapBits(hBitmap, (DWORD) nTemp, (LPSTR) BMPBuffer);

    if (!fNormSize)
	DeleteObject(SelectObject(hDCMemWeird, hDefBitmap));    // Delete off-size hBitmap
    DeleteObject(SelectObject(hDCMem, hDefBitmap));     // Delete hBitmap
    DeleteDC(fNormSize ? hDCMem : hDCMemWeird);

    // Ready to process XOR mask
    if (!fNormSize)
	{
	hDCMemWeird = CreateCompatibleDC(hDCDisp);
	SelectObject(hDCMemWeird, CreateDIBitmap(hDCDisp, lpBmih,
	    CBM_INIT, Offsets.XORMask, (LPBITMAPINFO) lpBmih, DIB_RGB_COLORS));

	hBitmap = CreateCompatibleBitmap(hDCMemWeird, nIcoX, nIcoY);
	SelectObject(hDCMem, hBitmap);

	nStrMode = SetStretchBltMode(hDCMem,
	    (1 << (lpBmih->biPlanes * lpBmih->biBitCount)) > 2 ? COLORONCOLOR : WHITEONBLACK);
	StretchBlt(hDCMem, 0, nIcoY - 1, nIcoX, -nIcoY, hDCMemWeird, 0, 0,
	    (int) lpBmih->biWidth, (int) lpBmih->biHeight, SRCCOPY);
	StretchBlt(hDCMem, 0, nIcoY - 1, nIcoX, -nIcoY, hDCMem, 0, 0,
	    nIcoX, nIcoY, SRCCOPY);
	SetStretchBltMode(hDCMem, nStrMode);

	DeleteObject(SelectObject(hDCMemWeird, hDefBitmap));
	DeleteDC(hDCMemWeird);
	}
    else
	hBitmap = CreateDIBitmap(hDCDisp, lpBmih,
	    CBM_INIT, Offsets.XORMask, (LPBITMAPINFO) lpBmih, DIB_RGB_COLORS);

    GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &Bitmap);
    nTemp = (int)
	((LONG) Bitmap.bmWidth  * (LONG) Bitmap.bmHeight *
	 (LONG) Bitmap.bmPlanes * (LONG) Bitmap.bmBitsPixel / 8L);
    DIBBuffer = (nTemp != nDIBSize) ? GlobalAllocPtr(GHND, nTemp) : Offsets.XORMask;

    // this copies a device-dependent form of the bits to lpDIBBuffer
    GetBitmapBits(hBitmap, (DWORD) nTemp, (LPSTR) DIBBuffer);
    if (!fNormSize)
	{
	SelectObject(hDCMem, hDefBitmap);
	DeleteDC(hDCMem);
	}
    DeleteObject(hBitmap);

    hIcon = CreateIcon(hInst, (int) Bitmap.bmWidth, (int) Bitmap.bmHeight,
	(BYTE) Bitmap.bmPlanes, (BYTE) Bitmap.bmBitsPixel, BMPBuffer, DIBBuffer);

    if (BMPBuffer != Offsets.ANDMask)   // Free only if it was a newly-allocated buffer
	GlobalFreePtr(BMPBuffer);
    if (DIBBuffer != Offsets.XORMask)
	GlobalFreePtr(DIBBuffer);

    DeleteDC(hDCDisp);
	GlobalFreePtr(Offsets.lpBuf);           // Frees buffer for icon allocated earlier
#ifdef DEBUGMSG
{
char    szDebugMsg[128];

wsprintf(szDebugMsg, "Creating Icon %04X\n", hIcon);
OutputDebugString(szDebugMsg);
}
#endif
	return(hIcon);
    }

