#ifndef LISTIN_HPP
#define LISTIN_HPP

/**
@file ListIn.hpp

This file defines a pair of templates (ListInRoot and ListInNode) that
implement an intrusive singly linked list.

@copyright  (c) 2014 Richard Damon
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

@see ListIn.h
@see DListIn.h
@ingroup IntrusiveContainers
*/

#include "ListIn.h"

/******************************************************************************************
Implementation

Note that when we need to convert a List or Node pointer/reference to a ListInRoot/ListInNode
pointer/reference we need to do an explicit static_cast, to avoid ambiquity in the case of
classes deriving from multiple versions of ListInRoot/ListInNode
*/

/**
Remove node from whatever list it is on, if it is on a list.
*/
template <class R, class N, int n> inline void ListInNode<R, N, n>::remove() {
    if (root_) {
        Root* mylist = static_cast<Root*>(root_);
        // We are on a list, are we the first node?
        if (mylist->first_ == this) {
            // Yes, point root to next node in list (if any), and disconnect us.
            mylist->first_ = next_;
            next_ = 0;
            root_ = 0;
        } else {
            // We aren't the first, so find the node before us.
            N* node = mylist->first_;

            while (node != 0) {
                Node* mynode = static_cast<Node*>(node);
                N* next = mynode->next_;
                if (next == this) {
                    // Found our predecessor, unlink
                    mynode->next_ = next_;
                    next_ = 0;
                    root_ = 0;
                    break;
                }
                node = mynode->next_;
                // we could assert node here, as we should be able to find ourselves on the list
            }
        }
    }
}


/**
Remove Node from List

@param node node to be removed.
If node is not on this list, nothing will be done.
*/
template <class R, class N, int n_> inline void ListInRoot<R, N, n_>::remove(N& node) {
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
template <class R, class N, int n_> inline void ListInRoot<R, N, n_>::remove(N* node) {
    if (node != 0) remove(*node);
}

/**
Add node to front of our list.

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, int n_> inline void ListInRoot<R, N, n_>::addFirst(N& node) {
    Node& mynode = static_cast<Node&>(node);

    if (mynode.root_ != 0) mynode.remove();
    mynode.root_ = static_cast<R*>(this);
    mynode.next_ = first_;
    first_ = &node;
}

/**
Add node to front of our list.

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, int n_> inline void ListInRoot<R, N, n_>::addFirst(N* node) {
    if (node != 0) addFirst(*node);
}

/**
Add node to end of our list.
Note that this operation is O(n) on list current size. See DListInRoot to make this O(1).

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/

template<class R, class N, int n_> inline void ListInRoot<R, N, n_>::addLast(N& node) {
    Node& mynode = static_cast<Node&>(node);

    if (mynode.root_ != 0) mynode.remove();

    if (first_ == 0) {
        first_ = &node;
    } else {
        N* scannode = first_;
        N* nextnode;
        while (nextnode = static_cast<Node*>(scannode)->next_) {
            scannode = nextnode;
        }
        static_cast<Node*>(scannode)->next_ = &node;
    }
    mynode.root_ = static_cast<R*>(this);
    mynode.next_ = 0;
}

/**
Add node to end of our list.
Note that this operation is O(n) on list current size. See DListInRoot to make this O(1).

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/

template<class R, class N, int n_> inline void ListInRoot<R, N, n_>::addLast(N* node) {
    if (node != 0) addLast(*node);
}



/**
Add node to our list, at "natural" point.
Note that this is the front for singly linked lists and the end for doubly linked list.

@param node The node to add to the list
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, int n_> inline void ListInRoot<R, N, n_>::add(N& node) {
    addFirst(node);
}


/**
Add node to our list, at "natural" point.
Note that this is the front for singly linked lists and the end for doubly linked lists.

@param node The node to add to the list
If node is nulR, Nothing is done.
If node is currently on a list (even us) it is removed before being added to the list
*/
template<class R, class N, int n_> inline void ListInRoot<R, N, n_>::add(N* node) {
    if (node != 0) add(*node);
}

/**
Add ourselfs to the front of a list

@param root List to add to.
*/
template<class R, class N, int n> inline void ListInNode<R, N, n>::addToFront(R& root) {
    static_cast<Root&>(root).addFirst(*static_cast<N*>(this));
}

/**
Add ourselfs to the front of a list

@param root List to add to.
If NULL, just remove from all lists.
*/
template<class R, class N, int n> inline void ListInNode<R, N, n>::addToFront(R* root) {
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
template<class R, class N, int n> inline void ListInNode<R, N, n>::addToEnd(R& root) {
	static_cast<Root&>(root).addLast(*static_cast<N*>(this));
}


/**
Add ourselfs to the End of a list

@param root List to add to.
If NULL, just remove from all lists.
*/
template<class R, class N, int n> inline void ListInNode<R, N, n>::addToEnd(R* root) {
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

template<class R, class N, int n> inline void ListInNode<R, N, n>::addAfter(N& node) {
	Node &mynode = static_cast<Node&>(node);
	N* me = static_cast<N*>(this);
	if (mynode.root_ && &node != me) {
		remove();
		root_ = mynode.root_;
		next_ = mynode.next_;
		mynode.next_ = me;
	}
}

/**
Add ourself to a list after another node.

@param node The node to add ourself after.
If Node is NULL, or not on a list, do nothing.
*/

template<class R, class N, int n> inline void ListInNode<R, N, n>::addAfter(N* node) {
	if (node != 0) {
		addAfter(*node);
	}
}

/**
Add ourself to a list at "natural" postion.
Note that this is the front for singly linked lists, and the end for doubly linked lists.

@param root List to add to.
*/
template<class R, class N, int n> void ListInNode<R, N, n>::addTo(R& root) {
    addToFront(root);
}

/**
Add ourself to a list at "natural" postion.
Note that this is the front for singly linked lists, and the end for doubly linked lists.

@param root List to add to.
*/
template<class R, class N, int n> void ListInNode<R, N, n>::addTo(R* root) {
    addToFront(root);
}


/**
Constructor.

Starts us as an empty list.
*/
template <class R, class N, int n> ListInRoot<R, N, n>::ListInRoot() :
first_(0) {}

/**
Destructor.

Removes all nodes attached to us.
*/
template <class R, class N, int n> ListInRoot<R, N, n>::~ListInRoot() {
    while (first_) remove(first_);
}

/**
Constructor.

@param root Pointer to list for node to be added to (if not NULL).
*/
template <class R, class N, int n> ListInNode<R, N, n>::ListInNode(R* root) :
root_(0),
next_(0) {
    if (root) addTo(root);
}

/**
Constructor.

@param root list we are to be added to.
*/
template <class R, class N, int n> ListInNode<R, N, n>::ListInNode(R& root) :
root_(0),
next_(0) 
{
    addTo(root);
}

/**
Destructor.

Remove us from we are on, if any.
*/
template <class R, class N, int n> ListInNode<R, N, n>::~ListInNode() {
    remove();
}
#endif