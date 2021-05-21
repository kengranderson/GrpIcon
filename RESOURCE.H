#ifndef _INC_RESOURCE
#define _INC_RESOURCE

#include "help\gihelp.h"

#define IDC_ICONLISTBOX	200
#define IDC_BITMAP		210
#define IDC_RESET		1000
#define	IDC_TILESTUFF	1001
#define	IDC_WIDTH		1002
#define	IDC_HEIGHT		1003
#define	IDC_COLORS		1004
#define IDC_AUTOSTART	103
#define IDC_PREVIEW		104
#define IDC_ICONSIZE	105

// Resource ID definitions

#define IDI_GRP30	1100
#define IDI_GRP31	1101
#define	IDI_XICON	1102

#define	BMP_BACKGROUND	1

// System Menus
#define IDM_SYSMENU			1
#define CMD_HIDE			400

// Help menu
#define CMD_HELPINDEX		900
#define CMD_HELPKEYBOARD	901
#define CMD_HELPCOMMANDS	902
#define CMD_HELPPROCEDURES	903
#define CMD_HELPUSINGHELP	904
#define CMD_HELPABOUT		905

// Just String IDs
#define IDS_EMSDETECTED		0
#define IDS_NOREALMODEEMS	1
#define IDS_APPNAME			2
#define IDS_PROGMAN			3
#define IDS_INI				4
#define IDS_ICONDLGTITLE	5
#define IDS_ICONFILESPECS	6
#define IDS_BMPFILESPECS	7
#define IDS_BMPDLGTITLE		8
#define IDS_ICODIR			9
#define IDS_BMPDIR			10
#define IDS_CONFIG			11
#define IDS_CLRDLGTITLE		12
#define	IDS_DLGAPPNAME		13
#define	IDS_DLGAPPDESC		14
#define	IDS_VERSION			15
#define	IDS_VERSIONTAG		16
#define	IDS_REGNAMETAG		17
#define	IDS_REGNUMTAG       18
#define	IDS_HELPFILE		19
#define	IDS_PREVIEW			20
#define	IDS_ICONSIZE		21
#define	IDS_MENUFONT		22

//	Multi-usage resource IDs
#define	MAX_MENUITEMTEXTLENGTH	40
#define	SHORT_STRING_LENGTH		10
#define ID_PROPERTIES	1000
#define ID_ASSIGNICON	1001
#define ID_SETCOLOR		1002
#define ID_WALLPAPER	1003
#define ID_SEPARATOR	1003	// This is used to insert a separator - update the value when the password stuff is put in.
#define	ID_NOTHING		1004	// Dummy placeholder - see Nag code to understand what this is for.
#define ID_ABOUT		1005
//#define ID_LOCK		1006
#define ID_UNLOCK		1007

#define BMP_HELPBUTN	7
#define BMP_FONTBUTN	6

#define IDC_NORMICON	111
#define IDC_SMALLICON	112
#define IDC_NOICON		113
#define IDC_SINGLETASK	114
#define IDC_MULTITASK	115
#define IDC_CURRENTFONT	116
#define IDC_FONTS		117

#define IDC_LISTBOX			100
#define IDC_RUNCOMBO		300
#define	IDC_ABOUTCANVAS		99

#define	IDD_TASK			1
#define	IDD_OPTIONS			2

#define CMD_ABOUT			800
#define CMD_RESTART			801
#define CMD_EXITWIN			802
#define CMD_OPTIONS			803
#define IDI_SMICONS			804

#endif  // !_INC_RESOURCE
