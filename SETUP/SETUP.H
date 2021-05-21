#ifndef		INC_SETUP
#define 	INC_SETUP

#include <windows.h>
#include <windowsx.h>
#include "dialog.h"
#include "setupinf.h"
#include <dde.h>
#include <dos.h>
#include <direct.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "\icsclass\icmeter.h"
#include "\icsclass\iccanvas.h"
#include "\icsclass\icstatic.h"
#include "\icsclass\icbutton.h"

#define	hBMPButts 	(_hGDIObj[5])

#ifndef	RC_INVOKED
extern char _szAppName[];
extern HINSTANCE _hInstance;
extern char _szSrcDir[MAXDIR];
extern char _szDstDir[_MAX_PATH];

// Prototypes for various functions.
BOOL FAR PASCAL RegisterDDEClient (HINSTANCE hInstance);
BOOL FAR PASCAL WelcomeDlgProc (HWND hDlg, WORD wMsg, WORD wParam, LONG lParam);
BOOL FAR PASCAL StatusDlgProc (HWND hDlg, WORD wMsg, WORD wParam, LONG lParam);
BOOL FAR PASCAL InsertDiskDlgProc (HWND hDlg, WORD wMsg, WORD wParam, LONG lParam);
BOOL FAR PASCAL	GroupDlgProc(HWND hDlg, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL	NuGrpDlg(HWND hDlg, WORD message, WORD wParam, LONG lParam);

int FAR cdecl MsgBox (HINSTANCE hInstance, HWND hWnd, WORD wID,
                   LPSTR szCaption, WORD wType, ...);

// *********** Macros for use by Window and Class Extra Bytes *****************

#define offsetof(Struct, Member) \
   ((unsigned int) &(((Struct NEAR *) 0)->Member))

#define GETWNDEB(hWnd, Struct, Member) \
   ((sizeof(((Struct FAR *)0)->Member) == sizeof(DWORD)) ? \
      GetWindowLong(hWnd, offsetof(Struct, Member)) : \
      GetWindowWord(hWnd, offsetof(Struct, Member)))

#define SETWNDEB(hWnd, Struct, Member, Value) \
   ((sizeof(((Struct FAR *)0)->Member) == sizeof(DWORD)) ? \
      SetWindowLong(hWnd, offsetof(Struct, Member), Value) : \
      SetWindowWord(hWnd, offsetof(Struct, Member), (WORD) Value))

#define GETCLSEB(hWnd, Struct, Member) \
   ((sizeof(((Struct FAR *)0)->Member) == sizeof(DWORD)) ? \
      GetClassLong(hWnd, offsetof(Struct, Member)) : \
      GetClassWord(hWnd, offsetof(Struct, Member)))

#define SETCLSEB(hWnd, Struct, Member, Value) \
   ((sizeof(((Struct FAR *)0)->Member) == sizeof(DWORD)) ? \
      SetClassLong(hWnd, offsetof(Struct, Member), Value) : \
      SetClassWord(hWnd, offsetof(Struct, Member), (WORD) Value))
#endif	//	RC_INVOKED

#endif	//	INC_SETUP
