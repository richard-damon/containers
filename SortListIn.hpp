#ifndef SortListIn_HPP
#define SortListIn_HPP

/**
 @file SortListIn.hpp
 @brief Intrusive Double Linked List, Implementation.

 This file defines the implementation for a pair of templates (DListInRoot and DListInNode) that
 implement an intrusive double linked list.

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

@see DListIn.h
@see ListIn.h
@ingroup IntrusiveContainers
*/

#include "SortListIn.h"
#include "ListIn.hpp"

/******************************************************************************************
Implementation

Note that when we need to convert a List or Node pointer/reference to a DListInRoot/ListInNode
pointer/reference we need to do an explicit static_cast, to avoid ambiguity in the case of
classes deriving from multiple versions of DListInRoot/ListInNode
*/


template <class R, class N, ContainerThreadSafety s, int n>
bool SortListInRoot<R, N, s, n>::check() const {
    bool flag = Base::check();
    // We presume that ListInRoot::check will scan the list
    return flag;
}

template <class R, class N, ContainerThreadSafety s, int n>
bool SortListInNode<R, N, s, n>::check() const {
    bool flag = Base::check();
    flag &= static_cast<Root*>(root())->compare(*static_cast<N const*>(this), *next()) >= 0;
    flag &= static_cast<Root*>(root())->compare(*next(), *static_cast<N const*>(this)) <= 0;
    return flag;
}

/**
Add node to list based on sorting function in Root.

The node is placed such that compare(node, this) will be positive (or zero) for all
 nodes before this one, and negative for nodes after (or zero if they were added after us)

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void SortListInRoot<R, N, s, n_>::add(N& node) {
    Node& mynode = node;
    // If node is on a list, remove it.
    // Don't bypass same list, as might be used to update sorting.
    if(mynode.root() != nullptr) mynode.remove();

    unsigned save = readLock(true);

    N* node1 = nullptr;
    N* node2 = first();
    while(node2 && this->compare(*node2, node) >= 0) {
        node1 = node2;
        node2 = static_cast<Node*>(node2)->next();
    }

    // node1 is the node that should be before the provided node

    if (node1) {
        node1->Node::addAfter(node, true);
    } else {
        Base::addFirst(node, true);
    }
    readUnlock(save);
}


/**
Add node to our list, at "natural" point.
Note that this is the front for singly linked lists and the end for doubly linked lists.

@param node The node to add to the list
If node is null, Nothing is done.
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void SortListInRoot<R, N, s, n_>::add(N* node) {
	if (node != nullptr) add(*node);
}

/**
Add ourself to a list at "natural" position.
Note that this is the front for singly linked lists, and the end for doubly linked lists.

@param myRoot List to add to.
*/
template<class R, class N, ContainerThreadSafety s, int n>
void SortListInNode<R, N, s, n>::addTo(R& myRoot) {
    static_cast<Root&>(myRoot).add(static_cast<N*>(this));
}

/**
Add ourself to a list at "natural" position.
Note that this is the front for singly linked lists, and the end for doubly linked lists.

@param myRoot List to add to.
*/
template<class R, class N, ContainerThreadSafety s, int n>
void SortListInNode<R, N, s, n>::addTo(R* myRoot) {
    if(myRoot) {
        static_cast<Root*>(myRoot)->add(static_cast<N*>(this));
    } else {
        remove();
    }
}


/**
Constructor.

Starts us as an empty list.
*/
template <class R, class N, ContainerThreadSafety s, int n>
SortListInRoot<R, N, s, n>::SortListInRoot()
{}

/**
Destructor.

Removes all nodes attached to us.
*/
template <class R, class N,  ContainerThreadSafety s, int n>
SortListInRoot<R, N, s, n>::~SortListInRoot() {
	while (first()) remove(first());
}

/**
Constructor.

@param myRoot Pointer to list for node to be added to (if not NULL).
*/
template <class R, class N, ContainerThreadSafety s, int n>
SortListInNode<R, N, s, n>::SortListInNode(R* myRoot)
{
	if (myRoot) addTo(myRoot);
}

/**
Constructor.

@param myRoot list we are to be added to.
*/
template <class R, class N, ContainerThreadSafety s, int n>
SortListInNode<R, N, s, n>::SortListInNode(R& myRoot)
{
	addTo(myRoot);
}

/**
Destructor.

Remove us from we are on, if any.
*/
template <class R, class N, ContainerThreadSafety s, int n>
SortListInNode<R, N, s, n>::~SortListInNode() {
	remove();
}
#endif
