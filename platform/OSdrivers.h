/*++
********************************************************************************
**    RomBurner Development - Systems Programming Group                       **
**    Copyright RomBurner Corporation 2004 - 2005                             **
********************************************************************************
**                                                                            **
**    RomBurner CONFIDENTIAL                                                  **
**    This file, software, or program is supplied under the terms of a        **
**    license agreement and/or nondisclosure agreement with RomBurner         **
**    and may not be copied or disclosed except in accordance with the        **
**    terms of that agreement.  This file, software, or program contains      **
**    copyrighted material and/or trade secret information of RomBurner       **
**    Corporation, and must be treated as such.  RomBurner reserves all       **
**    rights in this material, except as the license agreement or             **
**    nondisclosure agreement specifically indicate.                          **
**                                                                            **
**    All rights reserved.  No part of this program or publication            **
**    may be reproduced, transmitted, transcribed, stored in a                **
**    retrieval system, or translated into any language or computer           **
**    language, in any form or by any means, electronic, mechanical,          **
**    magnetic, optical, chemical, manual, or otherwise, without              **
**    the prior written permission of RomBurner Company                       **
**                                                                            **
**    RomBurner makes no warranty of any kind regarding this code.  This      **
**    code is provided on an "As Is" basis and RomBurner will not provide     **
**    any support, assistance, installation, training or other services.      **
**    RomBurner does not provide any updates, enhancements or extensions.     **
**    RomBurner specifically disclaims any warranty of merchantability,       **
**    noninfringement, fitness for any particular purpose, or any other       **
**    warranty.                                                               **
**                                                                            **
**    RomBurner disclaims all liability, including liability for infringement **
**    of any proprietary rights, relating to use of the code.  No license,    **
**    express or implied, by estoppel or otherwise, to any intellectual		  **
**    property rights is granted herein.                                      **
**                                                                            **
********************************************************************************
**
**    Module Name:
**      diskfun.h
**
**    Abstract:
**      Platform specifice OS functions
**
**    Product Name:  
**      Systems Library
**
**    Version:
**      $Revision: 1.6 $
**
**    Authors:
**      CWR
**
********************************************************************************
**    Revision History:
**      $Log: OSdrivers.h,v $
**      Revision 1.6  2010/03/05 01:18:00  chuck
**      new maciness for file finds
**
**      Revision 1.5  2009/08/19 03:52:17  chuck
**      XML processing added for fun and profit
**
**      Revision 1.4  2009/06/27 15:55:55  chuck
**      platforming for mac
**
**      Revision 1.3  2009/06/27 15:25:49  chuck
**      yA
**
**      Revision 1.2  2009/06/27 02:21:47  chuck
**      *** empty log message ***
**
**      Revision 1.1  2009/06/26 22:14:30  chuck
**      a
**
**
** $NoKeywords: $
********************************************************************************
--*/   

#if !defined(_OSDRIVERS_H_INCLUDED_)
#define _OSDRIVERS_H_INCLUDED_

#include "../platform/platinfo.h"
#include "../platform/sizedefs.h"

#ifdef WIN32

//#define	_USE_32BIT_TIME_T	1

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE	1
#endif

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif

#include <windows.h>
#include <process.h>
#include <io.h>
#include <direct.h>

#define	FINDHANDLE	u32
#endif

// Linux too ? how to do an OR?
#ifdef MACOSX
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <utime.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#define	FINDHANDLE	DIR *

typedef struct _FILETIME
{
	u32		lowtime;
	u32		hightime;
} FILETIME;
#endif

#ifdef IPHONE
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <utime.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#define	FINDHANDLE	DIR *

typedef struct _FILETIME
	{
		u32		lowtime;
		u32		hightime;
	} FILETIME;
#endif

#include "errors.h"

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>

#define	MAX_VOLUMES	26

typedef int (*CALL_PTR)(void *);

typedef struct
{
	char*	pattern;		// What pattern do we match?
    u64     size;           // File size!!!
    s64     createTime;     // File create time
    s64     accessTime;     // Last access time
    s64     writeTime;      // Last written time
    u32     attrib;         // Attributes
    u32     userID;         // User ID #
    u32     groupID;        // Group ID #
    char    name[260];      // The name of the file/folder
} FINDDATA;

typedef struct
{
    char    nameBuf[256];   // Volume name buffer storage
    char    fileSysType[16];// File system type info
    u32     componentLen;   // Max file component length
    u32     serialNum;      // Volume serial number
    u32     systemFlags;    // System flag information
    int     retVal;         // Return value from volume routine
} VOLUMEDATA;


#define FILE_ATTR_READONLY             0x00000001  
#define FILE_ATTR_HIDDEN               0x00000002  
#define FILE_ATTR_SYSTEM               0x00000004  
#define FILE_ATTR_DIRECTORY            0x00000010  
#define FILE_ATTR_ARCHIVE              0x00000020  
#define FILE_ATTR_ENCRYPTED            0x00000040  
#define FILE_ATTR_NORMAL               0x00000080  
#define FILE_ATTR_TEMPORARY            0x00000100  
#define FILE_ATTR_SPARSE_FILE          0x00000200  
#define FILE_ATTR_REPARSE_POINT        0x00000400  
#define FILE_ATTR_COMPRESSED           0x00000800  
#define FILE_ATTR_OFFLINE              0x00001000  
#define FILE_ATTR_NOT_CONTENT_INDEXED  0x00002000  
#define FILE_ATTR_LINK				   0x04000000
#define FILE_ATTR_SOCK				   0x08000000
#define FILE_ATTR_FIFO				   0x10000000
#define FILE_ATTR_DRIVEREFERENCE	   0x20000000
#define FILE_ATTR_RAWSECTORS		   0x40000000

#define	FATTRIBS	u32

// extern u32 bytesPerSector;

void *mAlloc(u32 size);
void mFree(void *);

void iTime(char *timeDate,u32 time,int type);
u32	iTimeSec(void);
u32	iSleep(u32 time);
char *iOsType(void);
char *iMachineName(void);
void *iThread(void *code, void* data);
int fExecute(char *execPath, char *paramList);

int mkDir(char *folder);
int chDir(char *folder);
int rmDir(char *folder);

FILE *fOpen(const char *fidname, const char *spec);
int fClose(FILE *handle);
int fExists(char *fidname);
int fDelete(char *fidname);
int fPos(s64 pos, FILE *handle);
u64 fEnd(FILE *handle);
u64 fTell(FILE *handle);
u64 fSize(FILE *handle);
int fRead(void *buffer, u32 len , FILE *handle);
int fWrite(void *buffer, u32 len, FILE *handle);
int fSetAttributes(char *fidname, FATTRIBS attribs);
int fSetFileTime(FILE *handle, FILETIME *createDate, FILETIME *accessDate, FILETIME *modifyDate);
int fGetFileTime(FILE *handle, FILETIME *createDate, FILETIME *accessDate, FILETIME *modifyDate);

int fEntry(char *fidname, FINDDATA *);
FINDHANDLE fFirst(char *pattern, FINDDATA *finddata);
int fNext(FINDHANDLE handle, FINDDATA *finddata);
int fStop(FINDHANDLE handle);

void fGetVolumeInfo(char *volume, VOLUMEDATA *vData);
int fReadSectors(u8 drive, u32 startSector, u32 numSectors, char *pSectBuff);
int fWriteSectors(u8 drive, u32 startSector, u32 numSectors, char *pSectBuff);

#endif
