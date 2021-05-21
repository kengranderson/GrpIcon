#include "Setup.h"
#include <dde.h>
#include <string.h>

char _szClassName[] = "DDEClient";

typedef struct {
   HWND hWndServer;
} WNDEB;

#define DDE_WAITTIME    3000
#define NWSM_DDE_EXECUTE WM_USER
#define NWSM_DDE_REQUEST WM_USER+1

LONG FAR PASCAL DDEClientWndProc (HWND hWnd, WORD wMsg, WORD wParam, LONG lParam)
    {
    static	WORD	wLastSent;
    DWORD dwResult = 0, dwStopTime;
    BOOL fCallDefProc = FALSE;
    HWND hWndServer = (HWND) GETWNDEB(hWnd, WNDEB, hWndServer);
    char szBuf[100]; MSG Msg;

    switch (wMsg)
        {
        case WM_NCCREATE:
            dwResult = DefWindowProc(hWnd, wMsg, wParam, lParam);
            if (dwResult == NULL)
                break;

            wLastSent = WM_DDE_INITIATE;
            SendMessage(HWND_BROADCAST, WM_DDE_INITIATE, (WPARAM) hWnd,
                (LONG) ((LPCREATESTRUCT) lParam)->lpCreateParams);

            if (GETWNDEB(hWnd, WNDEB, hWndServer) != NULL)
                break;

            // A conversation was not able to be established. Attempt to
            // execute the desired application.
            GlobalGetAtomName(LOWORD((LONG) ((LPCREATESTRUCT) lParam)->lpCreateParams),
                szBuf, sizeof(szBuf));
            WinExec(szBuf, SW_RESTORE);

            wLastSent = WM_DDE_INITIATE;
            SendMessage(HWND_BROADCAST, WM_DDE_INITIATE, (WPARAM) hWnd,
                (LONG) ((LPCREATESTRUCT) lParam)->lpCreateParams);

            if (GETWNDEB(hWnd, WNDEB, hWndServer) == NULL)
                DefWindowProc(hWnd, WM_NCDESTROY, wParam, lParam);

            break;

     case WM_DESTROY:
      	wLastSent = WM_DDE_TERMINATE;
         PostMessage(hWndServer, WM_DDE_TERMINATE, (WPARAM) hWnd, 0);
         SETWNDEB(hWnd, WNDEB, hWndServer, NULL);
         // From now on, do not send a WM_DDE_ACK message to the server in
         // response to any messages sent from the Server.

         // Wait for response from the Server.
         dwStopTime = GetTickCount() + DDE_WAITTIME;
         do {
            if (PeekMessage(&Msg, hWnd, WM_DDE_TERMINATE, WM_DDE_TERMINATE,
               PM_REMOVE)) break;

         } while (GetTickCount() < dwStopTime);
         break;

      case WM_DDE_DATA:
         if (hWndServer != (HWND) wParam)
         	{
            // Conversation not initiated with this Server or Server sent
            // after we have terminated the conversation.
            if (HIWORD(lParam) != NULL)
            	{
               	// Data handle is not.  If it were NULL, a link was set
               	// using the WM_DDE_ADVISE message.
               	GlobalFree((HGLOBAL) HIWORD(lParam));
            	}
            GlobalDeleteAtom(LOWORD(lParam));
         	}
         break;

      case NWSM_DDE_EXECUTE:
         // Because this is a Client and NOT a Server, this message was
         // sent to this window from the Setup Application.  The lParam
         // parameter contains the handle of the memory containing the
         // commands to be executed by the Server.

         // Verify that a conversation was started and hasn't been terminated.
         if (hWndServer == NULL) break;

      	wLastSent = WM_DDE_EXECUTE;
       	PostMessage(hWndServer, WM_DDE_EXECUTE, (WPARAM) hWnd, lParam);
         // Wait for response from the Server.
         GetMessage(&Msg, hWnd, WM_DDE_ACK, WM_DDE_ACK);

         // Return whether the command was acknowledged successfully.
         wParam = LOWORD(Msg.lParam);
         dwResult = ((DDEACK *) &wParam)->fAck;
         break;

   	case NWSM_DDE_REQUEST:
        // Because this is a Client and NOT a Server, this message was
        // sent to this window from the Setup Application.

        // Verify that a conversation was started and hasn't been terminated.
       	if (hWndServer == NULL)
        	break;

      	wLastSent = WM_DDE_REQUEST;
        PostMessage(hWndServer, WM_DDE_REQUEST, (WPARAM) hWnd, lParam);
        // Wait for response from the Server.
        dwStopTime = GetTickCount() + DDE_WAITTIME;
        do	{
         	if (PeekMessage(&Msg, hWnd, WM_DDE_ACK, WM_DDE_DATA, PM_REMOVE))
            	{
            	if (Msg.message == WM_DDE_DATA)
            		{
   	     			dwResult = Msg.lParam;
   	     			break;
   	     			}
            	else if (Msg.message == WM_DDE_ACK)
            		DispatchMessage(&Msg);
            	break;
            	}
         	} while (GetTickCount() < dwStopTime);
        break;

      case WM_DDE_TERMINATE:
         if (hWndServer == NULL)
         	break;
         else if (wParam == (WPARAM) hWndServer)
         	{
         	// The Server has terminated the conversation with us.
         	// We must send the WM_DDE_TERMINATE message back to the server.
      		wLastSent = WM_DDE_TERMINATE;
        	PostMessage(hWndServer, WM_DDE_TERMINATE, (WPARAM) hWnd, 0);
         	SETWNDEB(hWnd, WNDEB, hWndServer, NULL);
         	}
         else
         	{
         	// Message came from a refused prospective server. Do nothing.
         	}
         break;

      case WM_DDE_ACK:
         if (hWndServer == NULL)
         	{
            // No conversation initiated, WM_DDE_ACK must be from a
            // potential server that just received my WM_DDE_INITIATE message.
            SETWNDEB(hWnd, WNDEB, hWndServer, wParam);
            break;
         	}

         else if (wParam != (WPARAM) hWndServer)
         	{
	        // WM_DDE_ACK message received from a potential Server but we have
    	    // already established a conversation with another Server.  Tell the
        	// Server that we do not wish to continue our conversation with it.
	      	wLastSent = WM_DDE_TERMINATE;
         	PostMessage((HWND) wParam, WM_DDE_TERMINATE, (WPARAM) hWnd, 0);
         	}
         break;

      default:
         fCallDefProc = TRUE;
      break;
   }

   if (fCallDefProc)
      dwResult = DefWindowProc(hWnd, wMsg, wParam, lParam);

   return(dwResult);
	}


BOOL FAR PASCAL RegisterDDEClient (HINSTANCE hInstance)
	{
   WNDCLASS wc;
   wc.style = 0;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = sizeof(WNDEB);
   wc.lpfnWndProc = DDEClientWndProc;
   wc.hInstance = hInstance;
   wc.hIcon = NULL;
   wc.hCursor = NULL;
   wc.hbrBackground = NULL;
   wc.lpszMenuName = NULL;
   wc.lpszClassName = _szClassName;
   return(RegisterClass(&wc));
	}


// ********** Functions for adding files to the Program Manager **************

BOOL PASCAL CreatePMInfo (HINSTANCE hInstance)
    {
    int nDirIndex, nPMProg, nMaxPMProgs;
    DWORD dwTemp;
    BOOL fOk;
    char szPMGroup[100], szPMGroupFileName[100];
    char szPMProgName[100], szPMProgDesc[100], szPMIconFileName[100];
    char szBuf[100], szBuf2[100], szCmd[100], szProgMan[] = "PROGMAN";
    HWND hWndDDEClient, hWndPM;
    ATOM aApp, aTopic;
    GLOBALHANDLE hMem; LPSTR lpCommand;
    GLOBALHANDLE hGroupNames, hNames;
    LPSTR	lpGNames;
    LPSTR lpGroupNames;
    FARPROC	fpDlg;
    int	nGrp, nFileNum;

    // Initiate a conversation with the Program Manager.
    aApp = GlobalAddAtom(szProgMan);
    aTopic = GlobalAddAtom(szProgMan);
    hWndDDEClient = CreateWindow("DDEClient", "", 0, 0, 0, 0, 0,
        NULL, NULL, hInstance, (LPSTR) MAKELONG(aApp, aTopic));
    GlobalDeleteAtom(aApp);

    if (hWndDDEClient == NULL)
        {
        // Conversation could not be initiated.
	    GlobalDeleteAtom(aTopic);
        return(FALSE);
        }

    // Force the Program Manager to open so that the user can
    // see what group and applications we are adding.

    // Notice that I use the FindWindow function here.  I can not use the
    // window handle of the DDE Server window because the Program Manager
    // could acknowledge our DDE conversation by creating a "DDEServer"
    // window.  Calling the ShowWindow function and using this handle would
    // make the "DDEServer" window visible, this is definitely NOT desirable.
    BringWindowToTop(hWndPM = FindWindow(szProgMan, NULL));
    ShowWindow(hWndPM, SW_NORMAL);

    // Disable the Program Manager so that the user can't work with it
    // while we are doing our stuff.
    EnableWindow(hWndPM, FALSE);

	// Get the name of the Group to create or use.
	hGroupNames = (GLOBALHANDLE) LOWORD(SendMessage(hWndDDEClient, NWSM_DDE_REQUEST, 0, MAKELONG(CF_TEXT, aTopic)));
	lpGroupNames = (LPSTR) ((DDEDATA FAR *) GlobalLock(hGroupNames))->Value;
	// Make our own copy of the buffer.
	hNames = GlobalAlloc(GHND, GlobalSize(hGroupNames));
	lpGNames = GlobalLock(hNames);
	lstrcpy(lpGNames, lpGroupNames);
	// Free old buffer.
	GlobalUnlock(hGroupNames);
	GlobalFree(hGroupNames);

	// Ask user which group to put stuff in.
	fpDlg = MakeProcInstance((FARPROC) GroupDlgProc, hInstance);
	nGrp = DialogBoxParam(hInstance, MAKEINTRESOURCE(GRPDLG), hWndPM, (DLGPROC) fpDlg, (DWORD) lpGNames);
	FreeProcInstance(fpDlg);

    if (!*lpGNames)
    	{
    	// Create the PM Group box.
    	SetupInfoSys(SIM_GETPMGROUP, 0, szPMGroup);
    	SetupInfoSys(SIM_GETPMGROUPFILENAME, 0, szPMGroupFileName);
    	}
    else
    	{
    	lstrcpy(szPMGroup, lpGNames);
    	lstrcpy(szPMGroupFileName, "");
    	}

    wsprintf(szCmd, "[CreateGroup(%s%s%s)]",
        (LPSTR) szPMGroup, (LPSTR) (*szPMGroupFileName == 0 ? "" : ","),
        (LPSTR) szPMGroupFileName);

	GlobalUnlock(hNames);
	GlobalFree(hNames);
    GlobalDeleteAtom(aTopic);

    hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, lstrlen(szCmd) + 1);
    lpCommand = GlobalLock(hMem);
    lstrcpy(lpCommand, szCmd);
    GlobalUnlock(hMem);

    fOk = (BOOL) SendMessage(hWndDDEClient, NWSM_DDE_EXECUTE, 0, MAKELONG(0, hMem));
    GlobalFree(hMem);

    // Add the individual PM files to the Group box.
    nMaxPMProgs = (int) SetupInfoSys(SIM_GETNUMPMPROGS, 0, 0);
    for (nPMProg = 0; fOk && (nPMProg < nMaxPMProgs); nPMProg++)
    	{
        SetupInfoSys(SIM_GETPMPROGDESC, nPMProg, szPMProgDesc);
        nDirIndex = (int) SetupInfoSys(SIM_GETPMPROGNAME, nPMProg, szPMProgName);

		// See if the file is a pathed file or not.
		// Scan the files and look for the filename.
		lstrcpy(szBuf, "");
		lstrcpy(szBuf2, "");
		nFileNum = (int) SetupInfoSys(SIM_FINDFILENUM, 0, szPMProgName);
		if (!SetupInfoSys(SIM_ISFILECOMPRESSED, nFileNum, NULL))	// If file not on path, write the path.
			{
    	    // Calculate the top of the destination directory path.
        	wsprintf(szBuf, "%s%s", (LPSTR) _szDstDir, (LPSTR)
            	((*(_fstrrchr(_szDstDir, '\\') + 1) == 0) ? "" : "\\"));
	        lstrcpy(szBuf2, szBuf);

    	    // Append the sub-directory where the file is and the file's name.
        	SetupInfoSys(SIM_GETDIR, nDirIndex, _fstrchr(szBuf, 0));
	        if (szBuf[lstrlen(szBuf) - 1] != '.')
    	        lstrcat(szBuf, "\\");
        	else
            	szBuf[lstrlen(szBuf) - 1] = 0;
    	    }
        lstrcat(szBuf, szPMProgName);

        // Append the subdir where the icon file is and the icon file's name.
        dwTemp = SetupInfoSys(SIM_GETPMICONINFO, nPMProg, szPMIconFileName);
        SetupInfoSys(SIM_GETDIR, LOWORD(dwTemp), _fstrchr(szBuf2, 0));
        if (szBuf2[lstrlen(szBuf2) - 1] != '.')
            lstrcat(szBuf2, "\\");
        else
            szBuf2[lstrlen(szBuf2) - 1] = 0;
        lstrcat(szBuf2, szPMIconFileName);

        // Add the new file to the already created PM Group.
        wsprintf(szCmd, "[AddItem(%s,%s,%s,%d)]", (LPSTR) szBuf,
            (LPSTR) szPMProgDesc, (LPSTR) szBuf2, HIWORD(dwTemp));
        hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, lstrlen(szCmd) + 1);
        lpCommand = GlobalLock(hMem);
        lstrcpy(lpCommand, szCmd);
        GlobalUnlock(hMem);

        fOk = (BOOL) SendMessage(hWndDDEClient, NWSM_DDE_EXECUTE, 0, MAKELONG(0, hMem));
        GlobalFree(hMem);
        }
    // Terminate the DDE conversation with the Program Manager.
    DestroyWindow(hWndDDEClient);
    EnableWindow(hWndPM, TRUE);
    return(fOk);
    }

BOOL	FAR	PASCAL	GroupDlgProc (HWND hDlg, WORD message, WORD wParam, LONG lParam)
	{
	static	LPSTR	lpStr;

	switch (message)
		{
		case WM_INITDIALOG:
			{
			lpStr = (LPSTR) lParam;
			while (*lpStr)
				{
				*_fstrchr(lpStr, 13) = 0;
				SendDlgItemMessage(hDlg, IDC_GRPLISTBOX, LB_ADDSTRING, 0, (LONG) lpStr);
				lpStr += (lstrlen(lpStr) + 2);
				}
			SendDlgItemMessage(hDlg, IDC_GRPLISTBOX, LB_SETCURSEL, 0, 0L);
			lpStr = (LPSTR) lParam;
			}
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDC_NEWGROUP:
					{
					int	nResult;
					FARPROC	fpDlg = MakeProcInstance((FARPROC) NuGrpDlg, (HINSTANCE) GetWindowWord(hDlg, GWW_HINSTANCE));

					lstrcpy(lpStr, "GrpIcon");
					nResult = DialogBoxParam((HINSTANCE) GetWindowWord(hDlg, GWW_HINSTANCE),
						MAKEINTRESOURCE(NUDLG), hDlg, (DLGPROC) fpDlg, (LONG) lpStr);
					FreeProcInstance(fpDlg);
					if (nResult)
						EndDialog(hDlg, TRUE);
					break;
					}

				case IDOK:
					{
					int	nSel = (int) SendDlgItemMessage(hDlg, IDC_GRPLISTBOX, LB_GETCURSEL, 0, 0L);

					SendDlgItemMessage(hDlg, IDC_GRPLISTBOX, LB_GETTEXT, nSel, (LONG) lpStr);
					EndDialog(hDlg, TRUE);
					break;
					}

				case IDC_GRPLISTBOX:
					if (HIWORD(lParam) == LBN_DBLCLK)
						SendMessage(hDlg, WM_COMMAND, IDOK, 0L);
					break;
				}

		default:
			break;
		}
	return(FALSE);
	}

BOOL	FAR	PASCAL	NuGrpDlg (HWND hDlg, WORD message, WORD wParam, LONG lParam)
	{
	static	LPSTR	lpStr;

	switch (message)
		{
		case WM_INITDIALOG:
			lpStr = (LPSTR) lParam;
			SetDlgItemText(hDlg, IDC_NUGRP, lpStr);
			SendDlgItemMessage(hDlg, IDC_NUGRP, EM_LIMITTEXT, 25, 0L);
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDOK:
					{
					if (!GetDlgItemText(hDlg, IDC_NUGRP, lpStr, 25))
						{
						MessageBox(hDlg,
							"Please enter a name for the new Group\nor click the Cancel button to cancel.",
							"No Group name entered!", MB_OK | MB_ICONEXCLAMATION);
						break;
						}
					EndDialog(hDlg, TRUE);
					break;
					}

				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;
				}
		default:
			break;
		}
	return(FALSE);
	}

