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
PROJPATH = F:\GRPICON\SETUP\SITELIC\OECD\
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
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /D "SITE_LICENSE" /I "." /FR /GA /Fd"SETUP.PDB"
CFLAGS_R_WEXE = /nologo /G2 /W3 /AM /O1 /D "NDEBUG" /D "SITE_LICENSE" /I "." /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
LIBS_R_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = ..\..\SETUP.DEF
OBJS_EXT = 
LIBS_EXT = E:\LIB\KERNEL.LIB E:\LIB\LZEXPAND.LIB 
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

LZEXPAND_DEP = 

SETUP_DEP = f:\grpicon\setup\setup.h \
	f:\grpicon\setup\dialog.h \
	f:\grpicon\setup\setupinf.h \
	f:\icsclass\icmeter.h \
	f:\icsclass\iccustom.h \
	f:\classes\winxx.h \
	c:\include\bttncur.h \
	c:\include\undocwin.h \
	f:\icsclass\iccanvas.h \
	f:\classes\window.h \
	f:\classes\colors.h \
	f:\icsclass\icstatic.h \
	f:\icsclass\icbutton.h


SETUPDLG_DEP = f:\grpicon\setup\setup.h \
	f:\grpicon\setup\dialog.h \
	f:\grpicon\setup\setupinf.h \
	f:\icsclass\icmeter.h \
	f:\icsclass\iccustom.h \
	f:\classes\winxx.h \
	c:\include\bttncur.h \
	c:\include\undocwin.h \
	f:\icsclass\iccanvas.h \
	f:\classes\window.h \
	f:\classes\colors.h \
	f:\icsclass\icstatic.h \
	f:\icsclass\icbutton.h


SETUPINF_DEP = f:\grpicon\setup\setup.h \
	f:\grpicon\setup\dialog.h \
	f:\grpicon\setup\setupinf.h \
	f:\icsclass\icmeter.h \
	f:\icsclass\iccustom.h \
	f:\classes\winxx.h \
	c:\include\bttncur.h \
	c:\include\undocwin.h \
	f:\icsclass\iccanvas.h \
	f:\classes\window.h \
	f:\classes\colors.h \
	f:\icsclass\icstatic.h \
	f:\icsclass\icbutton.h


SETUPPM_DEP = f:\grpicon\setup\setup.h \
	f:\grpicon\setup\dialog.h \
	f:\grpicon\setup\setupinf.h \
	f:\icsclass\icmeter.h \
	f:\icsclass\iccustom.h \
	f:\classes\winxx.h \
	c:\include\bttncur.h \
	c:\include\undocwin.h \
	f:\icsclass\iccanvas.h \
	f:\classes\window.h \
	f:\classes\colors.h \
	f:\icsclass\icstatic.h \
	f:\icsclass\icbutton.h


SETUP_RCDEP = f:\grpicon\setup\dialog.h \
	f:\icsclass\iccustom.h \
	f:\classes\winxx.h \
	c:\include\bttncur.h \
	/icons/icslogo.ico \
	f:\grpicon\setup\disk.ico \
	f:\grpicon\setup\wck.ico \
	f:\grpicon\grp31.ico


ICCANVAS_DEP = f:\icsclass\iccanvas.h \
	f:\icsclass\iccustom.h \
	f:\classes\window.h \
	f:\classes\winxx.h \
	c:\include\undocwin.h \
	f:\classes\colors.h


ICCUSTOM_DEP = f:\icsclass\iccustom.h \
	f:\classes\winxx.h \
	c:\include\bttncur.h


ICMETER_DEP = f:\icsclass\icmeter.h \
	f:\icsclass\iccustom.h \
	f:\classes\winxx.h \
	c:\include\bttncur.h \
	c:\include\undocwin.h


ICSTATIC_DEP = f:\icsclass\icstatic.h \
	f:\icsclass\iccustom.h \
	f:\classes\window.h \
	c:\include\undocwin.h


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
	$(RC) $(RCFLAGS) $(RCDEFINES) -r -FoF:\GRPICON\SETUP\SITELIC\OECD\SETUP.RES ..\..\SETUP.RC

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
c:\lib\+
e:\msvc\lib\+
e:\msvc\mfc\lib\+
E:\LIB\KERNEL.LIB+
E:\LIB\LZEXPAND.LIB+
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
