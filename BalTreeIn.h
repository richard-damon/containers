#ifndef BalTreeIn_H
#define BalTreeIn_H

/**
@file BalTreeIn.h
@brief Intrusive Binary Tree (balenced).

This file defines a pair of templates (BalTreeInRoot and BalTreeInNode) that
implement an intrusive binary tree.

@copyright (c) 2022-2024 Richard Damon
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
BalTreeInRoot and BalTreeInNode provide a simple API to allow classes to provide a
basic Tree <-> Node relationship (1 Tree Root holding many Nodes) as a Binary Tree.

Note that this class makes no attempt to "balence" the tree, but is designed that it can be derived
from by a class to implement any needed balencing, but does supply the rotations to do that balencing.

To create this relationship, the class to act as the tree root derives from the template TreeInRoot,
and the class to act as the Node from TreeInNode, both with a first parameter of the class name
of the List class, a second parameter of the Node class, and a 3rd type parameter for the type for the
Key to use in searching the tree. There is an optional integral 4rd parameter
to allow the classes to inherent from these multiple times if you need to represent multiple simultaneous
relationships. This inheritance should be public, or you need to add the TreeInRoot and TreeInNode templates as
friends.

At the point of declaration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

The user of the class will need to define two member functions to configure the tree.
int TreeInRoot::compare(N& node1, N& node2) const;
and 
int TreeInRoot::compare(N& node, K key) const;

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from TreeInRoot

N: The "user" type that will be derived from TreeInNode

K: The "user" type that represents the Key for the tree.

n: The integer parameter for the template to allow multiple usage

List: The particular instance of TreeInRoot<R, N, K, n> being used

Node: The particular instance of TreeInNode<R, N, K, n> being used

node: will have type N*, to be used when walking the list.
@endparblock

@invariant
@parblock
From TreeIn.h:

Given:
+ R& root
+ N& node

TreeInRoot:
+ if root.m_base != nullptr:
  + root.m_base-> m_root == &root
  + root.m_base-> m_parent == nullptr

TreeInNode:
+ if node.m_root == NULL:  // Free Node
  + node.m_parent == NULL
  + node.m_left == NULL
  + node.m_right == NULL
+ if node.m_root != NULL and node.m_parent == NULL  // Base Node
  + node.m_root->m_base == &node
+ if node.m_parent != nullptr: // Child Node
  + Either node.m_parent->m_left or node.m_parent->m_right == &node
+ if node.m_left != nullptr
  + node.m_left->m_parent = &node
  + node.m_left->m_root == node.m_root
  + node.m_root->compare(node, *m_left) <= 0
  + node.m_root->compare(*m_left, node) >= 0
+ if node.m_right != nullptr:
  + node.m_right->m_parent = &node
  + node.m_right->m_root == node.m_root
  + node.m_root->compare(node, *m_right) >= 0
  + node.m_root->compare(*m_right, node) <= 0
@endparblock

@ingroup IntrusiveContainers
*/

#include <Containers/TreeIn.H>

template <class R, class N, class K, ContainerThreadSafety s=ContainerNoSafety, int n> class BalTreeInNode;
template <class R, class N, class K, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class BalTreeInRoot;

/**
@class BalTreeInRoot

Intrusive Binary Tree, Root.

@tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
@tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
@tparam K The class defining the Key used to lookup Nodes in the tree.
@tparam s The ContainerThreadSafety value to define the thread safety model of the Container
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.


@invariant
@parblock
 From TreeInRoot:
 + if m_base not nullptr:
   + m_base->m_root == this.
   + m_base->m_parent == nullptr
@endparblock

@see BalTreeInNode
@ingroup IntrusiveContainers

Note, because base class is a template, we have many forwarding functions to that
base to avoid errors and warnings about thing missing.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n> class BalTreeInRoot :
  public TreeInRoot<R,N,K,s, n> {

	friend  class BalTreeInNode<R, N, K, s, n>;
	typedef class TreeInRoot<R, N, K, s, n> Base;
	typedef class BalTreeInRoot<R, N, K, s, n> Root;   ///< Type of TreeInRoot
	typedef class BalTreeInNode<R, N, K, s, n> Node;   ///< Type of DListIInNode

public:
	BalTreeInRoot();
	virtual ~BalTreeInRoot();

	int compare(N const& node1, N const& node2) const {
	    return Base::compare(node1, node2);
	}
	int compareKey(N const& node, K key) const {
	    return Base::compareKey(node, key);
	}

//	virtual void add(N& node);
//	void add(N* node);
//	void remove(N& node);
//	void remove(N* node);
//	N* find(K key) const;

	        N*      base() const    { return Base::base(); }
	        N*      first() const   { return Base::first(); }
	        N*      last() const    { return Base::last(); }

    bool    check() const override;

	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const						{ return Container<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 Container<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock(bool upgrade) const 						{ return Container<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 Container<s>::readUnlock(save);}
};

/**
@class BalTreeInNode

Intrusive Binary Tree, Node.

@tparam R The class that will be the owner of the List. Must derive from TreeInRoot<R, N, K, n>
@tparam N The class that will be the nodes of the List. Must derive from TreeInNode<R, N, K, n>
@tparam K The class defining the Key used to lookup Nodes in the tree.
@tparam s The ContainerThreadSafety value to define the thread safety model of the Container
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
From TreeInNode:
 + if m_root == NULL:  // Free Node
  + m_parent == NULL
  + m_left == NULL
  + m_right == NULL
+ if m_root != NULL and m_parent == NULL  // Base Node
  + m_root->m_base == this
+ if m_parent != nullptr: // Child Node
  + Either m_parent->m_left or parent->m_right == this
+ if m_left != nullptr
  + m_left->m_parent = this
  + m_left->m_root == m_root
  + m_root->compare(*this, *m_left) <= 0
  + m_root->compare(*m_left, *this) >= 0
+ if m_right != nullptr:
  + m_right->m_parent = this
  + m_right->m_root == m_root
  + m_root->compare(*this, *m_right) >= 0
  + m_root->compare(*m_right, *this) <= 0
@endparblock

@see BalTreeInRoot
@ingroup IntrusiveContainers

*/
template <class R, class N, class K, ContainerThreadSafety s, int n> class BalTreeInNode :
  public TreeInNode<R, N, K, s, n> {
	friend  class BalTreeInRoot<R, N, K, s, n>;
	typedef class TreeInNode<R, N, K, s, n> Base;
	typedef class BalTreeInRoot<R, N, K, s, n> Root;   ///< Type of Root
	typedef class BalTreeInNode<R, N, K, s, n> Node;   ///< Type of Node
protected:
	BalTreeInNode(); // No default add as that needs our Parent already constructed
	virtual ~BalTreeInNode();

//	void addTo(R& root);
//	void addTo(R* root);
//	virtual void remove();           // Virtual so "fancier" trees can rebalence

            R* root() const     { return Base::root(); }
            N* parent() const   { return Base::parent(); }
            N* left() const     { return Base::left();  }
            N* right() const    { return Base::right(); }
            N* next() const     { return Base::next(); }
            N* prev() const     { return Base::prev(); }

            N*  rotateRight();
            N*  rotateLeft();

	bool    check() const override;
	void    rebalance() override = 0;

	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const						{ return ContainerNode<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 ContainerNode<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock(bool upgrade) const 						{ return ContainerNode<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 ContainerNode<s>::readUnlock(save);}
};

#endif
