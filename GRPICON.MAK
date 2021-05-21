# Microsoft Visual C++ generated build script - Do not modify

PROJ = GRPICON
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = I:\GRPICON\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = APP.C       
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AMw /YX /Od /D "_DEBUG" /FR /GA /Fd"GRPICON.PDB"    /Fp"GRPICON.PCH"
CFLAGS_R_WEXE = /nologo /G2 /W3 /AMw /YX /O1 /D "NDEBUG" /FR /GA /Fp"GRPICON.PCH"
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw mlibcew commdlg.lib 
LIBS_R_WEXE = oldnames libw mlibcew commdlg.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = GRPICON.DEF
OBJS_EXT = 
LIBS_EXT = ..\ICSDLL\ICS.LIB 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = APP.SBR \
		COMMDLGS.SBR \
		FRAME.SBR \
		PMGROUP.SBR \
		PROGMAN.SBR


ICS_DEP = 

APP_DEP = i:\grpicon\grpicon.h \
	c:\msvc\include\undocwin.h \
	i:\grpicon\resource.h \
	help\gihelp.h \
	i:\grpicon\app.h \
	..\icsdll\ics.h \
	i:\grpicon\frame.h \
	i:\grpicon\progman.h \
	i:\grpicon\commdlgs.h \
	i:\grpicon\pmgroup.h


COMMDLGS_DEP = i:\grpicon\grpicon.h \
	c:\msvc\include\undocwin.h \
	i:\grpicon\resource.h \
	help\gihelp.h \
	i:\grpicon\app.h \
	..\icsdll\ics.h \
	i:\grpicon\frame.h \
	i:\grpicon\progman.h \
	i:\grpicon\commdlgs.h \
	i:\grpicon\pmgroup.h


FRAME_DEP = i:\grpicon\grpicon.h \
	c:\msvc\include\undocwin.h \
	i:\grpicon\resource.h \
	help\gihelp.h \
	i:\grpicon\app.h \
	..\icsdll\ics.h \
	i:\grpicon\frame.h \
	i:\grpicon\progman.h \
	i:\grpicon\commdlgs.h \
	i:\grpicon\pmgroup.h


PMGROUP_DEP = i:\grpicon\grpicon.h \
	c:\msvc\include\undocwin.h \
	i:\grpicon\resource.h \
	help\gihelp.h \
	i:\grpicon\app.h \
	..\icsdll\ics.h \
	i:\grpicon\frame.h \
	i:\grpicon\progman.h \
	i:\grpicon\commdlgs.h \
	i:\grpicon\pmgroup.h


PROGMAN_DEP = i:\grpicon\grpicon.h \
	c:\msvc\include\undocwin.h \
	i:\grpicon\resource.h \
	help\gihelp.h \
	i:\grpicon\app.h \
	..\icsdll\ics.h \
	i:\grpicon\frame.h \
	i:\grpicon\progman.h \
	i:\grpicon\commdlgs.h \
	i:\grpicon\pmgroup.h


GRPICON_RCDEP = i:\grpicon\resource.h \
	help\gihelp.h \
	\icsdll\ics.h \
	c:\msvc\include\cpalette.h \
	i:\grpicon\grp31.ico \
	i:\grpicon\grp30.ico \
	i:\grpicon\xicon.ico \
	../icons/wpaper.ico \
	../icons/paint_4.ico \
	i:\icons/icslogo.ico \
	../icons/secur01a.ico \
	../icons/secur01b.ico


all:	$(PROJ).EXE $(PROJ).BSC

APP.OBJ:	APP.C $(APP_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c APP.C

COMMDLGS.OBJ:	COMMDLGS.C $(COMMDLGS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c COMMDLGS.C

FRAME.OBJ:	FRAME.C $(FRAME_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FRAME.C

PMGROUP.OBJ:	PMGROUP.C $(PMGROUP_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c PMGROUP.C

PROGMAN.OBJ:	PROGMAN.C $(PROGMAN_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c PROGMAN.C

GRPICON.RES:	GRPICON.RC $(GRPICON_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r GRPICON.RC


$(PROJ).EXE::	GRPICON.RES

$(PROJ).EXE::	APP.OBJ COMMDLGS.OBJ FRAME.OBJ PMGROUP.OBJ PROGMAN.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
APP.OBJ +
COMMDLGS.OBJ +
FRAME.OBJ +
PMGROUP.OBJ +
PROGMAN.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\MSVC\LIB\+
c:\MSVC\MFC\LIB\+
I:\LIB\+
..\ICSDLL\ICS.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) GRPICON.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	GRPICON.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) GRPICON.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
