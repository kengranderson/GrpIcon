#include "Setup.H"
#include "SetupInf.H"

struct
    {
    char szAppName[MAXAPPNAME];
    char szDefDir[MAXDIR];
    WORD wSpaceNeeded;
    char szPMGroupFileName[MAXFILENAME];
    char szPMGroupName[MAXPMDESC];
    WORD wNumDisks, wNumDirs, wNumFiles, wNumPMFiles;
    }
    SetupInfo;

struct
    {
    WORD wNum;
    char szDesc[MAXDISKDESC];
    }
    DiskInfo[MAXNUMDISKS];

struct
    {
    WORD wNum;
    char szDesc[MAXDIRDESC];
    }
    DirInfo[MAXNUMDIRS];

struct
    {
    char szDesc[MAXFILEDESC];
    char szFileName[MAXFILENAME];
    WORD wDirNum, wDiskNum;
    BOOL fCompressed;
    }
    FileInfo[MAXNUMFILES];

PMINFO	PMInfo[MAXPMFILES];


typedef enum
    {
    RS_UNDEFINED,
    RS_APPLICATION,
    RS_DISKS,
    RS_DIRS,
    RS_FILES,
    RS_PMINFO,
    RS_TERMINATE
    }
    READSTATE;


static WORD NEAR PASCAL latoi (LPSTR szString)
    {
    WORD wNum = 0;
    while (*szString >= '0' && *szString <= '9')
        wNum = (wNum * 10) + (*szString++ - '0');
    return(wNum);
    }

LPSTR NEAR PASCAL StripEndBlanks (LPSTR szString)
    {
    LPSTR p = szString, q = szString;

    while (*p == ' ' || *p == '\t')
        p++;
    while (*p) *szString++ =
        *p++;
    *szString-- = 0;
    while (szString >= q && (*szString == ' ' || *szString == '\t'))
        *szString-- = 0;
    return(q);
    }

static int NEAR PASCAL PrepareSetupInfo (LPSTR szInfoFile)
    {
    READSTATE State = RS_UNDEFINED;
    WORD wResult = TRUE;
    char szLine[100];
    LPSTR p, szData;
    HCURSOR hCursor;

    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    // Initialize application global information.
    SetupInfo.szAppName[0] = 0;
    SetupInfo.szDefDir[0] = 0;
    SetupInfo.wSpaceNeeded = 0;
    SetupInfo.szPMGroupFileName[0] = 0;
    SetupInfo.szPMGroupName[0] = 0;
    SetupInfo.wNumDisks = SetupInfo.wNumDirs = 0;
    SetupInfo.wNumFiles = SetupInfo.wNumPMFiles = 0;

    while (State != RS_TERMINATE)
        {
        // Read next line from data buffer.
        p = _fstrchr(szInfoFile, '\r');
        if (p != NULL)
            *p = 0;
        lstrcpy(szLine, szInfoFile);
        if (p != NULL)
            szInfoFile = p + 2;

        // Remove leading white-space.
        StripEndBlanks(szLine);

        // Check if the state has changed.
        if (*szLine == '[')
            {
            State = RS_UNDEFINED;

            if (lstrcmpi(szLine, "[End]") == 0)
                State = RS_TERMINATE;
            else if (lstrcmpi(szLine, "[Application]") == 0)
                State = RS_APPLICATION;
            else if (lstrcmpi(szLine, "[Disks]") == 0)
                State = RS_DISKS;
            else if (lstrcmpi(szLine, "[Dirs]") == 0)
                State = RS_DIRS;
            else if (lstrcmpi(szLine, "[Files]") == 0)
                State = RS_FILES;
            else if (lstrcmpi(szLine, "[PM Info]") == 0)
                State = RS_PMINFO;

            if (State == RS_UNDEFINED)
                {
                // Unrecognized section in Install.INF file.
                }
            continue;
            }

        // Line is part of the current state.
        if (*szLine == ';')
            continue;
        if (*szLine == 0)
            continue;

        if (State == RS_APPLICATION ||  State == RS_DISKS ||  State == RS_DIRS)
            {
            szData = _fstrchr(szLine, '=');
            if (szData != NULL)
                *szData++ = 0;
            }
        else
            szData = szLine;

        StripEndBlanks(szLine);
        if (szData != szLine)
            StripEndBlanks(szData);

        switch (State)
            {
            case RS_APPLICATION:
                if (lstrcmpi(szLine, "AppName") == 0)
                    lstrcpy(SetupInfo.szAppName, szData);

                if (lstrcmpi(szLine, "DefDir") == 0)
                    {
                    if (*szData)
                        lstrcpy(SetupInfo.szDefDir, szData);
                    else
                        GetWindowsDirectory(SetupInfo.szDefDir, MAXDIR);
                    }

                if (lstrcmpi(szLine, "SpaceNeeded") == 0)
                    SetupInfo.wSpaceNeeded = latoi(szData);

                if (lstrcmpi(szLine, "DefPMGroup") == 0)
                    {
                    p = _fstrchr(szData, ',');
                    *p = 0;
                    lstrcpy(SetupInfo.szPMGroupFileName, StripEndBlanks(szData));
                    lstrcpy(SetupInfo.szPMGroupName, StripEndBlanks(p + 1));
                    }
                break;

            case RS_DISKS:
                DiskInfo[SetupInfo.wNumDisks].wNum = latoi(szLine);
                lstrcpy(DiskInfo[SetupInfo.wNumDisks++].szDesc, szData);
                break;

            case RS_DIRS:
                DirInfo[SetupInfo.wNumDirs].wNum = latoi(szLine);
                lstrcpy(DirInfo[SetupInfo.wNumDirs++].szDesc, szData);
                break;

            case RS_FILES:
                p = _fstrchr(szData, ',');
                *p++ = 0;
                lstrcpy(FileInfo[SetupInfo.wNumFiles].szDesc,
                StripEndBlanks(szData));

                szData = p; p = _fstrchr(szData, ',');
                *p++ = 0;
                lstrcpy(FileInfo[SetupInfo.wNumFiles].szFileName,
                StripEndBlanks(szData));

                szData = p; p = _fstrchr(szData, ',');
                *p++ = 0;
                FileInfo[SetupInfo.wNumFiles].wDirNum = latoi(StripEndBlanks(szData));

                szData = p; p = _fstrchr(szData, ',');
                *p++ = 0;
                FileInfo[SetupInfo.wNumFiles].wDiskNum = latoi(StripEndBlanks(szData));

                StripEndBlanks(p);
                FileInfo[SetupInfo.wNumFiles++].fCompressed = (*p == 'Y' || *p == 'y');
                break;


            case RS_PMINFO:
                p = _fstrchr(szData, ',');
                *p++ = 0;
                lstrcpy(PMInfo[SetupInfo.wNumPMFiles].szFileName, StripEndBlanks(szData));
                szData = p;
                p = _fstrchr(szData, ',');
                if (p != NULL)
                    *p++ = 0;
                lstrcpy(PMInfo[SetupInfo.wNumPMFiles].szDesc,StripEndBlanks(szData));

                if (p == NULL)
                    {
                    PMInfo[SetupInfo.wNumPMFiles].szIconFileName[0] = 0;
                    PMInfo[SetupInfo.wNumPMFiles].wIconPos = 0;
                    }
                else
                    {
                    szData = p;
                    p = _fstrchr(szData, ',');
                    if (p != NULL)
                        *p++ = 0;

                    lstrcpy(PMInfo[SetupInfo.wNumPMFiles].szIconFileName,
                    StripEndBlanks(szData));
                    PMInfo[SetupInfo.wNumPMFiles].wIconPos = latoi(StripEndBlanks(p));
                }
                SetupInfo.wNumPMFiles++;
                break;
            }
        }
    SetCursor(hCursor);
    return(wResult);
    }

DWORD FAR PASCAL SetupInfoSys (SETUPINFOMSG Msg, WORD wParam, LPSTR lpBuffer)
    {
    DWORD dwResult = 0;
    GLOBALHANDLE hMem;
    int nFile;
    OFSTRUCT of;
    LPSTR p;
    WORD x;

    switch (Msg)
        {
        case SIM_INITIALIZE:
            // wParam = NU, lpBuffer = pathname.
            dwResult = SIM_INIT_NOERROR;
            hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, MAXSETUPINFOSIZE);
            if (hMem == NULL)
                {
                dwResult = SIM_INIT_NOMEM;
                break;
                }
            nFile = OpenFile(lpBuffer, &of, OF_READ);
            if (nFile == -1)
                {
                dwResult = SIM_INIT_NOFILE;
                break;
                }
            p = GlobalLock(hMem);
            // Put a terminating zero byte at the end of the buffer.
            *(p + _lread(nFile, p, MAXSETUPINFOSIZE)) = 0;
            _lclose(nFile);
            PrepareSetupInfo(p);
            GlobalUnlock(hMem);
            GlobalFree(hMem);
            break;

        case SIM_GETAPPNAME:
            // wParam: NU, lpBuffer: to buffer.
            lstrcpy(lpBuffer, SetupInfo.szAppName);
            break;

        case SIM_GETDEFDIR:
            // wParam: NU, lpBuffer: to buffer.
            lstrcpy(lpBuffer, SetupInfo.szDefDir);
            break;

        case SIM_GETSPACENEEDED:
            // wParam = NU, lpBuffer = NU.
            dwResult = SetupInfo.wSpaceNeeded;
            break;

        case SIM_GETNUMDISKS:
            // wParam: NU, lpBuffer = NU.
            dwResult = SetupInfo.wNumDisks;
            break;

        case SIM_GETDISKDESC:
            // wParam: Disk #, lpBuffer: LPSTR.
            lstrcpy(lpBuffer, DiskInfo[wParam].szDesc);
            break;

        case SIM_FINDDISKNUM:
            // wParam: Dir#, lpBuffer: NU.
            dwResult = SetupInfoSys(SIM_GETNUMDISKS, 0, 0);
            while (dwResult--)
                if (wParam == DiskInfo[(WORD) dwResult].wNum)
                    break;
            break;

        case SIM_GETNUMDIRS:
            // wParam: NU, lpBuffer: NU.
            dwResult = SetupInfo.wNumDirs;
            break;

        case SIM_GETDIR:
            // wParam: Dir#, lpBuffer = to buffer.
            lstrcpy(lpBuffer, DirInfo[wParam].szDesc);
            break;

        case SIM_FINDDIRNUM:
            // wParam: Dir#, lpBuffer: NU.
            dwResult = SetupInfoSys(SIM_GETNUMDIRS, 0, 0);
            while (dwResult--)
                if (wParam == DirInfo[(WORD) dwResult].wNum)
                    break;
            break;

        case SIM_GETNUMFILES:
            // wParam: NU, lpBuffer: NU.
            dwResult = SetupInfo.wNumFiles;
            break;

        case SIM_GETFILEDESC:
            // wParam: File#, lpBuffer: to buffer.
            lstrcpy(lpBuffer, FileInfo[wParam].szDesc);
            break;

        case SIM_GETFILENAME:
            // wParam: File#, lpBuffer: to buffer.
            lstrcpy(lpBuffer, FileInfo[wParam].szFileName);
            break;

        case SIM_GETFILEDISK:
            // wParam: File#, lpBuffer: to buffer 4 DESC. RETURNS #
            dwResult = FileInfo[wParam].wDiskNum;
            x = (WORD) SetupInfoSys(SIM_FINDDISKNUM, (WORD) dwResult, 0);
            lstrcpy(lpBuffer, DiskInfo[x].szDesc);
            break;

        case SIM_GETFILEDIR:
            // wParam: File#, lpBuffer: to buffer. Returns #.
            dwResult = FileInfo[wParam].wDirNum;
            x = (WORD) SetupInfoSys(SIM_FINDDIRNUM, (WORD) dwResult, 0);
            lstrcpy(lpBuffer, DirInfo[x].szDesc);
            break;

        case SIM_ISFILECOMPRESSED:
            // wParam: File#, lpBuffer: NU.
            dwResult =  FileInfo[wParam].fCompressed;
            break;

        case SIM_GETPMGROUP:
            // wParam: NU, lpBuffer: buffer.
            lstrcpy(lpBuffer, SetupInfo.szPMGroupName);
            break;

        case SIM_GETPMGROUPFILENAME:
            // wParam: NU, lpBuffer: buffer.
            lstrcpy(lpBuffer, SetupInfo.szPMGroupFileName);
            break;

        case SIM_GETNUMPMPROGS:
            // wParam: NU, lpBuffer: NU.
            dwResult = SetupInfo.wNumPMFiles;
            break;

		// Modified to ignore the last char in the filenames to match compressed names. -KG
        case SIM_FINDDIRFORFILE:
        	{
           	int	nLen = lstrlen(lpBuffer);

           	if (nLen > 0)
           		{
	            // wParam: NU, lpBuffer: filename.
    	        for (dwResult = 0; dwResult < SetupInfo.wNumFiles; dwResult++)
    	        	{
                	if (!_fstrnicmp(lpBuffer, FileInfo[(WORD) dwResult].szFileName, nLen - 1))
                    	break;
                    }

	            if (dwResult == SetupInfo.wNumFiles)
    	            dwResult = (DWORD) -1;
        	    else
                	dwResult = SetupInfoSys(SIM_FINDDIRNUM, FileInfo[(WORD) dwResult].wDirNum, 0);
				}
			}
            break;

        case SIM_GETPMPROGNAME:
            // wParam: PMProg#, lpBuffer: buffer.
            lstrcpy(lpBuffer, PMInfo[wParam].szFileName);
            // Return directory index.
            dwResult = SetupInfoSys(SIM_FINDDIRFORFILE, 0, lpBuffer);
            break;

        case SIM_GETPMPROGDESC:
            // wParam: PMProg#, lpBuffer: buffer.
            lstrcpy(lpBuffer, PMInfo[wParam].szDesc);
            break;

        case SIM_FINDFILENUM:
        	{
           	int	nLen = lstrlen(lpBuffer);

           	if (nLen > 0)
           		{
	            // wParam: NU, lpBuffer: filename.
    	        for (dwResult = 0; dwResult < SetupInfo.wNumFiles; dwResult++)
					if (!_fstrnicmp(lpBuffer, FileInfo[(WORD) dwResult].szFileName, nLen - 1))
                	    break;
	            if (dwResult == SetupInfo.wNumFiles)
    	            dwResult = (DWORD) -1;
				}
			}
            break;

        case SIM_GETPMICONINFO:
            // wParam: PMProg#, lpBuffer: buffer. Returns icon #.
            if (PMInfo[wParam].szIconFileName[0] == 0)
                {
                lstrcpy(lpBuffer, PMInfo[wParam].szFileName);
                }
            else
                {
                lstrcpy(lpBuffer, PMInfo[wParam].szIconFileName);
                }
            dwResult = MAKELONG((WORD) SetupInfoSys(SIM_FINDDIRFORFILE, 0, lpBuffer),
                PMInfo[wParam].wIconPos);
            break;
            }
    return(dwResult);
    }

