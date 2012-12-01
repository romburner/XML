/*++
********************************************************************************
**    RomBurner Development - Systems Programming Group                       **
**    Copyright RomBurner Corporation 2004 - 2007                             **
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
**      windos.cpp
**
**    Abstract:
**      OS dependent disk functions all live here
**      Windows/DOS compatible
**
**    Product Name:  
**      Systems Library
**
**    Version:
**      $Revision: 1.3 $
**
**    Authors:
**      CWR
**
********************************************************************************
**    Revision History:
**      $Log: OSwindos.c,v $
**      Revision 1.3  2010/01/07 22:10:57  mimi
**      wow, make it work with c/c++ on pc
**
**      Revision 1.2  2009/07/07 02:17:56  chuck
**      change for changes sake
**
**      Revision 1.1  2009/06/26 22:14:30  chuck
**      a
**
**      Revision 1.5  2006/06/28 17:39:20  chuck
**      fix return on delete
**
**      Revision 1.4  2006/06/28 17:33:02  chuck
**      Add stubs for banner screen, move xml type recognizer to it's own routine
**
**      Revision 1.3  2006/06/24 05:23:27  chuck
**      add setup reading for skin and language variables and swap skins on startup.
**
**      Revision 1.2  2006/06/23 17:35:45  chuck
**      convert to VC2005
**
**      Revision 1.1  2006/06/21 17:22:13  chuck
**      Initial entry for interface/XML/platform folders
** 
**
** $NoKeywords:$
********************************************************************************
--*/   

#define FIND_WIN  struct _finddata_t

HANDLE hDevice[64] = {0};   // Fillin our hDev!

#define	START_OF_TIME	0x01c0000000000000

#define	bytesPerSector	512

/******************************************************************************\
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:
\******************************************************************************/
void * mAlloc(u32 size)
{
	void *buf = malloc(size);
	if (!buf)
	{
		printf("Memory (%d) allocation error!",size);
		return 0;
	}
	memset(buf, 0, size);
	return buf;
}

/******************************************************************************\
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:
\******************************************************************************/
void mFree(void *mem)
{
	if (mem)
		free(mem);
}

/******************************************************************************\
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:
\******************************************************************************/
s64 timeTo64(u32 iTime)
{
	s64	outVar = (s64)iTime * 10000000 + START_OF_TIME;
	return outVar;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
void saveFileData(struct _finddata_t *filedat, FINDDATA *finddata)
{
    finddata->attrib = GetFileAttributesA(finddata->name);

    finddata->size = filedat->size;
    finddata->userID = 0;
    finddata->groupID = 0;

#ifdef TIME_IS_32BIT
    finddata->accessTime = timeTo64(filedat->time_access);
    finddata->createTime = timeTo64(filedat->time_create);
    finddata->writeTime  = timeTo64(filedat->time_write);
#else
    finddata->accessTime = filedat->time_access;
    finddata->createTime = filedat->time_create;
    finddata->writeTime  = filedat->time_write;

#endif
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    iMachineName

 DESCRIPTION:
	Retrieve this machines name

 ARGUMENTS:
    None

 RETURNS:

********************************************************************************
--*/
char * iMachineName()
{
//	using namespace System;
//	return Environment::MachineName;
	return "mycomputer";
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    iTime

 DESCRIPTION:
    Read system time in secs since 1970...

 ARGUMENTS:
    None

 RETURNS:
    the system time, in secs.

********************************************************************************
--*/
u32 iTimeSec()
{
	__time32_t timer;
    return (u32)_time32(&timer);
//    return (u32)time(&timer);
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    iTime

 DESCRIPTION:
    Return formatted system time

 ARGUMENTS:
    char *timeString,   pointer to  Formatted time string,
                          American  "02/28/2000 23:59:59.00"

 RETURNS:

********************************************************************************
--*/
void iTime(char *timeString, u32 time, int type)
{
    struct tm *timeDate;
	char *timeFmt;
    timeDate = _localtime32( (__time32_t *) &time );
	switch(type)
	{
	default:
	    timeFmt = "%m/%d/%Y %H:%M:%S";
		break;
	case 1:
	    timeFmt = "%Y%m%d%H%M%S";
		break;
	}
	strftime(timeString, 32, timeFmt, timeDate);
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
u32 iSleep(u32 time)
{
    Sleep(time);
    return 0;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
void *iThread(void *code, void* data)
{
    void * handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)code, data, 0, NULL);
    return handle;
}

/*++
 ********************************************************************************
 
 FUNCTION:
 fExecute
 DESCRIPTION:
 Execute a program
 
 ARGUMENTS:
 
 RETURNS:
 
 ********************************************************************************
 --*/
int fExecute(char *execPath, char *paramList)
{
	intptr_t err;
	char *args[10];
	args[0] = execPath;
	args[1] = paramList;
	args[2] = NULL;
	err = _spawnv(_P_WAIT, execPath, args);
    return err;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
FILE *fOpen(char *fidname, char *spec)
{
#if 0
	return fopen( fidname, spec );
#else
	FILE *handle;
	u32	dwDesiredAccess = 0;
	u32 dwShareMode = FILE_SHARE_READ;
	u32 dwCreationDisposition = 0;
	u32 dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

	if (strchr(spec,'r'))
	{
		dwDesiredAccess |= GENERIC_READ;
		dwCreationDisposition = OPEN_EXISTING;
	}
	if (strchr(spec,'w'))
	{
		dwDesiredAccess |= GENERIC_WRITE;
        dwCreationDisposition = CREATE_ALWAYS;
	}

	if (strchr(spec,'a'))
	{
		dwDesiredAccess |= GENERIC_WRITE;
        dwCreationDisposition = OPEN_ALWAYS;
	}

	handle = (struct _iobuf *)CreateFileA(
		fidname,			            // LPCTSTR lpFileName,	pointer to name of the file
		dwDesiredAccess,		        // DWORD dwDesiredAccess,	access (read-write) mode
		dwShareMode,		            // DWORD dwShareMode,	share mode
		NULL,							// pointer to security attributes
		dwCreationDisposition,			// how to create
		dwFlagsAndAttributes,			// file attributes
		NULL );							// handle to file with attributes to copy

	return (handle == INVALID_HANDLE_VALUE) ? 0 : handle;
#endif
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fClose( FILE *handle )
{
	return CloseHandle( handle );
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
	fPos

 DESCRIPTION:
	Move file pointer to a file offset

 ARGUMENTS:
	s64 pos				position in file to put file pointer
	FILE *handle

 RETURNS:

********************************************************************************
--*/
int fPos( s64 pos, FILE *handle  )
{
v64 verter;				// Used to convert 64 to two 32's
s32 *highWord;

	verter.single64 = pos;
	highWord = (verter.both64.high32) ? &verter.both64.high32 : 0;

	return SetFilePointer(
		handle,           // handle of file
		verter.both64.low32,  // number of bytes to move file pointer
		highWord,         // pointer to high-order DWORD of distance to move
		FILE_BEGIN);      // how to move

}

/*++
********************************************************************************
                                                                            
 FUNCTION:
	fEnd

 DESCRIPTION:

 ARGUMENTS:
	FILE *handle		File handle data pointer

 RETURNS:
	Position in file

********************************************************************************
--*/
u64 fEnd( FILE *handle  )
{
	return SetFilePointer(
		handle,           // handle of file
		0,			      // number of bytes to move file pointer
		0,                // pointer to high-order DWORD of distance to move
		FILE_END);        // how to move
 
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    fTell

 DESCRIPTION:
    Return current position in file

 ARGUMENTS:
    File handle

 RETURNS:
    Offset from start of file in bytes of current r/w pointer

********************************************************************************
--*/
u64 fTell( FILE *handle  )
{
	return SetFilePointer(
		handle,           // handle of file
		0,			      // number of bytes to move file pointer
		0,                // pointer to high-order DWORD of distance to move
		FILE_CURRENT);    // how to move
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    fSize

 DESCRIPTION:
    Return the size of the file

 ARGUMENTS:
    File handle

 RETURNS:
    Size of file in 64 bits

********************************************************************************
--*/
u64 fSize( FILE *handle )
{
	u64 size,pos;
	pos = fTell( handle );
	fEnd( handle );
	size = fTell( handle );
	fPos( pos, handle );
	return size;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fWrite( void *buffer, u32 len, FILE *handle )
{
u32 writeCount;
	if ( WriteFile(
		handle,                    // handle to file to write to
		buffer,		               // pointer to data to write to file
		len,					   // number of bytes to write
		&writeCount,			   // pointer to number of bytes written
		NULL ) ) {
		return writeCount;
	}
	return 0;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fRead( void *buffer, u32 len, FILE *handle )
{
u32 readCount;
	if ( ReadFile(
		handle,                 // handle of file to read
		buffer,	                // pointer to buffer that receives data
		len,				    // number of bytes to read
		&readCount,				// pointer to number of bytes read
		NULL ) ) {              // pointer to structure for data
		return readCount;
	}
	return 0;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    fEntry

 DESCRIPTION:
    Fill out file data information structure

 ARGUMENTS:
    char *fidname           A ptr to the file name to check for information
    FINDDATA * finddata     A file directory data information structure

 RETURNS:
    Filled out file data structure

********************************************************************************
--*/
int fEntry( char *fidname, FINDDATA * finddata )
{
    int retval;
    struct _finddata_t filedat;
    size_t len = strlen( fidname );
    char lastchar = fidname[len-1];
    if ( lastchar == FOLDER_CHAR )
        fidname[len-1] = 0;
    strcpy(finddata->name, fidname);
    retval =  ( _findfirst( fidname, &filedat) == -1L ) ? 0 : 1;
    if ( lastchar == FOLDER_CHAR )
        fidname[len-1] = FOLDER_CHAR;
    if (retval)
	{
        saveFileData(&filedat, finddata);
//		FILE *handle = fOpen(fidname, "r");
//	    GetFileTime(handle, (struct _FILETIME *)&finddata->createTime,
//							(struct _FILETIME *)&finddata->accessTime,
//							(struct _FILETIME *)&finddata->writeTime);
//		fClose(handle);
    }
    return retval;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:
    Check if file or folder exists

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fExists(char *fidname)
{
    FINDDATA findata;
    return fEntry( fidname, &findata );
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:
    Check if file or folder exists

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fDelete(char *fidname)
{
	return DeleteFileA(fidname);
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    fSetFileTime

 DESCRIPTION:
    Set the access times on a file

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fSetFileTime(FILE *handle, FILETIME *createDate, FILETIME *accessDate, FILETIME *modifyDate)
{
    return SetFileTime( handle, (const FILETIME *)createDate,
                                (const FILETIME *)accessDate,
                                (const FILETIME *)modifyDate )
                                ? 0 : GetLastError();
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    fSetFileTime

 DESCRIPTION:
    Set the access times on a file

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fGetFileTime(FILE *handle, FILETIME *createDate, FILETIME *accessDate, FILETIME *modifyDate)
{
    return GetFileTime( handle, (FILETIME *)createDate,
                                (FILETIME *)accessDate,
                                (FILETIME *)modifyDate )
                                ? 0 : GetLastError();
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fSetAttributes(char *fidname, u32 attribs)
{
    return SetFileAttributesA(fidname, attribs);
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
FINDHANDLE fFirst(char *pattern , FINDDATA *finddata )
{
    FINDHANDLE handle;
    struct _finddata_t filedat;
    handle = (FINDHANDLE)_findfirst( pattern, &filedat );
    if (handle)
	{
        strcpy(finddata->name, filedat.name);
        saveFileData(&filedat, finddata);
    }
    return handle;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fNext(FINDHANDLE handle, FINDDATA *finddata)
{
    int retVal;
    struct _finddata_t filedat;
    retVal = _findnext( handle, &filedat );
    if (retVal == 0)
	{
        strcpy(finddata->name, filedat.name);
        saveFileData(&filedat, finddata);
    }
    return !retVal;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fStop(FINDHANDLE handle)
{
    return _findclose( handle );
}


/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:
    Make directories

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int mkDir(char * folder)
{
//	SECURITY_ATTRIBUTES SecurityAttributes = { 0,		// nLength
//											   NULL,	// lpSecurityDescriptor
//											   NULL };  // bInheritHandle
//
	return CreateDirectoryA(folder, NULL) ? 0 : GetLastError();
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:
    Change directories

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int chDir(char * folder)
{
	return SetCurrentDirectoryA(folder) ? 0 : GetLastError();
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:
    Remove directories

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int rmDir(char * folder)
{

	return RemoveDirectoryA(folder) ? 0 : GetLastError();
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:
    Get volume information

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
void fGetVolumeInfo(char *volPath, VOLUMEDATA *vData)
{
    vData->retVal =
        GetVolumeInformationA(
            volPath,
            vData->nameBuf,
            sizeof(vData->nameBuf),
            &vData->serialNum,
            &vData->componentLen,
            &vData->systemFlags,
            vData->fileSysType,
            sizeof(vData->fileSysType));
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
   fReadSectors (bDrive, dwStartSector, wSectors, lpSectBuff)

 DESCRIPTION:
    Reads sectors from a logical drive. Use NT method of opening the device
        as a file

 ARGUMENTS:
      drive
         The MS-DOS logical drive number. 0 = A, 1 = B, 2 = C, etc.

      startSector
         The first logical sector to read

      numSectors
         The number of sectors to read

      pSectBuff
         The caller-supplied buffer that will contain the sector data

 RETURNS:
      Returns TRUE if successful, or FALSE if failure.

********************************************************************************
--*/
int fReadSectors(u8 drive, u32 startSector, u32 numSectors, char *pSectBuff)
{
    u32 bytesread;
    char devicename[] = "\\\\.\\A:";
    devicename[4] = (char)drive +'A';
    if (!hDevice[drive]) {
        hDevice[drive] = CreateFileA(devicename,
            GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice[drive] == INVALID_HANDLE_VALUE)
            return 1;
    }

    SetFilePointer (hDevice[drive], (startSector*bytesPerSector), NULL, FILE_BEGIN);
    if (!ReadFile (hDevice[drive], pSectBuff, bytesPerSector*numSectors, &bytesread, NULL) )
        return 1;
    return 0;
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
   fWriteSectors (bDrive, dwStartSector, wSectors, lpSectBuff)

 DESCRIPTION:
    Write sectors to a logical drive. Use NT method of opening the device
        as a file

 ARGUMENTS:
      bDrive
         The MS-DOS logical drive number. 1 = A, 2 = B, 3 = C, etc.

      dwStartSector
         The first logical sector to write

      wSectors
         The number of sectors to write

      lpSectBuff
         The caller-supplied buffer that contains the sector data

 RETURNS:
      Returns TRUE if successful, or FALSE if failure.

********************************************************************************
--*/
int fWriteSectors(u8 drive, u32 startSector, u32 numSectors, char *pSectBuff)
{
    u32 bytesWritten;
    char devicename[] = "\\\\.\\A:";
    devicename[4] = (char)drive + 'A';
    if (!hDevice[drive]) {
        hDevice[drive] = CreateFileA(devicename,
            GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice[drive] == INVALID_HANDLE_VALUE)
            return 1;
    }

    SetFilePointer (hDevice[drive], (startSector*bytesPerSector), NULL, FILE_BEGIN);
    if (!WriteFile( hDevice[drive], pSectBuff, bytesPerSector*numSectors, &bytesWritten, NULL) )
        return 1;
    return 0;
}






#if 0
#define VWIN32_DIOC_DOS_INT25     2
#define VWIN32_DIOC_DOS_INT26     3
#define VWIN32_DIOC_DOS_DRIVEINFO 6

#define CARRY_FLAG 1

#pragma pack(1)
typedef struct _DISKIO {
   DWORD  dwStartSector;   // starting logical sector number
   WORD   wSectors;        // number of sectors
   DWORD  dwBuffer;        // address of read/write buffer
} DISKIO, * PDISKIO;
#pragma pack()

typedef struct _DIOC_REGISTERS {
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
} DIOC_REGISTERS, *PDIOC_REGISTERS;


//BOOL fReadSectors(BYTE bDrive, DWORD dwStartSector, WORD wSectors, LPBYTE lpSectBuff)
int fReadSectors(u8 bDrive, u32 dwStartSector, u16 wSectors, char *pSectBuff)
{
BOOL           fResult;
DWORD          cb;
DIOC_REGISTERS reg = {0};
DISKIO         dio = {0};

   dio.dwStartSector = dwStartSector;
   dio.wSectors      = wSectors;
   dio.dwBuffer      = (DWORD)pSectBuff;

   reg.reg_EAX = bDrive - 1;
   reg.reg_EBX = (DWORD)&dio;
   reg.reg_ECX = 0xFFFF;

    if (!hDev) {
        hDev = CreateFile ( "\\\\.\\VWIN32", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL );
        if ( hDev == INVALID_HANDLE_VALUE )
            return FALSE;
    }

   fResult = DeviceIoControl(hDev, VWIN32_DIOC_DOS_INT25, &reg, sizeof(reg), &reg, sizeof(reg), &cb, 0);
   fResult = fResult && !(reg.reg_Flags & CARRY_FLAG);
   return fResult;
}

BOOL fWriteSectors (BYTE   bDrive, DWORD  dwStartSector, WORD   wSectors, LPBYTE lpSectBuff)
{
   BOOL           fResult;
   DWORD          cb;
   DIOC_REGISTERS reg = {0};
   DISKIO         dio = {0};

   dio.dwStartSector = dwStartSector;
   dio.wSectors      = wSectors;
   dio.dwBuffer      = (DWORD)lpSectBuff;

   reg.reg_EAX = bDrive - 1;
   reg.reg_EBX = (DWORD)&dio;
   reg.reg_ECX = 0xFFFF;

    if (!hDev) {
        hDev = CreateFile ( "\\\\.\\VWIN32", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL );
        if ( hDev == INVALID_HANDLE_VALUE )
            return FALSE;
    }

   fResult = DeviceIoControl(hDev, VWIN32_DIOC_DOS_INT26, &reg, sizeof(reg), &reg, sizeof(reg), &cb, 0);
   fResult = fResult && !(reg.reg_Flags & CARRY_FLAG);
   return fResult;
}

//BOOL fNewReadSectors (BYTE   bDrive, DWORD  dwStartSector, WORD   wSectors, LPBYTE lpSectBuff)
int fNewReadSectors(u8 bDrive, u32 dwStartSector, u16 wSectors, char *pSectBuff)
{
    BOOL           fResult;
    DWORD          cb;
    DIOC_REGISTERS reg = {0};
    DISKIO         dio;
  
    dio.dwStartSector = dwStartSector;
    dio.wSectors      = wSectors;
    dio.dwBuffer      = (DWORD)pSectBuff;
  
    reg.reg_EAX = 0x7305;
    reg.reg_EBX = (DWORD)&dio;
    reg.reg_ECX = -1;
    reg.reg_EDX = bDrive;

    if (!hDev) {
        hDev = CreateFile ( "\\\\.\\VWIN32", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL );
        if ( hDev == INVALID_HANDLE_VALUE )
            return FALSE;
    }

    fResult = DeviceIoControl(hDev, VWIN32_DIOC_DOS_DRIVEINFO, &reg, sizeof(reg), &reg, sizeof(reg), &cb, 0);
    fResult = fResult && !(reg.reg_Flags & CARRY_FLAG);
    return fResult;
}

BOOL fNewWriteSectors (BYTE bDrive, DWORD dwStartSector, WORD   wSectors, LPBYTE lpSectBuff)
{
  BOOL           fResult;
  DWORD          cb;
  DIOC_REGISTERS reg = {0};
  DISKIO         dio;

  dio.dwStartSector = dwStartSector;
  dio.wSectors      = wSectors;
  dio.dwBuffer      = (DWORD)lpSectBuff;

  reg.reg_EAX = 0x7305;
  reg.reg_EBX = (DWORD)&dio;
  reg.reg_ECX = -1;
  reg.reg_EDX = bDrive;
  reg.reg_ESI = 0x6001;   // Normal file data (See function documentation for other values)

    if (!hDev) {
        hDev = CreateFile ( "\\\\.\\VWIN32", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL );
        if ( hDev == INVALID_HANDLE_VALUE )
            return FALSE;
    }

  fResult = DeviceIoControl(hDev, VWIN32_DIOC_DOS_DRIVEINFO, &reg, sizeof(reg), &reg, sizeof(reg), &cb, 0);
  fResult = fResult && !(reg.reg_Flags & CARRY_FLAG);
  return fResult;
}

/*
GetVolumeInformation function. This function returns information about the current volume, such as the maximum length of filenames. Once you have called this function, you can use the value returned as the maximum file length within your application and dynamically allocate a buffer for your filenames and for paths. This is preferable to using static buffers for filenames and paths. If you absolutely must use static buffers, reserve 256 characters for filenames and 260 characters for paths. The GetVolumeInformation function is as follows:

BOOL GetVolumeInformation(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, 
   lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, 
   lpFileSystemNameBuffer, nFileSystemNameSize)

where 

LPTSTR  lpRootPathName is the string that contains the root directory of the volume to be queried.
        If this parameter is NULL, the root of the current directory is used. 
LPTSTR  lpVolumeNameBuffer is the buffer that is filled in with the name of the volume. 
DWORD   nVolumeNameSize is the length, in characters, of the volume name buffer.
        This parameter is ignored if the volume name buffer is not supplied. 
LPDWORD lpVolumeSerialNumber will be filled in with the volume serial number.
        This parameter can be NULL if the serial number is not needed. 
LPDWORD lpMaximumComponentLength will be filled in with the maximum length,
        in characters, of a filename component (the part of the filename between
        the backslashes) supported by the specified file system. 
LPDWORD lpFileSystemFlags will be filled in with the flags associated with the specified file system.
        This parameter can be any combination of the following flags: 

FS_CASE_IS_PRESERVED The file system preserves the case of filenames when it places a name on disk. 
FS_CASE_SENSITIVE The file system supports case-sensitive filenames. 
FS_UNICODE_STORED_ON_DISK The file system supports Unicode in filenames as they appear on disk. 
FS_PERSISTENT_ACLS The file system preserves and enforces ACLs. For example, NTFS preserves and enforces ACLs, and FAT does not.  
FS_FILE_COMPRESSION The file system supports file-based compression. 
FS_VOL_IS_COMPRESSED The specified volume is a compressed volume; for example, a DoubleSpace volume. 
FILE_SUPPORTS_ENCRYPTION The file system supports the Encrypted File System (EFS). 
FILE_SUPPORTS_OBJECT_IDS The file system supports object identifiers. 
FILE_SUPPORTS_REPARSE_POINTS The file system supports reparse points. 
FILE_SUPPORTS_SPARSE_FILES The file system supports sparse files. 
FILE_VOLUME_QUOTAS The file system supports disk quotas. 

LPTSTR  lpFileSystemNameBuffer will be filled in with the name of the file system (FAT, HPFS, or NTFS). 
DWORD   nFileSystemNameSize is the length, in characters, of the file system name buffer. This parameter
        is ignored if the file system name buffer is not supplied. 
*/

#endif

