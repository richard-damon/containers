#ifndef DListIn_H
#define DListIn_H

/**
 @file DListIn.h

 This file defines a pair of templates (DListInRoot and DListInNode) that
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

@par Overview
@parblock
DListInRoot and DListInNode provide a simple API to allow classes to provide a
basic List <-> Node relationship (1 List holding many Nodes) with a doubly linked list. 
(c.f. ListIn.h, ListInRoot, and ListInNode for singly linked list)

To create this relationship, the class to act as the list derives from the template DListInRoot,
and the class to act as the Node from DListInNode, both with a first parameter of the class name
of the List clase, and a second parameter of the Node clasee. There is an optional integral 3rd parameter
to allow the classes to inherent from these multiple times if you need to represent multiple simultaneous
relationships. This inheretance should be public, or you need to add the DListInRoot and DListInNode templates as
friends.

At the point of decleration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from DListInRoot

N: The "user" type that will be derived from DListInNode

n: The integer parameter for the template to allow multiple useage

Root: The particular instance of DListInRoot<R, N, n> being used

Node: The particular instance of DListInNode<R, N, n> being used

node: will have type N*, to be used when walking the list.
@endparblock

@par Invarients
@parblock
- root->first_ and root->last_ will either both be NULL, or neither will be NULL

- if root->first_ != NULL, 
 + root->first_->root_ == root
 + root->last_->root_ == root
 + root->first_->prev_ == NULL
 + root->last_->next_ == NULL

- if node->root_ == NULL:
 + node->next_ == NULL
 + node->prev_ == NULL

- if node->next_ != NULL: 
 + node->next_->prev_ == node
 + node->next_->root_ == node->root_

 - if node->root_ != NULL AND node->prev_ == NULL:
 + node->root_->first_ == node

- if node->root_ != NULL abd node->next_ == NULL:
 + node->root_->last_ == node

@endparblock

@see DListIn.hpp
@see ListIn.h

@ingroup IntrusiveContainers
*/
template <class R,class N, int n = 0> class DListInNode;
template <class R, class N, int n = 0> class DListInRoot;

/**
@class DListInRoot

Intrusive Doubly Linked List, List.

@tparam L The class that will be the owner of the List. Must derive from DListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from DListInNode<R, N, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@see DListInNode
@ingroup IntrusiveContainers

*/
template <class R, class N, int n> class DListInRoot {
	friend  class DListInNode<R, N, n>;
	typedef class DListInRoot<R, N, n> Root;   ///< Type of DListInRoot
	typedef class DListInNode<R, N, n> Node;   ///< Type of DListIInNode
public:
	DListInRoot();
	~DListInRoot();

	void add(N& node);
	void add(N* node);
	void addFirst(N& node);
	void addFirst(N* node);
	void addLast(N& node);
	void addLast(N* node);
	void remove(N& node);
	void remove(N* node);

	N* first() const {return first_;}
    N* last() const { return last_; }

private:
#if __cplusplus < 201101L
	DListInRoot(DListInRoot const&);      ///< We are not copyable.
	void operator =(DListInRoot const&);  ///< We are not assignable.
#else
	DListInRoot(DListInRoot const&) = delete;      ///< We are not copyable.
	void operator =(DListInRoot const&) = delete;  ///< We are not assignable.
#endif
	N* first_;                            ///< Pointer to first node on list.
    N* last_;                             ///< Pointer to last node on list.
};

/**
@class DListInNode

Intrusive Doubly Linked List, Node.

@tparam L The class that will be the owner of the List. Must derive from DListInRoot<R, N, n>
@tparam N The class that will be the nodes of the List. Must derive from DListInNode<R, N, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@see DListInRoot
@ingroup IntrusiveContainers

*/
template <class R, class N, int n> class DListInNode {
	friend  class DListInRoot<R, N, n>;
	typedef class DListInRoot<R, N, n> Root;   ///< Type of DListInRoot
	typedef class DListInNode<R, N, n> Node;   ///< Type of DListIInNode
public:
	DListInNode(R* root=0);
	DListInNode(R&);
	~DListInNode();

	void addTo(R& root);
	void addTo(R* root);
	void addToFront(R& root);
	void addToFront(R* root);
	void addToEnd(R& root);
	void addToEnd(R* root);
	void addAfter(N& node);
	void addAfter(N* node);
	void remove();

	R* root() const { return root_; }    ///< Return pointer to list we are on.
	N* next() const { return next_; }    ///< Return pointer to next node on list.
	N* prev() const { return prev_; }    ///< Return pointer to previous node on list.

private:
#if __cplusplus < 201101L
	DListInNode(DListInNode const&);      ///< we are not copyable.
	void operator =(DListInNode const&);  ///< we are not assignable.
#else
	DListInNode(DListInNode const&) = delete;      ///< we are not copyable.
	void operator =(DListInNode const&) = delete;  ///< we are not assignable.
#endif
	R*    root_;                          ///< Pointer to list we are on.
    N*    prev_;                          ///< Pointer to previous node on list.
    N*    next_;                          ///< Pointer to net node on list.
};
#endif
