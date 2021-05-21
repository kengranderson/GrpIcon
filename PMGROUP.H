#ifndef _INC_PMGROUP
#define _INC_PMGROUP

// Window procedures

LRESULT CALLBACK _export PMGroup_NewWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK _export Progman_NewWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Definition of BITMAPEX - used to encapsulate an HBITMAP and HPALETTE.
typedef struct  tagBITMAPEX
	{
	HBITMAP		hBitmap;
	HPALETTE	hPalette;
	BOOL		fTile;
	}   BITMAPEX, * HBITMAPEX, * PBITMAPEX, FAR * LPBITMAPEX;

// Message handler declarations
BOOL PMGroup_OnCreate(HWND hWnd, CREATESTRUCT FAR* lpCreateStruct);
void PMGroup_OnDestroy(HWND hWnd);
BOOL PMGroup_OnEraseBkgnd(HWND hWnd, HDC hDC);
void PMGroup_OnClose(HWND hWnd);
void PMGroup_OnPaint(HWND hWnd);
void PMGroup_OnSysCommand(HWND hWnd, UINT cmd, int x, int y);
void PMGroup_OnCmdAbout(HWND hWnd);
void PMGroup_OnCmdAssign(HWND hWnd);
void PMGroup_OnCmdWallPaper(HWND hWnd);
void PMGroup_OnCmdColor(HWND hWnd);
HICON PMGroup_OnQueryDragIcon(HWND hWnd);
void PMGroup_OnSetText(HWND hWnd, LPCSTR lpszText);
void PMGroup_OnMeasureItem(HWND hWnd, LPMEASUREITEMSTRUCT lpMis);
void PMGroup_OnDrawItem(HWND hWnd, const DRAWITEMSTRUCT FAR*  lpDis);
void PMGroup_OnPaletteChanged(HWND hWnd, HWND hwndPaletteChange);
BOOL PMGroup_OnQueryNewPalette(HWND hWnd, HWND hWndPM);
void PMGroup_OnMDIActivate(HWND hWnd, BOOL fActive, HWND hwndActivate, HWND hwndDeactivate);
void PMGroup_OnSysColorChange(HWND hwnd);
void PMGroup_NewGroupHandler(HWND hWnd);
void PMGroup_DestroyGroupHandler(HWND hWnd);
BOOL PMGroup_OnICSQueryDropIcon(HWND hWnd);
BOOL PMGroup_OnICSDropIcon(HWND hWnd, UINT nIconNum, LPSTR lpszIconFileName);

// Definition of PMGroup class - not a real class, used for storing class data.
typedef struct  tagPMGROUP
	{
	WNDPROC fpNewWndProc;       // PMGroup subclass proc
	WNDPROC fpOldWndProc;       // Original PMGroup proc
	HICON   hIcon;              // Default PMGroup icon
	HICON   hXIcon;             // Handle to the 'X' icon
	HBRUSH	hbrBackground;		// Handle to class brush
	HBRUSH	hbrOldBackground;	// Handle to class brush - for SYSCOLOR changes
	HBRUSH	hbrSysColor;		// System color index of brush
	int     nObjCount;          // Number of PMGroups
	}   PMGROUP, NEAR * PPMGROUP;

// Struct for passing info to enumeration function
typedef struct  tagENUMDATA
	{
	WNDPROC fpWndProc;          // PMGroup subclass proc
	BOOL    fInstall;           // TRUE if installing, FALSE if uninstalling
	}   ENUMDATA, FAR   *   LPENUMDATA;

// Custom functions/macros
#define SubclassClass(hWndPMGroup, fpWndProc) \
	SetClassLong(hWndPMGroup, GCL_WNDPROC, (DWORD) fpWndProc)
BOOL    PMGroup_SubClassClass(HWND hWndPM, BOOL fInstall);
BOOL    CALLBACK    _export SubclassPMGroup(HWND hWndPMGroup, LPARAM lpEnumData);
BOOL    UpdatePMGroupMenu(HWND hWndPMGroup, BOOL fInstall);
HWND    GetPMGroup(HWND hWndPM);
BOOL    CALLBACK    _export GetPMGroupEnumFunc(HWND hWndPMGroup, LPARAM lpdwData);
void    DrawPMIcon(HWND hWndPMGroup, HICON hIcon);
void 	HandleFocusState(const DRAWITEMSTRUCT FAR*  lpDis);
void	HandleSelectionState(HWND hWnd, const DRAWITEMSTRUCT FAR*  lpdis, HBRUSH * hbr);
HICON   PMGroup_GetIcon(HWND hWndPMGroup);
HICON   PMGroup_SetIcon(HWND hWndPMGroup, HICON hIcon);
HBRUSH	PMGroup_GetBrush(HWND hWndPMGroup);
HBRUSH	PMGroup_SetBrush(HWND hWnd, HBRUSH hBrush);
HBITMAPEX	PMGroup_GetWallPaper(HWND hWnd);
HBITMAPEX	PMGroup_SetWallPaper(HWND hWnd, HBITMAPEX hBitmapEx);
DWORD   PMGroup_OnMenuChar(HWND hWnd, UINT ch, UINT flags, HMENU hMenu);

void	Wait(DWORD dwMilliSeconds);
DWORD   ProcessMenuChar(UINT ch, UINT flags, HMENU hMenu);

extern PMGROUP _PMGroup;

#define KeyString(nID)	(LoadString(_App.hInst, (nID), _szKey, SHORT_STRING_LENGTH))
#define	IconOf(hWnd) ((HICON) GetProp(hWnd, (LPSTR) _szIcon))
#define	BrushOf(hWnd) ((HBRUSH) GetProp(hWnd, (LPSTR) _szBrush))
#define	WallPaperOf(hWnd) ((HBITMAP) GetProp(hWnd, (LPSTR) _szWallPaper))
#define	PaletteOf(hWnd) ((HPALETTE) GetProp(hWnd, (LPSTR) _szPalette))
#define	IsTiled(hWnd) ((BOOL) GetProp(hWnd, (LPSTR) _szTile))

void	RebuildPMGroupMenus(HWND hWnd);
BOOL    CALLBACK    _export RebuildPMGroupMenu(HWND hWndPMGroup, LPARAM lParam);
HMENU	CreateGrpPopupMenu(void);

void	GetFileSpecs(char *szFilter[], HINSTANCE hInst, UINT nID, PSTR pszFileSpecs, UINT nLen);

#endif  // !_INC_PMGROUP

