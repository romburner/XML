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
**      osMacOSX.c
**
**    Abstract:
**      OS dependent disk functions all live here
**      Macintosh/OSX compatible
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
**      $Log: OSMacOSX.c,v $
**      Revision 1.6  2010/04/02 04:03:09  chuck
**      more jumpiness
**
**      Revision 1.5  2010/03/14 00:30:44  chuck
**      maci
**
**      Revision 1.4  2010/03/05 01:18:00  chuck
**      new maciness for file finds
**
**      Revision 1.3  2010/01/12 19:09:43  chuck
**      fprintf foolishness again. must fix with var args fPrintf
**
**      Revision 1.2  2009/09/18 18:43:02  chuck
**      more of the mods that made the product less/more stable while increasing/decreasing feature creep
**
**      Revision 1.1  2009/06/27 15:55:55  chuck
**      platforming for mac
**
** $NoKeywords: $
********************************************************************************
--*/

#include "fnmatch.h"
#include "spawn.h"

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
		printf("Memory (%d) allocation error!",(int)size);
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
	thread_new
 DESCRIPTION:
	Create a new thread
 ARGUMENTS:
	void *(run)(void*)
	void *parm
 RETURNS:
\******************************************************************************/
int thread_new(void *(run)(void*), void *parm)
{
	int retc;
	pthread_t  p_thread;       // thread's structure

	retc = pthread_create(&p_thread, NULL, run, parm);
	return retc;
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
	void *handle = 0;
	thread_new(code, data);
//    void * handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)code, data, 0, NULL);
    return handle;
}

void fillArgs(char *args[], char *param)
{
	int i = 1;
	char *argh,ch,match;
	do
	{
		argh = args[i] = param;
		ch = *argh;
		if ((ch == '"') || (ch == '\''))
		{	match = ch;
			argh++;
			do
			{
				ch = *argh++;
			} while((ch!=0) && (ch!=match));
			argh--;
			*argh = 0;
			if (ch)
				argh++;
		}
		else
		for (;ch!=0;ch=*argh)
		{
			if (ch == ' ')
			{	*argh++ = 0;
				break;
			}
			argh++;
		}
		i++;
		param = argh;
	} while(*argh!=0);
	args[i] = NULL;	
}

/*++
 ********************************************************************************
 
 FUNCTION:
 
 DESCRIPTION:
 
 ARGUMENTS:
 
 RETURNS:
 
 ********************************************************************************
 --*/
int fExecute(char *execPath, char *paramList)
{
	int err = 0;
	pid_t pid = fork();
	if (pid==0)
	{	char *args[100];
		args[0] = execPath;
		fillArgs(args,paramList);
		err = execv(args[0], args);	// Launch external process
	}
	else
	{
		waitpid(pid,0,0); /* wait for child to exit */
	}
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
u32 iTimeSec()
{
    time_t timer;
    return time(&timer);
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
void fTime(char *timeString, u32 time)
{
    struct tm *timeDate;
    timeDate = localtime( (time_t *) &time );
    strftime(timeString, 32, "%m/%d/%Y %H:%M:%S", timeDate);
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
    sleep(time);
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
FILE *fOpen(const char *fidname, const char *spec)
{
    return fopen( fidname, spec );
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
    return fclose( handle );
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
int fPos( s64 pos, FILE *handle  )
{
    return fseek(handle, pos, SEEK_SET);
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
u64 fEnd( FILE *handle  )
{
    return fseek(handle, 0, SEEK_END);
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
	return remove(fidname);
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
    return ftell(handle);
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

void fPrintf(FILE *handle, char *fmt, ...)
{
	
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
    return fwrite(buffer, 1, len, handle );
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
    return fread(buffer, 1, len, handle );
}

/*++
********************************************************************************
                                                                            
 FUNCTION:
    fEntry

 DESCRIPTION:
    Get folder data

 ARGUMENTS:
    char *fidname       The folder name we are retrieving data for
    FINDDATA * finddata The directory data about the folder

 RETURNS:
    Filled out file data structure

********************************************************************************
--*/
int fEntry( char *fidname, FINDDATA * finddata )
{
    if (fFirst(fidname,  finddata ))  // "*", removed
        return 1;
    return 0;
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
FILE *fHand;
int retVal;
    if (fHand = fOpen(fidname, "rb")) {
        retVal = 1;
        fClose(fHand);
    }
    else {
        retVal = 0;
    }
    return retVal;
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
	return 0;
}

int fSetFileTime(FILE *handle, FILETIME *createDate, FILETIME *accessDate, FILETIME *modifyDate)
{
//    return SetFileTime( handle, (const FILETIME *)&toc->create_date,
//                                (const FILETIME *)&toc->access_date,
//                                (const FILETIME *)&toc->modify_date )
//                                ? 0 : GetLastError();
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
int fSetAttributes(char *fidname, FATTRIBS attribs)
{
//    return SetFileAttributes(fidname, attribs);
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
void saveFileData(struct stat statEntry, FINDDATA *finddata)
{
    finddata->attrib |= FILE_ATTR_ARCHIVE;
    finddata->accessTime = statEntry.st_atime;
    finddata->createTime = statEntry.st_ctime;
    finddata->writeTime  = statEntry.st_mtime;
    finddata->size       = statEntry.st_size;
    finddata->userID     = statEntry.st_uid;
    finddata->groupID    = statEntry.st_gid;

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
FINDHANDLE dirHandle;
	finddata->pattern = pattern;
    dirHandle = opendir(".");
	return (fNext(dirHandle, finddata)) ? dirHandle : NULL;
}

int fMatchs(char *pattern,char *dirName)
{
	return (fnmatch(pattern, dirName, 0) == 0) ? 1 : 0;
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
struct dirent *dirEntry;
struct stat statEntry;
	if (!handle) return 0;

	for (dirEntry = readdir(handle);dirEntry;dirEntry = readdir(handle))
    if (fMatchs(finddata->pattern,dirEntry->d_name))
	{
        strcpy(finddata->name, dirEntry->d_name);
        stat(finddata->name, &statEntry);
        switch (dirEntry->d_type) {
        case DT_DIR:
            finddata->attrib = FILE_ATTR_DIRECTORY;
            break;
        case DT_SOCK:
            finddata->attrib = FILE_ATTR_SOCK;
            break;
        case DT_LNK:
            finddata->attrib = FILE_ATTR_LINK;
            break;
        case DT_REG:
        default:
            finddata->attrib = 0;
            break;
        }
        saveFileData(statEntry, finddata);
        return 1;
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
int fStop(FINDHANDLE handle)
{
    return closedir(handle);
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
    return mkdir(folder, 770);
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
   return chdir(folder);
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
    return rmdir(folder);
}

/*++
********************************************************************************
                                                                            
 FUNCTION:

 DESCRIPTION:

 ARGUMENTS:

 RETURNS:

********************************************************************************
--*/
void fGetVolumeInfo(char *volPath, VOLUMEDATA *vData)
{
//    vData->retVal =
//        GetVolumeInformation(
//            volPath,
//            vData->nameBuf,
//            sizeof(vData->nameBuf),
//            &vData->serialNum,
//            &vData->componentLen,
//            &vData->systemFlags,
//            vData->fileSysType,
//            sizeof(vData->fileSysType));
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
    return 0;
}


#if 0
int stat(const char *file_name, struct stat *buf); 
struct stat {
    dev_t         st_dev;      /* device */
    ino_t         st_ino;      /* inode */
    mode_t        st_mode;     /* protection */
    nlink_t       st_nlink;    /* number of hard links */
    uid_t         st_uid;      /* user ID of owner */
    gid_t         st_gid;      /* group ID of owner */
    dev_t         st_rdev;     /* device type (if inode device) */
    off_t         st_size;     /* total size, in bytes */
    blksize_t     st_blksize;  /* blocksize for filesystem I/O */
    blkcnt_t      st_blocks;   /* number of blocks allocated */
    time_t        st_atime;    /* time of last access */
    time_t        st_mtime;    /* time of last modification */
    time_t        st_ctime;    /* time of last change */
};

int utime(const char *filename, struct utimbuf *buf); 
struct utimbuf {
        time_t actime;  /* access time */
        time_t modtime; /* modification time */
};

#endif

