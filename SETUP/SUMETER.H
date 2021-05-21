#ifndef __INC_SUMETER_H
#define __INC_SUMETER_H

// Definitions for NWS Custom Controls
#include	"n:\nwsdll\nwcustom.h"

#define NWSMeter_GetPartsInJob(hMeter) \
    SendMessage(hMeter, WM_GETPARTSINJOB, 0, 0)

#define NWSMeter_GetPartsComplete(hMeter) \
    SendMessage(hMeter, WM_GETPARTSCOMPLETE, 0, 0)

#define NWSMeter_GetCompleteColor(hMeter) \
    SendMessage(hMeter, WM_GETCOMPLETECOLOR, 0, 0)

#define NWSMeter_GetInCompleteColor(hMeter) \
    SendMessage(hMeter, WM_GETINCOMPLETECOLOR, 0, 0)

#define NWSMeter_GetTextColor(hMeter) \
    SendMessage(hMeter, WM_GETTEXTCOLOR, 0, 0)

#define NWSMeter_GetOrientation(hMeter) \
    SendMessage(hMeter, WM_GETORIENTATION, 0, 0)

#define NWSMeter_GetFont(hMeter) \
    SendMessage(hMeter, WM_NWSGETFONT, 0, 0)

// Common Meter colors
#define GREEN RGB(0, 255, 0)
#define RED   RGB(255, 0, 0)

#ifndef	STRICT
#define STRICT
#endif

#include <windowsx.h>
#include <windows.h>
#include <undocwin.h>

#define NWSMeter_SetPtr	SetPointer
#define NWSMeter_DefProc DefWindowProc

typedef	struct	tagNWSMeter
	{
	// Data members
	HWND		hWnd;
	HGDIOBJ		hFont;        	// Handle to font - not owned by meter!
	COLORREF	crColor;		// Color of text
	COLORREF	crIncColor;		// Color of Incomplete portion of meter
	COLORREF	crComColor;		// Color of Complete portion of meter
	DWORD		dwPartsInJob;
	DWORD		dwPartsComplete;
	BOOL		fHoriz;
	}	NWSMeter, * PNWSMeter, FAR * LPNWSMeter;

BOOL    NWSMeter_OnCreate(PNWSMeter pNWSMeter, LPCREATESTRUCT lpCreateStruct);
BOOL	NWSMeter_OnEraseBkgnd(PNWSMeter pNWSMeter, HDC hDC);
void	NWSMeter_OnPaint(PNWSMeter pNWSMeter);
UINT	NWSMeter_OnGetDlgCode(PNWSMeter pNWSMeter, MSG FAR* lpmsg);
void	NWSMeter_OnSetText(PNWSMeter pNWSMeter, LPCSTR lpszText);

DWORD	NWSMeter_SetPartsInJob(PNWSMeter pNWSMeter, DWORD dwParts);
DWORD	NWSMeter_SetPartsComplete(PNWSMeter pNWSMeter, DWORD dwParts);
COLORREF	NWSMeter_SetCompleteColor(PNWSMeter pNWSMeter, COLORREF rgbColor);
COLORREF	NWSMeter_SetInCompleteColor(PNWSMeter pNWSMeter, COLORREF rgbColor);
COLORREF	NWSMeter_SetTextColor(PNWSMeter pNWSMeter, COLORREF rgbColor);
BOOL	NWSMeter_SetOrientation(PNWSMeter pNWSMeter, BOOL fHoriz);
HFONT	NWSMeter_SetFont(PNWSMeter pNWSMeter, HFONT hFont);

BOOL    PASCAL  NWSSetupMeter_Register(HINSTANCE hInst);

WORD	GetSystemResources (VOID);

LPARAM	CALLBACK	NWSMeter_WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif	//__INC_SUMETER_H

