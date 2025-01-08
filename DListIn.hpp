#ifndef DListIn_HPP
#define DListIn_HPP

/**
 @file DListIn.hpp
 @brief Intrusive Double Linked List, Implementation.

 This file defines the implementation for a pair of templates (DListInRoot and DListInNode) that
 implement an intrusive double linked list.

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
 * Check a DListInRoot and the list connected
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @return true if check passes
 */

template <class R, class N, ContainerThreadSafety s, int n>
bool DListInRoot<R, N, s, n>::check() const {
    bool flag = true;
    if (first()) {
        N* node = last();
        flag &= (node != nullptr);              // if have first, must have last
        if (!flag) return flag;
        Node* mynode = node;
        flag &= (mynode->root() == this);       // last must point to us
        flag &= (mynode->next() == nullptr);    // last must not point to more
        node = first();
        mynode = node;
        flag &= (mynode->root() == this);        // first must point to us
        flag &= (mynode->prev() == nullptr);     // first must not have a previous
        while(flag && mynode){
            flag &= mynode->check();            // check all the nodes on the list
            mynode = mynode->next();
        }
    } else {
        flag &= (last() == nullptr);            // if no first, then no last either.
    }
    return flag;
}

/**
 * Check a DListInNode
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @return true if check passes
 */
template <class R, class N, ContainerThreadSafety s, int n>
bool DListInNode<R, N, s, n>::check() const {
    bool flag = true;
    if (root()) {
        Node* mynode = next();
        if (mynode) {
            flag &= mynode->root() == root();       // next must have the same root
        } else {
            flag &= (static_cast<Root*>(root())->last() == this);   // last in chain, so verify pointed to by root
        }
        mynode = prev();
        if (mynode) {
            flag &= mynode->root() == root();       // previous must have same root
        } else {
            flag &= (static_cast<Root*>(root())->first() == this);  // first in chian, verify pointed to by root
        }
    } else {
        flag &= next() == nullptr;                      // no root, so we must be unattached
    }
    return flag;
}



/**
 * Remove node from whatever list it is on, if it is on a list.
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
*/

template <class R, class N, ContainerThreadSafety s, int n>
inline void DListInNode<R, N, s, n>::remove() {
    unsigned save = writeLock(false);
	R* root = m_root;
	if (m_root) {
		if (m_next) { // if someone after us, update their back pointer
			static_cast<Node*>(m_next)->m_prev = m_prev;
		} else {
			// Else update end of list pointer
			static_cast<Root*>(m_root)->m_last = m_prev;
		}
		if (m_prev) { // if someone before us, update their next pointer
			static_cast<Node*>(m_prev)->m_next = m_next;
		} else { // if no one before us update list head
			static_cast<Root*>(m_root)->m_first = m_next;
		}
		m_next = nullptr;
		m_prev = nullptr;
		setRoot(nullptr);
	}
	root->Root::writeUnlock(save);
}

/**
 * Remove Node from List
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node node to be removed.
 *
 * If node is not on this list, nothing will be done.
*/
template <class R, class N, ContainerThreadSafety s, int n_>
inline void DListInRoot<R, N, s, n_>::remove(N& node) {
	Node& mynode = node;
	unsigned save = writeLock(false);
	if (mynode.m_root == this) {
		// Only remove if node is on this list.
		mynode.remove();
	}
	writeUnlock(save);
}


/**
 * Remove Node from List
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node Pointer to node to be removed.
 * If node is null, operation will be ignored.
 * If node is not on this list, nothing will be done.
*/
template <class R, class N, ContainerThreadSafety s, int n_>
inline void DListInRoot<R, N, s, n_>::remove(N* node) {
	if (node != nullptr) remove(*node);
}

/**
 * Add node to front of our list.
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInRoot)
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void DListInRoot<R, N, s, n_>::addFirst(N& node, bool upgrade) {
	Node& mynode = static_cast<Node&>(node);

	if (mynode.m_root != nullptr) {	
		mynode.remove();
	} 

	unsigned save = writeLock(upgrade);
	mynode.setRoot(static_cast<R*>(this));
	if (m_first == nullptr) {
		// Empty list, point tail pointer too.
		m_last = &node;
	} else {
        m_first ->Node::m_prev = &node;
    }
	mynode.m_next = m_first;
	mynode.m_prev = nullptr;
	m_first = &node;
	writeUnlock(save);
}

/**
Add node to front of our list.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInRoot)
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void DListInRoot<R, N, s, n_>::addFirst(N* node, bool upgrade) {
	if (node != nullptr) addFirst(*node, upgrade);
}

/**
 * Add node to end of our list.
 *
 * Note that this operation is O(n) on list current size. See DListInRoot to make this O(1).
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInRoot)
*/

template<class R, class N, ContainerThreadSafety s, int n_>
inline void DListInRoot<R, N, s, n_>::addLast(N& node, bool upgrade) {
	Node& mynode = node;

	if (mynode.m_root != nullptr) mynode.remove();
	
	unsigned save = writeLock(upgrade);
	if (m_last == nullptr) {
		// Empty List
		m_first = &node;
		mynode.m_prev = nullptr;
	} else {
		mynode.m_prev = m_last;
		static_cast<Node*>(m_last)->m_next = &node;
	}
	m_last = &node;
	mynode.m_next = nullptr;
	mynode.setRoot(static_cast<R*>(this));
	writeUnlock(save);
}

/**
 * Add node to end of our list.
 *
 * Note that this operation is O(n) on list current size. See DListInRoot to make this O(1).
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInRoot)
*/

template<class R, class N, ContainerThreadSafety s, int n_>
inline void DListInRoot<R, N, s, n_>::addLast(N* node, bool upgrade) {
	if (node != nullptr) addLast(*node, upgrade);
}



/**
 * Add node to our list, at "natural" point.
 * Note that this is the front for singly linked lists and the end for doubly linked list.
 *
 * If node is currently on a list (even us) it is removed before being added to the list
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
 *
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void DListInRoot<R, N, s, n_>::add(N& node, bool upgrade) {
	addLast(node, upgrade);
}

/**
 * Add node to our list, at "natural" point.
 *
 * Note that this is the front for singly linked lists and the end for doubly linked lists.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * If node is nulR, Nothing is done.
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void DListInRoot<R, N, s, n_>::add(N* node, bool upgrade) {
	if (node != nullptr) add(*node, upgrade);
}

/**
 * Add ourselves to the front of a list
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param myRoot List to add to.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n>
inline void DListInNode<R, N, s, n>::addToFront(R& myRoot, bool upgrade) {
	static_cast<Root&>(myRoot).addFirst(*static_cast<N*>(this), upgrade);
}

/**
Add ourselfs to the front of a list
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param myRoot List to add to.
 * If NULL, just remove from all lists.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n>
inline void DListInNode<R, N, s, n>::addToFront(R* myRoot, bool upgrade) {
	if (myRoot) {
		static_cast<Root*>(myRoot)->addFirst(*static_cast<N*>(this), upgrade);
	} else {
		remove();
	}
}

/**
Add ourselfs to the end of a list
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param myRoot List to add to.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n>
inline void DListInNode<R, N, s, n>::addToEnd(R& myRoot, bool upgrade) {
	static_cast<Root&>(myRoot).addLast(*static_cast<N*>(this), upgrade);
}


/**
Add ourselfs to the End of a list
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param myRoot List to add to.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)

If NULL, just remove from all lists.
*/
template<class R, class N, ContainerThreadSafety s, int n>
inline void DListInNode<R, N, s, n>::addToEnd(R* myRoot, bool upgrade) {
	if (myRoot) {
		static_cast<Root*>(myRoot)->addLast(*static_cast<N*>(this), upgrade);
	} else {
		remove();
	}
}

/**
Add ourself to a list after another node.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add ourself after. If that node is not on a list, do nothing.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/

template<class R, class N, ContainerThreadSafety s, int n>
inline void DListInNode<R, N, s, n>::addAfter(N& node, bool upgrade) {
	Node &mynode = node;
	N* me = static_cast<N*>(this);
	if (mynode.m_root && &node != me) {
		remove();
		unsigned save = mynode.writeLock(upgrade);
		setRoot(mynode.m_root);
		m_next = mynode.m_next;
		m_prev = &node;
		mynode.m_next = me;
		if (m_next) {
			static_cast<Node*>(m_next)->m_prev = me;
		} else {
			static_cast<Root*>(m_root)->m_last = me;
		}
		mynode.writeUnlock(save);
	}
}

/**
Add ourself to a list after another node.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add ourself after.
 * If Node is NULL, or not on a list, do nothing.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/

template<class R, class N, ContainerThreadSafety s, int n>
inline void DListInNode<R, N, s, n>::addAfter(N* node, bool upgrade) {
	if (node != nullptr) {
		addAfter(*node, upgrade);
	}
}

/**
Add ourself to a list at "natural" postion.
Note that this is the front for singly linked lists, and the end for doubly linked lists.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param myRoot List to add to.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n>
void DListInNode<R, N, s, n>::addTo(R& myRoot, bool upgrade) {
	addToEnd(myRoot, upgrade);
}

/**
Add ourself to a list at "natural" postion.
Note that this is the front for singly linked lists, and the end for doubly linked lists.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param myRoot List to add to.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n>
void DListInNode<R, N, s, n>::addTo(R* myRoot, bool upgrade) {
	addToEnd(myRoot, upgrade);
}


/**
Constructor.

Starts us as an empty list.

 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

*/
template <class R, class N, ContainerThreadSafety s, int n>
DListInRoot<R, N, s, n>::DListInRoot() :
m_first(nullptr),
m_last(nullptr)
{}

/**
Destructor.

Removes all nodes attached to us.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

*/
template <class R, class N,  ContainerThreadSafety s, int n>
DListInRoot<R, N, s, n>::~DListInRoot() {
	while (m_first) remove(m_first);
}

/**
Constructor.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@param myRoot Pointer to list for node to be added to (if not NULL).
*/
template <class R, class N, ContainerThreadSafety s, int n>
DListInNode<R, N, s, n>::DListInNode(R* myRoot) :
ContainerNode<s>(nullptr),
m_root(nullptr),
m_prev(nullptr),
m_next(nullptr)
{
	if (myRoot) addTo(myRoot);
}

/**
Constructor.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@param myRoot list we are to be added to.
*/
template <class R, class N, ContainerThreadSafety s, int n>
DListInNode<R, N, s, n>::DListInNode(R& myRoot) :
m_root(0),
m_prev(0),
m_next(0)
{
	addTo(myRoot);
}

/**
 * Destructor.
 *
 * Remove us from list we are on, if any.
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
*/
template <class R, class N, ContainerThreadSafety s, int n>
DListInNode<R, N, s, n>::~DListInNode() {
	remove();
}

#endif
