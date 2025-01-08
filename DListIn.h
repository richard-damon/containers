#ifndef DListIn_H
#define DListIn_H

/**
 @file DListIn.h
 @brief Intrusive Double Linked List.

 This file defines a pair of templates (DListInRoot and DListInNode) that
 implement an intrusive double linked list.

@copyright (c) 2014-2024 Richard Damon
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
DListInRoot and DListInNode provide a simple API to allow classes to provide a
basic List <-> Node relationship (1 List holding many Nodes) with a doubly linked list. 
(c.f. ListIn.h, ListInRoot, and ListInNode for singly linked list)

To create this relationship, the class to act as the list derives from the template DListInRoot,
and the class to act as the Node from DListInNode, both with a first parameter of the class name
of the List class, and a second parameter of the Node class. There is an optional integral 3rd parameter
to allow the classes to inherent from these multiple times if you need to represent multiple simultaneous
relationships. This inheritance should be public, or you need to add the DListInRoot and DListInNode templates as
friends.

At the point of declaration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from DListInRoot

N: The "user" type that will be derived from DListInNode

n: The integer parameter for the template to allow multiple useage

Root: The particular instance of DListInRoot<R, N, n> being used

Node: The particular instance of DListInNode<R, N, n> being used

node: will have type N*, to be used when walking the list.
@endparblock


@invariant
@parblock
 Given:
 + R& root
 + N& node

 DListInRoot:
 + if root.m_first == nullptr
   + root.m_last == nullptr
 + if root.m_first !- nullptr
   + root,m_last != nullptr
   + root.m_first->m_root == \&root
   + root.m_first->m_prev == nullptr
   + root.m_last->m_root == \&root
   + root.m_last->m_next == nullptr

 DListInNode:
 + if node.m_root == nullptr
   + node.m_nest == nullptr
   + node.m_prev == nullptr
 + if node.m_prev == nullptr
   + node.m_root->m_first = \&node;
 + if node.m_prev !- nullptr
   + node.m_prev->m_root == node.m_root
   + node.m_orev->m_next == \&node
 + if node.m_next == nullptr
   + node.m_root->m_last = \&node
 + if node.m_next != nullptr
   + node.m_next->m_root == node.m_root
   + node.m_next->m_prev == \&node
@endparblock

@see DListIn.hpp
@see ListIn.h

@ingroup IntrusiveContainers
*/
#include "Container.h"

template <class R, class N, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class DListInNode;
template <class R, class N, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class DListInRoot;

/**
@class DListInRoot

Intrusive Doubly Linked List, List.

@tparam R The class that will be the owner of the List. Must derive from DListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from DListInNode<R, N, n>
@tparam s
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
 + if m_first == nullptr
   + m_last == nullptr
 + if m_first !- nullptr
   + m_last != nullptr
   + m_first->m_root == this
   + m_first->m_prev == nullptr
   + m_last->m_root == this
   + m_last->m_next == nullptr
@endparblock

@see DListInNode
@ingroup IntrusiveContainers

*/
template <class R, class N, ContainerThreadSafety s, int n> class DListInRoot :
    public Container<s>
{
	friend  class DListInNode<R, N, s, n>;
	typedef class DListInRoot<R, N, s, n> Root;   ///< Type of DListInRoot
	typedef class DListInNode<R, N, s, n> Node;   ///< Type of DListIInNode
public:
	DListInRoot();
	~DListInRoot();

	void add(N& node, bool upgrade = false);
	void add(N* node, bool upgrade = false);
	void addFirst(N& node, bool upgrade = false);
	void addFirst(N* node, bool upgrade = false);
	void addLast(N& node, bool upgrade = false);
	void addLast(N* node, bool upgrade = false);
	void remove(N& node);
	void remove(N* node);

	N* first() const {return m_first;}
    N* last() const { return m_last; }

    bool check() const override;

	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const	                    { return Container<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 Container<s>::writeUnlock(save); }
    // Critical Section used to Read/Seach the Container
    unsigned readLock(bool upgrade) const 						{ return Container<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 Container<s>::readUnlock(save);}

private:
    N* m_first;                            ///< Pointer to first node on list.
    N* m_last;                             ///< Pointer to last node on list.
};

/**
@class DListInNode

Intrusive Doubly Linked List, Node.

@tparam R The class that will be the owner of the List. Must derive from DListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from DListInNode<R, N, n>
@tparam s The ContainerThreadSaftey value to define the thread safety model of the Container
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
 + if m_root == nullptr
   + m_nest == nullptr
   + m_prev == nullptr
 + if m_prev == nullptr
   + m_root->m_first = this;
 + if m_prev !- nullptr
   + m_prev->m_root == m_root
   + m_orev->m_next == this
 + if m_next == nullptr
   + m_root->m_last = this
 + if m_next != nullptr
   + m_next->m_root == m_root
   + m_next->m_prev == this
@endparblock

@see DListInRoot
@ingroup IntrusiveContainers

*/
template <class R, class N, ContainerThreadSafety s, int n> class DListInNode :
    public ContainerNode<s>
{
	friend  class DListInRoot<R, N, s, n>;
	typedef class DListInRoot<R, N, s, n> Root;   ///< Type of DListInRoot
	typedef class DListInNode<R, N, s, n> Node;   ///< Type of DListIInNode
public:
	DListInNode(R* root=nullptr);
	DListInNode(R&);
	~DListInNode();

	void addTo(R& root, bool upgrade = false);
	void addTo(R* root, bool upgrade = false);
	void addToFront(R& root, bool upgrade = false);
	void addToFront(R* root, bool upgrade = false);
	void addToEnd(R& root, bool upgrade = false);
	void addToEnd(R* root, bool upgrade = false);
	void addAfter(N& node, bool upgrade = false);
	void addAfter(N* node, bool upgrade = false);
	void remove();

	R* 		root() const { return m_root; }    ///< Return pointer to list we are on.
	N* 		next() const { return m_next; }    ///< Return pointer to next node on list.
	N* 		prev() const { return m_prev; }    ///< Return pointer to previous node on list.

    bool check() const override;

    void	setRoot(R* root) { m_root = root; ContainerNode<s>::setRoot(static_cast<Root*>(root)); }
	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const						{ return ContainerNode<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 ContainerNode<s>::writeUnlock(save); }
    // Critical Section used to Read/Seach the Container
    unsigned readLock(bool upgrade) const 						{ return ContainerNode<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 ContainerNode<s>::readUnlock(save);}

private:
	R*    m_root;                          ///< Pointer to list we are on.
    N*    m_prev;                          ///< Pointer to previous node on list.
    N*    m_next;                          ///< Pointer to net node on list.
};
#endif
