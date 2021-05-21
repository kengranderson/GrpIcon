#ifndef		INC_DIALOG
#define		INC_DIALOG

//#define IDC_FRAME   1010
#define DLG_WELCOME		1
#define DLG_INSERTDISK	2
#define DLG_STATUS      3
#define GRPDLG			4
#define NUDLG			5

#define BMP_BACKGROUND	1
#define BMP_GLOBE		2
#define	BMP_YESNOCANCEL	3

#define ICN_SETUP           10
#define ICN_DISK            11

#define IDC_METER		100
#define IDC_DISKNAME    101

#define IDC_LOGOCANVAS	101
#define IDC_LOGOBMP		102
#define IDC_PRESENTS	103
#define IDC_WCKCANVAS	104
#define IDC_WCK1		105
#define IDC_WCK2		106
#define IDI_WCK			107
#define IDC_APPCANVAS	108
#define IDC_MODULE		109
#define IDC_MODNAME		110
#define IDI_MODICON		111
#define IDC_DIRCANVAS	112
#define IDC_INSTRUCTIONS	113
#define IDC_COPYTO		114
#define IDC_DESTPATH	115
#define IDC_COPYRIGHT	116

#define IDC_STATLINE1	101
#define IDC_STATLINE2	102
#define IDC_SRCPATH		103

#define IDC_GRPLISTBOX	101
#define IDC_NUGRP		101
#define IDC_NEWGROUP	103

// Defines for use with SETUP's string table.
#define IDS_NOMEMORY          1001
#define IDS_NOSETUPINFOFILE   1002
#define IDS_NODISKSPACE       1003
#define IDS_CANTMAKEDIR       1004
#define IDS_QUERYABORT        1005
#define IDS_SETUPNOGOOD       1006
#define IDS_SETUPOKNOPM       1007
#define IDS_SETUPDONE         1008
#define IDS_PMADDOK           1009
#define IDS_PMADDNOGOOD       1010
#define IDS_NOMEMORYCOPY      1011
#define IDS_CANTOPENDST       1012
#define IDS_NOATMINI          1013

typedef	struct	tagREQUESTFONTSTRUCT
	{
	UINT	nStructSize;	// Size of struct, in bytes
	LPSTR	lpszFaceName;	// lp to requested font name
	UINT	nPtSize;		// Requested point size
	BOOL	fItalic;		// Italic flag
	UINT	nWeight;		// Font weight
	} REQUESTFONTSTRUCT, FAR * LPREQUESTFONTSTRUCT;

HFONT	RequestFont(LPREQUESTFONTSTRUCT lpRequestFont);
void AssignYNCBitmap(HWND hWndBut, HBITMAP hBitmap, int nColumn);

/*
#define IDABORT             3
#define IDCANCEL            2
#define IDIGNORE            5
#define IDNO                7
#define IDOK                1
#define IDRETRY             4
#define IDYES               6
*/
#endif	//	INC_DIALOG
