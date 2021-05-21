	cl /AM /EM /W3 /Ze /D_WINDOWS /G2w /Zp /O /Os /Gs /DNDEBUG -c APP.c
	cl /AM /EM /W3 /Ze /D_WINDOWS /G2w /Zp /O /Os /Gs /DNDEBUG -c FRAME.c
	cl /AM /EM /W3 /Ze /D_WINDOWS /G2w /Zp /O /Os /Gs /DNDEBUG -c PMGROUP.c
	cl /AM /EM /W3 /Ze /D_WINDOWS /G2w /Zp /O /Os /Gs /DNDEBUG -c PROGMAN.c
	cl /AM /EM /W3 /Ze /D_WINDOWS /G2w /Zp /O /Os /Gs /DNDEBUG -c COMMDLGS.c
	link /A:16/ST:5120  @GRPICON.LNK
	rc  -r GRPICON.rc
	rc -t -30 GRPICON.RES GRPICON.EXE
