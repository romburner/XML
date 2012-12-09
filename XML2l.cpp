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
**      XML2l.cpp
**
**    Abstract:
**      A tiny XML file reader test bed
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
**
** $NoKeywords: $
********************************************************************************
--*/

#include <iostream>
#include "xml.hpp"

static char	verboseFlag = 0;
static char	printFlag = 0;
static char	binFlag = 0;
static char	dumpFlag = 0;
static char	helpFlag = 0;
static char	findFlag = 0;
static char	makehFlag = 0;
static char	*makehString = 0;
static char	*findString = 0;
static char	basedFlag = 0;
static u8	*basedString = 0;
static i8	*binFile = 0;

static int xml_bufsize = 16384;

//static s32	desiredLen	= 20;
static s32	tabStop = 4;

static const char *helpString =
"Syntax: XML2l [options] input.xml [input1.xml..]\n\
options are:\n\
-d dump XML file\n\
-p print XML file\n\
-h write c header .h file\n\
-b write binary xml .bx file\n\
-f find a path in an XML file\n\
-m change buffer memory size\n\
-v verbose output\n\
-? help display\n\
";

/******************************************************************************\
# FUNCTION:
#
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
\******************************************************************************/
void showHelp(void)
{
	printf("%s",helpString);
}

/******************************************************************************\
# FUNCTION:
#
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
\******************************************************************************/
static void setScrollBuffer(u16 xsize, u16 ysize)
{
#ifdef WIN32
	HANDLE console = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(console, &info);
	info.dwSize.X = xsize;
	info.dwSize.Y = ysize;
	SetConsoleScreenBufferSize(console, info.dwSize);
	CloseHandle(console);
#endif
}

/******************************************************************************\
# FUNCTION:
#
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
\******************************************************************************/
void newExt(u8 *fileName, u8 *newExt)
{
	u32 i;
	u32 len = strlen((const char *)fileName);
	for(i = len-1; i>0; i--)
	{	if (fileName[i] == '.')
		{	strcpy((char *)fileName + i + 1, (const char *)newExt);
			return;
		}
	}
	fileName[len] = '.';
	strcpy((char *)fileName + len + 1, (const char *)newExt);
}

/******************************************************************************\
# FUNCTION:
#
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
\******************************************************************************/
void xml_2_h(Xml_node *node, i8 *container, u8 *filename)
{
	Xml_node *idNode, *valNode;
	u8 *hFormat[] = {	(u8*)"#define %s %s\n",
						(u8*)"#define %s\t%s\n",
						(u8*)"#define %s\t\t%s\n",
						(u8*)"#define %s\t\t\t%s\n",
						(u8*)"#define %s\t\t\t\t%s\n",
						(u8*)"#define %s\t\t\t\t\t%s\n"};
	FILE *handle;
	u8 outFile[258];
	s32 idx;

	strcpy((char *)outFile, (const char *)filename);
	newExt(outFile, (u8*)"h");
	handle = fOpen((char*)outFile, (char*)"w");
	xml_walkI(node, container)
	{	idNode  = node->getChild()->finds((i8*)"id");
		valNode = node->getChild()->finds((i8*)"val");
		if (idNode && valNode)
		{	idx = 5 - strlen((const char *)idNode->getValue())/tabStop;
			if (idx < 0)
				idx = 0;
			else if (idx > 5)
				idx = 5;
			sprintf((char *)outFile, (const char *)hFormat[idx], idNode->getValue(), valNode->getValue());
			fWrite(outFile, strlen((const char *)outFile), handle);
		}
	}
	fClose(handle);
}

/******************************************************************************\
# FUNCTION:
#
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
\******************************************************************************/
int readArgs(int argc, char **argv)
{
	char *input, ch;
	int i;
	for (i=1; i<argc; i++)
	{
		input = argv[i];
		ch = *input++;
		if (ch != '-')
			return i;
		ch = *input++;
		switch (ch)
		{
		case '?':
			helpFlag = 1;
			break;
		case 'p':
			printFlag = 1;
			break;
		case 'd':
			dumpFlag = 1;
			break;
		case 'b':
			binFlag = 1;
			i++;
			binFile = argv[i];
			break;
		case 's':
			basedFlag = 1;
			i++;
			basedString = (u8*)argv[i];
			break;
		case 'h':
			makehFlag = 1;
			i++;
			makehString = argv[i];
			break;
		case 'v':
			verboseFlag = 1;
			break;
		case 'f':
			findFlag = 1;
			i++;
			findString = argv[i];
			break;
		case 'm':
			i++;
			xml_bufsize = atoi(argv[i]);
			break;
		default:
			break;
		}

	}
	return i;
}


/******************************************************************************\
# FUNCTION:
#
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
\******************************************************************************/
int main(int argc, char **argv)
{
	int i = readArgs(argc, argv);
	Xml_head xmlHeader(xml_bufsize);
	setScrollBuffer(160, 5000);
	if (helpFlag)
		showHelp();

	for (; i<argc; i++)
	{	char *filename = argv[i];
		if (verboseFlag)
			printf("--- Start XML File %s\n", filename);
		Xml_node *node = xmlHeader.read(filename);
		if (!node)
		{	if (verboseFlag)
				printf("Error reading %s\n",filename);
			continue;
		}

		Xml_node *found = basedFlag ? node->findPath((i8 *)basedString) : node->getChild();
		if (found)
		{	if (findFlag)
			{	found = found->findPath((i8 *)findString);
				if (found)
					printf("found '%s' = '%s'\n", findString, found->getValue());
			}
			if (binFlag)
			{	found->savebin(binFile);
			}
			if (makehFlag && makehString)
			{	xml_2_h(found->getChild(), (i8*)makehString, (u8*)filename);
			}
		}
		else
		{
			if (verboseFlag)
				printf("Node was not found\n");
		}

		if (dumpFlag)
		{	node->getSibling()->dump(0);
			if (verboseFlag)
				printf("--- Data ----\n");
			node->getChild()->dump(0);
		}
		if (printFlag)
		{	node->getChild()->show(0);
		}
		node->release();
		if (verboseFlag)
			printf("--- Done XML File %s\n\n", filename);
	}
}