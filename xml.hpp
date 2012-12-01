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
**    express or implied, by estoppel or otherwise, to any intellectual       **
**    property rights is granted herein.                                      **
**                                                                            **
********************************************************************************
**
**    Module Name:
**      XML.hpp
**
**    Abstract:
**      A tiny XML file reader
**
**    Product Name:  
**      Systems Library
**
**    Version:
**      $Revision: 1.16 $
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

#if !defined(_XML_HPP_INCLUDED_)
#define _XML_HPP_INCLUDED_

extern "C"
{
#include "/Proj/platform/errors.h"
#include "/Proj/platform/sizedefs.h"
#include "/Proj/platform/OSdrivers.h"
}

#define	M32(a,b,c,d)	(256*256*256*a+256*256*b+256*c+d)
#define	X32(a,b,c,d)	(256*256*256*d+256*256*c+256*b+a)

#define	AllowNoCloseTag	1
#define	MAXEXTRACTCNT	1000

/******************************************************************************\
# CLASS:
#	xml_vals
#
# DESCRIPTION:
#
\******************************************************************************/
class  Xml_vals
{								// XML value extractor
private:

public:
	i8			*tag;			// Input string buffer area
	u32			varType;		// Variable type to parse
	void		*dest;			// Destination pointer for variables
};

/******************************************************************************\
# CLASS:
#	xml_node
#
# DESCRIPTION:
#	Store xml node data, in a single linked tree
#	Data consists of a tag and a value for the tag, plus type flags
\******************************************************************************/
class Xml_node
{								// XML Node 20 bytes each
private:
	Xml_node *	sibling;		// Next node at this level
	Xml_node *	child;			// Child node list
	union {i8 *	tag;			// Tag for node
	u32			hashTag;};		// Hashed version of tag
	union {i8 *	value;			// Value for tag
	u32			valueBin;};		// Binary version of value
	i8			flags;			// node flags
	i8			status;			// Node status byte
	i8			flagUnused1;
	i8			flagUnused2;

	static const int MAX_XML_NEST = 256;
	static const i8	USETABS = 1;
	static const i8	USEEND = 2;
	static const i8	SPACE = ' ';
	static const i8	TAB = 9;
	static const i8	LF = 10;
	static const i8	CR = 13;
	static const i8 * const predef[6];
	static const i8   preval[6];
	static const i8 * CDATAstart;
	static const i8 * CDATAend;

	static int	isWhite(i8 ch);
	static int	isSeparator(i8 ch);
	static int	skipComment(i8 * &buf);
	static void	White(i8 * &buf, i8 *wVal, u32 &len);
	static i8	LookChar(i8 * &buf);
	static i8	hexChar(i8 ch);
	static u64	hexVert(i8 *val);
	static void	nextPath(i8 *&buf, i8 *path, u32 len);
	static void	uniStrConv(i8 *buffer, u32 &len);
	static void	NextText(i8 * &buf,i8 *tokVal, u32 maxlen, u32 &len);
	static void	NextText(i8 * &buf, u32 &len);
	static i8	NextToken(i8 * &buf,i8 *tokVal,u32 maxlen, u32 &len);
	static i8	NextList(i8 * &buf,i8 *tagVal, u32 maxlen,u32 &len);
	static i8	NextTag(i8 * &buf,i8 *tagVal,u32 maxlen, u32 &len);
	static inline int	NextCDATA(i8 * &buf,i8 *&tokVal,u32 maxlen,u32 &len);
	static void NextLabel(i8 * &buf,i8 *tagVal,u32 maxlen,u32 &len);
   	int			delNodeR(Xml_node *find, Xml_node **nodeB);
	static i8 *	subfind(i8 *buf);
	static i8	predefs(i8 *&buf);
	static int	gatherep(i8 *&buf, i8 *outp, u32 maxlen, u32 &len);
	Xml_node *	findPathT(i8 *path);

public:
	Xml_node(void);
	Xml_node(int size, const i8 *tagVal, int sizeV=0, const i8 *value=0);
	~Xml_node(void);
	void * operator new(size_t size) { return mAlloc(size);	};
	void operator delete(void *mem)  { return mFree(mem); };
	Xml_node **	getSiblingP(void) {return &sibling;};		// Sibling node list
	Xml_node *	getSibling(void) {return sibling;};			// Sibling node list
	Xml_node **	getChildP(void) {return &child;};			// Child node list
	Xml_node *	getChild(void) {return child;};				// Child node list
	void		setChild(Xml_node *inp) { child = inp; };
	i8 *		getTag(void) { return tag; };
	int			setTag(int size, const i8 *tagVal);
	int			isTag(const i8*tag) { return (strcmp(this->tag, tag) == 0); };
	int			isTag(const u32 tag) { return (this->hashTag == tag); };
	i8 *		getValue(void) { return value; };
	void		setValue(i8 *theVal) { this->value = theVal; };
	int			setValue(int size, const i8 *theVal);
	int			isValue(const i8*val) {return (strcmp(this->value, val) == 0); };
	int			isValue(const u32 val) { return (this->valueBin == val); };
	void		setStatus(i8 stat) { this->status = stat; };
	i8			getFlag(void) { return this->flags;};
	void		setFlag(i8 flagVal) { this->flags |= flagVal; };
	static i8 *	val(Xml_node *node) {	return node->value; };
	Xml_node **	setBlink(Xml_node **bLink);
	static int	isRepElement(i8 ch);
	void		BinNode(i8 *buf, int depth, int flag);
	void		SaveNode(i8 *buf, int depth, int flag);
	void		FormatNode(i8 *buf, int depth);
	void		FormatNodeEx(i8 *buf, int depth);
	int			vallist(i8 * &buf, Xml_node **bLink);
	void		parse(i8 *&buf, Xml_node **bLink,int depth);
	static i8 *	field(i8 *codeStr, i8 sep, i8 *outBuf);
	static int	skipValue(i8 * &buf);
	static void	WhiteSpace(i8 * &buf);
	static void	WhiteSpace(i8 * &buf, i8 *wVal, u32 &len);
	static i8	NextChar(i8 * &buf);
	static void NextQuote(i8 * &buf,i8 *tokVal,u32 maxlen,i8 tokEqu,u32 &len);
	static i8	NextList(i8 * &buf, u32 &len);
	static i8	NextTagW(i8 * &buf, u32 &len);
	static i8	NextTag(i8 * &buf, u32 &len);
	static i8	NextTokenW(i8 * &buf, u32 &len);
	static i8	NextToken(i8 * &buf, u32 &len);
	static i8	NextTokenHead(i8 * &buf, u32 &len);
	int			release(void);
	void		releaseNode(void);
	void		show(int depth);
	void		dump(int depth);	
	int			extract(Xml_vals *valList, i8 *(valPtr)(Xml_node *) );
	int			extract(Xml_vals *valList)	{ return extract(valList, this->val); };
	int         extract_one(i8 type, i8 * &val, void * &pout);
	int			delNode(void);
	int			savebind(FILE *fid, s32 depth);
	int			saved(FILE *fid, s32 depth);
	int			savebin(char *FileName);
	int			save(char *FileName);
	Xml_node *	read(char *FileName);
	Xml_node *	root(void);
	Xml_node *	addNode(i8 *tag, i8 *value);
	Xml_node *	addChild(i8 *tag, i8 *value);
	Xml_node *	setDir(i8 *path);
	Xml_node *	set(i8 *tag, i8 *valNew);
	Xml_node *	setVal(i8 *tag, i8 *val, i8 *valNew);
	Xml_node *	setPath(i8 *path, i8 *valNew);
	Xml_node *	setPathVal(i8 *path, i8 *val, i8 *valNew);
	int			count(i8 *path);
	Xml_node *	finds(i8 *tag);
	Xml_node *	findVal(i8 *tag, i8 *val);
	Xml_node *	findPath(i8 *path);
	Xml_node *	findPathVal(i8 *path, i8 *value);
};

/******************************************************************************\
# CLASS:
#	xml_head
#
# DESCRIPTION:
#
\******************************************************************************/
class Xml_head
{								// XML header
private:
	i8			*buffer;		// Input string buffer area
	u32			LINE;			// Line number in input
	Xml_node	*ROOT;			// Root Node
	Errors		Error;			// Last error processed

	static const int MAX_FILE_DEPTH = 16;
	const static i8	UTF8sig[3];		// unicode utf8 signature
	const static i8 unihead[2];
	const static int buffersize;
	const static i8 * STRroot;
	const static i8 * STRhead;
	const static i8 * NDATA;
	const static i8 * DOCTYPE;
	const static i8 * ELEMENT;
	const static i8 * ENTITY;
	const static i8 * ATTLIST;
	const static i8 * NOTATION;
	const static i8 * SYSTEM;
	const static i8 * PUBLIC;
	const static i8 * PERCENT;

	i8			fixunicode(i8 *buf);
	void		header(i8 *&buf, Xml_node **bLink);
	void		element(i8 *&buf, Xml_node **bLink);
	static i8 *	getHttp(char *FileName);
	static i8 *	getFile(char *FileName);
	void		getDOCTYPE(i8 *&buf, Xml_node &newNode);
	void		getELEMENT(i8 *&buf, Xml_node &newNode);
	void		getATTLIST(i8 *&buf, Xml_node &newNode);
	void		getENTITY(i8 *&buf, Xml_node &newNode);
	int			getSysPub(i8 *&buf, Xml_node &newNode);
public:
	u32			bufsize;		// Storage buffer size

	static Xml_head	*head[MAX_FILE_DEPTH];
	static u32	headDepth;
	static i8 *	inputStack[8];
	static u32	stackDepth;
	static int	maxparse;
	static u32	TabSet;
	static i8	UTF8flag;
	static i8	UTF8xlat;

	const static i8 * CMNTstart;
	const static i8 * CMNTend;
	const static i8	FLAGequl	= 0x01;
	const static i8	FLAGattlist	= 0x02;
	const static i8	FLAGentity	= 0x04;
	const static i8	FLAGsystem	= 0x08;
	const static i8	FLAGpublic	= 0x10;
	const static i8	FLAGpercent	= 0x20;
	const static i8	FLAGbit6	= 0x40;
	const static i8	FLAGopen	= -128; // 0x80

	Xml_head(void);
	Xml_head(int size);
	~Xml_head(void);
	void * operator new(size_t size) { return mAlloc(size);	};
	void operator delete(void *mem)  { return mFree(mem); };

	int			init(int size);
	int			deinit(void);
	Errors		error(void)			{ return head[headDepth]->Error; };
	void		xlatUTF8(i8 flag)	{ UTF8xlat = flag; };
	Xml_node *	parse(i8 *fiddata);
	Xml_node *	read(char *FileName);
	i8 *		readFile(char *FileName);
	i8 *		getbuffer(void)		{return buffer;};
	u32			getLine(void)		{return LINE;};
	void		nextline(void)		{LINE++;};
	static Xml_node * getROOT(void) { return head[headDepth]->ROOT; };
	static Xml_node ** getRootPtr(void) { return &head[headDepth]->ROOT; };
};		// 28 bytes long

#define	xml_walk(node)	for (;node;node = node->getSibling())
#define	xml_walkT(node,tag)	for (;node;node = node->getSibling()) if(node->isTag(tag))
#define	xml_walkV(node,val)	for (;node;node = node->getSibling()) if(node->isValue(val))
#define	xml_walk1(node)	for (;node->sibling;node = node->getSibling())
#define	xml_walkC(node) for (node = node->getChild(); node; node = node->getSibling())
#define	xml_walkD(node,nodeD)	Xml_node *nodeD; for(nodeD = node;node;node = nodeD)
#define	xml_walkP(node,nodeP)	Xml_node *nodeP; for(nodeP = 0;node;nodeP=node,node=node->sibling)
#define	xml_walkI(node,container) for (node = node->finds(container);node;node=node->getSibling()->finds(container))
#define	xml_walkIC(node,container) for (node = node->getChild()->finds(container);node;node=node->getSibling()->finds(container))

#endif // !defined(_XML_H_INCLUDED_)
