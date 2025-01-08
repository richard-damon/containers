#ifndef TreeIn_HPP
#define TreeIn_HPP

/**
@file TreeIn.hpp
@brief Intrusive Binary Tree (unbalenced), Implementation.

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

@see TreeIn.h
@ingroup IntrusiveContainers
*/

#include "TreeIn.h"

#define TREEIN_CHECK_ASSERT 0x02    ///< @todo Implement conditional assertions
#define TREEIN_CHECK_AUTO   0x04
/// Define Non-Zero to perform frequent Invariants Checks
#ifndef TREEIN_CHECK
/**
 * TreeChecking
 * 0 = Disable Checking
 * +2 to check with asserts
 * +4 to check after operations
 *
 */
#define TREEIN_CHECK 0
#endif

#if TREEIN_CHECK
#include "FreeRTOS.h"

#if TREEIN_CHECK & TREEIN_CHECK_ASSERT
#define TREEIN_ASSERT(cond) configASSERT(cond)
#else
#define TREEIN_ASSERT(cond) if(!(cond)) { readUnlock(save); return false; }
#endif

#endif


/******************************************************************************************
Implementation

Note that when we need to convert a List or Node pointer/reference to a TreeInRoot/ListInNode
pointer/reference we need to do an explicit static_cast, to avoid ambiquity in the case of
classes deriving from multiple versions of TreeInRoot/ListInNode
*/

#if TREEIN_CHECK
/**
 * Tree Integrity check.
 * 
 * Constraints Checked:
 * + All Node in a tree point to the Root structure for that tree
 *   + Check that Node with Parent = NULL is pointed to be its Root
 *   + All other Nodes point to the same Root as their Parent
 *   + or if not in a tree, have no connections at all
 * + Linkage forms a tree
 *   + Check that all nodes are pointed to by the left or right of their parent
 * + Nodes are sorted Properly
 *   + Our Left Node is less than us
 *   + The Largest Node under our Left Node (right most child) is less than us
 *   + Our Right Node is greater than us
 *   + The Smallest Node under our Right Node (left most child) is greter than us
 */
template <class R, class N, class K, ContainerThreadSafety s, int n>
inline bool TreeInNode<R, N, K, s, n>::check() const {
	N const* me = static_cast<N const*>(this);
	unsigned save = readLock(false);
	if (m_root) {
		Root* myroot = m_root;
		if (m_parent == nullptr) {
			TREEIN_ASSERT(static_cast<Root*>(m_root)->m_base == me);
		} else {
			Node* myparent = m_parent;
			TREEIN_ASSERT(myparent->m_left == me || myparent->m_right == me);
			TREEIN_ASSERT(myparent->m_root == m_root);
		}

		if (m_left) {
			Node* myleft = m_left;
			TREEIN_ASSERT(myleft->m_parent == me);
			TREEIN_ASSERT(myroot->compare(*me, *m_left) <= 0);
			TREEIN_ASSERT(myroot->compare(*m_left, *me) >= 0);
			if(myleft->m_right) {
				// Travese to find right-most child
				while(myleft->m_right) {
					myleft = myleft->m_right;
				}
				N const* rightmost = static_cast<N const*>(myleft);
				TREEIN_ASSERT(myroot->compare(*me, *rightmost) <= 0);
				TREEIN_ASSERT(myroot->compare(*rightmost, *me) >= 0);
			}
		}

		if (m_right) {
			Node* myright = m_right;
			TREEIN_ASSERT(myright->m_parent == me);
			TREEIN_ASSERT(myroot->compare(*me, *m_right) >= 0);
			TREEIN_ASSERT(myroot->compare(*m_right, *me) <= 0);
			if(myright->m_left) {
				// Travers to find left-most child
				while(myright->m_left) {
					myright = myright->m_left;
				}
				N const* leftmost = static_cast<N const*>(myright);
				TREEIN_ASSERT(myroot->compare(*me, *leftmost) >= 0);
				TREEIN_ASSERT(myroot->compare(*leftmost, *me) <= 0);
			}
		}
	} else {
	    // root == 0 means we must be disconnected.
		TREEIN_ASSERT(m_parent == nullptr);
		TREEIN_ASSERT(m_left == nullptr);
		TREEIN_ASSERT(m_right == nullptr);
	}
	readUnlock(save);
	return true;
}

template <class R, class N, class K, ContainerThreadSafety s, int n>
inline bool TreeInRoot<R, N, K, s, n>::check() const {
	R const* me = static_cast<R const*>(this);
	bool  flag = true;
	unsigned save = readLock(false);
	if (m_base) {
		Node const* node = m_base;
		TREEIN_ASSERT(node->m_root == me);
		TREEIN_ASSERT(node->m_parent == nullptr);
		while(node) {
	        flag = node->check();
	        if(!flag) {
	            readUnlock(save);
	            return 0;
	        } else if(node->m_left != nullptr) {
		        node = node->m_left;
		    } else if(node->m_right != nullptr) {
		        node = node->m_right;
		    } else {
		        while(node->m_parent) {
	                Node* parent = node->m_parent;
	                if(parent->m_left == node && parent->m_right){
	                    node = parent->m_right;
	                    break;
	                }
	                node = parent;
		        }
		    }
		}
	}
	readUnlock(save);
	return flag;
}
#else
// Simple versions that bypass checking.

template <class R, class N, class K, ContainerThreadSafety s, int n> inline bool TreeInNode<R, N, K, s, n>::check() const {
    return true;
}
template <class R, class N, class K, ContainerThreadSafety s, int n> inline bool TreeInRoot<R, N, K, s, n>::check() const {
    return true;
}
#endif

/**
Return node next in sort sequence.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
inline N* TreeInNode<R, N, K, s, n>::next() const {
	N* node;
	Node* mynode;
	unsigned save = readLock(false);
	if (m_right) {
		// Node has a right child, next will be leftmost child of right child
		node = m_right;
		mynode = node;
		while (mynode->m_left) {
			node = mynode->m_left;
			mynode = node;
		}
		readUnlock(save);
		return node;
	} 
	// Node does not have a right child, accend parents chain until we reach a parent we are left linked to.
	mynode = const_cast<Node *>(this);
	node = static_cast<N*>(mynode);
	while (mynode->m_parent) {
		N* myparent = mynode->m_parent;
		mynode = myparent;
		if (mynode->m_left == node) {
		    readUnlock(save);
			return myparent;
		}
		node = myparent;
	}
	readUnlock(save);
	return nullptr;
}

/**
Return previous node in sort sequence.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
inline N* TreeInNode<R, N, K, s, n>::prev() const {
	N* node;
	Node* mynode;
	unsigned save = readLock(false);
	if (m_left) {
		// Node has a left child, prev will be rightmost child of left child
		node = m_left;
		mynode = node;
		while (mynode->m_right) {
			node = mynode->m_right;
			mynode = node;
		}
		readUnlock(save);
		return node;
	}
	// Node does not have a right child, ascend parents chain until we reach a parent we are left linked to.
	mynode = const_cast<Node*>(this);
	node = static_cast<N*>(mynode);
	while (mynode->m_parent) {
		N* myparent = mynode->m_parent;
		mynode = myparent;
		if (mynode->m_right == node) {
		    readUnlock(save);
			return myparent;
		}
		node = myparent;
	}
	readUnlock(save);
	return nullptr;
}

template <class R, class N, class K, ContainerThreadSafety s, int n>
inline N* TreeInRoot<R, N, K, s, n>::first() const {
    unsigned save = readLock(false);
	if (m_base == nullptr) return nullptr;
	N* node = m_base;
	Node* mynode = node;
	while (mynode->m_left) {
		node = mynode->m_left;
		mynode = node;
	}
	readUnlock(save);
	return node;
}


template <class R, class N, class K, ContainerThreadSafety s, int n>
inline N* TreeInRoot<R, N, K, s, n>::last() const {
	if (m_base == 0) return 0;
	unsigned save = readLock();
	N* node = m_base;
	Node* mynode = node;
	while (mynode->m_right) {
		node = mynode->m_right;
		mynode = node;
	}
	readUnlock(save);
	return node;
}

/**
 * Remove node from whatever tree it is on, if it is on a tree.
 * @verbatim

   Delete Node N

   Simple Cases, Not both childern (x is empty link)

   P     P    P    P    P    P
   | =>  x    | => |    | => |
   N          N    L    N    R
  x x        / x         \
            L             R

   Rebalance at P

More Complicated, as we have both childern

   More Complicated                    Special Case

   P                  P                P        P
   |      =>          |                |   =>   |
   N                  D                N        L
  / \                / \              / \      / \
 L   R              L   R            L   R    A   R
  \                  \              / x
   A                  A            A
    \                  \
     B                  B
      \                  \
       D                  C
      / x
     C

 Rebalance at B.  D might be L if it had no right child in which case rebalance at L

 L < A < B < C < D < N < R

 Link to P either a left or right, or Root if P is NULL.
 x indicates NULL links

   @endverbatim
 *
 */

template <class R, class N, class K,  ContainerThreadSafety s,int n>
inline void TreeInNode<R, N, K, s, n>::remove() {
 	if (m_root) {
		Root* root = m_root;
#if TREEIN_CHECK & TREEIN_CHECK_AUTO
 		root->check();
#endif
   		unsigned save = writeLock(false);

		Node* myparent = m_parent;
		N* newLink = nullptr;
		if (m_left == nullptr) {
			if (m_right == nullptr) {
				// We are child node, we can just unlink
			} else {
				// Only have right, link our parent to right
				newLink = m_right;
			}
		} else {
			if (m_right == nullptr) {
				// Only have left, link our parent to left
				newLink = m_left;
			} else {
				// Have both left and right children, rotate our "previous" into our spot.
				newLink = prev();
				Node* nl = newLink;
				// Our previous node should have its right linked to our right.
				// Our previous node's right will be null since it is below us
				nl->m_right = m_right;
				static_cast<Node*>(m_right)->m_parent = newLink;

				if (newLink != m_left){
					// if our previous is not our direct left, unlink it from its parent
					// linking in its stead it left.

					static_cast<Node*>(nl->m_parent)->m_right = nl->m_left;
					if (nl->m_left){
						static_cast<Node*>(nl->m_left)->m_parent = nl->m_parent;
					}
					// Link our left into our previous left now that it is open.

					nl->m_left = m_left;
					static_cast<Node*>(m_left)->m_parent = newLink;

				}
			}
		}
		// link our replacement to our parent.
		if (newLink) {
			static_cast<Node*>(newLink)->m_parent = m_parent;
		}
		// update our parent (or root) to point to our replacement (newLink)
		if (myparent) {
			if (myparent->m_left == static_cast<N*>(this)) {
				myparent->m_left = newLink;
			} else /*if (parent->m_right == static_cast<N*>(this))*/ {
				myparent->m_right = newLink;
			}
		} else {
			static_cast<Root*>(m_root)->m_base = newLink;
		}
#if TREEIN_CHECK & TREEIN_CHECK_AUTO
		static_cast<Root*>(m_root)->check();
#endif
		setRoot(nullptr);
		m_parent = nullptr;
		m_left = nullptr;
		m_right = nullptr;
		root->writeUnlock(save);
		rebalance();    // rebalence to update to being a "free" node.
#if TREEIN_CHECK & TREEIN_CHECK_AUTO
		check();
#endif
	}
}

/**
Remove Node from List

@param node node to be removed.
If node is not on this list, nothing will be done.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
inline void TreeInRoot<R, N, K, s, n>::remove(N& node) {
	Node& mynode = node;
	unsigned save = writeLock(false);
	if (mynode.m_root == this) {
		// Only remove if node is on this list.
		mynode.remove();
	}
	writeUnlock(save);
}


/**
Remove Node from List

@param node Pointer to node to be removed.
If node is null, operation will be ignored.
If node is not on this list, nothing will be done.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n_>
inline void TreeInRoot<R, N, K, s, n_>::remove(N* node) {
	if (node != nullptr) remove(*node);
}

/**
 * Add node to our tree, note trees are sorted by the compare member function which needs to be implemented by the user.
 *
 * @param node The node to add to the list
 * If node is currently on a different list it is removed before being added to the list.
 *
 * If node is on the requested list, do nothing. If trying to change value and position, use resort
 */

template<class R, class N, class K,  ContainerThreadSafety s,int n>
inline void TreeInRoot<R, N, K, s, n>::add(N& node) {
	Node& mynode = node;
	if (mynode.m_root == this) return; // if already here, do nothing.
	if (mynode.m_root != nullptr) mynode.remove(); // if on a different tree, remove.
    unsigned save = writeLock(false);
	if (m_base) {
		N* nodebase = m_base;
		while (1) {
			Node* mynodebase = nodebase;
			int cmp = compare(*nodebase, node);
			if (cmp < 0) {
				if (mynodebase->m_left) {
					nodebase = mynodebase->m_left;
				} else {
					mynodebase->m_left = &node;
					mynode.m_parent = nodebase;
					break;
				}
			} else {
				if (mynodebase->m_right) {
					nodebase = mynodebase->m_right;
				} else {
					mynodebase->m_right = &node;
					mynode.m_parent = nodebase;
					break;
				}
			}
		}
	} else {
		// Tree Empty, so simple to add
		m_base = &node;
		mynode.m_parent = nullptr;
	}
	mynode.setRoot(static_cast<R*>(this));
	mynode.m_left = nullptr;
	mynode.m_right = nullptr;
	writeUnlock(save);
	mynode.rebalance();
#if TREEIN_CHECK & TREEIN_CHECK_AUTO
	check();
#endif
}

/**
Add node to our tree.

@param node The node to add to the list
If node is null, Nothing is done.
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, class K, ContainerThreadSafety s, int n_>
inline void TreeInRoot<R, N, K, s, n_>::add(N* node) {
	if (node != nullptr) add(*node);
}

/**
Add ourself to a tree.

@param myroot Tree to add to.
*/


template<class R, class N, class K, ContainerThreadSafety s, int n>
void TreeInNode<R, N, K, s, n>::addTo(R& myroot) {
	// Defer the add to the Tree which knows the sort
	static_cast<Root&>(myroot).add(*static_cast<N*>(this));
}


/**
Add ourself to a tree

@param myroot Tree to add to.
*/

template<class R, class N, class K, ContainerThreadSafety s, int n>
void TreeInNode<R, N, K, s, n>::addTo(R* myroot) {
	if (myroot) {
		addTo(*myroot);
	} else {
		remove();
	}
}

/**
 * find a node
 */

template<class R, class N, class K, ContainerThreadSafety s, int n>
N* TreeInRoot<R, N, K, s, n>::find(K key) const {
	N* node = base();
	unsigned save = readLock(false);
	while(node) {
		int cmp = compareKey(*node, key);
		if(cmp == 0) break; // Found the node, return it
		if(cmp < 0) {
			node = static_cast<Node*>(node)->m_left;
		} else {
			node = static_cast<Node*>(node)->m_right;
		}
	}
	readUnlock(save);
	return node;
}
/**
 * find a node, or the node that would be just lower than this node
 */

template<class R, class N, class K, ContainerThreadSafety s, int n>
N* TreeInRoot<R, N, K, s, n>::findMinus(K key) const {
	N* node = base();
	N* cursor = node;
	unsigned save = readLock(false);
	while(cursor) {
		int cmp = compareKey(*cursor, key);
		if(cmp == 0) {
			node = cursor;
			break; // Found the node, return it
		}
		if(cmp < 0) {
			node = cursor;
			cursor = static_cast<Node*>(cursor)->m_left;
		} else {
			cursor = static_cast<Node*>(cursor)->m_right;
		}
	}
	readUnlock(save);
	return node;
}

/**
 * find a node, r the node that would be just above this node.
 */

template<class R, class N, class K, ContainerThreadSafety s, int n>
N* TreeInRoot<R, N, K, s, n>::findPlus(K key) const {
	N* node = base();
	N* cursor = node;
	unsigned save = readLock(false);
	while(cursor) {
		int cmp = compareKey(*cursor, key);
		if(cmp == 0) {
			node = cursor;
			break; // Found the node, return it
		}
		if(cmp < 0) {
			cursor = static_cast<Node*>(cursor)->m_left;
		} else {
			node = cursor;
			cursor = static_cast<Node*>(cursor)->m_right;
		}
	}
	readUnlock(save);
	return node;
}


/**
Constructor.

Starts us as an empty list.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
TreeInRoot<R, N, K, s, n>::TreeInRoot() :
m_base(nullptr)
{}

/**
Destructor.

Removes all nodes attached to us.

Doesn't actually call remove on every node to avoid possible unneeded rebalencing, just
manually unlinks all nodes.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
TreeInRoot<R, N, K, s, n>::~TreeInRoot() {
	Node* node = m_base;
	while (node) {
		if (node->m_left) {
			node = node->m_left;
		} else if (node->m_right) {
			node = node->m_right;
		} else {
			Node* parent = node->m_parent;
			node->m_left = nullptr;
			node->m_right = nullptr;
			node->m_parent = nullptr;
			if (parent) {
				if (parent->m_left == node) {
					parent->m_left = nullptr;
				}
				if (parent->m_right == node) {
					parent->m_right = nullptr;
				}
			}
			node = parent;
		}
	}
	m_base = nullptr;
}

/**
Constructor.

*/
template <class R, class N, class K,  ContainerThreadSafety s,int n>
TreeInNode<R, N, K, s, n>::TreeInNode() :
ContainerNode<s>(nullptr),
m_root(nullptr),
m_parent(nullptr),
m_left(nullptr),
m_right(nullptr) {
}

/**
Destructor.

Remove us from we are on, if any.
*/
template <class R, class N, class K, ContainerThreadSafety s,int n>
TreeInNode<R, N, K, s, n>::~TreeInNode() {
	remove();
}
#endif
