#ifndef _INC_FRAME
#define _INC_FRAME

// Public declarations

BOOL Frame_Initialize(APP* pApp);
void Frame_Terminate(APP* pApp);

HWND Frame_CreateWindow(LPCSTR lpszText, int x, int y, int cx, int cy, HINSTANCE hInst);

// Private declarations

// Window procedure

LRESULT CALLBACK _export Frame_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Default message handler

#define Frame_DefProc   DefWindowProc

// Instance data structure

typedef struct tagFRAME
	{
	HWND hWnd;
	} FRAME;

// Instance data pointer access functions

#if defined(M_I86SM) || defined(M_I86MM)
#define Frame_GetPointer(hWnd)          (FRAME*)GetWindowWord((hWnd), 0)
#define Frame_SetPointer(hWnd, pFrm)    (FRAME*)SetWindowWord((hWnd), 0, (WORD)(pFrm))
#else
#define Frame_GetPointer(hWnd)          (FRAME*)GetWindowLong((hWnd), 0)
#define Frame_SetPointer(hWnd, pFrm)    (FRAME*)SetWindowLong((hWnd), 0, (LONG)(pFrm))
#endif

// Message handler declarations

BOOL Frame_OnCreate(FRAME* pFrm, CREATESTRUCT FAR* lpCreateStruct);
void Frame_OnDestroy(FRAME* pFrm);
void Frame_OnClose(FRAME* pFrm);
void Frame_OnNCDestroy(FRAME* pFrm);
BOOL Frame_OnQueryOpen(FRAME* pFrm);
void Frame_OnSysCommand(FRAME* pFrm, UINT cmd, int x, int y);
void NEAR   GetConfigData(VOID);
BOOL Frame_OnIsInstalled(FRAME* pFrm);

#define WM_ISINSTALLED          (WM_USER)
#define HANDLE_WM_ISINSTALLED(hwnd, wParam, lParam, fn) \
	(LRESULT)(BOOL)(UINT)(fn)(hwnd)

extern  char    _szKey[];

#endif  // !_INC_FRAME
