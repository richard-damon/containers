#ifndef LISTIN_H
#define LISTIN_H

/**
@file ListIn.h

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

@par Overview
@parblock
ListInRoot and ListInNode provide a simple API to allow classes to provide a
basic List <-> Node relationship (1 List holding many Nodes), with a singly linked list.
(c.f. DListin.h, DListInRoot, and DListInNode for doubly linked list) 

To create this relationship, the class to act as the list derives from the template ListInRoot,
and the class to act as the Node from ListInNode, both with a first parameter of the class name
of the List clase, and a second parameter of the Node clasee. There is an optional integral 3rd parameter
to allow the classes to inherent from these multiple times if you need to represent multiple simultaneous
relationships. This inheretance should be public, or you need to add the ListInRoot and ListInNode templates as
friends.

At the point of decleration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from ListInRoot

N: The "user" type that will be derived from ListInNode

n: The integer parameter for the template to allow multiple useage

Root: The particular instance of ListInRoot<R, N, n> being used

Node: The particular instance of ListInNode<R, N, n> being used

node: will have type N*, to be used when walking the list.
@endparblock

@par Invarients
@parblock
- if root->first_ != NULL:
 + root->first_->root_ == root

- if node->root_ == NULL:
 + node->next_ == NULL

- if node->next_ != NULL:
 + node->next_->root_ == node->root_
@endparblock

@see DListIn.h
@ingroup IntrusiveContainers
*/


// L a class derived from ListInRoot<L,N,n>
// N a class derived from ListInNode<L,N,n>

template<class R, class N, int n = 0> class ListInRoot;
template<class R, class N, int n = 0> class ListInNode;

/**
@class ListInRoot

Intrusive Singly Linked List, List.

@tparam L The class that will be the owner of the List. Must derive from ListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from ListInNode<R, N, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@see ListInNode
@ingroup IntrusiveContainers

*/
template<class R, class N, int n> class ListInRoot {
    friend ListInNode<R, N, n>;
    typedef ListInRoot<R, N, n> Root;    ///< Type of ListInRoot
	typedef ListInNode<R, N, n> Node;    ///< Type of ListInNode
protected:
    ListInRoot();
	~ListInRoot();
    void add(N& node);
    void add(N* node);
    void addFirst(N& node);
    void addFirst(N* node);
    void addLast(N& node);
    void addLast(N* node);
    void remove(N& node);
    void remove(N* node);
    N* first() const { return first_; }   ///< Return pointer to first Node on list.
private:
#if __cplusplus < 201101L
	ListInRoot(ListInRoot const&);      ///< We are not copyable
	void operator =(ListInRoot const&); ///< We are not assignable;
#else
	ListInRoot(ListInRoot const&) = delete;      ///< We are not copyable
    void operator =(ListInRoot const&) = delete; ///< We are not assignable;
#endif
    N* first_;                          ///< Pointer to first Node on list
};

/**
@class ListInNode

Intrusive Singly Linked List, Node.

@tparam L The class that will be the owner of the List. Must derive from ListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from ListInNode<R, N, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@see ListInRoot
@ingroup IntrusiveContainers

*/
template<class R, class N, int n> class ListInNode {
    friend ListInRoot<R, N, n>;
	typedef ListInNode<R, N, n> Node;    ///< Type of ListInNode
    typedef ListInRoot<R, N, n> Root;    ///< Type of ListInRoot
protected:
    ListInNode(R& root);
    ListInNode(R* root = 0);
    ~ListInNode();

    void addTo(R& root);
    void addTo(R* root);
    void addToFront(R& root);
    void addToFront(R* root);
    void addToEnd(R& root);
    void addToEnd(R* root);
    void addAfter(N& node);
    void addAfter(N* node);
    void remove();

    R* root() const { return root_; }   ///< Return pointer to List we are on.
    N* next() const { return next_; }   ///< Return pointer to next Node on List.
private:
#if __cplusplus < 201101L
	ListInNode(ListInNode const& );      ///< we are not copyable.
	void operator =(ListInNode const& ); ///< we are not assignable.
#else
    ListInNode(ListInNode const& ) = delete;      ///< we are not copyable.
    void operator =(ListInNode const& ) = delete; ///< we are not assignable.
#endif
	R* root_;                           ///< Pointer to list we are on.
    N* next_;                           ///< Pointer to next Node in list.
};

#endif
