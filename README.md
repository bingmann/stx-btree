# STX B+ Tree C++ Template Classes v0.9

Author: Timo Bingmann (Mail: tb a-with-circle panthema dot net)<br/>
Date: 2013-05-05

[![CMake](https://github.com/Rembrant777/stx-btree/actions/workflows/build.yml/badge.svg)](https://github.com/Rembrant777/stx-btree/actions/workflows/build.yml)


# **The STX B+ Tree package is obsolete.**

## The B+ Tree code was merged into the **TLX Library**:<br/>
<b>https://github.com/tlx/tlx/tree/master/tlx/container</b>

This is an improved version with **better STL semantics** and it will be maintain in the TLX library.

## Summary

The STX B+ Tree package is a set of C++ template classes implementing a B+ tree
key/data container in main memory. The classes are designed as drop-in
replacements of the STL containers set, map, multiset and multimap and follow
their interfaces very closely. By packing multiple value pairs into each node
of the tree the B+ tree reduces heap fragmentation and utilizes cache-line
effects better than the standard red-black binary tree. The tree algorithms are
based on the implementation in Cormen, Leiserson and Rivest's Introduction into
Algorithms, Jan Jannink's paper and other algorithm resources. The classes
contain extensive assertion and verification mechanisms to ensure the
implementation's correctness by testing the tree invariants. To illustrate the
B+ tree's structure a wxWidgets demo program is included in the source package.

## Website / API Docs / Bugs / License

The current source package can be downloaded from
http://panthema.net/2007/stx-btree/

The include files are extensively documented using doxygen. The compiled
doxygen html documentation is included in the source package. It can also be
viewed online at
http://panthema.net/2007/stx-btree/stx-btree-0.9/doxygen-html/

The wxWidgets B+ tree demo program is located in the directory
wxbtreedemo. Compiled binary versions can be found on the package web page.

If bugs should become known they will be posted on the above web page together
with patches or corrected versions.

The B+ tree template source code is released under the Boost Software License,
Version 1.0, which can be found at the header of each include file.

All auxiliary programs like the wxWidgets demo, test suite and speed tests are
licensed under the GNU General Public License v3 (GPLv3), which can be found in
the file COPYING.GPLv3.

## Original Idea

The idea originally arose while coding a read-only database, which used a huge
map of millions of non-sequential integer keys to 8-byte file offsets. When
using the standard STL red-black tree implementation this would yield millions
of 20-byte heap allocations and very slow search times due to the tree's
height. So the original intension was to reduce memory fragmentation and
improve search times. The B+ tree solves this by packing multiple data pairs
into one node with a large number of descendant nodes.

In computer science lectures it is often stated that using consecutive bytes in
memory would be more cache-efficient, because the CPU's cache levels always
fetch larger blocks from main memory. So it would be best to store the keys of
a node in one continuous array. This way the inner scanning loop would be
accelerated by benefiting from cache effects and pipelining speed-ups. Thus the
cost of scanning for a matching key would be lower than in a red-black tree,
even though the number of key comparisons are theoretically larger. This second
aspect aroused my academic interest and resulted in the speed test experiments.

A third inspiration was that no working C++ template implementation of a B+
tree could be found on the Internet. Now this one can be found.

## Implementation Overview

This implementation contains five main classes within the stx namespace
(blandly named Some Template eXtensions). The base class btree implements the
B+ tree algorithms using inner and leaf nodes in main memory. Almost all
STL-required function calls are implemented (see below for the exceptions). The
asymptotic time requirements of the STL standard are theoretically not always
fulfilled. However in practice this B+ tree performs better than the STL's
red-black tree at the cost of using more memory. See the speed test results for
details.

The base class is then specialized into btree_set, btree_multiset, btree_map
and btree_multimap using default template parameters and facade
functions. These classes are designed to be drop-in replacements for the
corresponding STL containers.

The insertion function splits the nodes on recursion unroll. Erase is largely
based on Jannink's ideas. See http://dbpubs.stanford.edu:8090/pub/1995-19 for
his paper on "Implementing Deletion in B+-trees".

The two set classes (btree_set and btree_multiset) are derived from the base
implementation class btree by specifying an empty struct as data_type. All
functions are adapted to provide the base class with empty placeholder
objects. Note that it is somewhat inefficient to implement a set or multiset
using a B+ tree: a plain B tree (without +) would hold no extra copies of the
keys. The main focus was on implementing the maps.

## Problem with Separated Key/Data Arrays

The most noteworthy difference to the default red-black tree implementation of
std::map is that the B+ tree does not hold key/data pairs together in
memory. Instead each B+ tree node has two separate arrays containing keys and
data values. This design was chosen to utilize cache-line effects while
scanning the key array.

However it also directly generates many problems in implementing the iterators'
operators. These return a (writable) reference or pointer to a value_type,
which is a std::pair composition. These data/key pairs however are not stored
together and thus a temporary copy must be constructed. This copy should not be
written to, because it is not stored back into the B+ tree. This effectively
prohibits use of many STL algorithms which writing to the B+ tree's
iterators. I would be grateful for hints on how to resolve this problem without
folding the key and data arrays.

## Test Suite

The B+ tree distribution contains an extensive test suite. According to gcov
90.9% of the btree.h implementation is covered.

## STL Incompatibilities

#### Key and Data Type Requirements

The tree algorithms currently do not use copy-construction. All key/data items
are allocated in the nodes using the default-constructor and are subsequently
only assigned new data (using operator=).

#### Key Iterators' Operators

The most important incompatibility are the non-writable operator* and
operator-> of the iterator. See above for a discussion of the problem on
separated key/data arrays. Instead of *iter and iter-> use the new function
iter.data() which returns a writable reference to the data value in the tree.

#### Key Erase Functions

The B+ tree supports three erase functions:

size_type erase(const key_type &key); // erase all data pairs matching key
bool erase_one(const key_type &key);  // erase one data pair matching key
void erase(iterator iter);            // erase pair referenced by iter

The following STL-required function is not supported:

void erase(iterator first, iterator last);

## Extensions

Beyond the usual STL interface the B+ tree classes support some extra goodies.

```
// Output the tree in a pseudo-hierarchical text dump to std::cout. This
// function requires that BTREE_DEBUG is defined prior to including the btree
// headers. Furthermore the key and data types must be std::ostream printable.
void print() const;

// Run extensive checks of the tree invariants. If a corruption in found the
// program will abort via assert(). See below on enabling auto-verification.
void verify() const;

// Serialize and restore the B+ tree nodes and data into/from a binary image.
// This requires that the key and data types are integral and contain no
// outside pointers or references.
void dump(std::ostream &os) const;
bool restore(std::istream &is);
```

## B+ Tree Traits

All tree template classes take a template parameter structure which holds
important options of the implementation. The following structure shows which
static variables specify the options and the corresponding defaults:

```
struct btree_default_map_traits
{
    // If true, the tree will self verify it's invariants after each insert()
    // or erase(). The header must have been compiled with BTREE_DEBUG
    // defined.
    static const bool   selfverify = false;

    // If true, the tree will print out debug information and a tree dump
    // during insert() or erase() operation. The header must have been
    // compiled with BTREE_DEBUG defined and key_type must be std::ostream
    // printable.
    static const bool   debug = false;

    // Number of slots in each leaf of the tree. Estimated so that each node
    // has a size of about 128 bytes.
    static const int    leafslots =
                             MAX( 8, 128 / (sizeof(_Key) + sizeof(_Data)) );

    // Number of slots in each inner node of the tree. Estimated so that each
    // node has a size of about 128 bytes.
    static const int    innerslots =
                             MAX( 8, 128 / (sizeof(_Key) + sizeof(void*)) );

    // As of stx-btree-0.9, the code does linear search in find_lower() and
    // find_upper() instead of binary_search, unless the node size is larger
    // than this threshold. See notes at
    // http://panthema.net/2013/0504-STX-B+Tree-Binary-vs-Linear-Search
    static const size_t binsearch_threshold = 256;
};
```

## Speed Tests

The implementation was tested using the speed test sources contained in the
package. For a long discussion on results please see the web page

http://panthema.net/2007/stx-btree/

