#ifndef	INC_GRPICON
#define	INC_GRPICON

//
// GrpIcon include file
//
#ifndef	STRICT
#define	STRICT
#endif	//	STRICT

#include <windows.h>
#include <windowsx.h>
#include <undocwin.h>

#include "resource.h"
#include "app.h"
#include "frame.h"
#include "progman.h"
#include "commdlgs.h"
#include "pmgroup.h"
#include "..\Icsdll\ICS.h"
#include <commdlg.h>
#include <dlgs.h>

#define	GetString(nID) (LoadString(_App.hInst, (nID), _szString, 40))
#define	GetClassIcon(hWnd)	((HICON) GetClassWord((hWnd), GCW_HICON))
#define	SetClassIcon(hWnd, hIcon)	((HICON) SetClassWord((hWnd), GCW_HICON, (UINT) (hIcon)))
#define	GetClassBrush(hWnd) ((HBRUSH) GetClassWord((hWnd), GCW_HBRBACKGROUND))
#define	SetClassBrush(hWnd, hBrush) ((HBRUSH) SetClassWord((hWnd), GCW_HBRBACKGROUND, (UINT) (hBrush)))

#define	GENERICSTRLENGTH		255
#define	GENERIC_INI_STRLENGTH	20
#define MAXFILENAMELENGTH   	144
#define	ICS_WM_QUERYNEWPALETTE	(WM_USER+1000)
#define	ICONSIZE_NORMAL			0
#define	ICONSIZE_SMALL			1
#define	ICONSIZE_NONE			2

// Default values for startup
#define	PREVIEW_DEFAULT			0
#define	ICONSIZE_DEFAULT		ICONSIZE_NORMAL
#define	MENUFONT_DEFAULT "0,0,0,0,700,1,0,0,0,0,0,2,34,Helv"

#endif	//	INC_GRPICON
