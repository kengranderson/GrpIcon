# Microsoft Visual C++ generated build script - Do not modify

PROJ = SETUP
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\GRPICON\SETUP\SITELIC\TUTTLE\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = SETUP.C     
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /D "SITE_LICENSE" /I "." /I "\icsclass" /FR /GA /Fd"SETUP.PDB"
CFLAGS_R_WEXE = /nologo /G2 /W3 /AM /O1 /D "NDEBUG" /D "SITE_LICENSE" /I "." /I "\icsclass" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw mlibcew commdlg.lib lzexpand.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = oldnames libw mlibcew commdlg.lib lzexpand.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = ..\..\SETUP.DEF
OBJS_EXT = 
LIBS_EXT = ..\..\..\..\LIB\KERNEL.LIB 
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
SBRS = SETUP.SBR \
		SETUPDLG.SBR \
		SETUPINF.SBR \
		SETUPPM.SBR \
		ICCANVAS.SBR \
		ICCUSTOM.SBR \
		ICMETER.SBR \
		ICSTATIC.SBR


KERNEL_DEP = 

SETUP_DEP = e:\grpicon\setup\setup.h \
	e:\grpicon\setup\dialog.h \
	e:\grpicon\setup\setupinf.h \
	e:\icsclass\icmeter.h \
	e:\include\undocwin.h \
	e:\icsclass\iccanvas.h \
	e:\icsclass\icstatic.h \
	e:\icsclass\icbutton.h


SETUPDLG_DEP = e:\grpicon\setup\setup.h \
	e:\grpicon\setup\dialog.h \
	e:\grpicon\setup\setupinf.h \
	e:\icsclass\icmeter.h \
	e:\include\undocwin.h \
	e:\icsclass\iccanvas.h \
	e:\icsclass\icstatic.h \
	e:\icsclass\icbutton.h


SETUPINF_DEP = e:\grpicon\setup\setup.h \
	e:\grpicon\setup\dialog.h \
	e:\grpicon\setup\setupinf.h \
	e:\icsclass\icmeter.h \
	e:\include\undocwin.h \
	e:\icsclass\iccanvas.h \
	e:\icsclass\icstatic.h \
	e:\icsclass\icbutton.h


SETUPPM_DEP = e:\grpicon\setup\setup.h \
	e:\grpicon\setup\dialog.h \
	e:\grpicon\setup\setupinf.h \
	e:\icsclass\icmeter.h \
	e:\include\undocwin.h \
	e:\icsclass\iccanvas.h \
	e:\icsclass\icstatic.h \
	e:\icsclass\icbutton.h


SETUP_RCDEP = e:\grpicon\setup\dialog.h \
	e:\icsclass\iccustom.h \
	e:\include\bttncur.h \
	/icons/icslogo.ico \
	/grpicon/setup/disk.ico \
	/grpicon/setup/wck.ico \
	/grpicon/grp31.ico


ICCANVAS_DEP = e:\icsclass\iccanvas.h \
	e:\include\undocwin.h


ICMETER_DEP = e:\icsclass\icmeter.h


ICMETER_DEP = e:\include\undocwin.h


ICSTATIC_DEP = e:\icsclass\icstatic.h \
	e:\include\undocwin.h


all:	$(PROJ).EXE $(PROJ).BSC

SETUP.OBJ:	..\..\SETUP.C $(SETUP_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c ..\..\SETUP.C

SETUPDLG.OBJ:	..\..\SETUPDLG.C $(SETUPDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\SETUPDLG.C

SETUPINF.OBJ:	..\..\SETUPINF.C $(SETUPINF_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\SETUPINF.C

SETUPPM.OBJ:	..\..\SETUPPM.C $(SETUPPM_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\SETUPPM.C

SETUP.RES:	..\..\SETUP.RC $(SETUP_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r -FoE:\GRPICON\SETUP\SITELIC\TUTTLE\SETUP.RES ..\..\SETUP.RC

ICCANVAS.OBJ:	..\..\..\..\ICSCLASS\ICCANVAS.C $(ICCANVAS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\..\..\ICSCLASS\ICCANVAS.C

ICCUSTOM.OBJ:	..\..\..\..\ICSCLASS\ICCUSTOM.C $(ICCUSTOM_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\..\..\ICSCLASS\ICCUSTOM.C

ICMETER.OBJ:	..\..\..\..\ICSCLASS\ICMETER.C $(ICMETER_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\..\..\ICSCLASS\ICMETER.C

ICSTATIC.OBJ:	..\..\..\..\ICSCLASS\ICSTATIC.C $(ICSTATIC_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\..\..\ICSCLASS\ICSTATIC.C


$(PROJ).EXE::	SETUP.RES

$(PROJ).EXE::	SETUP.OBJ SETUPDLG.OBJ SETUPINF.OBJ SETUPPM.OBJ ICCANVAS.OBJ ICCUSTOM.OBJ \
	ICMETER.OBJ ICSTATIC.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
SETUP.OBJ +
SETUPDLG.OBJ +
SETUPINF.OBJ +
SETUPPM.OBJ +
ICCANVAS.OBJ +
ICCUSTOM.OBJ +
ICMETER.OBJ +
ICSTATIC.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
G:\MSVC\LIB\+
G:\MSVC\MFC\LIB\+
e:\lib\+
..\..\..\..\LIB\KERNEL.LIB+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) SETUP.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	SETUP.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) SETUP.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
