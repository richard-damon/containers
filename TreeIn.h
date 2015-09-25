#ifndef TreeIn_H
#define TreeIn_H

/**
@file TreeIn.h
@brief Intrusive Binary Tree (unbalenced).

This file defines a pair of templates (TreeInRoot and TreeInNode) that
implement an intrusive binary tree.

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
TreeInRoot and TreeInNode provide a simple API to allow classes to provide a
basic Tree <-> Node relationship (1 Tree Root holding many Nodes) as a Binary Tree.

Note that this class makes no attempt to "balence" the tree, but is designed that it can be derived
from by a class to implement any needed balencing.

To create this relationship, the class to act as the tree root derives from the template TreeInRoot,
and the class to act as the Node from TreeInNode, both with a first parameter of the class name
of the List clase, a second parameter of the Node clasee, and a 3rd type parameter for the type for the 
Key to use in searching the tree. There is an optional integral 4rd parameter
to allow the classes to inherent from these multiple times if you need to represent multiple simultaneous
relationships. This inheretance should be public, or you need to add the TreeInRoot and TreeInNode templates as
friends.

At the point of decleration, both classes need to be declared, so you will typically have a forward of the other
class before the definition of the class. At the point of usage of members, generally the full definition of both
classes is needed to instance the code for the template.

The user of the class will need to define two member functions to configure the tree.
int TreeInRoot::compare(N& node1, N& node2) const;
and 
int TreeInRoot::compare(N& node, K key) const;

Inside this file, the following types have the following meanings:

R: The "user" type that will be derived from TreeInRoot

N: The "user" type that will be derived from TreeInNode

K: The "user" type that represents the Key for the tree.

n: The integer parameter for the template to allow multiple useage

List: The particular instance of TreeInRoot<R, N, K, n> being used

Node: The particular instance of TreeInNode<R, N, K, n> being used

node: will have type N*, to be used when walking the list.
@endparblock

@par Invarients
@parblock
- if root->base_ != NULL
+  root->base_->root_ == &root
+  root->base_->parent_ == NULL

- if node->root_ == NULL
+  node->parent_ == NULL
+  node->left_ == NULL
+  node->right_ == NULL

- if node->root_ != NULL and node->parent_ == NULL
+  node->root_->base_ == &node

- if node->parent_ != NULL
+  Either node->parent_->left_ or node->parent_->right_ == &node

- if node->left_ != NULL
+  node->left_->parent_ = &node
+  node->left_->


@endparblock

@ingroup IntrusiveContainers
*/

template <class R, class N, class K, int n = 0> class TreeInNode;
template <class R, class N, class K, int n = 0> class TreeInRoot;

/**
@class TreeInRoot

Intrusive Binary Tree, Root.

@tparam L The class that will be the owner of the Tree. Must derive from TreeInRoot<R, N, K, n>
@tparam N The class that will be the nodes of the Tree. Must derive from TreeInNode<R, N, K, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@see TreeInNode
@ingroup IntrusiveContainers

*/
template <class R, class N, class K, int n> class TreeInRoot {
	friend  class TreeInNode<R, N, K, n>;
	typedef class TreeInRoot<R, N, K, n> Root;   ///< Type of TreeInRoot
	typedef class TreeInNode<R, N, K, n> Node;   ///< Type of DListIInNode
protected:
	TreeInRoot();
	~TreeInRoot();

	// These two member functions to be provided by the user.
	int compare(N const& node1, N const& node2) const;
	int compareKey(N const& node, K key) const;

	virtual void add(N& node);
	void add(N* node);
	void remove(N& node);
	void remove(N* node);
	N* find(K key) const;

	N* base() const { return base_; }
	N* first() const;
	N* last() const;

	void check() const;
private:
#if __cplusplus < 201101L
	TreeInRoot(TreeInRoot const&);      ///< We are not copyable.
	void operator =(TreeInRoot const&);  ///< We are not assignable.
#else
	TreeInRoot(TreeInRoot const&) = delete;      ///< We are not copyable.
	void operator =(TreeInRoot const&) = delete;  ///< We are not assignable.
#endif
	N* base_;                            ///< Pointer to root node on tree.
};

/**
@class TreeInNode

Intrusive Binary Tree, Node.

@tparam L The class that will be the owner of the List. Must derive from TreeInRoot<R, N, K, n>
@tparam N The class that will be the nodes of the List. Must derive from TreeInNode<R, N, K, n>
@tparam n A numerical parameter to allow a give List/Node combination to have multiple list-node relationships. Defaults to 0 if not provided.

@see TreeInRoot
@ingroup IntrusiveContainers

*/
template <class R, class N, class K, int n> class TreeInNode {
	friend  class TreeInRoot<R, N, K, n>;
	typedef class TreeInRoot<R, N, K, n> Root;   ///< Type of TreeInRoot
	typedef class TreeInNode<R, N, K, n> Node;   ///< Type of DListIInNode
protected:
	TreeInNode(); // No default add as that needs our Parent already constructed
	~TreeInNode();

	void addTo(R& root);
	void addTo(R* root);
	virtual void remove();           // Virtual so "fancier" trees can rebalence

	R* root() const { return root_; }    ///< Return pointer to list we are on.
	N* parent() const { return parent_; }
	N* left() const { return left_; }
	N* right() const { return right_; }
	N* next() const;
	N* prev() const;

	void check() const;

private:
#if __cplusplus < 201101L
	TreeInNode(TreeInNode const&);      ///< we are not copyable.
	void operator =(TreeInNode const&);  ///< we are not assignable.
#else
	TreeInNode(TreeInNode const&) = delete;      ///< we are not copyable.
	void operator =(TreeInNode const&) = delete;  ///< we are not assignable.
#endif
	R*    root_;                          ///< Pointer to tree we are on.
	N*    parent_;                        ///< Pointer to parent node on tree.
	N*    left_;                          ///< Pointer to left (lesser) child node on tree.
	N*    right_;                          ///< Pointer to right (greater) child node on tree.
};

#endif
