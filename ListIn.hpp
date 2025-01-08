#ifndef LISTIN_HPP
#define LISTIN_HPP

/**
@file ListIn.hpp
@brief Intrusive Singly Linked List, Implementation.

This file defines a pair of templates (ListInRoot and ListInNode) that
implement an intrusive singly linked list.

@copyright  (c) 2014-2024 Richard Damon
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
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @return True if check passes
 */

template <class R, class N, ContainerThreadSafety s, int n>
bool ListInRoot<R, N, s, n>::check() const {
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
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @return True if check passes
 */
template <class R, class N, ContainerThreadSafety s, int n>
bool ListInNode<R, N, s, n>::check() const {
    bool flag = true;
    if (root()) {
        Node* mynode = next();
        if (mynode) {
            flag &= mynode->root() == root();
        } else {
            flag &= (static_cast<Root*>(root())->last() == this);   // last in chain, so verify last
        }
    } else {
        flag &= next() == nullptr;                      // no root, so we must be unattached
    }
    return flag;
}

/**
 * Remove node from whatever list it is on, if it is on a list.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
*/
template <class R, class N, ContainerThreadSafety s, int n>
inline void ListInNode<R, N, s, n>::remove() {
    unsigned save = readLock(true);
    R* root = m_root;
    if (m_root) {
        Root* mylist = static_cast<Root*>(m_root);
        // We are on a list, are we the first node?
        if (mylist->m_first == this) {
            unsigned save1 = writeLock(true);
            R* root1 = m_root;
            // Yes, point root to next node in list (if any), and disconnect us.
            mylist->m_first = m_next;
            if(mylist->m_last == this) mylist->m_last = nullptr;
            m_next = nullptr;
            setRoot(nullptr);
            root1->Root::writeUnlock(save1);
        } else {
            // We aren't the first, so find the node before us.
            N* node = mylist->m_first;

            while (node != nullptr) {
                Node* mynode = node;
                N* nextNode = mynode->m_next;
                if (nextNode == this) {
                    unsigned save1 = writeLock(true);
                    R* root1 = m_root;
                    // Found our predecessor, unlink
                    mynode->m_next = m_next;
                    if(m_next == nullptr) {
                        mylist->m_last = node;
                    }
                    m_next = nullptr;
                    setRoot(nullptr);
                    root1->Root::writeUnlock(save1);
                    break;
                }
                node = mynode->m_next;
                // we could assert node here, as we should be able to find ourselves on the list
            }
        }
    }
    root->Root::readUnlock(save);
}


/**
Remove Node from List
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node node to be removed.
 * If node is not on this list, nothing will be done.
*/
template <class R, class N, ContainerThreadSafety s, int n_>
inline void ListInRoot<R, N, s, n_>::remove(N& node) {
    Node& mynode = node;
    if (mynode.m_root == this) {
        // Only remove if node is on this list.
        mynode.remove();
    }
}


/**
Remove Node from List
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node Pointer to node to be removed.
 * If node is null, operation will be ignored.
 * If node is not on this list, nothing will be done.
*/
template <class R, class N, ContainerThreadSafety s, int n_>
inline void ListInRoot<R, N, s, n_>::remove(N* node) {
    if (node != nullptr) remove(*node);
}

/**
Add node to front of our list.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void ListInRoot<R, N, s, n_>::addFirst(N& node, bool upgrade) {
    Node& mynode = node;

    if (mynode.m_root != nullptr) mynode.remove();
    unsigned save = writeLock(upgrade);
    mynode.setRoot(static_cast<R*>(this));
    mynode.m_next = m_first;
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
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void ListInRoot<R, N, s, n_>::addFirst(N* node, bool upgrade) {
    if (node != nullptr) addFirst(*node, upgrade);
}

/**
Add node to end of our list.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided
 * @param node The node to add to the list
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/

template<class R, class N, ContainerThreadSafety s, int n_>
inline void ListInRoot<R, N, s, n_>::addLast(N& node, bool upgrade) {
    Node& mynode = node;

    unsigned save;
    // if upgrading we already have the read lock
    if (!upgrade) save = readLock(false);
    if (mynode.m_root != nullptr) mynode.remove();

    unsigned save1 = writeLock(upgrade);
    if (m_last == nullptr) {
        // List is empty
        m_first = &node;
    } else {
        static_cast<Node*>(m_last)->m_next = &node;
    }
    m_last  = &node;
    mynode.setRoot(static_cast<R*>(this));
    mynode.m_next = nullptr;
    writeUnlock(save1);
    if (!upgrade) readUnlock(save);
}

/**
 * Add node to end of our list.
 * Note that this operation is O(n) on list current size. See DListInRoot to make this O(1).
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/

template<class R, class N, ContainerThreadSafety s, int n_>
inline void ListInRoot<R, N, s, n_>::addLast(N* node, bool upgrade) {
    if (node != nullptr) addLast(*node, upgrade);
}



/**
 * Add node to our list, at "natural" point.
 * Note that this is the front for singly linked lists and the end for doubly linked list.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param node The node to add to the list
 * If node is currently on a list (even us) it is removed before being added to the list
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n_>
inline void ListInRoot<R, N, s, n_>::add(N& node, bool upgrade) {
    addFirst(node, upgrade);
}


/**
Add node to our list, at "natural" point.
Note that this is the front for singly linked lists and the end for doubly linked lists.

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
inline void ListInRoot<R, N, s, n_>::add(N* node, bool upgrade) {
    if (node != nullptr) add(*node, upgrade);
}

/**
Add ourselfs to the front of a list

 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param myRoot List to add to.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/
template<class R, class N, ContainerThreadSafety s, int n>
inline void ListInNode<R, N, s, n>::addToFront(R& myRoot, bool upgrade) {
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
inline void ListInNode<R, N, s, n>::addToFront(R* myRoot, bool upgrade) {
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
inline void ListInNode<R, N, s, n>::addToEnd(R& myRoot, bool upgrade) {
	static_cast<Root&>(myRoot).addLast(*static_cast<N*>(this), upgrade);
}


/**
 * Add ourselves to the End of a list
 *
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
 * @param myRoot List to add to.
 * If NULL, just remove from all lists.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
 */
template<class R, class N, ContainerThreadSafety s, int n>
inline void ListInNode<R, N, s, n>::addToEnd(R* myRoot, bool upgrade) {
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
 * @param node The node to add ourself after.
 * If node is not on a list, do nothing.
 * @param upgrade Set True if caller has an upgradable Read Lock (Used by SortDListInNode)
*/

template<class R, class N, ContainerThreadSafety s, int n>
inline void ListInNode<R, N, s, n>::addAfter(N& node, bool upgrade) {
	Node &mynode = node;
	N* me = static_cast<N*>(this);
	if (mynode.m_root && &node != me) {
		remove();
    	unsigned save = mynode.writeLock(upgrade);
		setRoot(mynode.m_root);
		m_next = mynode.m_next;
		mynode.m_next = me;
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
inline void ListInNode<R, N, s, n>::addAfter(N* node, bool upgrade) {
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
void ListInNode<R, N, s, n>::addTo(R& myRoot, bool upgrade) {
    addToFront(myRoot, upgrade);
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
void ListInNode<R, N, s, n>::addTo(R* myRoot, bool upgrade) {
    addToFront(myRoot, upgrade);
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
ListInRoot<R, N, s, n>::ListInRoot() :
m_first(nullptr) {}

/**
Destructor.

Removes all nodes attached to us.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
*/
template <class R, class N, ContainerThreadSafety s, int n>
ListInRoot<R, N, s, n>::~ListInRoot() {
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
ListInNode<R, N, s, n>::ListInNode(R* myRoot) :
ContainerNode<s>(nullptr),
m_root(nullptr),
m_next(nullptr) {
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
ListInNode<R, N, s, n>::ListInNode(R& myRoot) :
m_root(0),
m_next(0) 
{
    addTo(myRoot);
}

/**
Destructor.

Remove us from we are on, if any.
 * @tparam R The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
 * @tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * @tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.
*/
template <class R, class N, ContainerThreadSafety s, int n>
ListInNode<R, N, s, n>::~ListInNode() {
    remove();
}
#endif
