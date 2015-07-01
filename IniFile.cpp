/**
@file IniFile.cpp

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

#include "IniFile.h"

#include "DListin.hpp"
#include "TreeIn.hpp"

#include <fstream>

IniItem::IniItem(IniGroup* group, std::string key) :
key_(key)
{
	TreeNode::addTo(group);
	ListNode::addTo(group);
}

IniItem::~IniItem() {
	TreeNode::remove();
	ListNode::remove();
}

IniItem* IniItem::nextItem() const {
	return ListNode::next();
}

IniItem* IniItem::nextItemAlpha() const {
	return TreeNode::next();
}

int TreeInRoot<IniGroup, IniItem, std::string>::compareKey(IniItem const& node, std::string key) const {
	if (node.key() < key) return 1;
	if (node.key() > key) return -1;
	return 0;
}

int TreeInRoot<IniGroup, IniItem, std::string>::compare(IniItem const&node1, IniItem const& node2) const {
	return compareKey(node1, node2.key());
}

IniGroup::IniGroup(IniFile* file, std::string key) :
key_(key)
{
	TreeNode::addTo(file);
	ListNode::addTo(file);
}

IniGroup::~IniGroup(){
	while (firstItem()) {
		delete firstItem();
	}
	TreeNode::remove();
	ListNode::remove();
}

IniItem* IniGroup::firstItem() const {
	return ListRoot::first();
}

IniItem* IniGroup::firstItemAlpha() const {
	return TreeRoot::first();
}

IniGroup* IniGroup::nextGroup() const {
	return ListNode::next();
}

IniGroup* IniGroup::nextGroupAlpha() const {
	return TreeNode::next();
}

int TreeInRoot<IniFile, IniGroup, std::string>::compareKey(IniGroup const& node, std::string key) const {
	if (node.key() < key) return 1;
	if (node.key() > key) return -1;
	return 0;
}

int TreeInRoot<IniFile, IniGroup, std::string>::compare(IniGroup const&node1, IniGroup const& node2) const {
	return compareKey(node1, node2.key());
}

IniFile::IniFile()
{
}


IniFile::~IniFile()
{
	while (firstGroup()) {
		delete firstGroup();
	}
}

IniGroup* IniFile::firstGroup() const {
	return ListRoot::first();
}

IniGroup* IniFile::firstGroupAlpha() const {
	return TreeRoot::first();
}

void IniFile::open(std::string filename) {
	/// @TODO
	std::ifstream file;
	file.open(filename, std::ios_base::in);
	if (!file){
		std::cout << "Can't open file: " << filename << "\n";
		throw std::exception("Can't open File");
	}
	IniGroup* group = 0;
	IniItem* item = 0;
	char buffer[1000];
	while (file){
		file.getline(buffer, 999);
		/// @todo Handle fail status
		char *ptr = buffer;
		while (*ptr == ' ') { ptr++; }
		if (*ptr == '['){
			char *ptr2 = ++ptr;
			while (*ptr2 && *ptr2 != ']') {
				ptr2++;
			}
			if (*ptr2 != ']') {
				std::cout << "Bad Group Line: " << buffer << "\n";
			}
			*ptr2 = 0;
			group = new IniGroup(this, ptr);
		}
		else {
			// Not a group, so is an item (or just a comment which is stored as an item with null key)
			// IF we don't have a group, make a blank group for inital info
			if (group == 0) {
				group = new IniGroup(this, "");
			}
			if (*ptr == '#' || *ptr == 0){
				// comment only line;
				item = new IniItem(group, "");
				item->comment = buffer;
 			}
			else {
				char *ptr2 = ptr;
				while (*ptr2 && *ptr2 != '=' && *ptr2) { ptr2++; }
				char *ptr3 = ptr2;
				while (ptr3[-1] == ' ') { ptr3--; }
				char c = *ptr3;
				*ptr3 = 0;
				item = new IniItem(group, ptr);
				*ptr3 = c;
				ptr = ptr2;
				if (*ptr == '=') {
					ptr++;
					while (*ptr == ' ') { ptr++; }
					ptr2 = ptr;
					while (*ptr2 && *ptr2 != '#') { ptr2++; }
					ptr3 = ptr2;
					while (ptr3[-1] == ' ') { ptr3--; }
					c = *ptr3;
					*ptr3 = 0;
					item->value = ptr;
					*ptr3 = c;
					if (*ptr2 == '#') {
						item->comment = ptr2;
					}
				}
			}
		}
	}
#if 0
	group = firstGroup();
	while (group){
		std::cout << "Group: " << group->key() << "\n";
		item = group->firstItem();
		while (item){
			std::cout << "Item: " << item->key() << " = " << item->value << " " << item->comment << "\n";
 			item = item->nextItem();
		}
		group = group->nextGroup();
	}
#endif
}
