/**
@file ManyMany.hpp
@brief Intrusive Many to Many Relationship.

This file defines a trio of templates (ManyManyLinkRoot, ManyManyLinkLink, and ManyManyLinkNode)
that implement a many-to-many interconnect web.

@warning The ManyMany classes are new, and have not been extensively tested
@bug The Locking implemented in the ManyMany containers is likely not sufficient yet.

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

@par Overview
@parblock
ListInRoot and ListInNode provide a simple API to allow classes to provide a
basic List <-> Node relationship (1 List holding many Nodes), with a singly linked list.
(c.f. DListin.h, DListInRoot, and DListInNode for doubly linked list)

To create this relationship, the class to act as the list derives from the template ListInRoot,
and the class to act as the Node from ListInNode, both with a first parameter of the class name
of the List class, and a second parameter of the Node class. There is an optional integral 3rd parameter
to allow the classes to inherent from these multiple times if you need to represent multiple simultaneous
relationships. This inheritance should be public, or you need to add the ListInRoot and ListInNode templates as
friends.

At the point of declaration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from ListInRoot

N: The "user" type that will be derived from ListInNode

n: The integer parameter for the template to allow multiple usage

Root: The particular instance of ListInRoot<R, N, n> being used

Node: The particular instance of ListInNode<R, N, n> being used

node: will have type N*, to be used when walking the list.
@endparblock

@ingroup IntrusiveContainers
*/


#ifndef CONTAINERS_MANYMANY_HPP
#define CONTAINERS_MANYMANY_HPP
#include "ManyMany.h"


template<class R, class N, ContainerThreadSafety s, int n, class L>
ManyManyRoot<R, N, s, n, L>::ManyManyRoot(N* node, L* link) {
    if (node) {
        add(node, link);
    }
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
ManyManyRoot<R, N, s, n, L>::~ManyManyRoot() {
    remove(nullptr);
}

/**
 * Remove Node for list
 * @tparam R
 * @tparam N
 * @tparam s
 * @tparam n
 * @tparam L
 * @param node the node to add to this Root
 * @return True if node was on list
 */
template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::add(N& node, L& link) {
    link.Link::add(*static_cast<R*>(this), node);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::add(N& node, L* link) {
    add(node, getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::add(N* node, L& link) {
    if (node) add(*node, link);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::add(N* node, L* link) {
    if (node) add(*node, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::addFirst(N& node, L& link) {
    // Link::add defaults to adding first
    link.Link::add(*static_cast<R*>(this), node);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::addFirst(N& node, L* link) {
    addFirst(node, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::addFirst(N* node, L& link) {
    if (node) addFirst(node, link);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::addFirst(N* node, L* link) {
    if (node) addFirst(*node, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::addLast(N& node, L& link) {
    // Get our last() pointer to add after
    link.Link::add(static_cast<R*>(this), &node, last());
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::addLast(N& node, L* link) {
    addLast(node, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::addLast(N* node, L& link) {
    if (node) addLast(*node, link);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyRoot<R, N, s, n, L>::addLast(N* node, L* link) {
    if (node) addLast(*node, Link::getLink(link));
}

/**
 * Remove Node for list
 * @tparam R
 * @tparam N
 * @tparam L
 * @tparam s
 * @tparam n
 * @param node the node to remove
 * @return True if node was on list
 */
template<class R, class N, ContainerThreadSafety s, int n, class L>
bool ManyManyRoot<R, N, s, n, L>::remove(N& node) {
    for (Link* link = first(); link; link = link->nextNode()) {
        if (link->node() == &node) {
            link->remove();
            return true;
        }
    }
    return false;   // didn't find the node;
}

/**
 * Remove Node for list
 * @tparam R
 * @tparam N
 * @tparam L
 * @tparam s
 * @tparam n
 * @param node pointer to node on list, nullptr removes ALL nodes
 * @return True if node was on list
 */
template<class R, class N, ContainerThreadSafety s, int n, class L>
bool ManyManyRoot<R, N, s, n, L>::remove(N* node) {
    if (node) {
        return remove(*node);
    } else {
        while(first()) {
            remove(first()->Link::node());
        }
        return true;
    }
}

/*************************************************************************/

template<class R, class N, ContainerThreadSafety s, int n, class L>
ManyManyNode<R, N, s, n, L>::ManyManyNode(R* root, L* link) {
    if (root) {
        add(root, link);
    }
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
ManyManyNode<R, N, s, n, L>::~ManyManyNode() {
    remove(nullptr);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::add(R& root, L& link) {
    link.Link::add(root, *static_cast<N*>(this));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::add(R* root, L& link) {
    if (root) add(*root, link);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::add(R& root, L* link) {
    add(root, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::add(R* root, L* link) {
    if (root) add(*root, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::addFirst(R& root, L& link) {
    // Link::add defaults to adding first
    link.Link::add(root, *static_cast<N*>(this));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::addFirst(R* root, L& link) {
    if (root) addFirst(*root, link);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::addFirst(R& root, L* link) {
    addFirst(root, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::addFirst(R* root, L* link) {
    if (root) addFirst(*root, Link::getLink(link));
}


template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::addLast(R& root, L& link) {
    link.Link::add(&root, static_cast<N*>(this), last());
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::addLast(R* root, L& link) {
    if (root) addLast(*root, link);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::addLast(R& root, L* link) {
    addLast(root, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyNode<R, N, s, n, L>::addLast(R* root, L* link) {
    if (root) addLast(*root, Link::getLink(link));
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
bool ManyManyNode<R, N, s, n, L>::remove(R& root) {
    for (Link* link=first(); link; link->nextRoot()) {
        if (link->root() == &root) {
            link->remove();
            return true;
        }
    }
    return false;
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
bool ManyManyNode<R, N, s, n, L>::remove(R* root) {
    if (root) {
        return remove(*root);
    }
    // null root given, remove all
    while(first()) {
        first()->Link::remove();
    }
    return true;
}

/*************************************************************************/

template<class R, class N, ContainerThreadSafety s, int n, class L>
ManyManyLink<R, N, s, n, L>::ManyManyLink(R* root, N* node, L* link1, L* link2) {
    if (root) {
        add(root, node, link1, link2);
    }
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
ManyManyLink<R, N, s, n, L>::~ManyManyLink() {
    m_dynamic = false;  // Don't let remove try to delete us since we are going away anyway.
    remove();
}

/**
 * Convert a possible link pointer into an actual link object.
 * 
 * If we are given a non-null pointer, us it, otherwise dynamically create the needed
 * ManyManyLink object, and mark it to be deleted when removed.
 */
template<class R, class N, ContainerThreadSafety s, int n, class L>
L& ManyManyLink<R, N, s, n, L>::getLink(L* link) {
    if (link == nullptr) {
        link = new L;
        // TODO should do something on out of memory if throwing has been disabled
        link->Link::m_dynamic = true;
    }
    return *link;
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
bool ManyManyLink<R, N, s, n, L>::check() const {
    L const* me = static_cast<L const*>(this);
    bool flag = true;
    Root* r = root();
    if (root()) {
        flag &= node() != nullptr;
        if (!flag) return 0;
        if (prevRoot()) {
            flag &= (prevRoot()->Link::nextRoot() == me);    // previous must point to us
            flag &= (prevRoot()->Link::root() == root());    // previous must point to our same root
        } else {        // we are the first of the root chain
            flag &= (r->first() == me);
        }
        if (nextRoot()) {
            flag &= (nextRoot()->Link::prevRoot() == me);
            flag &= (nextRoot()->Link::root() == root());
        } else {
            flag &= (r->last() == me);
        }
        if (prevNode()) {
            flag &= (prevNode()->Link::nextNode() == me);
            flag &= (prevNode()->Link::node() == node());
        } else {
            flag &= (node()->Node::first() == me);
        }
        if (nextNode()) {
            flag &= (nextNode()->Link::prevNode() == me);
            flag &= (nextNode()->Link::node() == node());
        } else {
            flag &= (node()-> Node::last() == me);
        }
    } else { // Link is disconnected
        flag &= prevRoot() == nullptr;
        flag &= nextRoot() == nullptr;
        flag &= prevNode() == nullptr;
        flag &= nextNode() == nullptr;
        flag &= node() == nullptr;
    }
    return flag;
}

/**
 * Link a Root to a Node with this Lisk
 *
 * @tparam R
 * @tparam N
 * @tparam L
 * @tparam s
 * @tparam n
 * @param root
 * @param node
 * @param link1
 * @param link2
 *
 * If link1 or link2 point to either the Root or Node we are to connect to, then we will link ourselves on that side
 * just after that link. Otherwise we link at the front of the lists.
 */
template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyLink<R, N, s, n, L>::add(R* root, N* node, L* link1, L* link2) {
    L* me = static_cast<L*>(this);

    // sanity checks, we can not add after ourselves
    if (link1 == this) {
        link1 = nullptr;
    }
    if (link2 == this) {
        link2 = nullptr;
    }

    if (this->root()) {
        // we are in use, this is an error, but best recovery is to disconnect us to be ready for the new use
        // Do not let us be deleted as we get removed.
        bool save = m_dynamic;
        m_dynamic = false;
        remove();
        m_dynamic = save;
    }

    Link *rlink = nullptr, *nlink = nullptr;
    unsigned saveRoot = BaseRoot::readLock(true);
    unsigned saveNode = BaseNode::readLock(true);

    if (link2) {
        if (link2->Link::root() == root) rlink = link2;
        if (link2->Link::node() == node) nlink = link2;
    }
    if (link1) {
        if (link1->Link::root() == root) rlink = link1;
        if (link1->Link::node() == node) nlink = link1;
    }

    if (rlink) {
        rlink->BaseRoot::addAfter(me, true);
    } else {
        root->Root::Base::add(me);
    }

    if (nlink) {
        nlink->BaseNode::addAfter(me, true);
    } else {
        node->Node::Base::add(me);
    }
    BaseRoot::readUnlock(saveRoot);
    BaseNode::readUnlock(saveNode);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyLink<R, N, s, n, L>::add(R& root, N& node) {
    add(&root, &node);
}

template<class R, class N, ContainerThreadSafety s, int n, class L>
void ManyManyLink<R, N, s, n, L>::remove() {
    BaseRoot::remove();
    BaseNode::remove();
    if (m_dynamic) {
        m_dynamic = false; // Don't repeat when called from destructor
        delete this;
    }
}

#endif
