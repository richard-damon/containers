/**
@file ManyMany.h
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
ManyManyRoot, ManyManyNode and ManyManyLink (which acts as an intermediary) provide a simple API to allow classes to provide a
basic Root <-> Node relationship where each Root can point to many nodes, and each node can be on many roots.

To create this set of relationships, the ManyManyRoot and ManyManyNode classes create lists of ManyManyLink nodes that
indicate each of those connections. 

The concept of "Root" and "Node" for this relationship is really arbitrary, but was chosen just to indicate the "sides" of the relationship in the links.

To create this relationship, the class to act as the list derives from the template ListInRoot,
and the class to act as the Node from ListInNode, both with a first parameter of the class name
of the List class, and a second parameter of the Node class. There is an optional integral 3rd parameter
to allow the classes to inherent from these multiple times if you need to represent multiple simultaneous
relationships. This inheritance should be public, or you need to add templates as friends.

At the point of declaration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from ManyManyRoot

N: The "user" type that will be derived from ManyManyNode

L: The "user" type that will be derived from ManyManyLink. 
There is a version of ManyManyLink with L=void that allows the templates to use ManyManyLink<R, N, s, n, void> as the link class 

s: The ContainerThreadSafety value for the cluster. Defaults to no builtin safety (ContainerNoSafety)

n: The integer parameter for the template to allow multiple usage

Root: The particular instance of ManyManyRoot<R, N, s, n, L> being used

Node: The particular instance of ManyManyNode<R, N, s, n, L> being used

Link: The particular instance of ManyManyLink<R, N, s, n, L> being used

node: will have type N*, to be used when walking the list.
@endparblock

@invariant
@parblock

 + R& root
 + N& node
 + L& link

Mostly from DListInRoot and DListInNode

 ManyManyRoot: (DListInRoot R -> L)
 + if root.first() == nullptr
   + root.last() == nullptr
 + if root.first() !- nullptr
   + root.last() != nullptr
   + root.first()->root() == \&root
   + root.first()->prev() == nullptr
   + root.last()->root() == \&root
   + root.last()->next() == nullptr

 ManyManyNode: (DListInRoot: N -> L)
 + if node.first() == nullptr
   + node.last() == nullptr
 + if node.first() !- nullptr
   + node.last() != nullptr
   + node.first()->root() == \&root
   + node.first()->prev() == nullptr
   + node.last()->root() == \&root
   + node.last()->next() == nullptr

 ManyManyLink: (DListInNode R->L, N->L)
 + if link.root() == nullptr
   + link.node() == nullptr    // we point to both or neither
   + link.nextRoot() == nullptr
   + link.prevRoot() == nullptr
   + link.nextNode() == nullptr
   + link.prevNode() == nullptr

 + if link.prevRoot() == nullptr
   + link.root()->first() = \&link;
 + if link.prevRoot() !- nullptr
   + link.prevRoot()->root() == link.root()
   + link.prevRoot()->nextRoot() == \&link
 + if link.nextRoot() == nullptr
   + link.root()->last() = \&link
 + if link.nextRoot() != nullptr
   + link.nextRoot()->root() == link.root()
   + link.nextRoot()->prevRoot() == \&link

 + if link.prevNode() == nullptr
   + link.node()->first() = \&link;
 + if link.prevNode() !- nullptr
   + link.prevNode()->node() == link.node()
   + link.prevNode()->nextNode() == \&link
 + if link.nextNode() == nullptr
   + link.node()->last() = \&link
 + if link.nextNode() != nullptr
   + link.nextNode()->node() == link.node()
   + link.nextNode()->prevNode() == \&link
@endparblock

@ingroup IntrusiveContainers

*/


#ifndef CONTAINERS_MANYMANY_H
#define CONTAINERS_MANYMANY_H

#include "Container.h"

#include "DListIn.h"

template <class R, class N, ContainerThreadSafety s, int n, class L> class ManyManyLink;
template <class R, class N, ContainerThreadSafety s = ContainerNoSafety, int n = 0, class L = ManyManyLink<R, N, s, n, void>> class ManyManyLink;
template <class R, class N, ContainerThreadSafety s = ContainerNoSafety, int n = 0, class L = ManyManyLink<R, N, s, n, void>> class ManyManyRoot;
template <class R, class N, ContainerThreadSafety s = ContainerNoSafety, int n = 0, class L = ManyManyLink<R, N, s, n, void>> class ManyManyNode;

/**
 * Root side of a many-many relationship
 * 
 * 
@invariant
@parblock
From DListInRoot:
 + if first() == nullptr
   + last() == nullptr
 + if first() !- nullptr
   + last() != nullptr
   + first()->root() == thiw
   + first()->prev() == nullptr
   + last()->root() == this
   + last()->next() == nullptr
@endparblock
 * @ingroup IntrusiveContainers
 */
template <class R, class N, ContainerThreadSafety s, int n, class L> class ManyManyRoot : public DListInRoot<R, L, s, 2*n> {
    typedef class ManyManyRoot<R, N, s, n, L> Root;
    typedef class ManyManyNode<R, N, s, n, L> Node;
    typedef class ManyManyLink<R, N, s, n, L> Link;
    typedef class DListInRoot<R, L, s, 2*n> Base;

    friend Node;
    friend Link;
public:  
    ManyManyRoot(N* node = nullptr, L* link = nullptr);
    ~ManyManyRoot();
    // 4 version of each to allow mixing and matching of pointers and references
    void  add(N& node, L& link);
    void  add(N& node, L* link = nullptr);
    void  add(N* node, L& link);
    void  add(N* node, L* link = nullptr);
    void  addFirst(N& node, L& link);
    void  addFirst(N& node, L* link = nullptr);
    void  addFirst(N* node, L& link);
    void  addFirst(N* node, L* link = nullptr);
    void  addLast(N& node, L& link);
    void  addLast(N& node, L* link = nullptr);
    void  addLast(N* node, L& link);
    void  addLast(N* node, L* link = nullptr);


    bool  remove(N& node);
    bool  remove(N* node);

    L*    first() const {return Base::first();}
    L*    last() const { return Base::last(); }

    bool  check() const override { return Base::check(); }

    // Critical Sections used to Update the Container
    unsigned writeLock() const				{ return Container<s>::writeLock(); }
    void writeUnlock(unsigned save) const	{ 		 Container<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock() const      			{ return Container<s>::readLock(); }
    void readUnlock(unsigned save) const  	{ 		 Container<s>::readUnlock(save);}
};
/**
 * Node side of a many-many relationship
 * 
@invariant
@parblock
From DListInRoot:
 + if first() == nullptr
   + last() == nullptr
 + if first() !- nullptr
   + last() != nullptr
   + first()->root() == thiw
   + first()->prev() == nullptr
   + last()->root() == this
   + last()->next() == nullptr
@endparblock

 * @ingroup IntrusiveContainers
 */
template <class R, class N, ContainerThreadSafety s, int n, class L> class ManyManyNode : public DListInRoot<N, L, s, 2*n+1> {
    typedef class ManyManyRoot<R, N, s, n, L> Root;
    typedef class ManyManyNode<R, N, s, n, L> Node;
    typedef class ManyManyLink<R, N, s, n, L> Link;
    typedef class DListInRoot<N, L, s, 2*n+1> Base;

    friend Root;
    friend Link;
public:
    ManyManyNode(R* root = nullptr, L* link = nullptr);
    ~ManyManyNode();

    void add(R& root, L* link = nullptr);
    void add(R* root, L* link = nullptr);
    void addFirst(R& root, L* link = nullptr);
    void addFirst(R* root, L* link = nullptr);
    void addLast(R& root, L* link = nullptr);
    void addLast(R* root, L* link = nullptr);

    void add(R& root, L& link);
    void add(R* root, L& link);
    void addFirst(R& root, L& link);
    void addFirst(R* root, L& link);
    void addLast(R& root, L& link);
    void addLast(R* root, L& link);

    bool remove(R& node);
    bool remove(R* node);

    L* first() const {return Base::first();}
    L* last() const { return Base::last(); }

    bool check() const override { return Base::check(); }

    // Critical Sections used to Update the Container
    unsigned writeLock() const				{ return Container<s>::writeLock(); }
    void writeUnlock(unsigned save) const	{ 		 Container<s>::writeUnlock(save); }
    // Critical Section used to Read/Search the Container
    unsigned readLock() const      			{ return Container<s>::readLock(); }
    void readUnlock(unsigned save) const  	{ 		 Container<s>::readUnlock(save);}
};

/**
 * Intermediate Link for a many-many relationship
 * 
 * Allowed to be pointing to a user class to allow properties on the relationship.
 * 
@invariant
@parblock
From DListInNode:
 + if root() == nullptr
   + node() == nullptr    // we point to both or neither
   + nextRoot() == nullptr
   + prevRoot() == nullptr
   + nextNode() == nullptr
   + prevNode() == nullptr

 + if prevRoot() == nullptr
   + root()->first() = this;
 + if prevRoot() !- nullptr
   + prevRoot()->root() == root()
   + prevRoot()->nextRoot() == this
 + if nextRoot() == nullptr
   + root()->last() = this
 + if nextRoot() != nullptr
   + nextRoot()->root() == link.root()
   + nextRoot()->prevRoot() == this

 + if link.prevNode() == nullptr
   + node()->first() = this
 + if prevNode() !- nullptr
   + prevNode()->node() == link.node()
   + prevNode()->nextNode() == this
 + if nextNode() == nullptr
   + node()->last() = this
 + if nextNode() != nullptr
   + nextNode()->node() == node()
   + nextNode()->prevNode() == this
@endparblock
 * @ingroup IntrusiveContainers
 */
template <class R, class N, ContainerThreadSafety s, int n, class L> class ManyManyLink : public DListInNode<R, L, s, 2*n>, public DListInNode<N, L, s, 2*n+1> {
    typedef class ManyManyRoot<R, N, s, n, L> Root;
    typedef class ManyManyNode<R, N, s, n, L> Node;
    typedef class ManyManyLink<R, N, s, n, L> Link;
    typedef class DListInNode<R, L, s, 2*n>   BaseRoot;
    typedef class DListInNode<N, L, s, 2*n+1> BaseNode;
    typedef class DListInRoot<R, L, s, 2*n>   RootRoot;
    typedef class DListInRoot<N, L, s, 2*n+1> RootNode;

    friend Node;
    friend Root;
public:
    ManyManyLink(R* root = nullptr, N* node = nullptr, L* link1 = nullptr, L* link2 = nullptr);
    virtual ~ManyManyLink();

    R* root() const { return BaseRoot::root(); }
    N* node() const { return BaseNode::root(); }
    L* prevRoot() const { return BaseRoot::prev(); }
    L* nextRoot() const { return BaseRoot::next(); }
    L* prevNode() const { return BaseNode::prev(); }
    L* nextNode() const { return BaseNode::next(); }

    void add(R* root, N* node, L* link1 = nullptr, L* link2 = nullptr);
    void add(R& root, N& node);
    void remove();

    bool check() const override;

private:
    static L& getLink(L* link);
    
    bool    m_dynamic = false;
};

/**
 * Default ManyManyLink final class if user has no need to add behaviors.
 */
template <class R, class N, ContainerThreadSafety s, int n> class ManyManyLink<R, N, s, n, void> : public ManyManyLink<R, N, s, n, ManyManyLink<R, N, s, n, void> > {
public:
};

#endif // CONTAINERS_MANYMANY_H
