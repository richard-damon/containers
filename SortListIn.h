#ifndef SortListIn_H
#define SortListIn_H

/**
 @file SortListIn.h
 @brief Intrusive Sorted Double Linked List.

 This file defines a pair of templates (SortListInRoot and SortListInNode) that
 implement an intrusive double linked list.

@warning The Sorted Lists are new additions and not fully tested

@copyright (c) 2023-2024 Richard Damon
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

@par Overview
@parblock
SortListInRoot and SortListInNode provide a simple API to allow classes to provide a
basic List <-> Node relationship (1 List holding many Nodes) with a sorted linked list.
(c.f. ListIn.h, ListInRoot, and ListInNode for an unsorted list)

To create this relationship, the class to act as the list derives from the template SortListInRoot,
and the class to act as the Node from SortListInNode, both with a first parameter of the class name
of the List class, and a second parameter of the Node class.
There is a third parameter, which can specify a thread safety option, defaulting to no thread safety provide by the
library. Use of any other requires the Containers.h/hpp be configured to understand the operating system that it is
being run under.
There is an optional integral 4th parameter to allow the classes to inherent from these multiple times if you need to
represent multiple simultaneous
relationships. This inheritance should be public, or you need to add the SortListInRoot and SortListInNode templates as
friends as they need to convert pointers between the two classes.

At the point of declaration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from SortListInRoot

N: The "user" type that will be derived from SortListInNode

s: The thread safety standard selected.

n: The integer parameter for the template to allow multiple usage

Root: The particular instance of SortListInRoot<R, N, n> being used

Node: The particular instance of SortListInNode<R, N, n> being used

node: will have type N*, to be used when walking the list.
@endparblock

@invariant
@parblock
 Given:
 + R& root
 + N& node

 New:

SortListInNode:
 + if node.m_next != nullptr
   + node.m_root->compare(node, *node.m_next) >= 0
   + node.m_root->compare(*node.m_next, node) <= 0

 From ListIn.h:

ListInRoot:
+ if root.m_first == nullptr
  + root.m_last == nullptr
+ if root.m_first !- nullptr
  + root,m_last != nullotr
  + root.m_first->m_root == \&root
  + root.m_last->m_root == \&root
  + root.m_last->m_next == nullptr

ListInNode:
+ if node.m_root == nullptr
  + node.m_nest == nullptr
+ if node.m_next == nullptr
  + node.m_root->m_last = \&node;
+ if node.m_next != nullptr
  + node.m_next->m_root == node.m_root
  + node.m_next != node.m_root->m_first

Last Criteria is closet expression to the fact that root.m_first points to a node that other node points to as its m_next
@endparblock

@see DListIn.hpp
@see ListIn.h

@ingroup IntrusiveContainers
*/
#include "ListIn.h"

template <class R, class N, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class SortListInNode;
template <class R, class N, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class SortListInRoot;

/**
@class SortListInRoot

Intrusive Doubly Linked List, List.

@tparam L The class that will be the owner of the List. Must derive from DListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from DListInNode<R, N, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
From ListIn.h, ListInRoot:
+ if m_first == nullptr
  + m_last == nullptr
+ if m_first !- nullptr
  + m_last != nullotr
  + m_first->m_root == this
  + m_last->m_root == this
  + m_last->m_next == nullptr
@endparblock

@see SortListInNode
@ingroup IntrusiveContainers

*/
template <class R, class N, ContainerThreadSafety s, int n> class SortListInRoot :
    private ListInRoot<R, N, s, n>
{
	friend  class SortListInNode<R, N, s, n>;
    friend  class ListInRoot<R, N, s, n>;
    friend  class ListInNode<R, N, s, n>;
	typedef class SortListInRoot<R, N, s, n> Root;   ///< Type of DListInRoot
	typedef class SortListInNode<R, N, s, n> Node;   ///< Type of DListIInNode
	typedef class ListInRoot<R, N, s, n> Base;
public:
	SortListInRoot();
	~SortListInRoot();

	void add(N& node);
	void add(N* node);

    void remove(N& node) { Base::remove(node); }
    void remove(N* node) { Base::remove(node); }

    /**
     * This member function to be provided by the user to define the sort order.
     * @param node1
     * @param node2
     * @return <0 if node1 should be before node2, >0 if after, 0 if they compare equal, new nodes will be added after.
     *
     * Sort order for nodes on the list should not change after they are added.
     */
    int compare(N const& node1, N const& node2) const;

    N* first() const    { return Base::first(); }
    N* last() const     { return Base::last(); }

    bool check() const override;

	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const	                    { return Container<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 Container<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock(bool upgrade) const 						{ return Container<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 Container<s>::readUnlock(save);}
};

/**
@class SortListInNode

Intrusive Doubly Linked List, Node.

@tparam L The class that will be the owner of the List. Must derive from DListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from DListInNode<R, N, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
 New:
+ if m_next != nullptr
  + m_root->compare(node, *node.m_next) >= 0
  + m_root->compare(*node.m_next, node) <= 0

From ListIn.h, ListInNode:
+ if m_root == nullptr
  + m_nest == nullptr
+ if m_next == nullptr
  + m_root->m_last = this;
+ if m_next != nullptr
  + m_next->m_root == m_root
  + m_next != m_root->m_first

Last Criteria is closet expression to the fact that root.m_first points to a node that other node points to as its m_next
@endparblock

@see SortListInRoot
@ingroup IntrusiveContainers

*/
template <class R, class N, ContainerThreadSafety s, int n> class SortListInNode :
    private ListInNode<R, N, s, n>
{
	friend  class SortListInRoot<R, N, s, n>;
    friend  class ListInRoot<R, N, s, n>;
    friend  class ListInNode<R, N, s, n>;
	typedef class SortListInRoot<R, N, s, n> Root;   ///< Type of DListInRoot
	typedef class SortListInNode<R, N, s, n> Node;   ///< Type of DListIInNode
	typedef class ListInNode<R, N, s, n> Base;
public:
	SortListInNode(R* root=nullptr);
	SortListInNode(R&);
	~SortListInNode();

	void addTo(R& root);
	void addTo(R* root);
	void remove() { Base::remove(); }

	R* root() const { return Base::root(); }    ///< Return pointer to list we are on.
	N* next() const { return Base::next(); }    ///< Return pointer to next node on list.
	N* prev() const { return Base::prev(); }    ///< Return pointer to previous node on list.

	bool check() const override;

 	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const	                    { return Container<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 Container<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock(bool upgrade) const 						{ return Container<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 Container<s>::readUnlock(save);}
};
#endif
