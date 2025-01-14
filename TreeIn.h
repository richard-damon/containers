#ifndef TreeIn_H
#define TreeIn_H

/**
@file TreeIn.h
@brief Intrusive Binary Tree (unbalanced).

This file defines a pair of templates (TreeInRoot and TreeInNode) that
implement an intrusive binary tree.

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
TreeInRoot and TreeInNode provide a simple API to allow classes to provide a
basic Tree <-> Node relationship (1 Tree Root holding many Nodes) as a Binary Tree.

Note that this class makes no attempt to "balance" the tree, but is designed that it can be derived
from by a class to implement any needed balancing.

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
Given:
+ R& root
+ N& node

TreeInRoot
+ if root.m_base != nullptr:
  + root.m_base-> m_root == &root
  + root.m_base-> m_parent == nullptr

TreeInNode
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

#include "Container.h"

template <class R, class N, class K, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class TreeInNode;
template <class R, class N, class K, ContainerThreadSafety s=ContainerNoSafety, int n = 0> class TreeInRoot;
// Forward declared so we can make friend.
template <class R, class N, class K, ContainerThreadSafety s, int n = 0> class BalTreeInNode;

/**
@class TreeInRoot

Intrusive Binary Tree, Root.

@tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
@tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
@tparam K The class defining the Key used to lookup Nodes in the tree.
@tparam s The ContainerThreadSafety value to define the thread safety model of the Container
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
+ if m_base != nullptr:
  + m_base->m_root == this.
  + m_base->m_parent == nullptr
@endparblock

@see TreeInNode
@ingroup IntrusiveContainers

@dotfile TreeIn_Structure.dot "Tree Structure"
Summary:
+ Root:
  + Base pointer to the Base node of the Tree
+ Node
  + (Red) Nodes have a Root pointer to the Root structure for the tree
  + (Blue) Parent pointer to the Node leading to the base of the Tree. 
    + The node will be on the Left or Right of that Parent. 
	+ The Base node will have a null parent.
  + Left pointer to the subtree of Nodes that are less than this node
  + Right pointer to the subtree of Nodes that are greater than this node  


*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
class TreeInRoot : public Container<s> {
	friend  class TreeInNode<R, N, K, s, n>;
    friend  class BalTreeInNode<R, N, K, s, n>;     ///< Let Balancer get to us.
	typedef class TreeInRoot<R, N, K, s,  n> Root;   ///< Type of TreeInRoot
	typedef class TreeInNode<R, N, K, s, n> Node;   ///< Type of DListIInNode
protected:
	TreeInRoot();
	virtual ~TreeInRoot();

	// These two member functions to be provided by the user.
	/**
	 * Compare Nodes
	 * Should return >0 if node2 > node1, and <0 if node2 < node1, 0 if equal
	 */
	int compare(N const& node1, N const& node2) const;
	/**
	 * Compare Key to node
	 * Should return >0 if key > node, <0 if key < node, 0 if key == node
	 */
	int compareKey(N const& node, K key) const;

	virtual void add(N& node);
	void add(N* node);
	void remove(N& node);
	void remove(N* node);
	N* find(K key) const;
	N* findPlus(K key) const;
	N* findMinus(K key) const;

	N* base() const { return m_base; }
	N* first() const;
	N* last() const;

	bool check() const override;

		// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const						{ return Container<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 Container<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock(bool upgrade) const 						{ return Container<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 Container<s>::readUnlock(save);}

private:
	N* m_base;                            ///< Pointer to root node on tree.
};

/**
@class TreeInNode
Intrusive Binary Tree, Node.

@tparam R The class that will be the owner of the List. Must derive from TreeInRoot<R, N, K, n>
@tparam N The class that will be the nodes of the List. Must derive from TreeInNode<R, N, K, n>
@tparam K The class defining the Key used to lookup Nodes in the tree.
@tparam s The ContainerThreadSafety value to define the thread safety model of the Container
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@invariant
@parblock
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

@see TreeInRoot
@ingroup IntrusiveContainers

@dotfile TreeIn_Structure.dot "Tree Structure"

Summary:
+ Root:
  + Base pointer to the Base node of the Tree
+ Node
  + (Red) Nodes have a Root pointer to the Root structure for the tree
  + (Blue) Parent pointer to the Node leading to the base of the Tree. 
    + The node will be on the Left or Right of that Parent. 
	+ The Base node will have a null parent.
  + Left pointer to the subtree of Nodes that are less than this node
  + Right pointer to the subtree of Nodes that are greater than this node  


*/
/*
@verbatim
                                        +------+
Root:                         /-------->| Root |<----------\
B: Base                       |         +------+           |
                              |         B |  ^             |
                              |           |  |    x        |
			                  |	          v  | R  | P      |
			                  |     +--------------+       |
Node:                         |     |  Node:Base   |       |
R: Root                       |     +--------------+       |
P: Parent                     |      L | ^   R | ^         |
L: Left                       |        | |     | |         |
R: Right                      |        / /     \ \         |
                              |       / /       \ \        |
			                  |      / /         \ \       |
			                  |     / /           \ \      |
			                  |    | |            | |      |  
                            R |    v | P          v | P    | R     
                          +--------------+     +--------------+
			              |  Node:Left   |     |  Node:Right  |
                          +--------------+     +--------------+
@endverbatim
*/

template <class R, class N, class K, ContainerThreadSafety s, int n>
class TreeInNode : public ContainerNode<s> {
	friend  class TreeInRoot<R, N, K, s, n>;
	friend  class BalTreeInNode<R, N, K, s, n>;     ///< Let Balancer get to us.
	typedef class TreeInRoot<R, N, K, s, n> Root;   ///< Type of TreeInRoot
	typedef class TreeInNode<R, N, K, s, n> Node;   ///< Type of DListIInNode
protected:
	TreeInNode(); // No default add as that needs our Parent already constructed
	virtual ~TreeInNode();

	        void    addTo(R& root);
	        void    addTo(R* root);
	virtual void    remove();           // Virtual so "fancier" trees can rebalence

	        R*      root() const { return m_root; }    ///< Return pointer to tree we are on.
	        N*      parent() const { return m_parent; }
	        N*      left() const { return m_left; }
	        N*      right() const { return m_right; }
	        N*      next() const;
	        N*      prev() const;

			void	setRoot(R* root) { m_root = root; ContainerNode<s>::setRoot(static_cast<Root*>(root)); }

			bool    check() const override;
	        /**
	         * Hook to allow Balanced trees to rebalance from current node, just changed
	         */
	virtual void    rebalance() { return; }
	        void    resort();

	// Critical Sections used to Update the Container
    unsigned writeLock(bool upgrade) const						{ return ContainerNode<s>::writeLock(upgrade); }
    void writeUnlock(unsigned save) const						{ 		 ContainerNode<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock(bool upgrade) const 						{ return ContainerNode<s>::readLock(upgrade); }
    void readUnlock(unsigned save) const 						{ 		 ContainerNode<s>::readUnlock(save);}

private:
	R*    m_root;     	///< Pointer to tree we are on.
	N*    m_parent;   	///< Pointer to parent node on tree.
	N*    m_left;      	///< Pointer to left (lesser) child node on tree.
	N*    m_right;		///< Pointer to right (greater) child node on tree.
};

#endif
