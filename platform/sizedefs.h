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
**      sizedefs.h
**
**    Abstract:
**      Size definitions on a platform basis
**      Also sets common platform definitions
**
**    Product Name:  
**      Systems Library
**
**    Version:
**      $Revision: 1.4 $
**
**    Authors:
**      CWR
**
********************************************************************************
**    Revision History:
**      $Log: sizedefs.h,v $
**      Revision 1.4  2009/08/19 03:52:17  chuck
**      XML processing added for fun and profit
**
**      Revision 1.3  2009/06/27 15:55:55  chuck
**      platforming for mac
**
**      Revision 1.2  2009/06/27 15:25:49  chuck
**      yA
**
**      Revision 1.1  2009/06/26 22:14:30  chuck
**      a
**
**
** $NoKeywords: $
********************************************************************************
--*/

#if !defined(_SIZEDEFS_H_INCLUDED_)
#define _SIZEDEFS_H_INCLUDED_

#ifdef  WIN32

typedef unsigned char                   u8;     /* unsigned  8-bit */
typedef unsigned short                  u16;    /* unsigned 16-bit */
typedef unsigned long                   u32;    /* unsigned 32-bit */
typedef unsigned __int64                u64;    /* unsigned 64-bit */

typedef signed char                     s8;     /* signed  8-bit */
typedef signed short                    s16;    /* signed 16-bit */
typedef signed long                     s32;    /* signed 32-bit */
typedef signed __int64                  s64;    /* signed 64-bit */            

typedef char                            i8;     /* integer  8-bit */
typedef short                           i16;    /* integer 16-bit */
typedef long                            i32;    /* integer 32-bit */
typedef __int64                         i64;    /* integer 64-bit */            

typedef	float							f32;
typedef double							f64;

static const char *FOLDER_STRING = "\\";
static const char  FOLDER_CHAR =   '\\';
static const char  SEPARATOR_CHAR =';';
static const char  DRIVE_CHAR =    ':';

static const int MAX_DIR_LEN = 260;
static const int MAX_FILE_LEN = 260;
static const int MAX_PATH_LEN = 2050;

#endif


#ifdef  MACOSX

typedef unsigned char                   u8;     /* unsigned  8-bit */
typedef unsigned short                  u16;    /* unsigned 16-bit */
typedef unsigned long                   u32;    /* unsigned 32-bit */
typedef unsigned long long              u64;    /* unsigned 64-bit */

typedef signed char                     s8;     /* signed  8-bit */
typedef signed short                    s16;    /* signed 16-bit */
typedef signed long                     s32;    /* signed 32-bit */
typedef signed long long                s64;    /* signed 64-bit */            

typedef char                            i8;     /* integer  8-bit */
typedef short                           i16;    /* integer  16-bit */
typedef long                            i32;    /* integer  32-bit */
typedef long long                       i64;    /* integer  64-bit */            

typedef	float							f32;
typedef double							f64;

#define FOLDER_STRING   "/"
#define FOLDER_CHAR     '/'
#define SEPARATOR_CHAR  ';'
#define DRIVE_CHAR      ':'

#define MAX_DIR_LEN     260
#define MAX_FILE_LEN    260
#define MAX_PATH_LEN    2050

#endif


#ifdef  IPHONE

typedef unsigned char                   u8;     /* unsigned  8-bit */
typedef unsigned short                  u16;    /* unsigned 16-bit */
typedef unsigned long                   u32;    /* unsigned 32-bit */
typedef unsigned long long              u64;    /* unsigned 64-bit */

typedef signed char                     s8;     /* signed  8-bit */
typedef signed short                    s16;    /* signed 16-bit */
typedef signed long                     s32;    /* signed 32-bit */
typedef signed long long                s64;    /* signed 64-bit */            

typedef char                            i8;     /* integer  8-bit */
typedef short                           i16;    /* integer  16-bit */
typedef long                            i32;    /* integer  32-bit */
typedef long long                       i64;    /* integer  64-bit */            

typedef	float							f32;
typedef double							f64;

#define FOLDER_STRING   "/"
#define FOLDER_CHAR     '/'
#define SEPARATOR_CHAR  ';'
#define DRIVE_CHAR      ':'

#define MAX_DIR_LEN     260
#define MAX_FILE_LEN    260
#define MAX_PATH_LEN    2050

#endif


#define TAB_CHAR        9
#define LF_CHAR         10
#define CR_CHAR         13

typedef struct {
	u32 low32;
	s32 high32;
} d64;

typedef union {
	s64	single64;
	d64 both64;
} v64;


#endif // !defined(_SIZEDEFS_H_INCLUDED_)

