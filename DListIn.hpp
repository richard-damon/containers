#ifndef DListIn_HPP
#define DListIn_HPP

/**
 @file DListIn.hpp

 This file defines the implementation for a pair of templates (DListInRoot and DListInNode) that
 implement an intrusive double linked list.

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

@see DListIn.h
@see ListIn.h
@ingroup IntrusiveContainers
*/

#include "DListIn.h"

/******************************************************************************************
Implementation

Note that when we need to convert a List or Node pointer/reference to a DListInRoot/ListInNode
pointer/reference we need to do an explicit static_cast, to avoid ambiquity in the case of
classes deriving from multiple versions of DListInRoot/ListInNode
*/

/**
Remove node from whatever list it is on, if it is on a list.
*/

template <class R, class N, int n = 0> inline void DListInNode<R, N, n>::remove() {
	if (root_) {
		if (next_) { // if someone after us, update their back pointer
			static_cast<Node*>(next_)->prev_ = prev_;
		} else {
			// Else update end of list pointer
			static_cast<Root*>(root_)->last_ = prev_;
		}
		if (prev_) { // if someone before us, update their next pointer
			static_cast<Node*>(prev_)->next_ = next_;
		} else { // if no one before us update list head
			static_cast<Root*>(root_)->first_ = next_;
		}
		next_ = 0;
		prev_ = 0;
		root_ = 0;
	}
}

/**
Remove Node from List

@param node node to be removed.
If node is not on this list, nothing will be done.
*/
template <class R, class N, int n_> inline void DListInRoot<R, N, n_>::remove(N& node) {
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
template <class R, class N, int n_> inline void DListInRoot<R, N, n_>::remove(N* node) {
	if (node != 0) remove(*node);
}

/**
Add node to front of our list.

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, int n_> inline void DListInRoot<R, N, n_>::addFirst(N& node) {
	Node& mynode = static_cast<Node&>(node);

	if (mynode.root_ != 0) mynode.remove();

	mynode.root_ = static_cast<R*>(this);
	if (first_ == 0) {
		// Empty list, point tail pointer too.
		last_ = &node;
	}
	mynode.next_ = first_;
	mynode.prev_ = 0;
	first_ = &node;
}

/**
Add node to front of our list.

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, int n_> inline void DListInRoot<R, N, n_>::addFirst(N* node) {
	if (node != 0) addFirst(*node);
}

/**
Add node to end of our list.
Note that this operation is O(n) on list current size. See DListInRoot to make this O(1).

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/

template<class R, class N, int n_> inline void DListInRoot<R, N, n_>::addLast(N& node) {
	Node& mynode = static_cast<Node&>(node);

	if (mynode.root_ != 0) mynode.remove();

	if (last_ == 0) {
		// Empty List
		first_ = &node;
		mynode.prev_ = 0;
	} else {
		mynode.prev_ = last_;
		static_cast<Node*>(last_)->next_ = &node;
	}
	last_ = &node;
	mynode.next_ = 0;
	mynode.root_ = static_cast<R*>(this);
}

/**
Add node to end of our list.
Note that this operation is O(n) on list current size. See DListInRoot to make this O(1).

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/

template<class R, class N, int n_> inline void DListInRoot<R, N, n_>::addLast(N* node) {
	if (node != 0) addLast(*node);
}



/**
Add node to our list, at "natural" point.
Note that this is the front for singly linked lists and the end for doubly linked list.

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, int n_> inline void DListInRoot<R, N, n_>::add(N& node) {
	addLast(node);
}


/**
Add node to our list, at "natural" point.
Note that this is the front for singly linked lists and the end for doubly linked lists.

@param node The node to add to the list
If node is nulR, Nothing is done.
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, int n_> inline void DListInRoot<R, N, n_>::add(N* node) {
	if (node != 0) add(*node);
}

/**
Add ourselfs to the front of a list

@param root List to add to.
*/
template<class R, class N, int n> inline void DListInNode<R, N, n>::addToFront(R& root) {
	static_cast<Root&>(root).addFirst(*static_cast<N*>(this));
}

/**
Add ourselfs to the front of a list

@param root List to add to.
If NULL, just remove from all lists.
*/
template<class R, class N, int n> inline void DListInNode<R, N, n>::addToFront(R* root) {
	if (root) {
		static_cast<Root*>(root)->addFirst(*static_cast<N*>(this));
	} else {
		remove();
	}
}

/**
Add ourselfs to the end of a list

@param root List to add to.
*/
template<class R, class N, int n> inline void DListInNode<R, N, n>::addToEnd(R& root) {
	static_cast<Root&>(root).addLast(*static_cast<N*>(this));
}


/**
Add ourselfs to the End of a list

@param root List to add to.
If NULL, just remove from all lists.
*/
template<class R, class N, int n> inline void DListInNode<R, N, n>::addToEnd(R* root) {
	if (root) {
		static_cast<Root*>(root)->addLast(*static_cast<N*>(this));
	} else {
		remove();
	}
}

/**
Add ourself to a list after another node.

@param node The node to add ourself after.
If node is not on a list, do nothing.
*/

template<class R, class N, int n> inline void DListInNode<R, N, n>::addAfter(N& node) {
	Node &mynode = static_cast<Node&>(node);
	N* me = static_cast<N*>(this);
	if (mynode.root_ && &node != me) {
		remove();
		root_ = mynode.root_;
		next_ = mynode.next_;
		prev_ = &node;
		mynode.next_ = me;
		if (next_) {
			static_cast<Node*>(next_)->prev_ = me;
		} else {
			static_cast<Root*>(root_)->last_ = me;
		}
	}
}

/**
Add ourself to a list after another node.

@param node The node to add ourself after.
If Node is NULL, or not on a list, do nothing.
*/

template<class R, class N, int n> inline void DListInNode<R, N, n>::addAfter(N* node) {
	if (node != 0) {
		addAfter(*node);
	}
}

/**
Add ourself to a list at "natural" postion.
Note that this is the front for singly linked lists, and the end for doubly linked lists.

@param root List to add to.
*/
template<class R, class N, int n> void DListInNode<R, N, n>::addTo(R& root) {
	addToFront(root);
}

/**
Add ourself to a list at "natural" postion.
Note that this is the front for singly linked lists, and the end for doubly linked lists.

@param root List to add to.
*/
template<class R, class N, int n> void DListInNode<R, N, n>::addTo(R* root) {
	addToFront(root);
}


/**
Constructor.

Starts us as an empty list.
*/
template <class R, class N, int n> DListInRoot<R, N, n>::DListInRoot() :
first_(0),
last_(0)
{}

/**
Destructor.

Removes all nodes attached to us.
*/
template <class R, class N, int n> DListInRoot<R, N, n>::~DListInRoot() {
	while (first_) remove(first_);
}

/**
Constructor.

@param root Pointer to list for node to be added to (if not NULL).
*/
template <class R, class N, int n> DListInNode<R, N, n>::DListInNode(R* root) :
root_(0),
prev_(0),
next_(0)
{
	if (root) addTo(root);
}

/**
Constructor.

@param root list we are to be added to.
*/
template <class R, class N, int n> DListInNode<R, N, n>::DListInNode(R& root) :
root_(0),
prev_(0),
next_(0)
{
	addTo(root);
}

/**
Destructor.

Remove us from we are on, if any.
*/
template <class R, class N, int n> DListInNode<R, N, n>::~DListInNode() {
	remove();
}
#endif
