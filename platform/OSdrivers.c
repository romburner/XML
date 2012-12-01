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
**      OSdrivers.c
**
**    Abstract:
**      OS dependent functions
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
**      $Log: OSdrivers.c,v $
**      Revision 1.3  2009/08/19 03:52:16  chuck
**      XML processing added for fun and profit
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

#include "OSdrivers.h"

#ifdef WIN32
#include "OSwindos.c"
#endif

#ifdef LINUX
#include "OSlinux.c"
#endif

#ifdef MACOSX
#include "OSmacosx.c"
#endif

#ifdef IPHONE
#include "OSmacosx.c"
#endif

char *fOsType(void)
{
#ifdef WIN32
	return "WIN32";
#endif
#ifdef LINUX
	return "Linux";
#endif
#ifdef MACOSX
	return "MacOSX";
#endif
#ifdef IPHONE
	return "iPhone";
#endif
}