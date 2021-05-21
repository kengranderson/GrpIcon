#ifndef _INC_APP
#define _INC_APP

// Public declarations

int PASCAL	WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lpszCmdLine, int nCmdShow);
int			ErrorMsg(HINSTANCE, int, int, WORD);

// Private declarations

typedef struct tagOPTIONS
	{
	HFONT		hFont;			// Font for menu items
	LOGFONT		lf;				// LogFont for same
	UINT		nIconState;		// Icons in menus setting 0 = Norm, 1 = Small, 2 = None
	BOOL		fStartup;		// Run on Windows Startup?
	BOOL		fPreview;		// Icon Preview?
	}	OPTIONS, * POPTIONS, FAR * LPOPTIONS;

typedef struct tagAPP
	{
    // Standard data members
    MSG         msg;
    HINSTANCE   hInst;
    HINSTANCE   hInstPrev;
    LPSTR       lpszCmdLine;
    int         nCmdShow;
    HWND        hWndFrame;
	// Custom data members
    HINSTANCE   hInstLib;		// NWS.DLL handle
	char		szINI[13];		// INI file
	BOOL		fInstalled;		// Has new wndproc been installed yet?
	BOOL		fPirated;		// Has program been modified?
	DWORD		dwLastNag;		// Time of last nag event
	HBRUSH		hbrDlgBkGnd;	// Dialog background brush
	HMENU		hGrpPopup;		// handle to popup menu for system menu
	SIZE		sizeIcon;		// Normal Icon dimensions
	SIZE		sizeicon;		// Small Icon dimensions
	HBRUSH		hbr[2];			// Owner-draw item background brushes
	WNDPROC 	fpNewWndProc;   // Progman subclass proc
	WNDPROC 	fpOldWndProc;   // Original Progman proc
	OPTIONS		options;		// User-selectable options
	} APP;

BOOL App_Initialize(APP* pApp);
int  App_Main(APP* pApp);
VOID App_Terminate(APP* pApp, BOOL fEndSession);

void	CALLBACK	Nothing(void);
void	CALLBACK	KillApp(void);

// Public globals

extern APP _App;
#include "..\ICsdll\ICS.h"		// Ths is so the following struct is declared
extern	LPICSREGISTERAPPSTRUCT	_lpICSras;
extern	DWORD	_dwLastNag;
extern	FARPROC	_pFunk[];


#endif  // !_INC_APP
