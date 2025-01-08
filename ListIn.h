/**
@file ListIn.h
@brief Intrusive Singly Linked List.

This file defines a pair of templates (ListInRoot and ListInNode) that
implement an intrusive singly linked list.

@copyright  (c) 2014-2024 Richard Damon
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
ListInRoot and ListInNode provide a simple API to allow classes to provide a
basic List <-> Node relationship (1 List holding many Nodes), with a singly linked list.
(c.f. DListin.h, DListInRoot, and DListInNode for doubly linked list) 

To create this relationship, the class to act as the list derives from the template ListInRoot,
and the class to act as the Node from ListInNode, both with a first parameter of the class name
of the List class, and a second parameter of the Node class. There is an optional integral 3rd parameter
to allow the classes to inherent from these multiple times if you need to represent multiple simultaneous
relationships. This inheritance should be public, or you need to add the ListInRoot and ListInNode templates as
friends.

At the point of declaration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from ListInRoot

N: The "user" type that will be derived from ListInNode

n: The integer parameter for the template to allow multiple usage

Root: The particular instance of ListInRoot<R, N, n> being used

Node: The particular instance of ListInNode<R, N, n> being used

node: will have type N*, to be used when walking the list.
@endparblock

@invariant
@parblock
 Given:
 + R& root
 + N& node

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

@see DListIn.h
@ingroup IntrusiveContainers
*/

#ifndef LISTIN_H
#define LISTIN_H

#include "Container.h"

// L a class derived from ListInRoot<L,N,n>
// N a class derived from ListInNode<L,N,n>

template<class R, class N, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class ListInRoot;
template<class R, class N, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class ListInNode;

/**
@class ListInRoot

Intrusive Singly Linked List, List.

@tparam R The class that will be the owner of the List. Must derive from ListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from ListInNode<R, N, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
 + if m_first == nullptr
   + m_last == nullptr
 + if m_first !- nullptr
   + m_last != nullotr
   + m_first->m_root == this
   + m_last->m_root == this
   + m_last->m_next == nullptr
@endparblock

@see ListInNode
@ingroup IntrusiveContainers

*/
template<class R, class N, ContainerThreadSafety s, int n> class ListInRoot :
    public Container<s>
{
    friend ListInNode<R, N, s, n>;
    friend N;
    typedef ListInRoot<R, N, s, n> Root;    ///< Type of ListInRoot
    typedef ListInNode<R, N, s, n> Node;    ///< Type of ListInNode

protected:
    ListInRoot();
    ~ListInRoot();
    void add(N& node, bool upgrade = false);
    void add(N* node, bool upgrade = false);
    void addFirst(N& node, bool upgrade = false);
    void addFirst(N* node, bool upgrade = false);
    void addLast(N& node, bool upgrade = false);
    void addLast(N* node, bool upgrade = false);
    void remove(N& node);
    void remove(N* node);
    N* first() const { return m_first; }   ///< Return pointer to first Node on list.
    N* last() const { return m_last; }     ///< Return pointer to last Node on list

    bool check() const override;

	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const	                    { return Container<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 Container<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock(bool upgrade) const 						{ return Container<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 Container<s>::readUnlock(save);}

private:
    N* m_first;                          ///< Pointer to first Node on list
    N* m_last;                           ///< Pointer to last Node on list
};

/**
@class ListInNode

Intrusive Singly Linked List, Node.

@tparam R The class that will be the owner of the List. Must derive from ListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from ListInNode<R, N, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
 + if m_root == nullptr
   + m_nest == nullptr
 + if m_next == nullptr
   + m_root->m_last = this;
 + if m_next != nullptr
   + m_next->m_root == m_root
   + m_next != m_root->m_first

Last Criteria is closet expression to the fact that root.m_first points to a node that other node points to as its m_next

@endparblock

@see ListInRoot
@ingroup IntrusiveContainers

*/
template<class R, class N, ContainerThreadSafety s, int n> class ListInNode :
    public ContainerNode<s>
{
    friend ListInRoot<R, N, s, n>;
    friend R;
    typedef ListInNode<R, N, s, n> Node;    ///< Type of ListInNode
    typedef ListInRoot<R, N, s, n> Root;    ///< Type of ListInRoot
protected:
    ListInNode(R& myRoot);
    ListInNode(R* myRoot = nullptr);
    ~ListInNode();

    void addTo(R& myRoot, bool upgrade = false);
    void addTo(R* myRoot, bool upgrade = false);
    void addToFront(R& myRoot, bool upgrade = false);
    void addToFront(R* myRoot, bool upgrade = false);
    void addToEnd(R& myRoot, bool upgrade = false);
    void addToEnd(R* myRoot, bool upgrade = false);
    void addAfter(N& myNode, bool upgrade = false);
    void addAfter(N* myNode, bool upgrade = false);
    void remove();

    R* root() const { return m_root; }   ///< Return pointer to List we are on.
    N* next() const { return m_next; }   ///< Return pointer to next Node on List.
    void	setRoot(R* root) { m_root = root; ContainerNode<s>::setRoot(static_cast<Root*>(root)); }

    bool check() const override;

	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const	{ return ContainerNode<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 ContainerNode<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock(bool upgrade) const 						{ return ContainerNode<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 ContainerNode<s>::readUnlock(save);}

private:
// private as only this template class should be able to change
	R* m_root;                           ///< Pointer to list we are on.
    N* m_next;                           ///< Pointer to next Node in list.
};

#endif
