@page containers Intrusive Containter Library
@copyright  (c) 2014-2024 Richard Damon
@brief Intrusive Container Documentation.

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

See @ref IntrusiveContainers for details of the containers

@defgroup IntrusiveContainers A Collection of Intrusive Containters.

The Intrusive Container Collection provides a set of containers that
provide 1:many and many:many mappings from a "Containing" class to a "Node" class.
This library assumes that the classes are "Identity" based (as opposed to
"Value" based). It also provides a bi-directional mapping where the Node know
what container it is in.

To use this library the Containing class (called the Root) and the Node class
must each derive from a class templates like xxxRoot<> and xxxNode<> repectfully
(xxx will be replaced with the name of the container type).
The templates will have two or three parameters.
The first is the type of the Root class,
the second is the type of the Node class,
and the third is an optional number to allow a given Root-Node pair to have
multiple relations of the same type, and have unique base types
 to perform these actions.

+ The types will need to be declared before use,
so typically there will need to be a forward decleration for the types.
+ The types will generally be defined before any of the member functions
in the container templates are used.
+ The derivation of the container base class should be public,
or at least the xxxRoot and xxxNode classes both made
friends as they need to be able to convert pointer types.
+ The operation functions in the container classes are declared protected,
if the Root/Node class wishes to make some
operation public, it should provide a "thunking" function to provide the access.
+ If a class derives from mutiple containers, it may need to use
scope resolution operators to select the right sub class to avoid ambiguity errors.

See @ref IntrusiveContainers for details of the containers

@defgroup IntrusiveContainers A Collection of Intrusive Containters.
@ingroup Library
See @ref containers
