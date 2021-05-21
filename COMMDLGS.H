#ifndef _INC_COMMDLGS
#define _INC_COMMDLGS

BOOL	WINAPI	_export SetColorHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL    WINAPI	_export AssignIconHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL 	WINAPI	_export	SetWallPaperHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void	CustomizeCommonDialog(HWND hDlg, UINT nID, LPARAM lpCommonStruct);

#endif	//  _INC_COMMDLGS
