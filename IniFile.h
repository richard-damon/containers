#ifndef INIFILE_H
#define INIFILE_H

/**
@file ini_file.h

This file defines the implementation for a pair of templates (DListInRoot and DListInNode) that
implement an intrusive double linked list.

@copyright (c) 2014 Richard Damon
@parblock
MIT License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
@endparblock

*/

#include "treein.h"
#include "dlistin.h"

#include <string>


class IniItem;
class IniGroup;
class IniFile;

class IniItem :
	public TreeInNode<IniGroup, IniItem, std::string>,
	public DListInNode < IniGroup, IniItem >
{
	typedef TreeInNode<IniGroup, IniItem, std::string> TreeNode;
	typedef DListInNode < IniGroup, IniItem > ListNode;

public:
	IniItem(IniGroup* group, std::string key);
	~IniItem();

	IniItem* nextItem() const;
	IniItem* nextItemAlpha() const;

	std::string key() const { return key_; }
	std::string value;
	std::string comment;
private:
	std::string key_;
};

class IniGroup :
	public TreeInNode<IniFile, IniGroup, std::string>,
	public DListInNode<IniFile, IniGroup>,
	public TreeInRoot<IniGroup, IniItem, std::string>,
	public DListInRoot <IniGroup, IniItem >
{
	typedef TreeInNode<IniFile, IniGroup, std::string> TreeNode;
	typedef DListInNode<IniFile, IniGroup> ListNode;
	typedef TreeInRoot<IniGroup, IniItem, std::string> TreeRoot;
	typedef DListInRoot <IniGroup, IniItem > ListRoot;
public:
	IniGroup(IniFile* file, std::string key);
	~IniGroup();

	IniGroup* nextGroup() const;
	IniGroup* nextGroupAlpha() const;
	IniItem* firstItem() const;
	IniItem* firstItemAlpha() const;
	IniItem* getItem(std::string);
	std::string key() const { return key_; }
private:
	std::string key_;
};

class IniFile :
	public TreeInRoot<IniFile, IniGroup, std::string>,
	public DListInRoot<IniFile, IniGroup>
{
	typedef TreeInRoot<IniFile, IniGroup, std::string> TreeRoot;
	typedef DListInRoot<IniFile, IniGroup> ListRoot;

public:
	IniFile();
	~IniFile();

	void open(std::string filename);
	IniGroup* firstGroup() const;
	IniGroup* firstGroupAlpha() const;
	IniGroup* getGroup(std::string);
	std::string filename;
};

#endif
