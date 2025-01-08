#ifndef BalTreeIn_HPP
#define BalTreeIn_HPP

/**
@file BalTreeIn.hpp
@brief Intrusive Binary Tree, Implementation of Balancing Primitives

This file defines a pair of templates (BalTreeInRoot and BalTreeInNode) that
implement the Balancing Primitives for an intrusive binary tree.
A Derived class is responsible for implementing the balancing rules.

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

@see BalTreeIn.h
@ingroup IntrusiveContainers
*/

#include "BalTreeIn.h"
#include "TreeIn.hpp"

/******************************************************************************************
Implementation

Note that when we need to convert a List or Node pointer/reference to a TreeInRoot/ListInNode
pointer/reference we need to do an explicit static_cast, to avoid ambiguity in the case of
classes deriving from multiple versions of TreeInRoot/ListInNode
*/

#if TREEIN_CHECK

// TODO Can this just fall back to TreeIn::check()?  I don't think we have anything additional to check.
/**
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
inline bool BalTreeInNode<R, N, K, s, n>::check() const {
	N const* me = static_cast<N const*>(this);
	unsigned save = readLock(false);
	if (root()) {
		Root* myroot = root();
		if (parent() == nullptr) {
			TREEIN_ASSERT(static_cast<Root*>(root())->m_base == me);
		} else {
			Node* myparent = parent();
			TREEIN_ASSERT(myparent->left() == me || myparent->right() == me);
			TREEIN_ASSERT(myparent->root() == root());
		}

		if (left()) {
			Node* myleft = left();
			TREEIN_ASSERT(myleft->m_parent == me);
			TREEIN_ASSERT(myroot->compare(*me, *left()) <= 0);
			TREEIN_ASSERT(myroot->compare(*left(), *me) >= 0);
		}

		if (right()) {
			Node* myright = right();
			TREEIN_ASSERT(myright->parent() == me);
			TREEIN_ASSERT(myroot->compare(*me, *right()) >= 0);
			TREEIN_ASSERT(myroot->compare(*right(), *me) <= 0);
		}
	} else {
		TREEIN_ASSERT(parent() == nullptr);
		TREEIN_ASSERT(left() == nullptr);
		TREEIN_ASSERT(right() == nullptr);
	}
	readUnlock(save);
	return true;
}

// TODO Can this just fall back to TreeInRoot::check()?
template <class R, class N, class K, ContainerThreadSafety s, int n>
inline bool BalTreeInRoot<R, N, K, s, n>::check() const {
	R const* me = static_cast<R const*>(this);
	unsigned save = readLock(false);
	if (base()) {
		Node* node = static_cast<Node*>(base());
		TREEIN_ASSERT(node->root() == me);
		TREEIN_ASSERT(node->parent() == nullptr);
		node->check();
	}
	readUnlock(save);
	return true;
}
#else
template <class R, class N, class K, ContainerThreadSafety s, int n> inline bool BalTreeInNode<R, N, K, s, n>::check() const { return true; }
template <class R, class N, class K, ContainerThreadSafety s, int n> inline bool BalTreeInRoot<R, N, K, s, n>::check() const { return true; }
#endif

/**
Constructor.

Starts us as an empty list.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
BalTreeInRoot<R, N, K, s, n>::BalTreeInRoot()
{}

/**
Destructor.


*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
BalTreeInRoot<R, N, K, s, n>::~BalTreeInRoot() {
}

/**
Constructor.

*/
template <class R, class N, class K,  ContainerThreadSafety s,int n>
BalTreeInNode<R, N, K, s, n>::BalTreeInNode()
{
}

/**
 * Destructor.
*/
template <class R, class N, class K,  ContainerThreadSafety s, int n>
BalTreeInNode<R, N, K, s, n>::~BalTreeInNode() {
	Base::remove();
}
#endif

/**
 * @brief Balance tree with a Left Rotate.
 *
 * @verbatim
  Given A < L < B < R < C
  Either P < A, or C < P
  (A, B, C are subtrees, relations apply to ALL members in that subtree)

        P                        P
        |                        |
        R    Rotate Right =>     L
       / \   <= Rotate Left     / \
      L   C                    A   R
     / \                          / \
    A   B                        B   C

   @endverbatim
 *
 *   Rotate Right moves the current node to the right of the node on its left
 *   Rotate Left moves the current to to the left of the node on its right.
 *
 *   Caller should have an upgradable read-lock
 * 
 *   Anyone walking the tree should have a readLock that we will wait to be released.
 *
 *   @return New subtree root after rotation.
 */

template<class R, class N, class K, ContainerThreadSafety s, int n>
N* BalTreeInNode<R, N, K, s, n>::rotateLeft() {
    unsigned save = writeLock(true);
    // this/root = L, pivot = R
    N* pivot = this->Base::m_right;
    Node* pivot_ = pivot;
    N* me = static_cast<N*>(this);

    // Move middle node (B) from Pivot to Root
    this->Base::m_right  = pivot->Base::m_left;
    if(this->Base::m_right){
        this->Base::m_right->Base::m_parent = me;
    }

    // Points Roots parent to Pivot
    pivot_->Base::m_parent = this->Base::m_parent;
    if(pivot_->Base::m_parent) { // Check if left, right or was tree root
        if(pivot->Base::m_parent->Base::m_left == this) {
            pivot_->Base::m_parent->Base::m_left = pivot;
        } else {
            pivot_->Base::m_parent->Base::m_right = pivot;
        }
    } else {
        Root* root = pivot_->Base::m_root;
        root->m_base = pivot;
    }

    // Connect Pivot to the (old) Root of the rotation.
    pivot_->Base::m_left  = me;
    this->Base::m_parent = pivot;
    writeUnlock(save);
    return pivot;
}

/**
 * @brief Balance Tree with a Right Rotate
 *
 * @see rotateLeft
 *
 * @return new subtree root after rotation.
 */
template<class R, class N, class K, ContainerThreadSafety s, int n>
N* BalTreeInNode<R, N, K, s, n>::rotateRight() {
    unsigned save = writeLock(true);
    //this/root = R, pivot = L
    N* pivot = this->Base::m_left;
    Node* pivot_ = pivot;
    N* me = static_cast<N*>(this);

    // Move middle node (B) from Pivot to Root
    this->Base::m_left = pivot->Base::m_right;
    if(this->Base::m_left) {
        this->Base::m_left->Base::m_parent = me;
    }

    // Point Roots Parent to Pivot
    pivot_->Base::m_parent = this->Base::m_parent;
    if(pivot->Base::m_parent) { // Check if left, right or was tree root
        if(pivot->Base::m_parent->Base::m_left == this) {
            pivot->Base::m_parent->Base::m_left = pivot;
        } else {
            pivot->Base::m_parent->Base::m_right = pivot;
        }
    } else {
        Root* root = pivot_->Base::m_root;
        root->m_base = pivot;
    }
    // Reconnect Pivot and (old) Root of the rotation
    pivot->Base::m_right = me;
    this->Base::m_parent = pivot;
    writeUnlock(save);

    return pivot;
}
