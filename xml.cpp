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
**      XML.cpp
**
**    Abstract:
**      A tiny XML file reader
**
**    Product Name:  
**      Systems Library
**
**    Version:
**      $Revision: 1.19 $
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
#define _XML_CPP_INCLUDED_

#define	U8

#include "xml.hpp"
//#include "assert.h"

/******************************************************************************\
# FUNCTION:
#	debug
# DESCRIPTION:
#	Debugging routine
# ARGUMENTS:
#
# RETURNS:
#
\******************************************************************************/
inline static void debug(int tval, char *str)
{
#if _DEBUG
	if (!tval)
		printf("**Error on line %d: %s\n", Xml_head::head[Xml_head::headDepth]->getLine(), str);
#endif
}

/*############################################################################*/
/*                                                                            */
/*  Xml_node                                                                  */
/*                                                                            */
/*############################################################################*/
static const int preLen	= 5;
const i8 * const Xml_node::predef[]={"lt", "gt", "amp", "apos", "quot"};
const i8   Xml_node::preval[]	=	{'<',  '>',  '&',   '\'',   '\"'};
const i8 * Xml_node::CDATAstart	= "<![CDATA[";
const i8 * Xml_node::CDATAend	= "]]>";

/******************************************************************************\
# FUNCTION:
#	Xml_node default constructor
# DESCRIPTION:
#	Builds one of them critters Xml_node
# ARGUMENTS:
#
# RETURNS:
#
\******************************************************************************/
Xml_node::Xml_node(void)
{
	sibling = 0;
	child = 0;
	tag = 0;
	value = 0;
	flags = 0;
	status = 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node default constructor
# DESCRIPTION:
#	Builds one of them critters Xml_node
# ARGUMENTS:
#
# RETURNS:
#
\******************************************************************************/
Xml_node::Xml_node(int sizeT, const i8 *tagVal,int sizeV, const i8 *value)
{
	setTag(sizeT,tagVal);
	setValue(sizeV, value);
}

/******************************************************************************\
# FUNCTION:
#	Xml_node default destructor
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
#
\******************************************************************************/
Xml_node::~Xml_node(void)
{
	mFree(tag);					// Free the tag field
	mFree(value);				// Free the value field
}

/******************************************************************************\
# FUNCTION:
#	setTag
# DESCRIPTION:
#	Set the tag field for the xml node
# ARGUMENTS:
#
# RETURNS:
#
\******************************************************************************/
int Xml_node::setTag(int size, const i8 *tagVal)
{
	if (tag)
		mFree(tag);
#ifdef _DEBUG
	if (tagVal && size != (int)(strlen(tagVal) + 1))
		size = (int)strlen(tagVal) + 1;
#endif
	if (size && (tag = (i8*)mAlloc(size+1)))
	{	memcpy(tag, tagVal, size);	// Move it into place
		return 1;
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	setValue
# DESCRIPTION:
#	
# ARGUMENTS:
#
# RETURNS:
#
\******************************************************************************/
int Xml_node::setValue(int size, const i8 *theVal)
{
	int msize,osize;
#ifdef _DEBUG
	if (theVal && size != (int)(strlen(theVal) + 1))
		size = (int)strlen(theVal) + 1;
#endif
	osize = value ? strlen(value) : 0;
	msize = osize + size;
	if (!msize)
		return 0;
	i8 *svalue = value;
	if (value = (i8*)mAlloc(msize+1))
	{	if (osize)
		{
			memcpy(value, svalue, osize);	// move old
			mFree(svalue);
		}
		memcpy(value+osize, theVal, size);	// Move it into place	
		return 1;
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::setBlink
# DESCRIPTION:
#	Set the back link for the linked list
# ARGUMENTS:
#
# RETURNS:
#
\******************************************************************************/
inline Xml_node **	Xml_node::setBlink(Xml_node **bLink)
{
	*bLink = this;
	return &sibling;
}

/******************************************************************************\
# FUNCTION:
#	hexChar
# DESCRIPTION:
#	Converts on ascii character to it's hex equivalent
# ARGUMENTS:
#	i8 ch	input character
# RETURNS:
#	4 bit binary number, -1 if not hex
\******************************************************************************/
inline i8 Xml_node::hexChar(i8 ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	return -1;
}

/******************************************************************************\
# FUNCTION:
#	hexVert
# DESCRIPTION:
#	Converts an hex ascii stream into 64 bit hex
# ARGUMENTS:
#	i8 *val
# RETURNS:
#	u64 binary conversion
\******************************************************************************/
u64 Xml_node::hexVert(i8 *val)
{
	u64 ans = 0;
	i8 ch;
	if (*val == '0')			// Skip leading 0x
	{	++val;
		if ((*val == 'x') || (*val == 'X'))
			++val;
	}
	while (ch = *val++)
	{	ans <<= 4;
		ch = hexChar(ch);
		if (ch == -1)			// Non hex character error
			break;
		ans += ch;
	}
	return ans;
}

u32 hexNumber(char *str)
{
	u32 number = 0;
	for ( ; isxdigit(*str) ; str++)
	{	number = number<<4;
		number += isdigit(*str) ? *str - '0' :tolower(*str) - 'a' + 10;
	}
	return number;
}

void binval(i8 *&buf, u32 val)
{
	if (val > 0xFF)
	{	*buf++ = (i8)((val >> 8) & 0xFF);
	}
	*buf++ = (i8)(val & 0xFF);
}

/******************************************************************************\
# FUNCTION:
#	predefs
# DESCRIPTION:
#	Locate a predefined replacement expression
# ARGUMENTS:
#	i8 *buf		Ptr to input buffer ptr 
# RETURNS:
\******************************************************************************/
i8 Xml_node::predefs(i8 *&buf)
{
	i8 *pIn = buf;
	u32 val;
	if (*pIn == '#')
	{	pIn++;
		val = (*pIn == 'x') || (*pIn == 'X') ? hexNumber(++pIn) : atoi(pIn);
	}
	else
	{	for (int i=0; i<preLen; i++)
			if (!strcmp(predef[i],pIn))
			{	val = preval[i];
				goto terminate;
			}
		return 0;
	}
terminate:
	binval(buf,val);
	return 1;
}

/******************************************************************************\
# FUNCTION:
#	subfind
# DESCRIPTION:
#	Locate a %string; or &string; expression
# ARGUMENTS:
#	i8 *buf		Ptr to input buffer ptr 
# RETURNS:
\******************************************************************************/
i8 * Xml_node::subfind(i8 *buf)
{
	Xml_node *node = Xml_head::head[Xml_head::headDepth]->getROOT()->sibling;
	xml_walk(node)
	{	if ((node->flags & Xml_head::FLAGentity) &&
			(strcmp(node->tag, buf)==0))
			return node->value;
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	NextChar
#
# DESCRIPTION:
#	returns next XML stream character
#
# ARGUMENTS:
#	i8 * &buf		Ptr to input buffer ptr 
#
# RETURNS:
#	i8 char from stream
\******************************************************************************/
i8 Xml_node::LookChar(i8 * &buf)
{
	if (buf && *buf)		// If there is a pointer and a value at the pointer
		return *buf;		// Return the next value in stream
	if (Xml_head::stackDepth > 0)
	{	// FIXME, this is SO SO WRONG!!!! make recursive.
		i8 *dwn = Xml_head::inputStack[Xml_head::stackDepth-1]; // old context
		if (!*dwn)
			debug(0,(i8*)"Fix lookahead to recurse!");
		return *dwn;
	}
	return 0;				// Thats all folks, empty now
}

/******************************************************************************\
# FUNCTION:
#	NextChar
#
# DESCRIPTION:
#	returns next XML stream character
#
# ARGUMENTS:
#	i8 * &buf		Ptr to input buffer ptr 
#
# RETURNS:
#	i8 char from stream
\******************************************************************************/
i8 Xml_node::NextChar(i8 * &buf)
{
	if (buf && *buf)
		return *buf++;
	if (Xml_head::stackDepth == 0)
		return 0;		// Empty, click, click, click, bang!!!
	buf = Xml_head::inputStack[--Xml_head::stackDepth];
	return NextChar(buf);
}

/******************************************************************************\
# FUNCTION:
#	xml_nextPath
# DESCRIPTION:
#	extract next name in buf to path!
#
# ARGUMENTS:
#	i8 *buf
#	i8 *path
#	u32 len
# RETURNS:
\******************************************************************************/
void Xml_node::nextPath(i8 * &buf, i8 *path, u32 len)
{
	i8 ch;
	for (ch = NextChar(buf);
		 ch != 0 && ch != '/' && ch != '\\' && len;
		 ch = NextChar(buf), len--)
	{	*path++ = ch;
//		if (ch == ':') break;
	}
	*path = 0;
//	return ch ? buf : buf-1;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::isRepElement
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
\******************************************************************************/
int Xml_node::isRepElement(i8 ch)
{
	return ((ch == '?') || (ch == '*') || (ch == '+')) ? 1 : 0;
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::isWhite
#
# DESCRIPTION:
#
# ARGUMENTS:
#	i8 ch		character to test for whitespace-ness

 RETURNS:
*******************************************************************************/
int Xml_node::isWhite(i8 ch)
{
	return ((ch == SPACE) || (ch == TAB) || (ch == CR) || (ch == LF)) ? -1 : 0;
}

/*******************************************************************************
 # FUNCTION:
 #	Xml_node::isSeparator
 #
 # DESCRIPTION:
 #
 # ARGUMENTS:
 #	i8 ch		character to test for whitespace-ness
 
 RETURNS:
 *******************************************************************************/
int Xml_node::isSeparator(i8 ch)
{
	return ((ch == SPACE) || (ch == TAB) || (ch == CR) || (ch == LF) || (ch == ',')) ? -1 : 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::White
# DESCRIPTION:
#
# ARGUMENTS:
#	i8 *buf
# RETURNS:
#	i8 * buffer pointer to next character past whitespace
\******************************************************************************/
void Xml_node::White(i8 * &buf, i8 *wVal, u32 &len)
{
	i8 ch;
	do
	{	ch = NextChar(buf);
		if (wVal)
			{ *wVal++ = ch; len++; }
		if (ch == LF)
			Xml_head::head[Xml_head::headDepth]->nextline();
	}
	while (isWhite(ch));
	if (wVal)
		*wVal = 0;	//	terminate string
	if (buf && *buf)	// Hmmm.
		--buf;
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::skipComment
#
# DESCRIPTION:
#	Skips over comments
#
# ARGUMENTS:
#	i8 * &buf
#
# RETURNS:
#	int skipped
*******************************************************************************/
int Xml_node::skipComment(i8 * &buf)
{
	if (memcmp(buf, Xml_head::CMNTstart, strlen(Xml_head::CMNTstart)))
		return 0;			// No comment found!
	buf += sizeof(Xml_head::CMNTstart);
	// count input lines here properly... FIXME FIXME FIXME!~!!!!!!
	i8 * endo = strstr(buf, Xml_head::CMNTend);
	if (endo)
	{	endo += sizeof(Xml_head::CMNTend)-1;
		buf = endo;
		return 1;
	}
	endo = buf + strlen(buf);
	buf = endo;
	return 0;		// Error!!! no end comment
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::WhiteSpace
#
# DESCRIPTION:
#	Skips all types of whitespace, including comments
#
# ARGUMENTS:
#	i8 *buf
#
# RETURNS:
#	i8 *buf
*******************************************************************************/
void Xml_node::WhiteSpace(i8 * &buf)
{
	u32 len=0;
	WhiteSpace(buf, 0, len);
}

void Xml_node::WhiteSpace(i8 * &buf, i8 *wVal, u32 &len)
{
	do
		White(buf,wVal,len);
	while (skipComment(buf));
}

/*******************************************************************************
 # FUNCTION:
 #	Xml_node::skipValue
 #
 # DESCRIPTION:
 #	Skips over stuff till a separator
 #
 # ARGUMENTS:
 #	i8 * &buf
 #
 # RETURNS:
 #	int last thing returned
 *******************************************************************************/
int Xml_node::skipValue(i8 * &buf)
{
	if (!buf)
		return 0;
	i8 ch;
	do
	{
		ch = *buf++;
	} while (ch && !isSeparator(ch));
	if (!ch) buf--;
	do
	{
		ch = *buf++;
	} while (ch && isSeparator(ch));
	buf--;
	return ch;
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::gatherep
#
# DESCRIPTION:
#	Gathers a replacement macro value
#
# ARGUMENTS:
#	i8 *buf
#
# RETURNS:
#	i8 *buf
*******************************************************************************/
int Xml_node::gatherep(i8 * &buf, i8 *outp, u32 maxlen, u32 &len)
{
	i8 *str = buf;
	i8 tk = LookChar(buf);
	len = 1;
	if (tk == '&' || tk == '%')
		NextChar(buf);
    for (tk = NextChar(buf);
    	 tk && maxlen && (tk!=';') && (tk!='&') && !isWhite(tk);
		maxlen--, len++, tk = NextChar(buf))
   		*outp++ = tk;			// save it in output
	if (tk!=';')
	{	buf = str;
		return 0;
	}
   	*outp = 0;
	return 1;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::uniStrConv
#
# DESCRIPTION:
#	change UTF8 strings to 16 bit unicode
#
# ARGUMENTS:
#	i8 *buf		Buffer of unicode characters
#
# RETURNS:
\******************************************************************************/
void Xml_node::uniStrConv(i8 *buffer, u32 &len)
{
	u32	flag = 0;
	if (Xml_head::UTF8xlat)
	{	for (u32 i = 0; i < len; i++)
			if (buffer[i] < 0)
				flag = 1;
#if	WIN32
		if (flag)
		{	u16 xlatStr[1025];
			MultiByteToWideChar(CP_UTF8, 0, buffer, -1, (LPWSTR)xlatStr, 1024);
			memcpy(buffer, (i8*)xlatStr, wcslen((const wchar_t *)xlatStr) * 2 + 2);
		}
#endif
	}
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::NextText
#
# DESCRIPTION:
#	returns next XML stream text string
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#	i8 *tokVal		output token value
#	u32 &len
# RETURNS:
#	i8 char token	
#					=, >, <? = ?, </ = /, /> = ]
\******************************************************************************/
void Xml_node::NextText(i8 * &buf,i8 *tokVal,u32 maxlen,u32 &len)
{
	i8 tk,ch;
	u32 replen;
	len = 1;
	maxlen -= 2;
	for (tk = LookChar(buf);
    	 tk && maxlen && !((tk == '/')&&(*buf == '>'));
    	 len++, maxlen--, tk = LookChar(buf))
	{	switch(tk)
		{
		case '&':
		case '%':
			if (gatherep(buf, tokVal, maxlen, replen))
			{	if (!predefs(tokVal))
				{
					len--;					// Subtract out the & char
					Xml_head::inputStack[Xml_head::stackDepth++] = buf; // Save
					buf = subfind(tokVal);
				}
			}
			else
			{	*tokVal++ = NextChar(buf); // save it in output
//				debug(tk!='&',"& used wrong -> &amp;");
			}
			break;
		case '<':
			if (NextCDATA(buf,tokVal,maxlen,len))
			{	maxlen -= len;
				continue;
			}
			else if (skipComment(buf))
			{	len--;
				continue;
			}
			else
				goto terminate;
		default:
			if (tk == LF)
				Xml_head::head[Xml_head::headDepth]->nextline();
			*tokVal++ = NextChar(buf);			// save it in output
		}
	}
terminate:
	if (!maxlen)
	{	do
		{	ch = NextChar(buf);	// Skip through the daisies.
			len++;
		} while (ch && ch!='<');
		if (ch=='<')
			buf--;
#ifdef _DEBUG
		i8 buffy[256];
		sprintf(buffy,"Field is %d bytes long, memory is %d",len,Xml_head::head[Xml_head::headDepth]->bufsize);
		debug(0, buffy);
#endif
	}
   	*tokVal = 0;
}

void Xml_node::NextText(i8 * &buf, u32 &len)
{
	NextText(buf, Xml_head::head[Xml_head::headDepth]->getbuffer(),
		Xml_head::head[Xml_head::headDepth]->bufsize, len);
	uniStrConv(Xml_head::head[Xml_head::headDepth]->getbuffer(), len);
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::NextToken
#
# DESCRIPTION:
#	returns next XML stream token
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#	i8 *tokVal		output token value
#	u32 &len
# RETURNS:
#	i8 char token	
#					=, >, <? = ?, </ = /, /> = ]
\******************************************************************************/
i8 Xml_node::NextToken(i8 * &buf,i8 *tokVal,u32 maxlen,u32 &len)
{
	i8 ch,tk;
	len = 1;
	tk = ch = LookChar(buf);	// Read the next token character
	switch (ch)
	{
	case 0:
	case '=':
	case '[':
	case '>':
		NextChar(buf);
		return ch;				// Return one char tokens =, >
	case '<':
		if (NextCDATA(buf,tokVal,maxlen,len))
			return 'c';			// return the quoted string
		if (skipComment(buf))
			return ' ';			// Return whitespace?
		NextChar(buf);
		tk = LookChar(buf);			// Get one lookahead character
		if ((tk == '?') ||		// is it one of the two char tokens?
			(tk == '!') ||
			(tk == '/'))
		{	NextChar(buf);
			return tk;			// Return two char tokens </ <? <!
		}
		return ch;
	case ']':
		NextChar(buf);
		tk = LookChar(buf);		// Get one lookahead character
		if (tk == '>')			// is it one of the two char tokens?
		{	NextChar(buf);
			return '}';			// ]} token
		}
		return ']';				// ] token
	case '/':
		NextChar(buf);
		tk = LookChar(buf);		// Get one lookahead character
		if (tk == '>')			// is it one of the two char tokens?
		{	NextChar(buf);
			return '_';			// Return two char token /> as _
		}
		return '\\';			// Return / char as back slash
	case '?':					// ?>
		NextChar(buf);
		tk = LookChar(buf);		// Get one lookahead character
		if (tk == '>')			// is it one of the two char tokens?
		{	NextChar(buf);
			return '^';			// Mark end of ?> block
		}
		return 'x';				// Some kind of error here?
	case '%':					// Replacement chars
	case '&':
		NextChar(buf);
		tk = LookChar(buf);			// Get one lookahead character
		if (isWhite(tk))
			return ch;			// Return one char tokens %, &
		if (gatherep(buf, tokVal, maxlen,len))
		if (predefs(tokVal))	// Check for a predef
			break;
		Xml_head::inputStack[Xml_head::stackDepth++] = buf; // Save
		buf = subfind(tokVal);
		WhiteSpace(buf);
		return NextToken(buf, tokVal, maxlen, len);
	case '"':					// Double and single
	case '\'':					// Quoted strings
		NextQuote(buf,tokVal,maxlen,ch,len);
		return ch;				// Return a string token
	default:
		if (isWhite(ch))
		{	WhiteSpace(buf);
			return ' ';				// Return whitespace
		}
	}
	return 'x';
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::NextQuote
#
# DESCRIPTION:
#	returns next XML stream token
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#	i8 *tokVal		output token value
#	u32 &len
# RETURNS:
#	i8 char token	
#					=, >, <? = ?, </ = /, /> = ]
\******************************************************************************/
void Xml_node::NextQuote(i8 * &buf,i8 *tokVal,u32 maxlen,i8 tokQuote,u32 &len)
{
	i8 tk;
	u32 replen;
	len = 1;
	tk = NextChar(buf);			// Get first one
	if (tk == tokQuote)
		tk = NextChar(buf);		// Get next one
	for (;
		 (tk != tokQuote) && tk && maxlen;
		 len++, maxlen--, tk = NextChar(buf))
	{	 if (tk=='&' || tk=='%')
		 {	if (gatherep(buf, tokVal, maxlen,replen))
				if (predefs(tokVal))
					continue;
				else
				{	Xml_head::inputStack[Xml_head::stackDepth++] = buf; // Save
					buf = subfind(tokVal);
					tk = NextChar(buf);		// Get next one
				}
//			else
//				debug(0,"& used wrong -> &amp;");
		 }
		*tokVal++ = tk;		// save it in output
	}
	debug(maxlen,(i8*)"string is too long!");
	*tokVal = 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::NextLabel
#
# DESCRIPTION:
#	returns next XML stream label
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#	i8 *tokVal		output token value
#	u32 &len
# RETURNS:
#	i8 char token	
#					=, >, <? = ?, </ = /, /> = ]
\******************************************************************************/
void Xml_node::NextLabel(i8 * &buf,i8 *tagVal,u32 maxlen,u32 &len)
{
	i8 ch;
	len = 1;
	for (ch = NextChar(buf);
		ch && !isWhite(ch) && (ch != '>') && (ch != '=') && !((ch == '/') && (*buf == '>')) && maxlen;
		 len++, ch = NextChar(buf), maxlen--)
		*tagVal++ = ch;		// save it in output
	if (ch && (len > 1))
		buf--;
	*tagVal = 0;		// Cap the tag off
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::NextList
#
# DESCRIPTION:
#	returns next XML stream list element
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#	i8 *tagVal		output tag value
#
# RETURNS:
#	i8 char tag ender
*******************************************************************************/
i8 Xml_node::NextList(i8 * &buf,i8 *tagVal,u32 maxlen,u32 &len)
{
	i8 ch;
	len = 1;
	for (ch = NextChar(buf);
		 (ch != ',') && (ch != ')') && (ch != '>') && (ch != '?') &&
		 (ch != '|') && (ch != '+') && (ch != '*') && maxlen ;
		 len++, ch = NextChar(buf), maxlen--)
		*tagVal++ = ch;		// save it in output
	*tagVal = 0;		// Cap the tag off
	buf--;
	if (isWhite(ch))
		return ' ';
	return ch;
}

i8 Xml_node::NextList(i8 * &buf, u32 &len)
{
	return NextList(buf, Xml_head::head[Xml_head::headDepth]->getbuffer(),
		Xml_head::head[Xml_head::headDepth]->bufsize,len);
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::NextTag
#
# DESCRIPTION:
#	returns next XML stream tag
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#	i8 *tagVal		output tag value
#
# RETURNS:
#	i8 char tag ender
*******************************************************************************/
i8 Xml_node::NextTag(i8 * &buf,i8 *tagVal,u32 maxlen, u32 &len)
{
	i8 ch = NextToken(buf, tagVal, maxlen, len);
	if (ch != 'x')
		return ch;
	NextLabel(buf, tagVal, maxlen, len);
	return 'x';
}

i8 Xml_node::NextTag(i8 * &buf, u32 &len)
{
	return NextTag(buf, Xml_head::head[Xml_head::headDepth]->getbuffer(),
		Xml_head::head[Xml_head::headDepth]->bufsize,len);
}


/******************************************************************************\
# FUNCTION:
#	Xml_node::NextCDATA
#
# DESCRIPTION:
#	returns next CDATA field
#
# ARGUMENTS:
#	i8 *&buf		Ptr to input buffer ptr 
#	i8 *tokVal		output token value
#	u32 &len
# RETURNS:
#	int		1 true, 0 false
\******************************************************************************/
inline int Xml_node::NextCDATA(i8 * &buf,i8 *&tokVal,u32 maxlen,u32 &len)
{
	if (memcmp(buf, CDATAstart, sizeof(CDATAstart)-1))
		return 0;
	buf += strlen(CDATAstart);
	i8 *end = strstr(buf, CDATAend);
	len = end - buf;
	if (len >= maxlen)
	{	len = maxlen-1;
		debug(0,(i8*)"Quoted string too long for buffer");
	}
	memcpy(tokVal, buf, len);
	tokVal += len++;
	*tokVal = 0;	// Cap off string!
	buf = end + sizeof(CDATAend)-1;
	return 1;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::NextToken
#
# DESCRIPTION:
#	returns next XML stream token
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#
# RETURNS:
#	i8 char token
\******************************************************************************/
i8 Xml_node::NextToken(i8 * &buf, u32 &len)
{
	return NextToken(buf,Xml_head::head[Xml_head::headDepth]->getbuffer(),
		Xml_head::head[Xml_head::headDepth]->bufsize,len);
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::NextTokenW
#
# DESCRIPTION:
#	returns next XML stream token
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#
# RETURNS:
#	i8 char token
\******************************************************************************/
i8 Xml_node::NextTokenW(i8 * &buf, u32 &len)
{
	WhiteSpace(buf);
	return NextToken(buf,len);
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::NextTokenHead
#
# DESCRIPTION:
#	returns next XML stream token
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#
# RETURNS:
#	i8 char token
\******************************************************************************/
i8 Xml_node::NextTokenHead(i8 * &buf, u32 &len)
{
	i8 ch = NextChar(buf);
	if (ch == '[')
		return ch;
	buf--;
	return NextTag(buf, len);
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::NextTagW
#
# DESCRIPTION:
#	returns next XML stream tag, after skipping whitespace
#
# ARGUMENTS:
#	i8 * &buffer		Ptr to input buffer ptr 
#	i8 *tagVal		output tag value
#
# RETURNS:
#	i8 char tag ender
*******************************************************************************/
i8 Xml_node::NextTagW(i8 * &buf, u32 &len)
{
	WhiteSpace(buf);
	return NextTag(buf, len);
}


/******************************************************************************\
# FUNCTION:
#	vallist
# DESCRIPTION:
#	Extract value lists in the form tag="value" tag="value"
# ARGUMENTS:
#	i8 *buf
#	Xml_node **bLink
#	i8 type
# RETURNS:
\******************************************************************************/
int Xml_node::vallist(i8 * &buf, Xml_node **bLink)
{
	i8 ch,*old,*oldr;
	u32 len,len1;
	i8 *buffer = Xml_head::head[Xml_head::headDepth]->getbuffer();
	for (old = buf, ch = NextTagW(buf, len);	// Get first token
		 ch == 'x';								// till option list stops
		 old = buf, ch = NextTagW(buf, len))	// Get next token
	{	oldr = buf;
		ch = NextTokenW(buf, len1);				// Get the "=" token
		if (ch == '=')
		{	Xml_node *newNode = new Xml_node(len,buffer);		// Allocate a node for it.
			bLink = newNode->setBlink(bLink);
			ch = NextTokenW(buf, len);			// Get the quoted value
			debug(ch == '"' || ch == '\'',(i8*)"expecting a quoted string");
			newNode->setValue(len, buffer);
			newNode->setFlag(Xml_head::FLAGequl);
		}
		else
		{	buf = oldr;
			setValue(strlen(buffer)+1, buffer);
			return 0;
		}
	}
	if ((ch == '^'))
	{	buf = old;
		return 0;
	}
	if ((ch == '>' && *buffer == '?') || (ch == '/' && *buf == '>'))
	{	buf = old;
		return 1;
	}
	if (ch == '>')
		return 1;
	if (ch == '_')		// /> token
		return 0;
	debug(1,(i8*)"List ended badly");
	return 1;
}

/******************************************************************************\
# FUNCTION:
#	parse
# DESCRIPTION:
#
# ARGUMENTS:
#	i8 *buf
#	Xml_node **bLink
#	i8 type
# RETURNS:
\******************************************************************************/
void Xml_node::parse(i8 *&buf, Xml_node **bLink, int depth)
{
	if (depth < 0) return; // Exit if overflow
	i8 ch,*old;
	u32 len; //,len1
	Xml_node *anode, *listnode, *parsenode, *newNode = this ;
	i8 *buffer = Xml_head::head[Xml_head::headDepth]->getbuffer();
	for(old=buf, ch=NextTokenW(buf, len);// Get first token
		ch;
		old=buf, ch=NextTokenW(buf, len))
	{	switch(ch)
		{
		case '<':
			ch = NextTag(buf, len);		// Get next tag
			newNode = new Xml_node(len, buffer, 0, 0);
			bLink = newNode->setBlink(bLink);
			ch = NextToken(buf, len);		// Get the next token
			switch (ch)
			{
			case '>':						// Opening a deeper context
				newNode->parse(buf, &newNode->child,--depth);// Walk child nodes
				break;
			case ' ':						// Must be an option list
				parsenode = listnode = 0;
				if (newNode->vallist(buf, &listnode)) // Walk child nodes
					newNode->parse(buf, &parsenode,++depth);
				if (listnode)
				{
					for (newNode->child=anode=listnode; anode->sibling; anode = anode->sibling);
					anode->sibling = parsenode;
				}
				break;
			case '_':
				break;
			case ']':						// closed tag
//				ch = NextToken(&buf, &len);	// skip token
				break;
			default:
				debug(0,(i8*)"add option here");
			}
			break;
		case '/':							// </ end tag
			ch = NextTag(buf, len);		// Get next tag
			if (ch == '>')				// Check for unnamed closes
			{	debug(AllowNoCloseTag,(i8*)"missing closing tag");	// Warn for naked close???
			}
			else
			{	debug(strcmp(tag, buffer) == 0,(i8*)"Unmatched closing tags");
				ch = NextToken(buf, len);		// Get the > token
				debug(ch == '>',(i8*)"should close with '>'");
			}
//			return buf;						// Recurse back to lower level
		case '_':							// /> ending tag! debug(0,"Wrong close tag");
		case 0:
			return;							// Done with file
		case '>':
			break;							// Stay at this level, wait for /> close tag
		case '&':
			debug(0,(i8*)"Ampersand used wrong");
			break;
		case '?':
			debug(0,(i8*)"<? tag used wrong");
			break;
		default:
			i8 buffy[256];
			sprintf(buffy, "what is this '%c' char? ", ch);
			debug(0,buffy);
		case ']':
		case '[':
		case 'x':	// Text data field
			buf = old;
			NextText(buf, len);
		case 'c':	// CDATA field
		case '\'':	// set quoted fields
		case '"':
			setValue(len, buffer);
			break;
		}
	}
	return;
}

/******************************************************************************\
# FUNCTION:
#	Element
# DESCRIPTION:
#
# ARGUMENTS:
#	i8 *buf
#	Xml_node **bLink
# RETURNS:
\******************************************************************************/
void Xml_head::element(i8 * &buf, Xml_node **bLink)
{
	i8 tk,ch;
	u32 len;
	Xml_node *eNode = NULL;
	tk = Xml_node::NextChar(buf);			// Get next char
	debug(tk == '(',(i8*)"expecting a ( character");
	do
	{	Xml_node::WhiteSpace(buf);		// and skip trailing space
		if (*buf == '(')				// Deeper context?
		{	element(buf, bLink);	// Parse an element list
			if (ch = *buf++);			// Get next char
			else buf--;
		}
		else
		{	ch = Xml_node::NextList(buf, len);
			eNode = new Xml_node(len, head[headDepth]->getbuffer(),0,0);
			bLink = eNode->setBlink(bLink);
			if (eNode->isRepElement(*buf))
				eNode->setStatus(eNode->NextChar(buf));		// Get trailing char
			eNode->WhiteSpace(buf);			// and skip trailing space
			ch = eNode->NextChar(buf);		// Get next char
		}
	} while ((ch == ',') || (ch == '|'));
	if (eNode->isRepElement(*buf))
	{	eNode->setStatus(eNode->NextChar(buf));		// Get trailing char
		ch = *buf;
	}
	debug((ch == ')')||(ch == '>'),(i8*)"expecting a ) character");
}

/******************************************************************************\
# FUNCTION:
#	BinNode
# DESCRIPTION:
#
# ARGUMENTS:
#	i8 *buf
#	Xml_node *node
#	int depth
#	int flag
# RETURNS:
\******************************************************************************/
void Xml_node::BinNode(i8 *buf, int depth, int flag)
{
// FIXME FIXME!!! make into binary version
	if (depth > MAX_XML_NEST)
		return;
	if (flag & USETABS)					// Tabs or spaces?
		memset(buf, '\t', depth);		// Use tabs
	else
	{	depth *= Xml_head::TabSet;			// Tab out output w/ spaces
		memset(buf, ' ', depth);
	}
	char *outPtr = buf + depth;				// Move past white space
	flag & USEEND ?
		sprintf(outPtr,"</%s>\n", tag) :	// Ending tag
		value ?
			sprintf(outPtr,"<%s>%s</%s>\n", tag, value, tag) :
			sprintf(outPtr,"<%s>\n", tag);// Starting tag
}

/******************************************************************************\
# FUNCTION:
#	SaveNode
# DESCRIPTION:
#
# ARGUMENTS:
#	i8 *buf
#	Xml_node *node
#	int depth
#	int flag
# RETURNS:
\******************************************************************************/
void Xml_node::SaveNode(i8 *buf, int depth, int flag)
{
	if (depth > MAX_XML_NEST)
		return;
	if (flag & USETABS)					// Tabs or spaces?
		memset(buf, '\t', depth);		// Use tabs
	else
	{	depth *= Xml_head::TabSet;			// Tab out output w/ spaces
		memset(buf, ' ', depth);
	}
	char *outPtr = buf + depth;				// Move past white space
	flag & USEEND ?
		sprintf(outPtr,"</%s>\n", tag) :	// Ending tag
		value ?
			sprintf(outPtr,"<%s>%s</%s>\n", tag, value, tag) :
			sprintf(outPtr,"<%s>\n", tag);// Starting tag
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::FormatNode
# DESCRIPTION:
#
# ARGUMENTS:
#	i8 *buf
#	Xml_node *node
#	int depth
# RETURNS:
\******************************************************************************/
void Xml_node::FormatNode(i8 *buf, int depth)
{
	if (depth > MAX_XML_NEST)
		return;
	depth *= Xml_head::TabSet;				// Tab out output w/ spaces
	memset(buf, ' ', depth);
	char *outPtr = buf + depth;
	char setv = flags & Xml_head::FLAGequl ? '=' : '-';
	unsigned int maxlen = (tag ? strlen(tag)+depth : depth)+9;
	maxlen += value ? strlen(value) : 0;
	if (maxlen > Xml_head::head[Xml_head::headDepth]->bufsize)
	{	sprintf(outPtr,"%s -> **Error: value field is too large(%i), increase buffer memory!!\n",tag,maxlen);
		return;
	}
	value ?
		status ?
			sprintf(outPtr,"%s %c> '%s' '%c'\n", tag, setv, value, status) :
			sprintf(outPtr,"%s %c> '%s'\n", tag, setv, value)	:
		status ?
			sprintf(outPtr,"%s : '%c'\n", tag, status) :
			sprintf(outPtr,"%s\n", tag);
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::FormatNodeEx
# DESCRIPTION:
#
# ARGUMENTS:
#	i8 *buf
#	Xml_node *node
#	int depth
# RETURNS:
\******************************************************************************/
void Xml_node::FormatNodeEx(i8 *buf, int depth)
{
char *outPtr;
char openFlag;
	if (depth > MAX_XML_NEST)
		return;
	depth *= Xml_head::TabSet;				// Tab out output w/ spaces
	memset(buf, ' ', depth);
	outPtr = buf + depth;
	openFlag = (flags && Xml_head::FLAGopen) ? '-' : '+';
	sprintf(outPtr,"(%c) ",openFlag);
	outPtr = outPtr + strlen(outPtr);
	if (value)
		status ?
			sprintf(outPtr,"%s -> '%s' '%c'\n", tag, value, status) :
			sprintf(outPtr,"%s -> '%s'\n", tag, value);
	 else
		status ?
			sprintf(outPtr,"%s : '%c'\n", tag, status) :
			sprintf(outPtr,"%s\n", tag);
}


/******************************************************************************\
# FUNCTION:
#    Xml_node::saved
## DESCRIPTION:
#	save a node parse into an XML file
## ARGUMENTS:
#	Xml_node *node
#	FILE	*fid
#	s32		depth
# RETURNS:
#	int status
\******************************************************************************/
int Xml_node::saved(FILE *fid, s32 depth)
{
	i8 *buffer = Xml_head::head[Xml_head::headDepth]->getbuffer();
	if (depth > MAX_XML_NEST)
		return 1;
	Xml_node *node = this;
	xml_walk(node)				// Walk through all the nodes
	{	node->SaveNode(buffer, depth, USETABS);	// Format up one line
		fWrite(buffer, strlen(buffer), fid);
		if (node->child)							// Check for deeper levels
		{	node->child->saved(fid, depth+1);	// Run at a deeper position
			node->SaveNode(buffer, depth, USETABS|USEEND);	// Format up close tag
			fWrite(buffer, strlen(buffer), fid);
		}
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#    Xml_node::savebind
## DESCRIPTION:
#	save a node tree into a binary format XML file
## ARGUMENTS:
#	Xml_node *node
#	FILE	*fid
#	s32		depth
# RETURNS:
#	int status
\******************************************************************************/
int Xml_node::savebind(FILE *fid, s32 depth)
{
	i8 *buffer = Xml_head::head[Xml_head::headDepth]->getbuffer();
	if (depth > MAX_XML_NEST)
		return 1;
	Xml_node *node = this;
	xml_walk(node)				// Walk through all the nodes
	{	node->BinNode(buffer, depth, USETABS);	// Format up one line
		fWrite(buffer, strlen(buffer), fid);
		if (node->child)							// Check for deeper levels
		{	node->child->savebind(fid, depth+1);	// Run at a deeper position
			node->BinNode(buffer, depth, USETABS|USEEND);
			fWrite(buffer, strlen(buffer), fid);
		}
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::field
#
# DESCRIPTION:
#	Pull apart strings into character separated fields
#
# ARGUMENTS:
#	i8 *codeStr
#	i8 sep
#	i8 *outBuf
#
# RETURNS:
\******************************************************************************/
i8 * Xml_node::field(i8 *codeStr, i8 sep, i8 *outBuf)
{
i8	*codeEnd;
u32 len;

	if (!codeStr)
		return 0;
	if (codeEnd = strchr(codeStr, sep))	// Calc length of this field
	{	len = codeEnd - codeStr;
		codeEnd++;						// Skip separator
	}
	else
		len = strlen(codeStr);
	memcpy(outBuf, codeStr, len);		// Move to output buffer
	outBuf[len] = 0;					// and terminate the string
	return codeEnd;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::extract_one
#
# DESCRIPTION:
#	Extract one variable from an xml node based on coded values
#	b = 8bit boolean, can be 0/1 yes/no true/false
#	f = 32bit float
#	F = 64bit float
#	h = 32bit hex
#	H = 64bit hex
#	c = 8bit char
#	w = 16bit integer
#	i = 32bit integer
#	I = 64bit integer
#	s = c string
#	j = callback with node
#
# ARGUMENTS:
#	i8 type			type code
#	i8 *val			pointer to parse string
#	void *pout		output buffer pointer
#	Xml_node *node	this entries node
# RETURNS:
\******************************************************************************/
int Xml_node::extract_one(i8 type, i8 * &val, void * &pout)
{
	i8 *pc = (i8*)pout;
	if (!val) val = (i8*)"";
	switch(type)
	{
	case 'b':
		if (*val == 'y' || *val == 'Y' || *val == 't' || *val == 'T')
			*(i8*)pout = 1;
		else if (*val == 'n' || *val == 'N' || *val == 'f' || *val == 'F')
			*(i8*)pout = 0;
		else
			*(i8*)pout = (i8)atoi(val);
		pc += 1;
		break;
	case 'c':
		*(i8*)pout = (i8)atoi(val);
		pc += 1;
		break;
	case 'f':
		*(f32*)pout = (f32)atof(val);
		pc += 4;
		break;
	case 'F':
		*(f64*)pout = (f64)atof(val);
		pc += 8;
		break;
	case 'h':
		*(u32*)pout = (u32)hexVert(val);
		pc += 4;
		break;
	case 'H':
		*(u64*)pout = hexVert(val);
		pc += 8;
		break;
	case 'w':
		*(u16*)pout = (u16)atoi(val);
		pc += 2;
		break;
	case 'i':
		*(s32*)pout = atoi(val);
		pc += 4;
		break;
	case 'I':
		*(s64*)pout = atoi(val);
		pc += 8;
		break;
	case 's':
		*(u32*)pout = (u32)val;
		pc += 4;
		pout = pc;
		return 1;
	case 'j':
		((void(*)(Xml_node *))(pout))(this);
		return 1;
	default:		// Error here
		break;
	}
	pout = pc;
	return skipValue(val); // works barely. FIXME.
}

/******************************************************************************\
# FUNCTION:
#	 Xml_node::delNodeR
#
# DESCRIPTION:
#	Recursively find and delete a node and the tree underneath it
#
# ARGUMENTS:
#	Xml_node *node
#	Xml_node *find
#
# RETURNS:
#
\******************************************************************************/
int Xml_node::delNodeR(Xml_node *find, Xml_node **nodeB)
{
	Xml_node *node = this;
	xml_walk(node)							// Walk through the node list
	{	if (node == find)
		{	*nodeB = node->sibling;			// Remove from chain
			node->sibling = 0;				// Kill node list
			return node->release();			// Release the node tree
		}
		if (node->child)					// If there is a deeper context
			return node->child->delNodeR(find, &node->child); // Look in it
		nodeB = &node->sibling;
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	 Xml_node::addNode
#
# DESCRIPTION:
#	Add a node to the xml tree at this node level
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *tag
#	i8 *value
#
# RETURNS:
#	Xml_node * to new node
#
\******************************************************************************/
Xml_node * Xml_node::addNode(i8 *tag, i8 *value)
{
	if (!this)
		return 0;
	Xml_node *newNode = new Xml_node(strlen(tag), tag, strlen(value), value);	// Allocate up a node
	Xml_node *node = this;
	xml_walk1(node);								// Skip to end of list
	node->sibling = newNode;						// Hook into list
	return newNode;									// Return fresh node
}

/******************************************************************************\
# FUNCTION:
#	 Xml_node::addChild
#
# DESCRIPTION:
#	Add a node to the xml tree at the nodes child level
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *tag
#	i8 *value
#
# RETURNS:
#	Xml_node * to new node
#
\******************************************************************************/
Xml_node * Xml_node::addChild(i8 *tag, i8 *value)
{
	if (!this)
		return 0;
	Xml_node *newNode = new Xml_node(strlen(tag), tag, strlen(value), value);	// Allocate up a node
	Xml_node *node = this;
	if (node->child)
	{	node = node->child;
		xml_walk1 (node);							// Skip to end of list
		node->sibling = newNode;					// Hook into child list
	}
	else
		node->child = newNode;						// Make a child list
	return newNode;									// Return fresh node
}

/******************************************************************************\
 # FUNCTION:
 #	count
 #
 # DESCRIPTION:
 #	Find a node at this node level with a node name
 #
 # ARGUMENTS:
 #	i8 *path
 #
 # RETURNS:
 \******************************************************************************/
int Xml_node::count(i8 *path)
{
	int cnt = 0;
	Xml_node *node = this;
	xml_walkT(node,path)
	{	cnt ++;
	}
	return cnt;
}

/******************************************************************************\
# FUNCTION:
#	finds
#
# DESCRIPTION:
#	Find a node at this node level with a node name
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *path
#
# RETURNS:
\******************************************************************************/
Xml_node * Xml_node::finds(i8 *tag)
{
	Xml_node *node = this;
	xml_walk(node)
	{	if (strcmp(node->tag, tag) == 0)
			return node;
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::findVal
#
# DESCRIPTION:
#	Find a node at this node level with a node tag and value
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *tag
#	i8 *val
#
# RETURNS:
\******************************************************************************/
Xml_node * Xml_node::findVal(i8 *tag, i8 *val)
{
	Xml_node *node = this;
	xml_walk(node)
	{	if ((strcmp(node->tag, tag) == 0) && (strcmp(node->value, val) == 0))
			return node;
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::findPath
#
# DESCRIPTION:
#	Locate a node with a path name
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *path
#
# RETURNS:
\******************************************************************************/
Xml_node * Xml_node::findPath(i8 *path)
{
	Xml_node *next;
	nextPath(path,	Xml_head::head[Xml_head::headDepth]->getbuffer(),
					Xml_head::head[Xml_head::headDepth]->bufsize);	// extract next name in path!
	Xml_node *node = this;
	xml_walk(node)								// Walk the whole node list
	{	if (next = node->finds(Xml_head::head[Xml_head::headDepth]->getbuffer()))	// Is it on this list?
			return *path ?							// Path to look for still?
				next->child->findPath(path) :	// Look one level deeper
				next;
	}
	return 0;
}

/******************************************************************************\
 # FUNCTION:
 #	Xml_node::findPathT
 #
 # DESCRIPTION:
 #	Locate a node with a path name
 #
 # ARGUMENTS:
 #	Xml_node *node
 #	i8 *path
 #
 # RETURNS:
 \******************************************************************************/
Xml_node * Xml_node::findPathT(i8 *path)
{
	Xml_node *next;
	nextPath(path,	Xml_head::head[Xml_head::headDepth]->getbuffer(),
			 Xml_head::head[Xml_head::headDepth]->bufsize);	// extract next name in path!
	Xml_node *node = this;
	xml_walk(node)								// Walk the whole node list
	{	if (next = node->finds(Xml_head::head[Xml_head::headDepth]->getbuffer()))	// Is it on this list?
		return *path ?							// Path to look for still?
		next->child->findPathT(path) :	// Look one level deeper
		next;
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::findPath
#
# DESCRIPTION:
#	Locate a node with a path name
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *path
#
# RETURNS:
\******************************************************************************/
Xml_node * Xml_node::findPathVal(i8 *path, i8 *value)
{
	Xml_node *next,*node = this;
	char pathName[256];
	nextPath(path, pathName, 255);		// extract next name in path!
	for (node = *path ? node->finds(pathName) :
					    node->findVal(pathName, value);
		node;
		node = *path ? node->sibling->finds(pathName) :
					   node->sibling->findVal(pathName, value) )
   {	if (!*path)
			return node;
		if (next = node->child->findPathVal(path, value)) // Look one level deeper
			return next;
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::set
#
# DESCRIPTION:
#	Find a node at this node level with a node name
#	and set its value
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *path
#	i8 *valNew
#
# RETURNS:
\******************************************************************************/
Xml_node * Xml_node::set(i8 *tag, i8 *valNew)
{
	Xml_node *thisNode;
	if (thisNode = finds(tag))
	{	mFree(thisNode->value);
		thisNode->setValue(strlen(valNew), valNew);
	}
	return thisNode;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::setVal
#
# DESCRIPTION:
#	Find a node at this node level with a node tag and value
#	and set its value to the new one
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *tag
#	i8 *val
#
# RETURNS:
\******************************************************************************/
Xml_node * Xml_node::setVal(i8 *tag, i8 *val, i8 *valNew)
{
	Xml_node *thisNode;
	if (thisNode = findVal(tag, val))
	{	mFree(thisNode->value);
		thisNode->setValue(strlen(valNew), valNew);
	}
	return thisNode;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::setPath
#
# DESCRIPTION:
#	Set a value in a node with path name
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *path
#
# RETURNS:
\******************************************************************************/
Xml_node * Xml_node::setPath(i8 *path, i8 *valNew)
{
	Xml_node *thisNode;
	if (thisNode = findPath(path))
	{	mFree(thisNode->value);
		thisNode->setValue(strlen(valNew), valNew);
	}
	return thisNode;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::setPathVal
#
# DESCRIPTION:
#	Locate a node with a path name
#
# ARGUMENTS:
#	Xml_node *node
#	i8 *path
#
# RETURNS:
\******************************************************************************/
Xml_node * Xml_node::setPathVal(i8 *path, i8 *val, i8 *valNew)
{
	Xml_node *thisNode;
	if (thisNode = findPathVal(path, val))
	{	mFree(thisNode->value);
		thisNode->setValue(strlen(valNew), valNew);
	}
	return thisNode;

}

/******************************************************************************\
# FUNCTION:
#	Xml_node::extract
#
# DESCRIPTION:
#	Extract variable from an xml node based on coded values
#	f = 32bit float, F = 64bit float, etc. 8f will get an array of 8f floats
#
# ARGUMENTS:
#	Xml_node *node			The node to search
#	Xml_vals *valList		The list of parse values
#	i8 *(valPtr)(Xml_node *) Callback for extracting node values
#
# RETURNS:
\******************************************************************************/
int Xml_node::extract(Xml_vals *valList, i8 *(valPtr)(Xml_node *) )
{
	int cnt=0;
	for (u32 i = 0; valList[i].tag; i++)
	{	if (Xml_node *test = findPathT(valList[i].tag))
		{	i8 *valString = valPtr(test);
			void *pGeneric = valList[i].dest;
			u32 varType = valList[i].varType;
			u32 varCount =0 ;
			i8 varOne = varType & 0xFF;		// Get first
			if (isdigit(varOne))
			{	for (;isdigit(varOne);)			// Numeric count header!
				{	varCount *= 10;
					varCount += (varOne-'0');
					varType >>= 8;
					varOne = ((i8)varType) & 0xFF;
				}
				if ((varCount>0) && (varCount<MAXEXTRACTCNT))
				for (u32 varNum = 0; varNum < varCount; varNum++)
				{	test->extract_one(varOne, valString, pGeneric);
					cnt++;
				}
			}
			else
			for (u32 varNum = 0; varNum < 4 && varType; varNum++)
			{	varOne = ((i8)varType) & 0xFF;
				varType >>= 8;
				test->extract_one(varOne, valString, pGeneric);
				cnt++;
			}
		}
	}
	return cnt;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::dump
#
# DESCRIPTION:
#	Recursively print the xml structure
#
# ARGUMENTS:
#	Xml_node *node
#	int depth
#
# RETURNS:
\******************************************************************************/
void Xml_node::dump(int depth)
{
	if (depth > MAX_XML_NEST)
		return;
	Xml_node *node = this;
	xml_walk(node)						// Walk through all the nodes
	{	node->FormatNode(Xml_head::head[Xml_head::headDepth]->getbuffer(), depth); // Format up on line
		printf("%s",Xml_head::head[Xml_head::headDepth]->getbuffer());	// Print it out
		if (node->child)							// Check for deeper levels
			node->child->dump(depth+1);			// Run at a deeper position
	}
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::show
#
# DESCRIPTION:
#	Recursively print the xml structure
#
# ARGUMENTS:
#	Xml_node *node
#	int depth
#
# RETURNS:
\******************************************************************************/
void Xml_node::show(int depth)
{
	if (depth > MAX_XML_NEST)
		return;
	Xml_node *node = this;
	xml_walk(node)							// Walk through the node list
	{	node->FormatNodeEx(Xml_head::head[Xml_head::headDepth]->getbuffer(), depth); // Format up on line
		printf("%s",Xml_head::head[Xml_head::headDepth]->getbuffer());					// Display it
		if (node->child && (node->flags & Xml_head::FLAGopen))
			node->child->show(depth+1);			// Walk the next deeper
	}
}

/******************************************************************************\
# FUNCTION:
#	releaseNode
#
# DESCRIPTION:
#	Release a nodes resources
#
# ARGUMENTS:
#	Xml_node *node
#
# RETURNS:
\******************************************************************************/
void Xml_node::releaseNode(void)
{
	mFree(tag);					// Free the tag field
	mFree(value);				// Free the value field
	mFree(this);				// Free the node itself
}
		
/******************************************************************************\
# FUNCTION:
#    release
#
# DESCRIPTION:
#
# ARGUMENTS:
#	Xml_node *node
#
# RETURNS:
\******************************************************************************/
int Xml_node::release(void)
{
	if (!this)							// process if this node exists
		return 0;
	Xml_node *node = this;
	xml_walkD(node, nodeD)				// Walk through the node list
	{	node->child->release();			// recursively run them all
		nodeD = node->sibling;			// Find next sibling in list
		delete node;
	}
	return 1;
}

/******************************************************************************\
# FUNCTION:
#	 Xml_node::delNode
#
# DESCRIPTION:
#	delete a node and the tree underneath it
#
# ARGUMENTS:
#	Xml_node *find
#
# RETURNS:
#
\******************************************************************************/
int Xml_node::delNode(void)
{
	return delNodeR(Xml_head::getROOT(),Xml_head::getRootPtr());
}

/******************************************************************************\
# FUNCTION:
#    save
#
# DESCRIPTION:
#	save a node tree into an XML file
#
# ARGUMENTS:
#	Xml_node *node
#	char *FileName	Input file name to read
#
# RETURNS:
#	int status
\******************************************************************************/
int Xml_node::save(char *FileName)
{
	FILE *fid;
	if (this && FileName)
	{	if (fid = fOpen(FileName, (i8*)"w"))		// Open the xml file
		{	child->saved(fid, 0);	// Save off the data tree
			fClose(fid);
			return 1;
		}
	}
	return 0;
}

/******************************************************************************\
# FUNCTION:
#    Xml_node::savebin
#
# DESCRIPTION:
#	save a node tree into a binary format XML .bx file
#
# ARGUMENTS:
#	Xml_node *node
#	char *FileName	Input file name to read
#
# RETURNS:
#	int status
\******************************************************************************/
int Xml_node::savebin(char *FileName)
{
	FILE *fid;
	if (this && FileName)
	{	if (fid = fOpen(FileName, (i8*)"w"))		// Open the xml file
		{	child->savebind(fid, 0);	// Save off the data tree
			fClose(fid);
			return 1;
		}
	}
	return 0;
}

/*############################################################################*/
/*                                                                            */
/*  Xml_head                                                                  */
/*                                                                            */
/*############################################################################*/
u32			Xml_head::TabSet		= 3;
i8			Xml_head::UTF8flag		= 0;
i8			Xml_head::UTF8xlat		= 0;
i8 *		Xml_head::inputStack[8];
u32			Xml_head::stackDepth;
Xml_head *	Xml_head::head[MAX_FILE_DEPTH];
u32			Xml_head::headDepth		= (u32)-1;
int			Xml_head::maxparse		= 16384;
const int	Xml_head::buffersize	= 4096;
const i8	Xml_head::unihead[2]	= {'\xFF','\xFE'};
const i8	Xml_head::UTF8sig[3]	= {'\xEF','\xBB','\xBF'};	// unicode utf8 signature
const i8 *	Xml_head::STRroot		= ":";
const i8 *	Xml_head::STRhead		= "xml";
const i8 *	Xml_head::PERCENT		= "%";
const i8 *	Xml_head::CMNTstart		= "<!--";
const i8 *	Xml_head::CMNTend		= "-->";
const i8 *	Xml_head::DOCTYPE		= "DOCTYPE";
const i8 *	Xml_head::ELEMENT		= "ELEMENT";
const i8 *	Xml_head::ENTITY		= "ENTITY";
const i8 *	Xml_head::ATTLIST		= "ATTLIST";
const i8 *	Xml_head::NOTATION		= "NOTATION";
const i8 *	Xml_head::SYSTEM		= "SYSTEM";
const i8 *	Xml_head::PUBLIC		= "PUBLIC";
const i8 *	Xml_head::NDATA			= "NDATA";

/******************************************************************************\
# FUNCTION:
#	Xml_head
# DESCRIPTION:
#	Initialize xml system
# ARGUMENTS:
# RETURNS:
\******************************************************************************/
Xml_head::Xml_head(void)
{
	init(buffersize);
}

/******************************************************************************\
# FUNCTION:
#	Xml_head
# DESCRIPTION:
#	Initialize xml system
# ARGUMENTS:
# RETURNS:
\******************************************************************************/
Xml_head::Xml_head(int size)
{
	init(size);
}

/******************************************************************************\
# FUNCTION:
#	Xml_head::init
# DESCRIPTION:
#	Initialize xml system
# ARGUMENTS:
# RETURNS:
\******************************************************************************/
Xml_head::~Xml_head(void)
{
	deinit();
}

/******************************************************************************\
# FUNCTION:
#	Xml_head::init
# DESCRIPTION:
#	Initialize xml system
# ARGUMENTS:
#	int size
# RETURNS:
\******************************************************************************/
int Xml_head::init(int size)
{
	head[++headDepth] = this;
	bufsize = size;
	buffer = (i8*)mAlloc(size+16);
	LINE = 1;
	Error = Error_none;
	stackDepth = 0;
	return headDepth;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::deinit
# DESCRIPTION:
#
# ARGUMENTS:
#
# RETURNS:
#	int depth of init tree
\******************************************************************************/
int Xml_head::deinit(void)
{
	mFree(head[headDepth]->buffer);
	return headDepth--;
}

/******************************************************************************\
# FUNCTION:
#	Xml_node::fixunicode
#
# DESCRIPTION:
#	change 16 bit unicode to 8 bit output
#
# ARGUMENTS:
#	i8 *buf		Buffer of unicode characters
#
# RETURNS:
\******************************************************************************/
i8 Xml_head::fixunicode(i8 *buf)
{
	if (!buf)
		return 0;
	if (!  ((buf[0] == unihead[0] && buf[1] == unihead[1]) ||
			(buf[0] == unihead[1] && buf[1] == unihead[0])))
		return 0;
	i8 *outbuf = buf;
	buf += 2;
	u16 *unibuf = (u16*)buf;
	u16 uni;
	do
	{	uni = *unibuf++;
		*outbuf++ = (i8)uni;
		if (uni >= 256)
			*outbuf++ = (i8)(uni>>8);
	} while (uni);	
	return 1;
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::getHttp
#
# DESCRIPTION:
#
# ARGUMENTS:
#	char *FileName
#
# RETURNS:
#	i8 * buffer pointer
*******************************************************************************/
i8 *Xml_head::getHttp(char *FileName)
{
	i8 buffy[256];
	sprintf(buffy,"http protocol not supported yet, file: '%s'", FileName);
	debug(0, buffy);
	u32 len = 2;
	i8 *buffer = (i8 *)mAlloc(len+2);	// Malloc up the storage for input
	buffer[len] = buffer[len+1] = 0;
	return buffer;					// return file input.
}

/*******************************************************************************
# FUNCTION:
#	Xml_node::getFile
#
# DESCRIPTION:
#
# ARGUMENTS:
#	char *FileName
#
# RETURNS:
#	i8 * buffer pointer
*******************************************************************************/
i8 *Xml_head::getFile(char *FileName)
{
	FILE *fid = fOpen(FileName, (i8*)"r");		// Open the xml file
	if (!fid)						// Check if file opened succesfully
	{	head[headDepth]->Error = Error_File_not_found;
		return 0;					// Return error
	}
	u64 size = fSize( fid );			// Find out the size of the file contents
	u32 len = (u32)size;				// No bigger than 32 bits.
	i8 *buffer = (i8 *)mAlloc(len+2);	// Malloc up the storage for input
	if (!buffer)					// Error, out of memory
	{	fClose(fid);				// Close the xml file
		head[headDepth]->Error = Error_Out_of_memory;
		return 0;
	}
	u32 res = fRead(buffer, len , fid );// Read entire file in one piece
	if (res != len)
		head[headDepth]->Error = Error_File_read;
	fClose(fid);					// Close the xml file
	buffer[len] = buffer[len+1] = 0;
	return buffer;					// return file input.
}

/******************************************************************************\
# FUNCTION:
#	readFile
# DESCRIPTION:
#
# ARGUMENTS:
#	char *FileName
# RETURNS:
\******************************************************************************/
i8 * Xml_head::readFile(char *FileName)
{
	i8 *buffer;
	head[headDepth]->Error = Error_none;	// Reset error code
	head[headDepth]->LINE = 1;				// Reset to line 1
	if (memcmp("http:", FileName, 5) == 0)
		buffer = getHttp(FileName);			// Read the web file into memory
	else
		buffer = getFile(FileName);			// Read the disk file into memory
	i8 buffy[256];
	sprintf(buffy,"File not found/read - '%s'", FileName);
	debug(buffer!=0, buffy);
	fixunicode(buffer);						// Change unicode to 8 bit
	return buffer;							// Return read buffer
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
int Xml_head::getSysPub(i8 *&buf, Xml_node &newNode)
{
	u32 len;
	i8	ch;
	if (!strcmp(SYSTEM, buffer))
	{	newNode.setFlag(FLAGsystem);
		ch = newNode.NextTokenW(buf, len);		// Should be a quoted string
		debug(ch=='"',(i8*)"Should be quote string");
	}
	else if (!strcmp(PUBLIC, buffer))
	{	newNode.setFlag(FLAGpublic);
		ch = newNode.NextTokenW(buf, len);		// Should be a quoted string
		debug(ch=='"',(i8*)"Should be quote string");
		newNode.setValue(len, buffer);
		ch = newNode.NextTokenW(buf, len);		// Get the filename part
	}
	else
		return 0;			// Not system or public...

	Xml_node *sysNode = new Xml_node(len, head[headDepth]->getbuffer(),0,0);
	Xml_head *xmlHead = new Xml_head(head[headDepth]->bufsize);
	newNode.setChild(sysNode);
	newNode.setValue(readFile(sysNode->getTag()));
	delete xmlHead;
	return 1;
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
void Xml_head::getDOCTYPE(i8 *&buf, Xml_node &newNode)
{
	u32 len;
	newNode.WhiteSpace(buf);					// and skip trailing space
	i8 ch = newNode.NextTokenHead(buf, len);	// What is the next tag in stream
	newNode.setTag(len, head[headDepth]->getbuffer());
	newNode.WhiteSpace(buf);					// and skip trailing space
	i8 tk = newNode.NextTokenHead(buf, len);
	if (tk != '[')
	{	getSysPub(buf, newNode);				// Recognize Public/system blocks
		newNode.WhiteSpace(buf);				// and skip trailing space
		ch = newNode.NextTokenHead(buf, len);	// What is the next tag in stream
		debug(ch=='[' || ch=='>',(i8*)"expecting a [ or > character");
	}
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
void Xml_head::getELEMENT(i8 *&buf, Xml_node &newNode)
{
	u32 len;
	i8	ch;
	newNode.WhiteSpace(buf);			// and skip trailing space
	ch = newNode.NextTokenHead(buf, len);		// What is the next tag in stream
	newNode.setTag(len, head[headDepth]->getbuffer());
	newNode.WhiteSpace(buf);			// and skip trailing space
	Xml_node **eLink = newNode.getChildP();
	element(buf, eLink);					// Parse an element list
	ch = newNode.NextTokenW(buf, len);
	debug(ch == '>',(i8*)"expecting a > character");
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
void Xml_head::getATTLIST(i8 *&buf, Xml_node &newNode)
{
	u32 len;
	i8	ch;
	newNode.setFlag(FLAGattlist);
	ch = newNode.NextTagW(buf, len);
	do
		ch = newNode.NextChar(buf);			// Skip for now.
	while(ch != '>' && ch != 0);
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
void Xml_head::getENTITY(i8 *&buf, Xml_node &newNode)
{
	u32 len;
	i8	ch,*chp;
	chp = head[headDepth]->getbuffer();
	newNode.setFlag(FLAGentity);
	ch = Xml_node::NextTagW(buf, len);		// Get entity name
	if (ch == '%')
	{	newNode.setFlag(FLAGpercent);
		newNode.WhiteSpace(buf);			// skip leading space
		ch = newNode.NextTokenHead(buf, len);
	}
	debug(ch=='x',(i8*)"label belongs here");
	newNode.setTag(len, chp);				// Save name of entity
	ch = newNode.NextTagW(buf, len);		// read system/public
	if (!getSysPub(buf, newNode))
		newNode.setValue(len, chp);			// Save value
	ch = newNode.NextTagW(buf, len);		// Get trailing >
	if (strcmp(NDATA,chp) == 0)				// Actually is an NDATA tag
	{	ch = newNode.NextTagW(buf, len);	// Get data type
		// save data type here!!!! fixme
		ch = newNode.NextTokenW(buf, len);	// trailing close tag
	}
	debug(ch == '>',(i8*)"expected ;");
}

/******************************************************************************\
# FUNCTION:
#	header
# DESCRIPTION:
#	Read an xml header
# ARGUMENTS:
#	i8 *buf
#	Xml_node **bLink
# RETURNS:
\******************************************************************************/
void Xml_head::header(i8 *&buf, Xml_node **bLink)
{
	i8 tk,ch;
	u32 len;
	i8 *old,*chp = head[headDepth]->getbuffer();
	if (!memcmp(buf, UTF8sig, sizeof(UTF8sig)))
	{	buf += sizeof(UTF8sig);
		UTF8flag = 1;
	}
	for (old = buf, tk = Xml_node::NextTokenW(buf, len);
		(tk == '?')  || (tk == '!') || (tk == '&') || (tk == '%');				// Recognize <? <!
		old = buf, tk = Xml_node::NextTokenW(buf, len) )
	{	ch = Xml_node::NextTag(buf, len);			// What is the next tag in stream
		Xml_node *newNode = new Xml_node;
		bLink = newNode->setBlink(bLink);
		switch(tk)
		{
		case '?':
			if (ch == 'x')	// text label after <?
			{	newNode->setTag(len, chp);
				newNode->vallist(buf, newNode->getChildP());// Walk child nodes
			}
			tk = Xml_node::NextTokenW(buf, len);	// Terminator ?>
			debug(tk=='^',(i8*)"wrong list end");
			break;
		case '!':
			if (!strcmp(DOCTYPE, chp))		// DOCTYPE?
				getDOCTYPE(buf, *newNode);
			else if (!strcmp(ELEMENT, chp))	// ELEMENT
				getELEMENT(buf, *newNode);
			else if (!strcmp(ATTLIST, chp))	// ATTLIST
				getATTLIST(buf, *newNode);
			else if (!strcmp(ENTITY, chp))	// ENTITY
				getENTITY(buf, *newNode);
			else
			{
				debug(0,(i8*)"Unrecognized option");
			}
			break;
		case '&':
		case '%':
			break;
		default:
			debug(0,(i8*)"Shouldn't get here!");
			break;
		}
	}
	if (tk!='}')
		buf = old;
}

/******************************************************************************\
 # FUNCTION:
 #    parse
 #
 # DESCRIPTION:
 #	parse an XML file into tree
 #
 # ARGUMENTS:
 #	i8 *fiddata		XML data ptr
 #
 # RETURNS:
 #	Xml_node * to root node of XML structure
 \******************************************************************************/
Xml_node * Xml_head::parse(i8 *fiddata)
{
	Xml_node *node = head[headDepth]->ROOT =
	new Xml_node(strlen(STRroot)+1, STRroot,0,0);	// Save root node ptr
	if (!node) return node;
	header(fiddata, node->getSiblingP());			// Read the header
	node->parse(fiddata, node->getChildP(),maxparse);// Walk the XML data tree
	return node;									// Return the top XML node
}

/******************************************************************************\
# FUNCTION:
#    read
#
# DESCRIPTION:
#	Read an XML file and parse into tree
#
# ARGUMENTS:
#	char *FileName	Input file name to read
#
# RETURNS:
#	Xml_node * to root node of XML structure
\******************************************************************************/
Xml_node * Xml_head::read(char *FileName)
{
i8 *buffer;
	if (!(buffer = readFile(FileName)))				// Try to read the file
		return 0;									// Couldn't read file
	Xml_node *node = parse(buffer);					// parse the tree
	mFree(buffer);									// Release the file buffer
	return node;									// Return the top XML node
}
