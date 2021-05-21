#include "Setup.H"
#include <lzexpand.h>

BOOL	NEAR PASCAL CreateDstDirTree(HWND);
BOOL	NEAR PASCAL CopyAllFiles(HWND);
BOOL	PASCAL CreatePMInfo(HINSTANCE);
BOOL	NEAR PASCAL UpdateWinIni(void);
LPSTR	lstrstr(LPSTR, LPSTR);
LPSTR	lstrchr(LPSTR, char);
BOOL	InstallInWinIni(void);

#define WasCancelled(hDlg) (!IsWindowEnabled(GetDlgItem(hDlg, IDCANCEL)))

char    _szAppName[] = "GISetup";
HINSTANCE  _hInstance;
char    _szSrcDir[MAXDIR] = "x:\\"; // Where SETUP.EXE was run from.
char    _szDstDir[_MAX_PATH];
char    _szDiskDesc[MAXDISKDESC];
char	_szRunning[] = "GrpIcon\0TaskList\0ICS Icon Lib\0\0";
char	_szMsg[255];

extern	HGDIOBJ	_hGDIObj[];
extern	BITMAP	_bmButtons;
extern	PMINFO	PMInfo[];

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
	{
	int nResult;
	HWND hDlgStatus;
	FARPROC lpfnStatDlgProc, lpfn;
	DWORD dwDiskSpaceNeeded, dwFreeDiskSpace;
	struct diskfree_t DiskFreeSpace;
	char szBuf[100];
	HWND    hWndRunning;
	PSTR	pszRunning = _szRunning;

	// Don't let another instance of this application execute.
	if (hPrevInstance)
		return(0);

	// Prepare the DDE Client window class so that we can use it later.
	if (!RegisterDDEClient(hInstance))
		return(0);

	// Initialize the default source path so that it uses the same drive
	// letter that the SETUP.EXE application was executed from.
	GetModuleFileName(hInstance, _szSrcDir, sizeof(_szSrcDir));
	*(_fstrrchr(_szSrcDir, '\\') + 1) = 0;

	_hInstance = hInstance;

    ICSMeter_Register(hInstance);
	ICSCanvas_Register(hInstance);
    ICSStatic_Register(hInstance);

	// Read the SETUP.INF file into memory.
	wsprintf(szBuf, "%s%sSETUP.INF", (LPSTR) _szSrcDir, (LPSTR)
		((*(_fstrrchr(_szSrcDir, '\\') + 1) == 0) ? "" : "\\"));

	nResult = (SIM_INITIALIZE_ERROR) SetupInfoSys(SIM_INITIALIZE, 0, szBuf);
	if (nResult != SIM_INIT_NOERROR)
		{
		MsgBox(hInstance, NULL,
			(nResult == SIM_INIT_NOMEM) ? IDS_NOMEMORY : IDS_NOSETUPINFOFILE,
			_szAppName, MB_ICONINFORMATION | MB_OK | MB_TASKMODAL, (LPSTR) szBuf);
		return(0);
		}

	// Get the amount of memory (in K) that is needed for the installation.
	dwDiskSpaceNeeded = SetupInfoSys(SIM_GETSPACENEEDED, 0, 0);

	_hGDIObj[5] = (HGDIOBJ) LoadBitmap(hInstance, MAKEINTRESOURCE(BMP_YESNOCANCEL));
	GetBitmap(_hGDIObj[5], &_bmButtons);
	_bmButtons.bmWidth /= 2;
	_bmButtons.bmHeight /= 3;

	// Create the Status dialog box.
	lpfnStatDlgProc = MakeProcInstance((FARPROC) StatusDlgProc, hInstance);
	hDlgStatus = CreateDialog(hInstance, MAKEINTRESOURCE(DLG_STATUS),
		NULL, (DLGPROC) lpfnStatDlgProc);

	do
		{
		// Welcome user to setup program and prompt for destination directory.
		lpfn = MakeProcInstance((FARPROC) WelcomeDlgProc, hInstance);
		nResult = DialogBoxParam(hInstance, MAKEINTRESOURCE(DLG_WELCOME), NULL, (DLGPROC) lpfn, (LPARAM) (UINT) hInstance);
		FreeProcInstance(lpfn);
		if (nResult == IDCANCEL)
			break;

		// check if there is sufficient disk space on the destination drive.
		_dos_getdiskfree(_szDstDir[0] - 'A' + 1, &DiskFreeSpace);
		dwFreeDiskSpace = ((DWORD) DiskFreeSpace.avail_clusters *
			(DWORD) DiskFreeSpace.sectors_per_cluster *
			(DWORD) DiskFreeSpace.bytes_per_sector) / 1024UL;

		if (dwFreeDiskSpace < dwDiskSpaceNeeded)
			{
			MsgBox(hInstance, NULL, IDS_NODISKSPACE, _szAppName,
				MB_OK | MB_ICONINFORMATION | MB_TASKMODAL,
				_szDstDir[0], dwFreeDiskSpace, dwDiskSpaceNeeded);
			continue;
			}

		// Try to create the destination directory tree.
		ShowWindow(hDlgStatus, SW_SHOW);
		UpdateWindow(hDlgStatus);
		nResult = CreateDstDirTree(hDlgStatus);
		ShowWindow(hDlgStatus, SW_HIDE);

		if (nResult == FALSE)
			{
			// If the directory tree cannot be created, force loop to repeat.
			dwFreeDiskSpace = 0;
			}
		}
		while (dwFreeDiskSpace < dwDiskSpaceNeeded);

	if (nResult == IDCANCEL)
		{
		DeleteBitmap(_hGDIObj[5]);
		_hGDIObj[5] = NULL;
		DestroyWindow(hDlgStatus);
		FreeProcInstance(lpfnStatDlgProc);
		return(0);
		}

	// Close GrpIcon or TaskList if they are running.
	while (*pszRunning)
		{
		if (hWndRunning = FindWindow(pszRunning, NULL))
			{
			wsprintf(_szMsg, "Setup has detected an instance of %s already running on your system.\n"
				"Setup will close %s.", (LPSTR) pszRunning, (LPSTR) pszRunning);
			MessageBox(GetFocus(), _szMsg,	"Installing GrpIcon 2.01", MB_OK | MB_ICONEXCLAMATION);
			MessageBeep(MB_ICONEXCLAMATION);
			ShowWindow(hWndRunning, SW_SHOW);
			SendMessage(hWndRunning, WM_CLOSE, 0, 0L);
			}
		pszRunning += (1 + lstrlen(pszRunning));
		}

	// Make the destination directory the current directory.
	chdir(_szDstDir);

	// Try to copy the files.
	ShowWindow(hDlgStatus, SW_SHOW);
	UpdateWindow(hDlgStatus);
	nResult = CopyAllFiles(hDlgStatus);
	ShowWindow(hDlgStatus, SW_HIDE);

	// Cleanup the things that we no longer need.
	DeleteBitmap(_hGDIObj[5]);
	_hGDIObj[5] = NULL;
	DestroyWindow(hDlgStatus);
	FreeProcInstance(lpfnStatDlgProc);

	if (nResult == FALSE)
		{
		// Installation not complete.
		MsgBox(hInstance, NULL, IDS_SETUPNOGOOD, _szAppName,
			MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
		return(0);
		}

	if (MessageBox(NULL, "Would you like Setup to install GrpIcon in the Program Manager?",
		"Installing GrpIcon 2.01", MB_YESNO) == IDYES)
		{
		MsgBox(hInstance, NULL,
			CreatePMInfo(hInstance) ? IDS_PMADDOK : IDS_PMADDNOGOOD,
			_szAppName, MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
		}
	InstallInWinIni();

	MessageBox(NULL, "GrpIcon Setup is complete", "Setup complete!", MB_OK);

	// Display README.TXT.
	if (_szDstDir[lstrlen(_szDstDir) - 1] != 92)
		lstrcat(_szDstDir, "\\");
	lstrcat(_szDstDir, "readme.txt");
	lstrcpy(_szMsg, "notepad ");
	lstrcat(_szMsg, _szDstDir);
	WinExec(_szMsg, SW_SHOWNORMAL);
	return(0);
	}


// ********** Functions for Creating the destination directory tree **********
BOOL NEAR PASCAL CreateDstDirTree (HWND hDlgStatus)
	{
	int nResult, nMaxDirs, nDirNum;
	char szBuf[MAXDIR];
	MSG Msg;

	SetDlgItemText(hDlgStatus, IDC_STATLINE1, "Creating destination directory tree...");
	nMaxDirs = (int) SetupInfoSys(SIM_GETNUMDIRS, 0, 0);
	SendDlgItemMessage(hDlgStatus, IDC_METER, WM_SETPARTSCOMPLETE, 0, 0);
	SendDlgItemMessage(hDlgStatus, IDC_METER, WM_SETPARTSINJOB, 0, nMaxDirs + 1);
	SetDlgItemText(hDlgStatus, IDC_STATLINE2, _szDstDir);

	// Create the destination directory.
	nResult = chdir(_szDstDir);
	if (nResult != 0)
		{
		nResult = mkdir(_szDstDir);
		if (nResult != 0)
			{
			MsgBox(_hInstance, hDlgStatus, IDS_CANTMAKEDIR, _szAppName,
				MB_ICONINFORMATION | MB_OK, (LPSTR) _szDstDir);
			return(FALSE);
			}
		else
			chdir(_szDstDir);
		}
	SendDlgItemMessage(hDlgStatus, IDC_METER, WM_SETPARTSCOMPLETE, 0, 1);

	// Create any subdirectories under the destination directory.
	for (nDirNum = 0; nDirNum < nMaxDirs; nDirNum++)
		{
		// Let some other applications execute.
		while (PeekMessage(&Msg, NULL, NULL, NULL, PM_REMOVE))
			{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
			}

		if (WasCancelled(hDlgStatus))
			{
			nResult = IDCANCEL;
			break;
			}

		wsprintf(szBuf, "%s%s", (LPSTR) _szDstDir,
			(LPSTR) ((*(_fstrrchr(_szDstDir, '\\') + 1) == 0) ? "" : "\\"));
		SetupInfoSys(SIM_GETDIR, nDirNum, _fstrchr(szBuf, 0));
		SetDlgItemText(hDlgStatus, IDC_STATLINE2, szBuf);

		nResult = chdir(szBuf);
		if (nResult != 0)
			{
			nResult = mkdir(szBuf);
			if (nResult != 0)
				{
				MsgBox(_hInstance, hDlgStatus, IDS_CANTMAKEDIR, _szAppName,
					MB_ICONINFORMATION | MB_OK, (LPSTR) szBuf);
				nResult = IDCANCEL;
				break;
				}
			else
				chdir(szBuf);
			}
		nResult = IDOK;
		SendDlgItemMessage(hDlgStatus, IDC_METER, WM_SETPARTSCOMPLETE, 0, nDirNum + 2);
		}
	return(nResult != IDCANCEL);
	}


// ******************* Functions for Copying Files ***************************

typedef enum
	{
	CFE_NOERROR,
	CFE_NOMEMORY,
	CFE_CANTOPENSRC,
	CFE_CANTOPENDST,
	} COPYFILE_ERROR;

COPYFILE_ERROR NEAR PASCAL CopyFile (LPSTR szSrcPath, LPSTR szDstPath)
	{
	static	OFSTRUCT ofStrSrc;
	static	OFSTRUCT ofStrDest;
	int		hSrcFile, hDestFile;
	char	szDestFile[144];
	LPSTR	lp;

	// Open the source file.
	hSrcFile = LZOpenFile(szSrcPath, (LPOFSTRUCT) &ofStrSrc, OF_READ);
	if (hSrcFile < 0)
		return(hSrcFile == LZERROR_GLOBALLOC ? CFE_NOMEMORY : CFE_CANTOPENSRC);

	// Create the destination file.
	// Truncate szDstPath after last backslash
	if (lp = _fstrrchr(szDstPath, 92))
		*++lp = 0;

	GetExpandedName(szSrcPath, szDestFile);
	// Truncate szDstPath after last backslash
	lp = max(szDestFile, 1 + _fstrrchr(szDestFile, 92));

	// Concat szDstPath and szDestFile
	lstrcat(szDstPath, lp);

	hDestFile = LZOpenFile(szDstPath, (LPOFSTRUCT) &ofStrDest, OF_CREATE);
	if (hDestFile < 0)
		{
		LZClose(hSrcFile);
		return(hDestFile == LZERROR_GLOBALLOC ? CFE_NOMEMORY : CFE_CANTOPENDST);
		}

	// Copy the source file to the destination file.
	LZCopy(hSrcFile, hDestFile);
	// Close the files.
	LZClose(hSrcFile);
	LZClose(hDestFile);
	return(CFE_NOERROR);
	}


BOOL NEAR PASCAL CopyAllFiles (HWND hDlgStatus)
	{
	int nMaxFiles, nFileNum, nResult;
	COPYFILE_ERROR CFE;
	char szSrcPath[MAXPATH], szDstPath[MAXPATH], szFileName[MAXFILENAME];
	char szFileDesc[MAXFILEDESC], szDir[MAXDIRDESC];
	MSG Msg;
	FARPROC lpfn;

	HANDLE hDlgRes;

	// Get the handle of the "InsertDisk" dialog box from the EXEcutable file.
	hDlgRes = FindResource(_hInstance, MAKEINTRESOURCE(DLG_INSERTDISK), RT_DIALOG);

	// Get the memory handle of the "InsertDisk" dialog box in memory.
	// The block is already in memory because the dialog box is marked as
	// PRELOAD FIXED.
	hDlgRes = LoadResource(_hInstance, hDlgRes);

	// Force the memory block to be locked down.  This prohibits Windows
	// from discarding the dialog box template from memory.
	LockResource(hDlgRes);

	SetDlgItemText(hDlgStatus, IDC_STATLINE1, "Copying files...");
	nMaxFiles = (int) SetupInfoSys(SIM_GETNUMFILES, 0, 0);
	SendDlgItemMessage(hDlgStatus, IDC_METER, WM_SETPARTSCOMPLETE, 0, 0);
	SendDlgItemMessage(hDlgStatus, IDC_METER, WM_SETPARTSINJOB, 0, nMaxFiles);

	lpfn = MakeProcInstance((FARPROC) InsertDiskDlgProc, _hInstance);
	for (nFileNum = 0; nFileNum < nMaxFiles; nFileNum++)
		{
		SetupInfoSys(SIM_GETFILEDESC, nFileNum, szFileDesc);
		SetupInfoSys(SIM_GETFILENAME, nFileNum, szFileName);
		SetupInfoSys(SIM_GETFILEDIR,  nFileNum, szDir);
		SetupInfoSys(SIM_GETFILEDISK, nFileNum, _szDiskDesc);

		// If the file is COMMDLG.DLL, only copy if running Win30 and there isn't one already.
		if (!lstrcmpi(szFileName, "COMMDLG.DL_"))
			{
			OFSTRUCT	OfStruct;

			if (IsWin31() || OpenFile("COMMDLG.DLL", &OfStruct, OF_EXIST) != HFILE_ERROR)
				continue;
			}
		if (!lstrcmpi(szFileName, "BTTNCUR.DL_"))
			{
			OFSTRUCT	OfStruct;

			if (OpenFile("BTTNCUR.DLL", &OfStruct, OF_EXIST) != HFILE_ERROR)
				continue;
			}

		SetDlgItemText(hDlgStatus, IDC_STATLINE2, szFileDesc);


		// If the 'compressed' flag is NOT set, copy to destdir - else copy to windows dir.
		if (!SetupInfoSys(SIM_ISFILECOMPRESSED, nFileNum, 0))
			wsprintf(szDstPath, "%s%s%s\\%s", (LPSTR) _szDstDir, (LPSTR)
				((*(_fstrrchr(_szDstDir, '\\') + 1) == 0) ? "" : "\\"),
				(LPSTR) szDir, (LPSTR) szFileName);
		else
			{
			GetWindowsDirectory(szDstPath, sizeof(szDstPath));
			if (szDstPath[lstrlen(szDstPath) - 1] != '\\')
				lstrcat(szDstPath, "\\");
			lstrcat(szDstPath, szDir);
			if (szDstPath[lstrlen(szDstPath) - 1] != '\\')
				lstrcat(szDstPath, "\\");
			lstrcat(szDstPath, szFileName);
			}

		do
			{
			// Let other applications execute.
			while (PeekMessage(&Msg, NULL, NULL, NULL, PM_REMOVE))
				{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
				}

			if (WasCancelled(hDlgStatus))
				{
				nResult = IDCANCEL;
				break;
				}

			wsprintf(szSrcPath, "%s%s%s\\%s", (LPSTR) _szSrcDir, (LPSTR)
				((*(_fstrrchr(_szSrcDir, '\\') + 1) == 0) ? "" : "\\"),
				(LPSTR) szDir, (LPSTR) szFileName);

			nResult = IDOK;

			CFE = CopyFile(szSrcPath, szDstPath);
			switch (CFE)
				{
				case CFE_NOERROR:
					nResult = IDOK;
					break;

				case CFE_NOMEMORY:
					nResult = MsgBox(_hInstance, hDlgStatus, IDS_NOMEMORYCOPY,
						_szAppName, MB_ICONINFORMATION | MB_RETRYCANCEL);
					break;

				case CFE_CANTOPENSRC:
					nResult = DialogBoxParam(_hInstance,
						MAKEINTRESOURCE(DLG_INSERTDISK), hDlgStatus, (DLGPROC) lpfn,
						(LPARAM) (LPSTR) _szDiskDesc);

					// Normally, Windows would have discarded the dialog box
					// template from memory after the dialog box had been
					// created.  By forcing the memory block to be locked by the
					// call to LockResource() above, the template will NOT be
					// discarded.  If the template were discarded, the next time
					// this dialog box needed to be created Windows would have
					// to load the template from the EXEcutable file.  However,
					// the SETUP.EXE file is probably not on the diskette that
					// is currently in the drive.  This would cause the program
					// to behave erratically.
					break;

				case CFE_CANTOPENDST:
					nResult = MsgBox(_hInstance, hDlgStatus, IDS_CANTOPENDST,
						_szAppName, MB_ICONINFORMATION | MB_RETRYCANCEL);
					break;
				}

			// Make sure that the user really wants to cancel Setup.
			if (nResult == IDCANCEL)
				{
				nResult = MsgBox(_hInstance, hDlgStatus, IDS_QUERYABORT,
					_szAppName, MB_ICONQUESTION | MB_YESNO);
				if (nResult == IDYES)
					{
					nResult = IDCANCEL;
					break;
					}
				}
			}
			while (nResult != IDCANCEL && CFE != CFE_NOERROR);
		if (nResult == IDCANCEL)
			break;

		SendDlgItemMessage(hDlgStatus, IDC_METER, WM_SETPARTSCOMPLETE, 0, nFileNum + 1);
		}

	// The dialog box template is no longer necessary to keep around so
	// it may be unlocked and removed from memory.
	UnlockResource(hDlgRes);
	FreeResource(hDlgRes);

	FreeProcInstance(lpfn);
	return(nResult != IDCANCEL);
	}


// *********************** Miscellaneous Function ****************************
int FAR cdecl MsgBox (HINSTANCE hInstance, HWND hWnd, WORD wID, LPSTR szCaption, WORD wType, ...)
	{
	char szResString[200], szText[200];
	void FAR *VarArgList = (WORD FAR *) &wType + 1;

	LoadString(hInstance, wID, szResString, sizeof(szResString) - 1);
	wvsprintf(szText, szResString, VarArgList);
	return(MessageBox(hWnd, szText, szCaption, wType));
	}

LPSTR lstrstr (LPSTR lpStr1, LPSTR lpStr2)
	{
	LPSTR   lpEnd, lpStr;

	while (*lpStr1)
		{
		// move lpStr1 forward until we find 1st char of lpStr2
		while (*++lpStr1 != *lpStr2)
			if (!*lpStr1)
				return(NULL);  // didn't find it

		// We found 1st char of lpStr2.
		lpEnd = lpStr1;
		lpStr = lpStr2;
		// move forward until end of lpStr2 or lpStr1 or mismatch
		while (*lpEnd == *lpStr)
			{
			if (!*++lpStr)
				return(lpStr1);    // Found it!
			else if (!*++lpEnd)
				return(NULL);  // didn't find it
			}
		}
	return(NULL);
	}

BOOL InstallInWinIni (void)
	{
	char szWindows[] = "windows";
	char szLoad[] = "load";
	char szRun[] = "run";
	char szLoadLine[255];
	char szGrpIcon[] = "GrpIcon";
	char	szInstalling[] = "Installing GrpIcon 2.01";
	int nDirIndex;
	char szFileName[144];
	char szPMProgName[15];

	nDirIndex = (int) SetupInfoSys(SIM_GETPMPROGNAME, 0, szPMProgName);
	// Calculate the top of the destination directory path.
	wsprintf(szFileName, "%s%s", (LPSTR) _szDstDir, (LPSTR)
		((*(_fstrrchr(_szDstDir, '\\') + 1) == 0) ? "" : "\\"));

	// Append the sub-directory where the file is and the file's name.
	SetupInfoSys(SIM_GETDIR, nDirIndex, _fstrchr(szFileName, 0));
	if (szFileName[lstrlen(szFileName) - 1] != '.')
		lstrcat(szFileName, "\\");
	else
		szFileName[lstrlen(szFileName) - 1] = 0;
	lstrcat(szFileName, szPMProgName);

	GetProfileString(szWindows, szRun, "", szLoadLine, 255);
	if (!lstrstr(szLoadLine, szGrpIcon))
		{
		GetProfileString(szWindows, szLoad, "", szLoadLine, 255);
		if (!lstrstr(szLoadLine, szGrpIcon))
			{
			if (MessageBox(NULL, "Would you like GrpIcon to start whenever Windows starts?",
				szInstalling, MB_YESNO) == IDYES)
				{
				lstrcat(szLoadLine, " ");
				lstrcat(szLoadLine, szFileName);
				WriteProfileString(szWindows, szLoad, szLoadLine);
				}
			}
		}

	// If this is a Site License Setup, write the Licensee Name/Number in the INI File.
#ifdef	SITE_LICENSE
#include	<License.h>
	{
	char	szConfig[] = "Config";
	char	szINI[] = "GRPICON.INI";
	
	WritePrivateProfileString(szConfig, "Name", LICENSEE_NAME, szINI);
	WritePrivateProfileString(szConfig, "Number", LICENSEE_NUMBER, szINI);
	}
#endif	// SITE_LICENSE

	if (MessageBox(NULL, "Would you like to try out GrpIcon right now?",
		szInstalling, MB_YESNO) == IDYES)
		WinExec(szFileName, SW_SHOW);

	return(TRUE);
	}

