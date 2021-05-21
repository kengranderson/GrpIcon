#include "sumeter.h"
#include "\nwcanvas\nwcanvas.h"

#define BLACK   RGB(0, 0, 0)
#define WHITE   RGB(0xFF, 0xFF, 0xFF)

char    _szNWSMeterClassName[] = "NWSSetupMeter";

BOOL    PASCAL  NWSSetupMeter_Register (HINSTANCE hInst)
	{
	WNDCLASS wc;            // Structure used to register Windows class.

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = NWSMeter_WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(NWSMeter *);
	wc.hInstance     = hInst;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = _szNWSMeterClassName;
	return(RegisterClass(&wc));
	}

char	szNWSCanvasClassName[] = "NWSSetupCanvas";

BOOL    PASCAL  NWSSetupCanvas_Register (HINSTANCE hInst)
	{
	WNDCLASS wc;   			// Structure used to register Windows class.

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = NWSCanvas_WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra	 = sizeof(NWSCanvas *);
	wc.hInstance	 = hInst;
	wc.hIcon		 = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = szNWSCanvasClassName;
	return(RegisterClass(&wc));
	}

