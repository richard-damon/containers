#ifndef AATreeIn_HPP
#define AATreeIn_HPP

/**
@file AATreeIn.hpp
@brief Intrusive Binary Tree (balanced), Implementation.

This file defines a pair of templates (TreeInRoot and TreeInNode) that
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

@see AATreeIn.h
@ingroup IntrusiveContainers
*/

#include "AATreeIn.h"
#include "BalTreeIn.hpp"

/******************************************************************************************
Implementation

Note that when we need to use a Root or Node pointer/reference as a AATreeInRoot/AAListInNode
pointer/reference we need to do an explicit static_cast, to avoid ambiguity in the case of
classes deriving from multiple versions of TreeInRoot/ListInNode
*/

template <class R, class N, class K, ContainerThreadSafety s, int n>
bool AATreeInNode<R, N, K, s, n>::check() const {
    bool flag = Base::check();
#if TREEIN_CHECK
    unsigned save = readLock(false);
    Node* node;
    node = left();
    if(node) {
        TREEIN_ASSERT(level == node->level+1); // Rule 2
    } else {
        if(root()) {
            TREEIN_ASSERT(level == 1);     // Rule 1
        } else {
            TREEIN_ASSERT(level == 0);     // Rule 0
        }
    }
    node = right();
    if(node) {
        TREEIN_ASSERT(level == node->level || level == node->level+1); // Rule 3
        node = node->right();
        if(node) {
            TREEIN_ASSERT(level > node->level);
        }
    } else {
        if(root()) {
            TREEIN_ASSERT(level == 1); // Rule 1
        } else {
            TREEIN_ASSERT(level == 0); // Rule 0
        }
    }
    readUnlock(save);
#endif
    return flag;
}


/**
Constructor.

Starts us as an empty list.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
AATreeInRoot<R, N, K, s, n>::AATreeInRoot()
{}

/**
Destructor.

Removes all nodes attached to us.

Doesn't actually call remove on every node to avoid possible unneeded rebalencing, just
manually unlinks all nodes.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
AATreeInRoot<R, N, K, s, n>::~AATreeInRoot() {
}

/**
Constructor.

*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
AATreeInNode<R, N, K, s, n>::AATreeInNode()
{
}

/**
Destructor.

Remove us from we are on, if any.
*/
template <class R, class N, class K, ContainerThreadSafety s, int n>
AATreeInNode<R, N, K, s, n>::~AATreeInNode() {
	Base::remove();
}

/**
 * @brief AATree Balancing
 *
 * Basic Rules
 * @invariant
 * @parblock
 * AATrees have the following invariants:
 *
 *  1. leaf nodes have level = 1
 *  2. left child have level = parent - 1
 *  3. right child have level = parent - 1, or parent
 *  4. right child of right child has level < grandparent (only 1 == in a row)
 *  5. if level > 1, has both left and right.
 *
 * Implementation of Rules (reversing to look Up not down)
 *
 * Let L be the level of our Left child node
 * Let R be the level of our Right child node
 * Let G be the level of our Right-Right grandchild node.
 *
 * Non-existent nodes have a level of 0
 *
 *  1. Our level needs to be L+1
 *  2. This value needs to be R+1, or G+1 (so R == L or G == L)
 *  3. If this isn't possible, need to rotate to make this possible.
 *  4. If L > R then our children our out of balance and we need to move nodes from
 * the left to the right, so rotate to the right
 *  5. if L == R or L == G, then our level = L+1, and can move up the tree
 *  6. Else (L too much smaller than R) our children are out of balance so rotate
 * to the left.
 *
 * Rule Check:
 *  1. Satisfied, Leaf Nodes have no Left, or Left+1 = 1
 *  2. Satisfied, Our basic rule, our level = Left+1
 *  3. Satisfied, If L == R, then we are Right+1, if L == G then we are Right or Right+1
 *  4. Satisfied, Since G can't be > L, we can't be == G.
 *  5. Satisfied, If left empty, we are level 1, if right empty, then left must be
 *    empty or we can't get L == R or L == G
 * @endparblock
 */

template <class R, class N, class K,  ContainerThreadSafety s,int n>
void AATreeInNode<R, N, K, s, n>::rebalance(){
    Node* node = this;
    unsigned save = readLock(true);     // rebalence will need to upgrade
    if(node->root() == nullptr) {
        level = 0;
    }
    while(node) {
        int levelL = 0, levelR = 0, levelG = 0;
        Node* nodeL = node->left();
        if(nodeL) {
            levelL = nodeL->level;
        }
        Node* nodeR = node->right();
        if(nodeR) {
            levelR = nodeR->level;
            nodeR = nodeR->right();
            if(nodeR) {
                levelG = nodeR->level;
            }
        }
        if(levelL > levelR) {
            node->rotateRight();
        } else if(levelL == levelR || levelL == levelG) {
            node->level = levelL + 1;
            node = node->parent();
        } else {
            node->rotateLeft();
        }
    }
    readUnlock(save);
}

#endif
