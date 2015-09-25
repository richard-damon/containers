#ifndef TreeIn_HPP
#define TreeIn_HPP

/**
@file TreeIn.hpp
@brief Intrusive Binary Tree (unbalenced), Implementation.

This file defines a pair of templates (TreeInRoot and TreeInNode) that
implement an intrusive binary tree.

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

@see TreeIn.h
@ingroup IntrusiveContainers
*/

#include "TreeIn.h"

/// Define Non-Zero to perform frequent Invariants Checks
#define TREEIN_CHECK 1

#if TREEIN_CHECK
#include "FreeRTOS.h"
#define assert configASSERT
#include <iostream>
#endif


/******************************************************************************************
Implementation

Note that when we need to convert a List or Node pointer/reference to a TreeInRoot/ListInNode
pointer/reference we need to do an explicit static_cast, to avoid ambiquity in the case of
classes deriving from multiple versions of TreeInRoot/ListInNode
*/

#if TREEIN_CHECK
/**
*/
template <class R, class N, class K, int n> inline void TreeInNode<R, N, K, n>::check() const {
	N const* me = static_cast<N const*>(this);
	if (root_) {
		Root* myroot = root_;
		if (parent_ == 0) {
			assert(static_cast<Root*>(root_)->base_ == me);
		} else {
			Node* myparent = static_cast<Node*>(parent_);
			assert(myparent->left_ == me || myparent->right_ == me);
			assert(myparent->root_ == root_);
		}

		if (left_) {
			Node* myleft = static_cast<Node*>(left_);
			assert(myleft->parent_ == me);
			assert(myroot->compare(*me, *left_) <= 0);
			assert(myroot->compare(*left_, *me) >= 0);
		}

		if (right_) {
			Node* myright = static_cast<Node*>(right_);
			assert(myright->parent_ == me);
			assert(myroot->compare(*me, *right_) >= 0);
			assert(myroot->compare(*right_, *me) <= 0);
		}
	} else {
		assert(parent_ == 0);
		assert(left_ == 0);
		assert(right_ == 0);
	}
}

template <class R, class N, class K, int n> inline void TreeInRoot<R, N, K, n>::check() const {
	R const* me = static_cast<R const*>(this);
	if (base_) {
		Node* node = static_cast<Node*>(base_);
		assert(node->root_ == me);
		assert(node->parent_ == 0);
		node->check();
	}
}
#endif

/**
Return node next in sort sequence.
*/
template <class R, class N, class K, int n> inline N* TreeInNode<R, N, K, n>::next() const {
	N const* node;
	Node const* mynode;
	if (right_) {
		// Node has a right child, next will be leftmost child of right child
		node = right_;
		mynode = node;
		while (mynode->left_) {
			node = mynode->left_;
			mynode = node;
		}
		return const_cast<N*>(node);
	} 
	// Node does not have a right child, accend parents chain until we reach a parent we are left linked to.
	mynode = this;
	node = static_cast<N const*>(mynode);
	while (mynode->parent_) {
		N* parent = mynode->parent_;
		mynode = parent;
		if (mynode->left_ == node) {
			return parent;
		}
		node = parent;
	}
	return 0;
}

/**
Return previous node in sort sequence.
*/
template <class R, class N, class K, int n> inline N* TreeInNode<R, N, K, n>::prev() const {
	N const* node;
	Node const* mynode;
	if (left_) {
		// Node has a left child, prev will be rightmost child of left child
		node = left_;
		mynode = node;
		while (mynode->right_) {
			node = mynode->right_;
			mynode = node;
		}
		return const_cast<N*>(node);
	}
	// Node does not have a right child, accend parents chain until we reach a parent we are left linked to.
	mynode = this;
	node = static_cast<N const*>(mynode);
	while (mynode->parent_) {
		N* parent = mynode->parent_;
		mynode = parent;
		if (mynode->right_ == node) {
			return parent;
		}
		node = parent;
	}
	return 0;
}

template <class R, class N, class K, int n> inline N* TreeInRoot<R, N, K, n>::first() const {
	if (base_ == 0) return 0;
	N* node = base_;
	Node* mynode = node;
	while (mynode->left_) {
		node = mynode->left_;
		mynode = node;
	}
	return node;
}


template <class R, class N, class K, int n> inline N* TreeInRoot<R, N, K, n>::last() const {
	if (base_ == 0) return 0;
	N* node = base_;
	Node* mynode = node;
	while (mynode->right_) {
		node = mynode->right_;
		mynode = node;
	}
	return node;
}

/**
Remove node from whatever tree it is on, if it is on a tree.
*/

template <class R, class N, class K, int n> inline void TreeInNode<R, N, K, n>::remove() {
 	if (root_) {
		static_cast<Root*>(root_)->check();
		Node* parent = static_cast<Node*>(parent_);
		N* newLink = 0;
		if (left_ == 0) {
			if (right_ == 0) {
				// We are child node, we can just unlink
			} else {
				// Only have right, link our parent to right
				newLink = right_;
			}
		} else {
			if (right_ == 0) {
				// Only have left, link our parent to left
				newLink = left_;
			} else {
				// Have both left and right children, rotate our "previous" into our spot.
				newLink = prev();
				Node* nl = static_cast<Node*>(newLink);
				// Our previous node should have its right linked to our right.
				// Our previous node's right will be null since it is below us
				nl->right_ = right_;
				static_cast<Node*>(right_)->parent_ = newLink;

				if (newLink != left_){
					// if our previous is not our direct left, unlink it from its parent
					// linking in its stead it left.

					static_cast<Node*>(nl->parent_)->right_ = nl->left_;
					if (nl->left_){
						static_cast<Node*>(nl->left_)->parent_ = nl->parent_;
					}
					// Link our left into our previous left now that it is open.

					nl->left_ = left_;
					static_cast<Node*>(left_)->parent_ = newLink;

				}
			}
		}
		// link our replacement to our parent.
		if (newLink) {
			static_cast<Node*>(newLink)->parent_ = parent_;
		}
		// update our parent (or root) to point to our replacement (newLink)
		if (parent) {
			if (parent->left_ == static_cast<N*>(this)) {
				parent->left_ = newLink;
			} else /*if (parent->right_ == static_cast<N*>(this))*/ {
				parent->right_ = newLink;
			}
		} else {
			static_cast<Root*>(root_)->base_ = newLink;
		}
		static_cast<Root*>(root_)->check();
		root_ = 0;
		parent_ = 0;
		left_ = 0;
		right_ = 0;
		check();
	}
}

/**
Remove Node from List

@param node node to be removed.
If node is not on this list, nothing will be done.
*/
template <class R, class N, class K, int n> inline void TreeInRoot<R, N, K, n>::remove(N& node) {
	Node& mynode = static_cast<Node&>(node);
	if (mynode.root_ == this) {
		// Only remove if node is on this list.
		mynode.remove();
	}
}


/**
Remove Node from List

@param node Pointer to node to be removed.
If node is null, operation will be ignored.
If node is not on this list, nothing will be done.
*/
template <class R, class N, class K, int n_> inline void TreeInRoot<R, N, K, n_>::remove(N* node) {
	if (node != 0) remove(*node);
}

/**
Add node to our list, at "natural" point.
Note that this is the front for singly linked lists and the end for doubly linked list.

@param node The node to add to the list
If node is currently on a different list it is removed before being added to the list
*/

template<class R, class N, class K, int n> inline void TreeInRoot<R, N, K, n>::add(N& node) {
	Node& mynode = node;
	if (mynode.root_ == this) return; // if already here, do nothing.
	if (mynode.root_ != 0) mynode.remove(); // if on a different tree, remove.
	if (base_) {
		N* nodebase = base_;
		while (1) {
			Node* mynodebase = nodebase;
			int cmp = compare(*nodebase, node);
			if (cmp < 0) {
				if (mynodebase->left_) {
					nodebase = mynodebase->left_;
				} else {
					mynodebase->left_ = &node;
					mynode.parent_ = nodebase;
					break;
				}
			} else {
				if (mynodebase->right_) {
					nodebase = mynodebase->right_;
				} else {
					mynodebase->right_ = &node;
					mynode.parent_ = nodebase;
					break;
				}
			}
		}
	} else {
		// Tree Empty, so simple to add
		base_ = &node;
		Node& mynode = node;
		mynode.parent_ = 0;
	}
	mynode.root_ = static_cast<R*>(this);
	mynode.left_ = 0;
	mynode.right_ = 0;
	check();
}

/**
Add node to our list, at "natural" point.
Note that this is the front for singly linked lists and the end for doubly linked lists.

@param node The node to add to the list
If node is nulR, Nothing is done.
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, class K, int n_> inline void TreeInRoot<R, N, K, n_>::add(N* node) {
	if (node != 0) add(*node);
}

/**
Add ourself to a list at "natural" postion.
Note that this is the front for singly linked lists, and the end for doubly linked lists.

@param root List to add to.
*/


template<class R, class N, class K, int n> void TreeInNode<R, N, K, n>::addTo(R& root) {
	static_cast<Root&>(root).add(*static_cast<N*>(this));
}


/**
Add ourself to a tree

@param root Tree to add to.
*/

template<class R, class N, class K, int n> void TreeInNode<R, N, K, n>::addTo(R* root) {
	if (root) {
		addTo(*root);
	} else {
		remove();
	}
}

/**
 * find a node
 */

template<class R, class N, class K, int n> N* TreeInRoot<R, N, K, n>::find(K key) const {
	N* node = base();
	while(node) {
		int cmp = compareKey(*node, key);
		if(cmp == 0) break; // Found the node, return it
		if(cmp < 0) {
			node = static_cast<Node*>(node)->left_;
		} else {
			node = static_cast<Node*>(node)->right_;
		}
	}
	return node;
}

/**
Constructor.

Starts us as an empty list.
*/
template <class R, class N, class K, int n> TreeInRoot<R, N, K, n>::TreeInRoot() :
base_(0)
{}

/**
Destructor.

Removes all nodes attached to us.

Doesn't actually call remove on every node to avoid possible unneeded rebalencing, just
manually unlinks all nodes.
*/
template <class R, class N, class K, int n> TreeInRoot<R, N, K, n>::~TreeInRoot() {
	Node* node = base_;
	while (node) {
		if (node->left_) {
			node = node->left_;
		} else if (node->right_) {
			node = node->right_;
		} else {
			Node* parent = node->parent_;
			node->left_ = 0;
			node->right_ = 0;
			node->parent_ = 0;
			if (parent) {
				if (parent->left_ == node) {
					parent->left_ = 0;
				}
				if (parent->right_ == node) {
					parent->right_ = 0;
				}
			}
			node = parent;
		}
	}
	base_ = 0;
}

/**
Constructor.

*/
template <class R, class N, class K, int n> TreeInNode<R, N, K, n>::TreeInNode() :
root_(0),
parent_(0),
left_(0),
right_(0) {
}

/**
Destructor.

Remove us from we are on, if any.
*/
template <class R, class N, class K, int n> TreeInNode<R, N, K, n>::~TreeInNode() {
	remove();
}
#endif
