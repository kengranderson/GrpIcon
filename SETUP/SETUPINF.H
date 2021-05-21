#ifndef		INC_SETUPINF
#define		INC_SETUPINF

/****************************************************************************
Module name: SetupInf.H
Programmer : Jeffrey M. Richter.
*****************************************************************************/

// Defines used by the SETUP program.
#define MAXPATH   80
#define MAXDRIVE  3
#define MAXDIR    66
#define MAXFILE   9
#define MAXEXT    5
#define MAXFILENAME  (MAXFILE + MAXEXT)
#define MAXSETUPINFOSIZE   10240
#define MAXNUMDISKS         10
#define MAXNUMDIRS          25
#define MAXNUMFILES         200
#define MAXPMFILES          20

// Defines used by the SETUP program.
#define MAXDISKDESC     50
#define MAXDIRDESC      30
#define MAXFILEDESC     50
#define MAXPMDESC       30
#define MAXAPPNAME      40

typedef enum {
   SIM_INITIALIZE,      // wParam: NU, lpBuffer: SETUP.INF file's pathname.

   SIM_GETAPPNAME,      // wParam: NU, lpBuffer: buffer.
   SIM_GETDEFDIR,       // wParam: NU, lpBuffer: buffer.
   SIM_GETSPACENEEDED,  // wParam: NU, lpBuffer: NU.

   SIM_GETNUMDISKS,     // wParam: NU, lpBuffer: NU.
   SIM_GETDISKDESC,     // wParam: Disk#, lpBuffer: NU.
   SIM_FINDDISKNUM,     // wParam: Disk#, lpBuffer: NU.

   SIM_GETNUMDIRS,      // wParam: NU, lpBuffer: NU.
   SIM_GETDIR,          // wParam: Dir#, lpBuffer: buffer.
   SIM_FINDDIRNUM,      // wParam: Dir#, lpBuffer: NU.

   SIM_GETNUMFILES,     // wParam: NU, lpBuffer: NU.
   SIM_GETFILEDESC,     // wParam: File#, lpBuffer: buffer.
   SIM_GETFILENAME,     // wParam: File#, lpBuffer: buffer.
   SIM_GETFILEDIR,      // wParam: File#, lpBuffer: buffer. Returns #.
   SIM_GETFILEDISK,     // wParam: File#, lpBuffer: buffer 4 DESC. RETURNS #
   SIM_ISFILECOMPRESSED,// wParam: File#, lpBuffer: NU.
   SIM_FINDDIRFORFILE,  // wParam: NU, lpBuffer: Filename.

   SIM_GETPMGROUP,      // wParam: NU, lpBuffer: buffer.
   SIM_GETPMGROUPFILENAME, // wParam: NU, lpBuffer: buffer.
   SIM_GETNUMPMPROGS,   // wParam: NU, lpBuffer: NU.
   SIM_GETPMPROGNAME,   // wParam: PMProg#, lpBuffer: buffer.
   SIM_GETPMPROGDESC,   // wParam: PMProg#, lpBuffer: buffer.
   SIM_GETPMICONINFO,    // wParam: PMProg#, lpBuffer: buffer. Returns icon #.
   SIM_FINDFILENUM		// wParam: NU, lpBuffer: buffer.
} SETUPINFOMSG;


// Possible return values from sending the SIM_INITIALIZE message.
typedef enum {
   SIM_INIT_NOERROR,
   SIM_INIT_NOMEM,
   SIM_INIT_NOFILE
} SIM_INITIALIZE_ERROR;

typedef	struct	tagPMInfo
    {
    char szFileName[MAXFILENAME];
    char szDesc[MAXPMDESC];
    char szIconFileName[MAXFILENAME];
    WORD wIconPos;
    }	PMINFO;

DWORD FAR PASCAL SetupInfoSys (SETUPINFOMSG Msg, WORD wParam, LPSTR lpBuffer);

#endif		//	INC_SETUPINF
