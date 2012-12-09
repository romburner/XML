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
**    express or implied, by estoppel or otherwise, to any intellectual       **
**    property rights is granted herein.                                      **
**                                                                            **
********************************************************************************
**
**    Module Name:
**      errors.h
**
**    Abstract:
**      Common application error list
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
**      $Log: errors.h,v $
**      Revision 1.3  2012/12/09 20:43:13  chuck
**      new error
**
**      Revision 1.3  2011/07/21 05:12:55  chuck
**      find dangler
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

#if !defined(_ERRORS_H_INCLUDED_)
#define _ERRORS_H_INCLUDED_

typedef enum
{
	Error_none,
	Error_Out_of_memory,
	Error_File_not_found,
	Error_File_read,
    Error_Overflow,
	Global_Error_Count
} Errors;

#endif // !defined(_ERRORS_H_INCLUDED_)
