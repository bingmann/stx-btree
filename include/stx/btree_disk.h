// $Id$

#ifndef _STX_BTREE_DISK_H_
#define _STX_BTREE_DISK_H_

// *** Required Headers from the STL

#include <functional>
#include <algorithm>
#include <istream>
#include <ostream>
#include <assert.h>

#include <vector>

#include "btree_pagefile.h"

// *** Debugging Macros

#ifdef BTREE_DEBUG

#include <iostream>

#define BTREE_PRINT(x)		do { if (debug) (std::cout << x); } while(0)
#define BTREE_ASSERT(x)		do { assert(x); } while(0)

#else

#define BTREE_PRINT(x)		do { } while(0)
#define BTREE_ASSERT(x)		do { } while(0)

#endif

#define BTREE_MAX(a,b)		((a) < (b) ? (b) : (a))

/// STX - Some Template Extensions namespace
namespace stx {

/** Generates default traits for a map B+ tree. It estimates leaf and inner
 * node sizes by assuming a cache line size of 256 bytes. */
template <typename _Key, typename _Data>
struct btree_disk_default_traits
{
    /// If true, the tree will self verify it's invariants after each insert()
    /// or erase(). The header must have been compiled with BTREE_DEBUG defined.
    static const bool	selfverify = false;

    /// If true, the tree will print out debug information and a tree dump
    /// during insert() or erase() operation. The header must have been
    /// compiled with BTREE_DEBUG defined and key_type must be std::ostream
    /// printable.
    static const bool	debug = false;
};

/** @brief Basic class implementing a B+ tree data structure in memory.
 *
 */
template <typename _Key, typename _Data,
	  typename _Compare = std::less<_Key>,
	  typename _Traits = btree_disk_default_traits<_Key, _Data>,
	  bool _Duplicates = false>
class btree_disk
{
public:
    // *** Template Parameter Types

    /// First template parameter: The key type of the btree. This is stored in
    /// inner nodes and leaves
    typedef _Key			key_type;

    /// Second template parameter: The data type associated with each
    /// key. Stored in the B+ tree's leaves
    typedef _Data			data_type;

    /// Thrid template parameter: Key comparison function object
    typedef _Compare			key_compare;

    /// Fourth template parameter: Traits object used to define more parameters
    /// of the B+ tree
    typedef _Traits			traits;

    /// Fifth template parameter: Allow duplicate keys in the btree. Used to
    /// implement multiset and multimap.
    static const bool			allow_duplicates = _Duplicates;

    class btree_vector_page_manager;
    class btree_memory_page_manager;
    class btree_disk_page_manager;

    struct treeinfo;

    /// This will become a template parameter
    typedef class btree_disk_page_manager	page_manager_type;

public:
    // *** Constructed Types

    /// Typedef of our own type
    typedef btree_disk<key_type, data_type, key_compare, traits, allow_duplicates>	btree_self;

    /// Size type used to count keys
    typedef size_t				size_type;

    /// The STL-required value_type
    typedef std::pair<key_type, data_type>	value_type;

    typedef typename page_manager_type::pageid_type	pageid_type;

    static const pageid_type		NOPAGE = page_manager_type::NOPAGE;

public:
    // *** Static Constant Options and Values of the B+ Tree

    /// Base B+ tree parameter: The number of size of each page on the disk
    static const unsigned short		pagesize =  page_manager_type::pagesize;

    /// Debug parameter: Enables expensive and thorough checking of the B+ tree
    /// invariants after each insert/erase operation.
    static const bool 			selfverify = traits::selfverify;

    /// Debug parameter: Prints out lots of debug information about how the
    /// algorithms change the tree. Requires the header file to be compiled
    /// with BTREE_PRINT and the key type must be std::ostream outputable.
    static const bool 			debug = traits::debug;

public:
    // *** Static Calculation of Slots in Inner and Leaf Pages from Pagesize

    /// Precalculated size of the node header
    static const unsigned int		sizeof_node = 2 * sizeof(unsigned short);

    /// Precalculated size of inner node's extra items
    static const unsigned int		sizeof_inner_node = 0;

    /// Precalculated size of leaf node's extra items
    static const unsigned int		sizeof_leaf_node = 2 * sizeof(pageid_type);

    /// Base B+ tree parameter: The number of key/data slots in each leaf
    static const unsigned short		leafslotmax =  (pagesize - sizeof_node - sizeof_leaf_node) / ( sizeof(key_type) + sizeof(data_type) );

    /// Base B+ tree parameter: The number of key slots in each inner node,
    /// this can differ from slots in each leaf. Note the extra space for the
    /// last child pointer.
    static const unsigned short		innerslotmax =  (pagesize - sizeof_node - sizeof_inner_node - sizeof(pageid_type)) / ( sizeof(key_type) + sizeof(pageid_type) );

    /// Computed B+ tree parameter: The minimum number of key/data slots used
    /// in a leaf. If fewer slots are used, the leaf will be merged or slots
    /// shifted from it's siblings.
    static const unsigned short 	minleafslots = (leafslotmax / 2);

    /// Computed B+ tree parameter: The minimum number of key slots used
    /// in an inner node. If fewer slots are used, the inner node will be
    /// merged or slots shifted from it's siblings.
    static const unsigned short 	mininnerslots = (innerslotmax / 2);

public:
    // *** Node Classes for In-Memory Nodes

    /// The header structure of each node in-memory. This structure is extended
    /// by inner_node or leaf_node.
    struct node
    {
	/// Level in the b-tree, if level == 0 -> leaf node
	unsigned short	level;

	/// Number of key slotuse use, so number of valid children or data
	/// pointers
	unsigned short 	slotuse;

	/// Delayed initialisation of constructed node
	inline node(const unsigned short l)
	{
	    level = l;
	    slotuse = 0;
	}
	
	/// True if this is a leaf node
	inline bool isleafnode() const
	{
	    return (level == 0);
	}
    };

    /// Extended structure of a inner node in-memory. Contains only keys and no
    /// data items.
    struct inner_node : public node
    {
	/// Keys of children or data pointers
	key_type	slotkey[innerslotmax];

	/// Pointers to children
	pageid_type	childid[innerslotmax+1];

	/// Set variables to initial values
	inline inner_node(const unsigned short l)
	    : node(l)
	{
	}

	/// True if the node's slots are full
	inline bool isfull() const
	{
	    return (node::slotuse == innerslotmax);
	}

	/// True if few used entries, less than half full
	inline bool isfew() const
	{
	    return (node::slotuse <= mininnerslots);
	}

	/// True if node has too few entries
	inline bool isunderflow() const
	{
	    return (node::slotuse < mininnerslots);
	}
    };

    /// Extended structure of a leaf node in memory. Contains pairs of keys and
    /// data items. Key and data slots are kept in separate arrays, because the
    /// key array is traversed very often compared to accessing the data items.
    struct leaf_node : public node
    {
	/// Double linked list pointers to traverse the leaves
	pageid_type	prevleaf;

	/// Double linked list pointers to traverse the leaves
	pageid_type	nextleaf;

	/// Keys of children or data pointers
	key_type	slotkey[leafslotmax];

	/// Array of data
	data_type	slotdata[leafslotmax];

	/// Set variables to initial values
	inline leaf_node()
	    : node(0), prevleaf(NOPAGE), nextleaf(NOPAGE)
	{
	}

	/// True if the node's slots are full
	inline bool isfull() const
	{
	    return (node::slotuse == leafslotmax);
	}

	/// True if few used entries, less than half full
	inline bool isfew() const
	{
	    return (node::slotuse <= minleafslots);
	}

	/// True if node has too few entries
	inline bool isunderflow() const
	{
	    return (node::slotuse < minleafslots);
	}
    };

    /** Test B+ tree page manager implemented using memory pages */
    class btree_memory_page_manager
    {
    public:

	/// Size of each node on the disk.
	static const int 	pagesize = 1024;
 
	struct opaque_page_type;

	/// opaque pointer for the memory pages
	typedef unsigned int 	pageid_type;

	static const pageid_type NOPAGE = 0U;

	/// load a page into memory
	inline class node* load(pageid_type pp)
	{
	    return reinterpret_cast<class node*>(pp);
	}

	inline pageid_type allocate()
	{
	    return reinterpret_cast<pageid_type>(new char[pagesize]);
	}

	/// release lock of page
	inline void release(pageid_type pp, const class node* mp)
	{
	    assert( reinterpret_cast<class node*>(pp) == mp );
	}

	/// free a page 
	inline void free(pageid_type pp)
	{
	    char *pc = reinterpret_cast<char*>(pp);
	    delete [] pc;
	}
    };

    class btree_vector_page_manager
    {
    public:

	/// Size of each node on the disk.
	static const int 	pagesize = 1024;
 
	struct opaque_page_type;

	/// opaque pointer for the memory pages
	typedef unsigned int	pageid_type;

	static const pageid_type NOPAGE = 0U;

	typedef std::vector<char*> vector_type;

	vector_type		pages;

	btree_vector_page_manager()
	{
	    pages.push_back(new char[1]);
	}

	inline pageid_type allocate()
	{
	    vector_type::iterator freeiter = std::find(pages.begin(), pages.end(), static_cast<char*>(NULL));
	    if (freeiter != pages.end())
	    {
		unsigned int pi = freeiter - pages.begin();
		pages[pi] = new char[pagesize];
		return pi;
	    }

	    pages.push_back(new char[pagesize]);
	    return pages.size() - 1;
	}

	/// load a page into memory
	inline class node* load(pageid_type pi)
	{
	    return reinterpret_cast<class node*>( pages.at(pi) );
	}

	/// release lock of page
	inline void release(pageid_type pi, const class node* np)
	{
	    assert( reinterpret_cast<class node*>( pages.at(pi) ) == np );
	}

	/// free a page 
	inline void free(pageid_type pp)
	{
	    assert( pp < pages.size() );
	    delete pages[pp];
	    pages[pp] = NULL;
	}
    };

    class btree_disk_page_manager
    {
    public:

	btree_pagefile		pagefile;

	/// Size of each node on the disk.
	static const int 	pagesize = 1024;
 
	struct opaque_page_type;

	/// opaque pointer for the memory pages
	typedef unsigned int	pageid_type;

	static const pageid_type NOPAGE = 0U;

	inline btree_disk_page_manager(const char *filename, unsigned int dbnumber=0)
	    : pagefile(filename, dbnumber)
	{
	}

	inline pageid_type allocate()
	{
	    return pagefile.allocate();
	}

	/// load a page into memory
	inline class node* load(pageid_type pi)
	{
	    char *mempage = new char[pagesize];
	    assert(pi != 0);

	    if (!pagefile.get(pi, mempage)) {
		assert(0);
		return NULL;
	    }

	    return reinterpret_cast<class node*>(mempage);
	}

	/// release lock of page
	inline void release(pageid_type pi, class node* np)
	{
	    char *mempage = reinterpret_cast<char*>(np);

	    if (!pagefile.put(pi, mempage)) {
		assert(0);
		return;
	    }

	    delete [] mempage;
	}

	inline struct treeinfo* get_dbinfo()
	{
	    return reinterpret_cast<struct treeinfo*>(pagefile.get_dbinfo());
	}

	inline void set_dbinfo(struct treeinfo* ti)
	{
	    pagefile.set_root(reinterpret_cast<char*>(ti));
	}

	/// free a page 
	inline void free(pageid_type pp)
	{
	    assert(0);
	}
    };

public:
    // *** Iterators and Reverse Iterators

    /// STL-like iterator object for B+ tree items. The iterator points to a
    /// specific slot number in a leaf.
    class iterator
    {
    public:
	// *** Types

	/// The key type of the btree. Returned by key().
	typedef typename btree_disk::key_type		key_type;

	/// The data type of the btree. Returned by data().
	typedef typename btree_disk::data_type		data_type;

	/// The value type of the btree. Returned by operator*().
	typedef typename btree_disk::value_type		value_type;

	/// Reference to the value_type. Required by the reverse_iterator.
	typedef value_type&		reference;

	/// Pointer to the value_type. Required by the reverse_iterator.
	typedef value_type*		pointer;

	/// STL-magic iterator category
	typedef std::bidirectional_iterator_tag iterator_category;

	/// STL-magic
	typedef ptrdiff_t               difference_type;

	/// Our own type
	typedef iterator		self;

    private:
	// *** Members

	/// Reference to the btree object.
	btree_disk&		tree;

	/// The currently referenced leaf node of the tree
	pageid_type		currnode;

	/// Currently locked node page
	typename btree_disk::leaf_node* currleaf;

	/// Current key/data slot referenced
	unsigned short		currslot;
    
	/// Friendly to the const_iterator, so it may access the two data items
	/// directly
	friend class btree_disk<key_type, data_type, key_compare, traits, allow_duplicates>::const_iterator;
	
	/// Evil! A temporary value_type to STL-correctly deliver operator* and
	/// operator->
	mutable value_type		value;

    public:
	// *** Methods

	/// Constructor of a mutable iterator
	inline iterator(btree_disk &t, pageid_type l, unsigned short s)
	    : tree(t), currnode(l), currleaf(NULL), currslot(s)
	{
	    load();
	}

	/// Copy-Constructor
	inline iterator(const iterator &it)
	    : tree(it.tree), currnode(it.currnode), currleaf(NULL), currslot(it.currslot)
	{
	    load();
	}

	/// Release the locked page
	inline ~iterator()
	{
	    save();
	}

	/// Load the reference data from the disk page, keeps the lock
	inline void load()
	{
	    if (!currnode) return;

	    currleaf = tree.load_leaf(currnode);

	    if (currslot == static_cast<unsigned short>(-1)) {
		currslot = currleaf->slotuse - 1;
	    }

	    if (currslot < currleaf->slotuse) {
		value = value_type(currleaf->slotkey[currslot],
				   currleaf->slotdata[currslot]);
	    }
	    else {
		value = value_type();
	    }
	}

	/// Save temporary data to the disk page
	inline void save()
	{
	    if (!currleaf) return;
	    if (!currnode) return;

	    if (currslot < currleaf->slotuse)
	    {
		currleaf->slotdata[currslot] = value.second;
	    }

	    tree.release(currnode, currleaf);
	    currleaf = NULL;
	}

	/// Dereference the iterator, this is not a value_type& because key and
	/// value are not stored together
	inline reference operator*() const
	{
	    return value;
	}

	/// Dereference the iterator. Do not use this if possible, use key()
	/// and data() instead. The B+ tree does not stored key and data
	/// together.
	inline pointer operator->() const
	{
	    return &value;
	}

	/// Key of the current slot
	inline const key_type& key() const
	{
	    return currleaf->slotkey[currslot];
	}

	/// Writable reference to the current data object
	inline data_type& data() const
	{
	    return currleaf->slotdata[currslot];
	}

	/// Prefix++ advance the iterator to the next slot
	inline self& operator++()
	{
	    unsigned short leafslotuse = currleaf->slotuse;
	    pageid_type nextleaf = currleaf->nextleaf;

	    save();

	    if (currslot + 1 < leafslotuse) {
		++currslot;
	    }
	    else if (nextleaf != NOPAGE) {
		currnode = nextleaf;
		currslot = 0;
	    }
	    else {
		// this is end()
		currslot = leafslotuse;
	    }

	    load();
	    return *this;
	}

	/// Postfix++ advance the iterator to the next slot
	inline self operator++(int)
	{
	    self tmp = *this;	// copy ourselves

	    unsigned short leafslotuse = currleaf->slotuse;
	    pageid_type nextleaf = currleaf->nextleaf;

	    save();

	    if (currslot + 1 < leafslotuse) {
		++currslot;
	    }
	    else if (nextleaf != NOPAGE) {
		currnode = nextleaf;
		currslot = 0;
	    }
	    else {
		// this is end()
		currslot = leafslotuse;
	    }

	    load();
	    return tmp;
	}

	/// Prefix-- backstep the iterator to the last slot
	inline self& operator--()
	{
	    pageid_type prevleaf = currleaf->prevleaf;

	    save();

	    if (currslot > 0) {
		--currslot;
	    }
	    else if (prevleaf != NOPAGE) {
		currnode = prevleaf;
		load();
		currslot = currleaf->slotuse - 1;
		return *this;
	    }
	    else {
		// this is begin()
		currslot = 0;
	    }

	    load();
	    return *this;
	}

	/// Postfix-- backstep the iterator to the last slot
	inline self operator--(int)
	{
	    self tmp = *this;	// copy ourselves

	    pageid_type prevleaf = currleaf->prevleaf;

	    save();
	    if (currslot > 0) {
		--currslot;
	    }
	    else if (prevleaf != NOPAGE) {
		currnode = prevleaf;
		load();
		currslot = currleaf->slotuse - 1;

		return tmp;
	    }
	    else {
		// this is begin()
		currslot = 0;
	    }
	    load();

	    return tmp;
	}

	/// Equality of iterators
	inline bool operator==(const self& x) const
	{
	    return (x.currnode == currnode) && (x.currslot == currslot);
	}

	/// Inequality of iterators
	inline bool operator!=(const self& x) const
	{
	    return (x.currnode != currnode) || (x.currslot != currslot);
	}    
    };

    /// STL-like read-only iterator object for B+ tree items. The iterator
    /// points to a specific slot number in a leaf.
    class const_iterator
    {
    public:
	// *** Types

	/// The key type of the btree. Returned by key().
	typedef typename btree_disk::key_type		key_type;

	/// The data type of the btree. Returned by data().
	typedef typename btree_disk::data_type		data_type;

	/// The value type of the btree. Returned by operator*().
	typedef typename btree_disk::value_type		value_type;

	/// Reference to the value_type. Required by the reverse_iterator.
	typedef const value_type&	reference;

	/// Pointer to the value_type. Required by the reverse_iterator.
	typedef const value_type*	pointer;

	/// STL-magic iterator category
	typedef std::bidirectional_iterator_tag iterator_category;

	/// STL-magic
	typedef ptrdiff_t         	difference_type;

	/// Our own type
	typedef const_iterator		self;

    private:
	// *** Members

	/// Reference to the btree object.
	btree_disk&		tree;

	/// The currently referenced leaf node of the tree
	pageid_type		currnode;

	/// Currently locked node page
	typename btree_disk::leaf_node* currleaf;

	/// Current key/data slot referenced
	unsigned short		currslot;
    
	/// Evil! A temporary value_type to STL-correctly deliver operator* and
	/// operator->
	mutable value_type		value;

    public:
	// *** Methods

	/// Construtor of a const iterator
	inline const_iterator(btree_disk &t, pageid_type l, unsigned short s)
	    : tree(t), currnode(l), currleaf(NULL), currslot(s)
	{
	    load();
	}

	/// Copy-constructor from a mutable const iterator
	inline const_iterator(const const_iterator &it)
	    : tree(it.tree), currnode(it.currnode), currleaf(NULL), currslot(it.currslot)
	{
	    load();
	}

	/// Copy-constructor from a mutable const iterator
	inline const_iterator(const iterator &it)
	    : tree(it.tree), currnode(it.currnode), currleaf(NULL), currslot(it.currslot)
	{
	    load();
	}

	/// Release the locked page
	inline ~const_iterator()
	{
	    release();
	}

	/// Load the reference data from the disk page, keeps the lock
	inline void load()
	{
	    if (!currnode) return;

	    currleaf = tree.load_leaf(currnode);

	    if (currslot == static_cast<unsigned short>(-1)) {
		currslot = currleaf->slotuse - 1;
	    }

	    if (currslot < currleaf->slotuse) {
		value = value_type(currleaf->slotkey[currslot],
				   currleaf->slotdata[currslot]);
	    }
	    else {
		value = value_type();
	    }
	}

	/// Save temporary data to the disk page
	inline void release()
	{
	    if (!currleaf) return;
	    if (!currnode) return;

	    tree.release(currnode, currleaf);
	    currleaf = NULL;
	}

	/// Dereference the iterator. Do not use this if possible, use key()
	/// and data() instead. The B+ tree does not stored key and data
	/// together.
	inline reference operator*() const
	{
	    return value;
	}

	/// Dereference the iterator. Do not use this if possible, use key()
	/// and data() instead. The B+ tree does not stored key and data
	/// together.
	inline pointer operator->() const
	{
	    return &value;
	}

	/// Key of the current slot
	inline const key_type& key() const
	{
	    return currleaf->slotkey[currslot];
	}

	/// Read-only reference to the current data object
	inline const data_type& data() const
	{
	    return currleaf->slotdata[currslot];
	}

	/// Prefix++ advance the iterator to the next slot
	inline self& operator++()
	{
	    unsigned short leafslotuse = currleaf->slotuse;
	    pageid_type nextleaf = currleaf->nextleaf;

	    release();

	    if (currslot + 1 < leafslotuse) {
		++currslot;
	    }
	    else if (nextleaf != NOPAGE) {
		currnode = nextleaf;
		currslot = 0;
	    }
	    else {
		// this is end()
		currslot = leafslotuse;
	    }

	    load();
	    return *this;
	}

	/// Postfix++ advance the iterator to the next slot
	inline self operator++(int)
	{
	    self tmp = *this;	// copy ourselves

	    unsigned short leafslotuse = currleaf->slotuse;
	    pageid_type nextleaf = currleaf->nextleaf;

	    release();

	    if (currslot + 1 < leafslotuse) {
		++currslot;
	    }
	    else if (nextleaf != NOPAGE) {
		currnode = nextleaf;
		currslot = 0;
	    }
	    else {
		// this is end()
		currslot = leafslotuse;
	    }

	    load();
	    return tmp;
	}

	/// Prefix-- backstep the iterator to the last slot
	inline self& operator--()
	{
	    pageid_type prevleaf = currleaf->prevleaf;

	    release();

	    if (currslot > 0) {
		--currslot;
	    }
	    else if (prevleaf != NOPAGE) {
		currnode = prevleaf;
		load();
		currslot = currleaf->slotuse - 1;
		return *this;
	    }
	    else {
		// this is begin()
		currslot = 0;
	    }

	    load();
	    return *this;
	}

	/// Postfix-- backstep the iterator to the last slot
	inline self operator--(int)
	{
	    self tmp = *this;	// copy ourselves

	    pageid_type prevleaf = currleaf->prevleaf;

	    release();
	    if (currslot > 0) {
		--currslot;
	    }
	    else if (prevleaf != NOPAGE) {
		currnode = prevleaf;
		load();
		currslot = currleaf->slotuse - 1;

		return tmp;
	    }
	    else {
		// this is begin()
		currslot = 0;
	    }
	    load();

	    return tmp;
	}

	/// Equality of iterators
	inline bool operator==(const self& x) const
	{
	    return (x.currnode == currnode) && (x.currslot == currslot);
	}

	/// Inequality of iterators
	inline bool operator!=(const self& x) const
	{
	    return (x.currnode != currnode) || (x.currslot != currslot);
	}    
    };

    /// create mutable reverse iterator by using STL magic
    typedef std::reverse_iterator<iterator>       reverse_iterator;

    /// create constant reverse iterator by using STL magic
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
    // *** Small Statistics Structure

    /** A small struct containing basic statistics about the B+ tree. It can be
     * fetched using get_stats(). */
    struct treeinfo
    {
	/// Pointer to the B+ tree's root node, either leaf or inner node
	pageid_type	root;

	/// Pointer to first leaf in the double linked leaf chain
	pageid_type	headleaf;

	/// Pointer to last leaf in the double linked leaf chain
	pageid_type	tailleaf;

	/// Number of items in the B+ tree
	size_type	itemcount;

	/// Number of leaves in the B+ tree
	size_type	leaves;

	/// Number of inner nodes in the B+ tree
	size_type	innernodes;

	/// Levels in the B+ tree
	unsigned short	levels;

	/// Base B+ tree parameter: The number of key/data slots in each leaf
	static const unsigned short	leafslots = btree_self::leafslotmax;

	/// Base B+ tree parameter: The number of key slots in each inner node.
	static const unsigned short	innerslots = btree_self::innerslotmax;

	/// Zero initialized
	inline treeinfo()
	    : root(NOPAGE), headleaf(NOPAGE), tailleaf(NOPAGE),
	      itemcount(0),
	      leaves(0), innernodes(0), levels(0)
	{
	}

	/// Return the total number of nodes
	inline size_type nodes() const
	{
	    return innernodes + leaves;
	}

	/// Return the average fill of leaves
	inline double avgfill_leaves() const
	{
	    return static_cast<double>(itemcount) / (leaves * leafslots);
	}
    };

private:
    // *** Tree Object Data Members

    /// Other small statistics about the B+ tree
    treeinfo	info;

    /// Key comparison object. More comparison functions are generated from
    /// this < relation.
    key_compare	key_less;

    /// The page manager holding the page's data
    page_manager_type	&page_manager;

public:
    // *** Constructors and Destructor

    /// Default constructor initializing an empty B+ tree with the standard key
    /// comparison function
    inline btree_disk(page_manager_type	&pm)
	: page_manager(pm)
    {
	info = *page_manager.get_dbinfo();
    }

    /// Constructor initializing an empty B+ tree with a special key
    /// comparison object
    inline btree_disk(page_manager_type	&pm, const key_compare &kcf)
	: key_less(kcf), page_manager(pm)
    {
	info = *page_manager.get_dbinfo();
    }

    /// Constructor initializing a B+ tree with the range [first,last)
    template <class InputIterator>
    inline btree_disk(page_manager_type	&pm, InputIterator first, InputIterator last)
	: page_manager(pm)
    {
	info = *page_manager.get_dbinfo();
	insert(first, last);
    }

    /// Constructor initializing a B+ tree with the range [first,last) and a
    /// special key comparison object
    template <class InputIterator>
    inline btree_disk(page_manager_type	&pm, InputIterator first, InputIterator last, const key_compare &kcf)
	: key_less(kcf), page_manager(pm)
    {
	info = *page_manager.get_dbinfo();
	insert(first, last);
    }

    /// Frees up all used B+ tree memory pages
    inline ~btree_disk()
    {
	page_manager.set_dbinfo(&info);
    }

    /// Fast swapping of two identical B+ tree objects.
    void swap(btree_self& from)
    {
	std::swap(info, from.info);
    }

    inline void sync()
    {
	page_manager.set_dbinfo(&info);
    }

public:
    // *** Key and Value Comparison Function Objects

    /// Function class to compare value_type objects. Required by the STL
    class value_compare
    {
    protected:
	/// Key comparison function from the template parameter
	key_compare	key_comp;
 
	/// Constructor called from btree_disk::value_comp()
	inline value_compare(key_compare kc)
	    : key_comp(kc)
	{ }

	/// Friendly to the btree class so it may call the constructor
	friend class btree_disk<key_type, data_type, key_compare, traits, allow_duplicates>;
 
    public:
	/// Function call "less"-operator resulting in true if x < y.
	inline bool operator()(const value_type& x, const value_type& y) const
	{
	    return key_comp(x.first, y.first);
	}
    };

    /// Constant access to the key comparison object sorting the B+ tree
    inline key_compare key_comp() const
    {
	return key_less;
    }

    /// Constant access to a constructed value_type comparison object. Required
    /// by the STL
    inline value_compare value_comp() const
    {
	return value_compare(key_less);
    }

private:
    // *** Convenient Key Comparison Functions Generated From key_less

    /// True if a <= b ? constructed from key_less()
    inline bool key_lessequal(const key_type &a, const key_type b) const
    {
	return !key_less(b, a);
    }

    /// True if a > b ? constructed from key_less()
    inline bool key_greater(const key_type &a, const key_type &b) const
    {
	return key_less(b, a);
    }

    /// True if a >= b ? constructed from key_less()
    inline bool key_greaterequal(const key_type &a, const key_type b) const
    {
	return !key_less(a, b);
    }

    /// True if a == b ? constructed from key_less(). This requires the <
    /// relation to be a total order, otherwise the B+ tree cannot be sorted.
    inline bool key_equal(const key_type &a, const key_type &b) const
    {
	return !key_less(a, b) && !key_less(b, a);
    }

private:
    // *** Front-end Functions to the Page Manager

    /// Allocate and initialize a leaf node
    inline pageid_type allocate_leaf()
    {
	pageid_type np = page_manager.allocate();

	node* n = page_manager.load(np);
	new (n) leaf_node();
	page_manager.release(np, n);

	info.leaves++;
	return np;
    }

    /// Allocate and initialize an inner node
    inline pageid_type allocate_inner(unsigned short l)
    {
	pageid_type np = page_manager.allocate();

	node* n = page_manager.load(np);
	new (n) inner_node(l);
	page_manager.release(np, n);

	info.innernodes++;
	return np;
    }

    /// Load a page without checking what type it is
    inline class node* load_node(pageid_type pp)
    {
	return page_manager.load(pp);
    }

    /// Load a page and check that it's a leaf node
    inline class leaf_node* load_leaf(pageid_type pp)
    {
	node *n = page_manager.load(pp);
	BTREE_ASSERT( n->isleafnode() );
	return static_cast<class leaf_node*>(n);
    }

    /// Load a page and check that it's an inner node
    inline class inner_node* load_inner(pageid_type pp)
    {
	node *n = page_manager.load(pp);
	BTREE_ASSERT( !n->isleafnode() );
	return static_cast<class inner_node*>(n);
    }

    /// Convenience function when descending into the tree. Release a page and
    /// load the next in one call.
    inline class node* release_load_next(pageid_type pp, class node* mp, pageid_type next)
    {
	release(pp, mp);
	return load_node(next);
    }

    /// Release a page/node after it has been read/modified
    inline void release(pageid_type pp, class node* mp)
    {
	page_manager.release(pp, mp);
    }
    
    /// Correctly free either inner or leaf node, destructs all contained key
    /// and value objects
    inline void free_node(pageid_type pi, node *n)
    {
	if (n->isleafnode()) {
	    static_cast<leaf_node*>(n)->~leaf_node();
	    page_manager.free(pi);
	    info.leaves--;
	}
	else {
	    static_cast<inner_node*>(n)->~inner_node();
	    page_manager.free(pi);
	    info.innernodes--;
	}
    }

public:
    // *** Fast Destruction of the B+ Tree

    /// Frees all key/data pairs and all nodes of the tree
    void clear()
    {
	if (info.root)
	{
	    node *rootnode = load_node(info.root);
	    clear_recursive(rootnode);
	    release(info.root, rootnode);

	    free_node(info.root, rootnode);

	    info = treeinfo();
	}

	BTREE_ASSERT(info.itemcount == 0);
    }

private:
    /// Recursively free up nodes
    void clear_recursive(class node *node)
    {
	if (node->isleafnode())
	{
	    leaf_node *leafnode = static_cast<leaf_node*>(node);

	    for (unsigned int slot = 0; slot < leafnode->slotuse; ++slot)
	    {
		// data objects are deleted by leaf_node's destructor
	    }
	}
	else
	{
	    inner_node *innernode = static_cast<inner_node*>(node);

	    for (unsigned short slot = 0; slot < innernode->slotuse + 1; ++slot)
	    {
		pageid_type childid = innernode->childid[slot];

		class node* childnode = load_node(childid);
		clear_recursive(childnode);
		release(childid, childnode);

		free_node(childid, childnode);
	    }
	}
    }

public:
    // *** STL Iterator Construction Functions

    /// Constructs a read/data-write iterator that points to the first slot in
    /// the first leaf of the B+ tree.
    inline iterator begin()
    {
	return iterator(*this, info.headleaf, 0);
    }

    /// Constructs a read/data-write iterator that points to the first invalid
    /// slot in the last leaf of the B+ tree.
    inline iterator end()
    {
	return iterator(*this, info.tailleaf, static_cast<unsigned short>(-1));
    }

    /// Constructs a read-only constant iterator that points to the first slot
    /// in the first leaf of the B+ tree.
    inline const_iterator begin() const
    {
	return const_iterator(info.headleaf, 0);
    }

    /// Constructs a read-only constant iterator that points to the first
    /// invalid slot in the last leaf of the B+ tree.
    inline const_iterator end() const
    {
	return const_iterator(info.tailleaf, info.tailleaf->slotuse);
    }

    /// Constructs a read/data-write reverse iterator that points to the first
    /// invalid slot in the last leaf of the B+ tree. Uses STL magic.
    inline reverse_iterator rbegin()
    {
	return reverse_iterator(end());
    }

    /// Constructs a read/data-write reverse iterator that points to the first
    /// slot in the first leaf of the B+ tree. Uses STL magic.
    inline reverse_iterator rend()
    {
	return reverse_iterator(begin());
    }

    /// Constructs a read-only reverse iterator that points to the first
    /// invalid slot in the last leaf of the B+ tree. Uses STL magic.
    inline const_reverse_iterator rbegin() const
    {
	return const_reverse_iterator(end());
    }

    /// Constructs a read-only reverse iterator that points to the first slot
    /// in the first leaf of the B+ tree. Uses STL magic.
    inline const_reverse_iterator rend() const
    {
	return const_reverse_iterator(begin());
    }

private:
    // *** B+ Tree Node Binary Search Functions

    /// Searches for the first key in the node n less or equal to key. Uses
    /// binary search with an optional linear self-verification. This is a
    /// template function, because the slotkey array is located at different
    /// places in leaf_node and inner_node.
    template <typename node_type>
    inline int find_lower(const node_type *n, const key_type& key) const
    {
	if (n->slotuse == 0) return 0;

	int lo = 0,
	    hi = n->slotuse - 1;

	while(lo < hi)
	{
	    int mid = (lo + hi) / 2;

	    if (key_lessequal(key, n->slotkey[mid])) {
		hi = mid - 1;
	    }
	    else {
		lo = mid + 1;
	    }
	}

	if (hi < 0 || key_less(n->slotkey[hi], key))
	    hi++;

	BTREE_PRINT("btree::find_lower: on " << n << " key " << key << " -> (" << lo << ") " << hi << ", ");

	// verify result using simple linear search
	if (selfverify)
	{
	    int i = n->slotuse - 1;
	    while(i >= 0 && key_lessequal(key, n->slotkey[i]))
		i--;
	    i++;

	    BTREE_PRINT("testfind: " << i << std::endl);
	    BTREE_ASSERT(i == hi);
	}
	else {
	    BTREE_PRINT(std::endl);
	}
	
	return hi;
    }

    /// Searches for the first key in the node n greater than key. Uses binary
    /// search with an optional linear self-verification. This is a template
    /// function, because the slotkey array is located at different places in
    /// leaf_node and inner_node.
    template <typename node_type>
    inline int find_upper(const node_type *n, const key_type& key) const
    {
	if (n->slotuse == 0) return 0;

	int lo = 0,
	    hi = n->slotuse - 1;

	while(lo < hi)
	{
	    int mid = (lo + hi) / 2;

	    if (key_less(key, n->slotkey[mid])) {
		hi = mid - 1;
	    }
	    else {
		lo = mid + 1;
	    }
	}

	if (hi < 0 || key_lessequal(n->slotkey[hi], key))
	    hi++;

	BTREE_PRINT("btree::find_upper: on " << n << " key " << key << " -> (" << lo << ") " << hi << ", ");

	// verify result using simple linear search
	if (selfverify)
	{
	    int i = n->slotuse - 1;
	    while(i >= 0 && key_less(key, n->slotkey[i]))
		i--;
	    i++;

	    BTREE_PRINT("btree::find_upper testfind: " << i << std::endl);
	    BTREE_ASSERT(i == hi);
	}
	else {
	    BTREE_PRINT(std::endl);
	}
	
	return hi;
    }

public:
    // *** Access Functions to the Item Count

    /// Return the number of key/data pairs in the B+ tree
    inline size_type size() const
    {
	return info.itemcount;
    }

    /// Returns true if there is at least one key/data pair in the B+ tree
    inline bool empty() const
    {
	return (size() == size_type(0));
    }
    
    /// Returns the largest possible size of the B+ Tree. This is just a
    /// function required by the STL standard, the B+ Tree can hold more items.
    inline size_type max_size() const
    {
	return size_type(-1);
    }

    /// Return a const reference to the current statistics.
    inline const struct treeinfo& get_stats() const
    {
	return info;
    }

public:
    // *** Standard Access Functions Querying the Tree by Descending to a Leaf

    /// Non-STL function checking whether a key is in the B+ tree. The same as
    /// (find(k) != end()) or (count() != 0).
    bool exists(const key_type &key)
    {
	pageid_type id = info.root;
	if (!id) return false;

	class node* node = load_node(id);

	while(!node->isleafnode())
	{
	    const inner_node *inner = static_cast<const inner_node*>(node);
	    int slot = find_lower(inner, key);

	    pageid_type nextid = inner->childid[slot];

	    node = release_load_next(id, node, nextid);
	    id = nextid;
	}

	const leaf_node *leaf = static_cast<const leaf_node*>(node);

	int slot = find_lower(leaf, key);
	bool ret = key_equal(key, leaf->slotkey[slot]);
	release(id, node);

	return ret;
    }

    /// Tries to locate a key in the B+ tree and returns an iterator to the
    /// key/data slot if found. If unsuccessful it returns end().
    iterator find(const key_type &key)
    {
	pageid_type id = info.root;
	if (!id) return end();

	class node* node = load_node(id);

	while(!node->isleafnode())
	{
	    const inner_node *inner = static_cast<const inner_node*>(node);
	    int slot = find_lower(inner, key);

	    pageid_type nextid = inner->childid[slot];

	    node = release_load_next(id, node, nextid);
	    id = nextid;
	}

	leaf_node *leaf = static_cast<leaf_node*>(node);

	int slot = find_lower(leaf, key);
	iterator ret = key_equal(key, leaf->slotkey[slot]) ? iterator(*this, id, slot) : end();
	release(id, node);

	return ret;
    }

#if 0
    /// Tries to locate a key in the B+ tree and returns an constant iterator
    /// to the key/data slot if found. If unsuccessful it returns end().
    const_iterator find(const key_type &key) const
    {
	pageid_type id = info.root;
	if (!id) return end();

	class node* node = load_node(id);

	while(!node->isleafnode())
	{
	    const inner_node *inner = static_cast<const inner_node*>(node);
	    int slot = find_lower(inner, key);

	    pageid_type nextid = inner->childid[slot];

	    node = release_load_next(id, node, nextid);
	    id = nextid;
	}

	const leaf_node *leaf = static_cast<const leaf_node*>(node);

	int slot = find_lower(leaf, key);
	iterator ret = key_equal(key, leaf->slotkey[slot]) ? const_iterator(*this, id, slot) : end();
	release(id, node);

	return ret;
    }
#endif

    /// Tries to locate a key in the B+ tree and returns the number of
    /// identical key entries found.    
    size_type count(const key_type &key)
    {
	pageid_type id = info.root;
	if (!id) return 0;

	class node* node = load_node(id);

	while(!node->isleafnode())
	{
	    const inner_node *inner = static_cast<const inner_node*>(node);
	    int slot = find_lower(inner, key);

	    pageid_type nextid = inner->childid[slot];

	    node = release_load_next(id, node, nextid);
	    id = nextid;
	}

	const leaf_node *leaf = static_cast<const leaf_node*>(node);

	int slot = find_lower(leaf, key);
	size_type num = 0;

	while (leaf && key_equal(key, leaf->slotkey[slot]))
	{
	    ++num;
	    if (++slot >= leaf->slotuse)
	    {
		pageid_type nextid = leaf->nextleaf;
		node = release_load_next(id, node, nextid);
		id = nextid;
		leaf = static_cast<const leaf_node*>(node);

		slot = 0;
	    }
	}
	release(id, node);

	return num;
    }

#if 0
    /// Searches the B+ tree and returns an iterator to the first key less or
    /// equal to the parameter. If unsuccessful it returns end().
    iterator lower_bound(const key_type& key)
    {
	node *n = info.root;
	if (!n) return end();

	while(!n->isleafnode())
	{
	    const inner_node *inner = static_cast<const inner_node*>(n);
	    int slot = find_lower(inner, key);

	    n = inner->childid[slot];
	}

	leaf_node *leaf = static_cast<leaf_node*>(n);

	int slot = find_lower(leaf, key);
	return iterator(leaf, slot);
    }

    /// Searches the B+ tree and returns an constant iterator to the first key less or
    /// equal to the parameter. If unsuccessful it returns end().
    const_iterator lower_bound(const key_type& key) const
    {
	const node *n = info.root;
	if (!n) return end();

	while(!n->isleafnode())
	{
	    const inner_node *inner = static_cast<const inner_node*>(n);
	    int slot = find_lower(inner, key);

	    n = inner->childid[slot];
	}

	const leaf_node *leaf = static_cast<const leaf_node*>(n);

	int slot = find_lower(leaf, key);
	return const_iterator(leaf, slot);
    }

    /// Searches the B+ tree and returns an iterator to the first key greater
    /// than the parameter. If unsuccessful it returns end().
    iterator upper_bound(const key_type& key)
    {
	node *n = info.root;
	if (!n) return end();

	while(!n->isleafnode())
	{
	    const inner_node *inner = static_cast<const inner_node*>(n);
	    int slot = find_upper(inner, key);

	    n = inner->childid[slot];
	}

	leaf_node *leaf = static_cast<leaf_node*>(n);

	int slot = find_upper(leaf, key);
	return iterator(leaf, slot);
    }

    /// Searches the B+ tree and returns an constant iterator to the first key
    /// greater than the parameter. If unsuccessful it returns end().
    const_iterator upper_bound(const key_type& key) const
    {
	const node *n = info.root;
	if (!n) return end();

	while(!n->isleafnode())
	{
	    const inner_node *inner = static_cast<const inner_node*>(n);
	    int slot = find_upper(inner, key);

	    n = inner->childid[slot];
	}

	const leaf_node *leaf = static_cast<const leaf_node*>(n);

	int slot = find_upper(leaf, key);
	return const_iterator(leaf, slot);
    }

    /// Searches the B+ tree and returns both lower_bound() and upper_bound().
    inline std::pair<iterator, iterator> equal_range(const key_type& key)
    {
	return std::pair<iterator, iterator>(lower_bound(key), upper_bound(key));
    }

    /// Searches the B+ tree and returns both lower_bound() and upper_bound().
    inline std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    {
	return std::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
    }

public:
    // *** B+ Tree Object Comparison Functions

    /// Equality relation of B+ trees of the same type. B+ trees of the same
    /// size and equal elements (both key and data) are considered
    /// equal. Beware of the random ordering of duplicate keys.
    inline bool operator==(const btree_self &other) const
    {
	return (size() == other.size()) && std::equal(begin(), end(), other.begin());
    }

    /// Inequality relation. Based on operator==.
    inline bool operator!=(const btree_self &other) const
    {
	return !(*this == other);
    }

    /// Total ordering relation of B+ trees of the same type. It uses
    /// std::lexicographical_compare() for the actual comparison of elements.
    inline bool operator<(const btree_self &other) const
    {
	return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
    }

    /// Greater relation. Based on operator<.
    inline bool operator>(const btree_self &other) const
    {
	return other < *this;
    }

    /// Less-equal relation. Based on operator<.
    inline bool operator<=(const btree_self &other) const
    {
	return !(other < *this);
    }

    /// Greater-equal relation. Based on operator<.
    inline bool operator>=(const btree_self &other) const
    {
	return !(*this < other);
    }

public:
    /// *** Fast Copy: Assign Operator and Copy Constructors

    /// Assignment operator. All the key/data pairs are copied
    inline btree_self& operator= (const btree_self &other)
    {
	if (this != &other)
	{
	    clear();

	    key_less = other.key_comp();
	    if (other.size() != 0)
	    {
		info.leaves = info.innernodes = 0;
		info.root = copy_recursive(other.info.root);
		info = other.info;
	    }

	    if (selfverify) verify();
	}
	return *this;
    }

    /// Copy constructor. The newly initialized B+ tree object will contain a
    /// copy or all key/data pairs.
    inline btree_disk(const btree_self &other)
	: info( other.info ),
	  key_less( other.key_comp() )
    {
	if (size() > 0)
	{
	    info.leaves = info.innernodes = 0;
	    info.root = copy_recursive(info.other.root);
	    if (selfverify) verify();
	}
    }
    
private:
    /// Recursively copy nodes from another B+ tree object
    node* copy_recursive(const node *node)
    {
	if (node->isleafnode())
	{
	    const leaf_node *leaf = static_cast<const leaf_node*>(node);
	    leaf_node *newleaf = allocate_leaf();

	    newleaf->slotuse = leaf->slotuse;
	    std::copy(leaf->slotkey, leaf->slotkey + leaf->slotuse, newleaf->slotkey);
	    std::copy(leaf->slotdata, leaf->slotdata + leaf->slotuse, newleaf->slotdata);

	    if (info.headleaf == NOPAGE)
	    {
		info.headleaf = info.tailleaf = newleaf;
		newleaf->prevleaf = newleaf->nextleaf = NOPAGE;
	    }
	    else
	    {
		newleaf->prevleaf = info.tailleaf;
		info.tailleaf->nextleaf = newleaf;
		info.tailleaf = newleaf;
	    }

	    return newleaf;
	}
	else
	{
	    const inner_node *inner = static_cast<const inner_node*>(node);
	    inner_node *newinner = allocate_inner(inner->level);

	    newinner->slotuse = inner->slotuse;
	    std::copy(inner->slotkey, inner->slotkey + inner->slotuse, newinner->slotkey);

	    for (unsigned short slot = 0; slot <= inner->slotuse; ++slot)
	    {
		newinner->childid[slot] = copy_recursive(inner->childid[slot]);
	    }

	    return newinner;
	}
    }
#endif

public:
    // *** Public Insertion Functions

    /// Attempt to insert a key/data pair into the B+ tree. If the tree does not
    /// allow duplicate keys, then the insert may fail if it is already
    /// present.
    inline std::pair<iterator, bool> insert(const value_type& x)
    {
	return insert_start(x.first, x.second);
    }
    
    /// Attempt to insert a key/data pair into the B+ tree. Beware that if
    /// key_type == data_type, then the template iterator insert() is called
    /// instead. If the tree does not allow duplicate keys, then the insert may
    /// fail if it is already present.
    inline std::pair<iterator, bool> insert(const key_type& key, const data_type& data)
    {
	return insert_start(key, data);
    }

    /// Attempt to insert a key/data pair into the B+ tree. This function is the
    /// same as the other insert, however if key_type == data_type then the
    /// non-template function cannot be called. If the tree does not allow
    /// duplicate keys, then the insert may fail if it is already present.
    inline std::pair<iterator, bool> insert2(const key_type& key, const data_type& data)
    {
	return insert_start(key, data);
    }

    /// Attempt to insert a key/data pair into the B+ tree. The iterator hint
    /// is currently ignored by the B+ tree insertion routine.
    inline iterator insert(iterator /* hint */, const value_type &x)
    {
	return insert_start(x.first, x.second).first;
    }

    /// Attempt to insert a key/data pair into the B+ tree. The iterator hint is
    /// currently ignored by the B+ tree insertion routine.
    inline iterator insert2(iterator /* hint */, const key_type& key, const data_type& data)
    {
	return insert_start(key, data).first;
    }

    /// Attempt to insert the range [first,last) of value_type pairs into the B+
    /// tree. Each key/data pair is inserted individually.
    template <typename InputIterator>
    inline void insert(InputIterator first, InputIterator last)
    {
	InputIterator iter = first;
	while(iter != last)
	{
	    insert(*iter);
	    ++iter;
	}
    }

private:
    // *** Private Insertion Functions

    /// Start the insertion descent at the current root and handle root
    /// splits. Returns true if the item was inserted
    std::pair<iterator, bool> insert_start(const key_type& key, const data_type& value)
    {
	pageid_type newchild = NOPAGE;
	key_type newkey = key_type();

	if (info.root == NOPAGE)
	{
	    info.root = info.headleaf = info.tailleaf = allocate_leaf();
	    info.levels++;
	    page_manager.set_dbinfo(&info);
	}

	std::pair<iterator, bool> r = insert_descend(info.root, key, value, &newkey, &newchild);

	if (newchild)
	{
	    node *rootnode = load_node(info.root);
	    pageid_type newid = allocate_inner(rootnode->level + 1);
	    release(info.root, rootnode);

	    inner_node *newroot = load_inner(newid);

	    newroot->slotkey[0] = newkey;

	    newroot->childid[0] = info.root;
	    newroot->childid[1] = newchild;

	    newroot->slotuse = 1;

	    release(newid, newroot);

	    info.levels++;
	    info.root = newid;
	    page_manager.set_dbinfo(&info);
	}

	// increment itemcount if the item was inserted
	if (r.second) ++info.itemcount;

	if (debug)
	    print();

	if (selfverify) {
	    verify();
	    BTREE_ASSERT(exists(key));
	}

	return r;
    }

    /**
     * @brief Insert an item into the B+ tree.
     *
     * Descend down the nodes to a leaf, insert the key/data pair in a free
     * slot. If the node overflows, then it must be split and the new split
     * node inserted into the parent. Unroll / this splitting up to the root.
    */
    std::pair<iterator, bool> insert_descend(pageid_type nodeid,
					     const key_type& key, const data_type& value,
					     key_type* splitkey, pageid_type* splitnode)
    {
	node *node = load_node(nodeid);

	if (!node->isleafnode())
	{
	    inner_node *inner = static_cast<inner_node*>(node);

	    key_type newkey = key_type();
	    pageid_type newchild = NOPAGE;

	    int slot = find_lower(inner, key);

	    BTREE_PRINT("btree::insert_descend into " << inner->childid[slot] << std::endl);

	    std::pair<iterator, bool> r = insert_descend(inner->childid[slot],
							 key, value, &newkey, &newchild);

	    if (newchild)
	    {
		BTREE_PRINT("btree::insert_descend newchild with key " << newkey << " node " << newchild << " at slot " << slot << std::endl);

		if (inner->isfull())
		{
		    split_inner_node(inner, splitkey, splitnode, slot);

		    BTREE_PRINT("btree::insert_descend done split_inner: putslot: " << slot << " putkey: " << newkey << " upkey: " << *splitkey << std::endl);

		    if (debug)
		    {
			print_node(nodeid);
			print_node(*splitnode);
		    }

		    // check if insert slot is in the split sibling node
		    BTREE_PRINT("btree::insert_descend switch: " << slot << " > " << inner->slotuse+1 << std::endl);

		    inner_node *splitinner = load_inner(*splitnode);

		    if (slot == inner->slotuse+1 && inner->slotuse < splitinner->slotuse)
		    {
			// special case when the insert slot matches the split
			// place between the two nodes, then the insert key
			// becomes the split key.

			BTREE_ASSERT(inner->slotuse + 1 < innerslotmax);

			// move the split key and it's datum into the left node
			inner->slotkey[inner->slotuse] = *splitkey;
			inner->childid[inner->slotuse+1] = splitinner->childid[0];
			inner->slotuse++;

			// set new split key and move corresponding datum into right node
			splitinner->childid[0] = newchild;
			*splitkey = newkey;

			release(*splitnode, splitinner);
			release(nodeid, node);
			return r;
		    }
		    else if (slot >= inner->slotuse+1)
		    {
			// in case the insert slot is in the newly create split
			// node, we reuse the code below.
			slot -= inner->slotuse+1;

			release(nodeid, node);

			nodeid = *splitnode;
			node = inner = splitinner;

			BTREE_PRINT("btree::insert_descend switching to splitted node " << nodeid << " slot " << slot << std::endl);
		    }
		}

		// put pointer to child node into correct slot
		BTREE_ASSERT(slot >= 0 && slot <= inner->slotuse);

		int i = inner->slotuse;

		while(i > slot) {
		    inner->slotkey[i] = inner->slotkey[i - 1];
		    inner->childid[i + 1] = inner->childid[i];
		    i--;
		}

		inner->slotkey[slot] = newkey;
		inner->childid[slot + 1] = newchild;
		inner->slotuse++;
	    }
	    
	    release(nodeid, node);
	    return r;
	}
        else // n->isleafnode() == true
	{
	    leaf_node *leaf = static_cast<leaf_node*>(node);

	    int slot = find_lower(leaf, key);

	    if (!allow_duplicates && slot < leaf->slotuse && key_equal(key, leaf->slotkey[slot])) {
		release(nodeid, node);
		return std::pair<iterator, bool>(iterator(*this, nodeid, slot), false);
	    }

	    if (leaf->isfull())
	    {
		split_leaf_node(nodeid, leaf, splitkey, splitnode);

		// check if insert slot is in the split sibling node
		if (slot >= leaf->slotuse)
		{
		    slot -= leaf->slotuse;
		    release(nodeid, node);

		    node = leaf = load_leaf(*splitnode);
		    nodeid = *splitnode;
		}
	    }

	    // put data item into correct data slot

	    int i = leaf->slotuse - 1;
	    BTREE_ASSERT(i + 1 < leafslotmax);

	    while(i >= 0 && key_less(key, leaf->slotkey[i])) {
		leaf->slotkey[i + 1] = leaf->slotkey[i];
		leaf->slotdata[i + 1] = leaf->slotdata[i];
		i--;
	    }
	    
	    leaf->slotkey[i + 1] = key;
	    leaf->slotdata[i + 1] = value;
	    leaf->slotuse++;

	    if (splitnode && nodeid != *splitnode && slot == leaf->slotuse-1)
	    {
		// special case: the node was split, and the insert is at the
		// last slot of the old node. then the splitkey must be
		// updated.
		*splitkey = key;
	    }

	    release(nodeid, node);
	    return std::pair<iterator, bool>(iterator(*this, nodeid, i + 1), true);
	}
    }

    /// Split up a leaf node into two equally-filled sibling leaves. Returns
    /// the new nodes and it's insertion key in the two parameters.
    void split_leaf_node(pageid_type leafid, leaf_node* leaf, key_type* _newkey, pageid_type* _newleaf)
    {
	BTREE_ASSERT(leaf->isfull());

	unsigned int mid = leaf->slotuse / 2;

	BTREE_PRINT("btree::split_leaf_node on " << leafid << std::endl);

	*_newleaf = allocate_leaf();

	leaf_node *newleaf = load_leaf(*_newleaf);

	newleaf->slotuse = leaf->slotuse - mid;

	newleaf->nextleaf = leaf->nextleaf;
	if (newleaf->nextleaf == NOPAGE) {
	    BTREE_ASSERT(leafid == info.tailleaf);
	    info.tailleaf = *_newleaf;
	}
	else {
	    leaf_node *newnextleaf = load_leaf(newleaf->nextleaf);
	    newnextleaf->prevleaf = *_newleaf;
	    release(newleaf->nextleaf, newnextleaf);
	}

	for(unsigned int slot = mid; slot < leaf->slotuse; ++slot)
	{
	    unsigned int ni = slot - mid;
	    newleaf->slotkey[ni] = leaf->slotkey[slot];
	    newleaf->slotdata[ni] = leaf->slotdata[slot];
	}
	    
	leaf->slotuse = mid;
	leaf->nextleaf = *_newleaf;
	newleaf->prevleaf = leafid;

	*_newkey = leaf->slotkey[leaf->slotuse-1];
	release(*_newleaf, newleaf);
    }

    /// Split up an inner node into two equally-filled sibling nodes. Returns
    /// the new nodes and it's insertion key in the two parameters. Requires
    /// the slot of the item will be inserted, so the nodes will be the same
    /// size after the insert.
    void split_inner_node(inner_node* inner, key_type* _newkey, pageid_type* _newinner, unsigned int addslot)
    {
	BTREE_ASSERT(inner->isfull());

	unsigned int mid = inner->slotuse / 2;

	BTREE_PRINT("btree::split_inner: mid " << mid << " addslot " << addslot << std::endl);

	// if the split is uneven and the overflowing item will be put into the
	// larger node, then the smaller split node may underflow
	if (addslot <= mid && mid > inner->slotuse - (mid + 1))
	    mid--;

	BTREE_PRINT("btree::split_inner: mid " << mid << " addslot " << addslot << std::endl);

	BTREE_PRINT("btree::split_inner_node on " << inner << " into two nodes " << mid << " and " << inner->slotuse - (mid + 1) << " sized" << std::endl);

	*_newinner = allocate_inner(inner->level);
	inner_node *newinner = load_inner(*_newinner);

	newinner->slotuse = inner->slotuse - (mid + 1);

	for(unsigned int slot = mid + 1; slot < inner->slotuse; ++slot)
	{
	    unsigned int ni = slot - (mid + 1);
	    newinner->slotkey[ni] = inner->slotkey[slot];
	    newinner->childid[ni] = inner->childid[slot];
	}
	newinner->childid[newinner->slotuse] = inner->childid[inner->slotuse];
	    
	inner->slotuse = mid;

	*_newkey = inner->slotkey[mid];
	release(*_newinner, newinner);
    }

private:
    // *** Support Class Encapsulating Deletion Results

    /// Result flags of recursive deletion.
    enum result_flags_t
    {
	/// Deletion successful and no fix-ups necessary.
	btree_ok = 0,

	/// Deletion not successful because key was not found.
	btree_not_found = 1,

	/// Deletion successful, the last key was updated so parent slotkeys
	/// need updates.
	btree_update_lastkey = 2,

	/// Deletion successful, children nodes were merged and the parent
	/// needs to remove the empty node.
	btree_fixmerge = 4
    };

    /// \internal B+ tree recursive deletion has much information which is
    /// needs to be passed upward.
    struct result_t
    {
	/// Merged result flags
	result_flags_t	flags;

	/// The key to be updated at the parent's slot
	key_type	lastkey;

	/// Constructor of a result with a specific flag, this can also be used
	/// as for implicit conversion.
	inline result_t(result_flags_t f = btree_ok)
	    : flags(f), lastkey()
	{ }

	/// Constructor with a lastkey value.
	inline result_t(result_flags_t f, const key_type &k)
	    : flags(f), lastkey(k)
	{ }

	/// Test if this result object has a given flag set.
	inline bool has(result_flags_t f) const
	{
	    return (flags & f);
	}

	/// Merge two results OR-ing the result flags and overwriting lastkeys.
	inline result_t& operator|= (const result_t &other)
	{
	    flags = result_flags_t(flags | other.flags);

	    // we overwrite existing lastkeys on purpose
	    if (other.has(btree_update_lastkey))
		lastkey = other.lastkey;

	    return *this;
	}
    };

#if 0
public:
    // *** Public Erase Functions

    /// Erases one (the first) of the key/data pairs associated with the given
    /// key.
    bool erase_one(const key_type &key)
    {
	BTREE_PRINT("btree::erase_one(" << key << ") on btree size " << size() << std::endl);

	if (selfverify) verify();
	
	result_t result = erase_one_descend(key, info.root, NULL, NULL, NULL, NULL, NULL, 0);

	if (!result.has(btree_not_found))
	    --info.itemcount;

	if (debug) print();
	if (selfverify) verify();

	return !result.has(btree_not_found);
    }

    /// Erases all the key/data pairs associated with the given key. This is
    /// implemented using erase_one().
    size_type erase(const key_type &key)
    {
	size_type c = 0;

	while( erase_one(key) )
	{
	    ++c;
	    if (!allow_duplicates) break;
	}

	return c;
    }

#ifdef BTREE_TODO
    /// Erase the key/data pair referenced by the iterator.
    void erase(iterator iter)
    {

    }
#endif

#ifdef BTREE_TODO
    /// Erase all key/data pairs in the range [first,last). This function is
    /// currently not implemented by the B+ Tree.
    void erase(iterator /* first */, iterator /* last */)
    {
	abort();
    }
#endif

private:
    // *** Private Erase Functions

    /** @brief Erase one (the first) key/data pair in the B+ tree matching key.
     *
     * Descends down the tree in search of key. During the descent the parent,
     * left and right siblings and their parents are computed and passed
     * down. Once the key/data pair is found, it is removed from the leaf. If
     * the leaf underflows 6 different cases are handled. These cases resolve
     * the underflow by shifting key/data pairs from adjacent sibling nodes,
     * merging two sibling nodes or trimming the tree.
     */
    result_t erase_one_descend(const key_type& key,
			       node *curr,
			       node *left, node *right,
			       inner_node *leftparent, inner_node *rightparent,
			       inner_node *parent, unsigned int parentslot)
    {
	if (curr->isleafnode())
	{
	    leaf_node *leaf = static_cast<leaf_node*>(curr);
	    leaf_node *leftleaf = static_cast<leaf_node*>(left);
	    leaf_node *rightleaf = static_cast<leaf_node*>(right);

	    int slot = find_lower(leaf, key);

	    if (!key_equal(key, leaf->slotkey[slot]))
	    {
		BTREE_PRINT("Could not find key " << key << " to erase." << std::endl);

		return btree_not_found;
	    }

	    BTREE_PRINT("Found key in leaf " << curr << " at slot " << slot << std::endl);

	    for (int i = slot; i < leaf->slotuse - 1; i++)
	    {
		leaf->slotkey[i] = leaf->slotkey[i + 1];
		leaf->slotdata[i] = leaf->slotdata[i + 1];
	    }
	    leaf->slotuse--;

	    result_t myres = btree_ok;

	    // if the last key of the leaf was changed, the parent is notified
	    // and updates the key of this leaf
	    if (slot == leaf->slotuse)
	    {
		if (parent && parentslot < parent->slotuse)
		{
		    BTREE_ASSERT(parent->childid[parentslot] == curr);
		    parent->slotkey[parentslot] = leaf->slotkey[leaf->slotuse - 1];
		}
		else
		{
		    BTREE_PRINT("Schedueling lastkeyupdate: key " << leaf->slotkey[leaf->slotuse - 1] << std::endl);
		    myres |= result_t(btree_update_lastkey, leaf->slotkey[leaf->slotuse - 1]);
		}
	    }

	    if (leaf->isunderflow() && !(leaf == info.root && leaf->slotuse >= 1))
	    {
		// determine what to do about the underflow

		// case : if this empty leaf is the root, there is no way to
		// correct underflow
		if (leftleaf == NULL && rightleaf == NULL)
		{
		    return btree_ok;
		}
		// case : if both left and right leaves would underflow in case of
		// a shift, then merging is necessary. choose the more local merger
		// with our parent
		else if ( (leftleaf == NULL || leftleaf->isfew()) && (rightleaf == NULL || rightleaf->isfew()) )
		{
		    if (leftparent == parent)
			myres |= merge_leaves(leftleaf, leaf, leftparent);
		    else
			myres |= merge_leaves(leaf, rightleaf, rightparent);
		}
		// case : the right leaf has extra data, so balance right with current
		else if ( (leftleaf != NULL && leftleaf->isfew()) && (rightleaf != NULL && !rightleaf->isfew()) )
		{
		    if (rightparent == parent)
			myres |= shift_left_leaf(leaf, rightleaf, rightparent, parentslot);
		    else
			myres |= merge_leaves(leftleaf, leaf, leftparent);
		}
		// case : the left leaf has extra data, so balance left with current
		else if ( (leftleaf != NULL && !leftleaf->isfew()) && (rightleaf != NULL && rightleaf->isfew()) )
		{
		    if (leftparent == parent)
			shift_right_leaf(leftleaf, leaf, leftparent, parentslot - 1);
		    else
			myres |= merge_leaves(leaf, rightleaf, rightparent);
		}
		// case : both the leaf and right leaves have extra data and our
		// parent, choose the leaf with more data
		else if (leftparent == rightparent)
		{
		    if (leftleaf->slotuse <= rightleaf->slotuse)
			myres |= shift_left_leaf(leaf, rightleaf, rightparent, parentslot);
		    else
			shift_right_leaf(leftleaf, leaf, leftparent, parentslot - 1);
		}
		else
		{
		    if (leftparent == parent)
			shift_right_leaf(leftleaf, leaf, leftparent, parentslot - 1);
		    else
			myres |= shift_left_leaf(leaf, rightleaf, rightparent, parentslot);
		}
	    }

	    return myres;
	}
	else // !curr->isleafnode()
	{
	    inner_node *inner = static_cast<inner_node*>(curr);
	    inner_node *leftinner = static_cast<inner_node*>(left);
	    inner_node *rightinner = static_cast<inner_node*>(right);

	    node *myleft, *myright;
	    inner_node *myleftparent, *myrightparent;

	    int slot = find_lower(inner, key);

	    if (slot == 0) {
		myleft = (left == NULL) ? NULL : (static_cast<inner_node*>(left))->childid[left->slotuse - 1];
		myleftparent = leftparent;
	    }
	    else {
		myleft = inner->childid[slot - 1];
		myleftparent = inner;
	    }

	    if (slot == inner->slotuse) {
		myright = (right == NULL) ? NULL : (static_cast<inner_node*>(right))->childid[0];
		myrightparent = rightparent;
	    }
	    else {
		myright = inner->childid[slot + 1];
		myrightparent = inner;
	    }

	    BTREE_PRINT("erase_one_descend into " << inner->childid[slot] << std::endl);

	    result_t result = erase_one_descend(key,
						inner->childid[slot],
						myleft, myright,
						myleftparent, myrightparent,
						inner, slot);

	    result_t myres = btree_ok;

	    if (result.has(btree_not_found))
	    {
		return result;
	    }

	    if (result.has(btree_update_lastkey))
	    {
		if (parent && parentslot < parent->slotuse)
		{
		    BTREE_PRINT("Fixing lastkeyupdate: key " << result.lastkey << " into parent " << parent << " at parentslot " << parentslot << std::endl);

		    BTREE_ASSERT(parent->childid[parentslot] == curr);
		    parent->slotkey[parentslot] = result.lastkey;
		}
		else
		{
		    BTREE_PRINT("Forwarding lastkeyupdate: key " << result.lastkey << std::endl);
		    myres |= result_t(btree_update_lastkey, result.lastkey);
		}
	    }

	    if (result.has(btree_fixmerge))
	    {
		// either the current node or the next is empty and should be removed
		if (inner->childid[slot]->slotuse != 0)
		    slot++;

		// this is the child slot invalidated by the merge
		BTREE_ASSERT(inner->childid[slot]->slotuse == 0);

		free_node(inner->childid[slot]);

		for(int i = slot; i < inner->slotuse; i++)
		{
		    inner->slotkey[i - 1] = inner->slotkey[i];
		    inner->childid[i] = inner->childid[i + 1];
		}
		inner->slotuse--;

		if (inner->level == 1)
		{
		    // fix split key for children leaves
		    slot--;
		    leaf_node *child = static_cast<leaf_node*>(inner->childid[slot]);
		    inner->slotkey[slot] = child->slotkey[ child->slotuse-1 ];
		}
	    }

	    if (inner->isunderflow() && !(inner == info.root && inner->slotuse >= 1))
	    {
		// case: the inner node is the root and has just one child. that child becomes the new root
		if (leftinner == NULL && rightinner == NULL)
		{
		    BTREE_ASSERT(inner == info.root);
		    BTREE_ASSERT(inner->slotuse == 0);

		    info.root = inner->childid[0];

		    inner->slotuse = 0;
		    free_node(inner);

		    return btree_ok;
		}
		// case : if both left and right leaves would underflow in case of
		// a shift, then merging is necessary. choose the more local merger
		// with our parent
		else if ( (leftinner == NULL || leftinner->isfew()) && (rightinner == NULL || rightinner->isfew()) )
		{
		    if (leftparent == parent)
			myres |= merge_inner(leftinner, inner, leftparent, parentslot - 1);
		    else
			myres |= merge_inner(inner, rightinner, rightparent, parentslot);
		}
		// case : the right leaf has extra data, so balance right with current
		else if ( (leftinner != NULL && leftinner->isfew()) && (rightinner != NULL && !rightinner->isfew()) )
		{
		    if (rightparent == parent)
			shift_left_inner(inner, rightinner, rightparent, parentslot);
		    else
			myres |= merge_inner(leftinner, inner, leftparent, parentslot - 1);
		}
		// case : the left leaf has extra data, so balance left with current
		else if ( (leftinner != NULL && !leftinner->isfew()) && (rightinner != NULL && rightinner->isfew()) )
		{
		    if (leftparent == parent)
			shift_right_inner(leftinner, inner, leftparent, parentslot - 1);
		    else
			myres |= merge_inner(inner, rightinner, rightparent, parentslot);
		}
		// case : both the leaf and right leaves have extra data and our
		// parent, choose the leaf with more data
		else if (leftparent == rightparent)
		{
		    if (leftinner->slotuse <= rightinner->slotuse)
			shift_left_inner(inner, rightinner, rightparent, parentslot);
		    else
			shift_right_inner(leftinner, inner, leftparent, parentslot - 1);
		}
		else
		{
		    if (leftparent == parent)
			shift_right_inner(leftinner, inner, leftparent, parentslot - 1);
		    else
			shift_left_inner(inner, rightinner, rightparent, parentslot);
		}
	    }

	    return myres;
	}
    }

    /// Merge two leaf nodes. The function moves all key/data pairs from right
    /// to left and sets right's slotuse to zero. The right slot is then
    /// removed by the calling parent node.
    result_t merge_leaves(leaf_node* left, leaf_node* right, inner_node* parent)
    {
	BTREE_PRINT("Merge leaf nodes " << left << " and " << right << " with common parent " << parent << "." << std::endl);
	(void)parent;

	BTREE_ASSERT(left->isleafnode() && right->isleafnode());
	BTREE_ASSERT(parent->level == 1);

	BTREE_ASSERT(left->slotuse + right->slotuse < leafslotmax);

	for (unsigned int i = 0; i < right->slotuse; i++)
	{
	    left->slotkey[left->slotuse + i] = right->slotkey[i];
	    left->slotdata[left->slotuse + i] = right->slotdata[i];
	}
	left->slotuse += right->slotuse;

	left->nextleaf = right->nextleaf;
	if (left->nextleaf)
	    left->nextleaf->prevleaf = left;
	else
	    info.tailleaf = left;

	right->slotuse = 0;

	return btree_fixmerge;
    }

    /// Merge two inner nodes. The function moves all key/childid pairs from
    /// right to left and sets right's slotuse to zero. The right slot is then
    /// removed by the calling parent node.
    static result_t merge_inner(inner_node* left, inner_node* right, inner_node* parent, unsigned int parentslot)
    {
	BTREE_PRINT("Merge inner nodes " << left << " and " << right << " with common parent " << parent << "." << std::endl);

	BTREE_ASSERT(left->level == right->level);
	BTREE_ASSERT(parent->level == left->level + 1);

	BTREE_ASSERT(parent->childid[parentslot] == left);

	BTREE_ASSERT(left->slotuse + right->slotuse < innerslotmax);

	if (selfverify)
	{
	    // find the left node's slot in the parent's children
	    unsigned int leftslot = 0;
	    while(leftslot <= parent->slotuse && parent->childid[leftslot] != left)
		++leftslot;

	    BTREE_ASSERT(leftslot < parent->slotuse);
	    BTREE_ASSERT(parent->childid[leftslot] == left);
	    BTREE_ASSERT(parent->childid[leftslot+1] == right);

	    BTREE_ASSERT(parentslot == leftslot);
	}

	// retrieve the decision key from parent
	left->slotkey[left->slotuse] = parent->slotkey[parentslot];
	left->slotuse++;

        // copy over keys and children from right
	for (unsigned int i = 0; i < right->slotuse; i++)
	{
	    left->slotkey[left->slotuse + i] = right->slotkey[i];
	    left->childid[left->slotuse + i] = right->childid[i];
	}
	left->slotuse += right->slotuse;

	left->childid[left->slotuse] = right->childid[right->slotuse];

	right->slotuse = 0;

	return btree_fixmerge;
    }

    /// Balance two leaf nodes. The function moves key/data pairs from right to
    /// left so that both nodes are equally filled. The parent node is updated
    /// if possible.
    static result_t shift_left_leaf(leaf_node *left, leaf_node *right, inner_node *parent, unsigned int parentslot)
    {
	BTREE_ASSERT(left->isleafnode() && right->isleafnode());
	BTREE_ASSERT(parent->level == 1);

	BTREE_ASSERT(left->nextleaf == right);
	BTREE_ASSERT(left == right->prevleaf);

	BTREE_ASSERT(left->slotuse < right->slotuse);
	BTREE_ASSERT(parent->childid[parentslot] == left);

	unsigned int shiftnum = (right->slotuse - left->slotuse) / 2;

	BTREE_PRINT("Shifting (leaf) " << shiftnum << " entries to left " << left << " from right " << right << " with common parent " << parent << "." << std::endl);

	BTREE_ASSERT(left->slotuse + shiftnum < leafslotmax);

	// copy the first items from the right node to the last slot in the left node.
	for(unsigned int i = 0; i < shiftnum; i++)
	{
	    left->slotkey[left->slotuse + i] = right->slotkey[i];
	    left->slotdata[left->slotuse + i] = right->slotdata[i];
	}
	left->slotuse += shiftnum;
	
	// shift all slots in the right node to the left
    
	right->slotuse -= shiftnum;
	for(int i = 0; i < right->slotuse; i++)
	{
	    right->slotkey[i] = right->slotkey[i + shiftnum];
	    right->slotdata[i] = right->slotdata[i + shiftnum];
	}

	// fixup parent
	if (parentslot < parent->slotuse) {
	    parent->slotkey[parentslot] = left->slotkey[left->slotuse - 1];
	    return btree_ok;
	}
	else { // the update is further up the tree
	    return result_t(btree_update_lastkey, left->slotkey[left->slotuse - 1]);
	}
    }

    /// Balance two inner nodes. The function moves key/data pairs from right
    /// to left so that both nodes are equally filled. The parent node is
    /// updated if possible.
    static void shift_left_inner(inner_node *left, inner_node *right, inner_node *parent, unsigned int parentslot)
    {
	BTREE_ASSERT(left->level == right->level);
	BTREE_ASSERT(parent->level == left->level + 1);

	BTREE_ASSERT(left->slotuse < right->slotuse);
	BTREE_ASSERT(parent->childid[parentslot] == left);

	unsigned int shiftnum = (right->slotuse - left->slotuse) / 2;

	BTREE_PRINT("Shifting (inner) " << shiftnum << " entries to left " << left << " from right " << right << " with common parent " << parent << "." << std::endl);

	BTREE_ASSERT(left->slotuse + shiftnum < innerslotmax);

	if (selfverify)
	{
	    // find the left node's slot in the parent's children and compare to parentslot

	    unsigned int leftslot = 0;
	    while(leftslot <= parent->slotuse && parent->childid[leftslot] != left)
		++leftslot;

	    BTREE_ASSERT(leftslot < parent->slotuse);
	    BTREE_ASSERT(parent->childid[leftslot] == left);
	    BTREE_ASSERT(parent->childid[leftslot+1] == right);

	    BTREE_ASSERT(leftslot == parentslot);
	}

	// copy the parent's decision slotkey and childid to the first new key on the left
	left->slotkey[left->slotuse] = parent->slotkey[parentslot];
	left->slotuse++;

	// copy the other items from the right node to the last slots in the left node.
	for(unsigned int i = 0; i < shiftnum - 1; i++)
	{
	    left->slotkey[left->slotuse + i] = right->slotkey[i];
	    left->childid[left->slotuse + i] = right->childid[i];
	}
	left->slotuse += shiftnum - 1;

	// fixup parent
	parent->slotkey[parentslot] = right->slotkey[shiftnum - 1];
	// last pointer in left
	left->childid[left->slotuse] = right->childid[shiftnum - 1];
	
	// shift all slots in the right node
    
	right->slotuse -= shiftnum;
	for(int i = 0; i < right->slotuse; i++)
	{
	    right->slotkey[i] = right->slotkey[i + shiftnum];
	    right->childid[i] = right->childid[i + shiftnum];
	}
	right->childid[right->slotuse] = right->childid[right->slotuse + shiftnum];
    }

    /// Balance two leaf nodes. The function moves key/data pairs from left to
    /// right so that both nodes are equally filled. The parent node is updated
    /// if possible.
    static void shift_right_leaf(leaf_node *left, leaf_node *right, inner_node *parent, unsigned int parentslot)
    {
	BTREE_ASSERT(left->isleafnode() && right->isleafnode());
	BTREE_ASSERT(parent->level == 1);

	BTREE_ASSERT(left->nextleaf == right);
	BTREE_ASSERT(left == right->prevleaf);
	BTREE_ASSERT(parent->childid[parentslot] == left);

	BTREE_ASSERT(left->slotuse > right->slotuse);

	unsigned int shiftnum = (left->slotuse - right->slotuse) / 2;

	BTREE_PRINT("Shifting (leaf) " << shiftnum << " entries to right " << right << " from left " << left << " with common parent " << parent << "." << std::endl);

	if (selfverify)
	{
	    // find the left node's slot in the parent's children
	    unsigned int leftslot = 0;
	    while(leftslot <= parent->slotuse && parent->childid[leftslot] != left)
		++leftslot;

	    BTREE_ASSERT(leftslot < parent->slotuse);
	    BTREE_ASSERT(parent->childid[leftslot] == left);
	    BTREE_ASSERT(parent->childid[leftslot+1] == right);

	    BTREE_ASSERT(leftslot == parentslot);
	}

	// shift all slots in the right node
    
	BTREE_ASSERT(right->slotuse + shiftnum < leafslotmax);

	for(int i = right->slotuse; i >= 0; i--)
	{
	    right->slotkey[i + shiftnum] = right->slotkey[i];
	    right->slotdata[i + shiftnum] = right->slotdata[i];
	}
	right->slotuse += shiftnum;

	// copy the last items from the left node to the first slot in the right node.
    	for(unsigned int i = 0; i < shiftnum; i++)
	{
	    right->slotkey[i] = left->slotkey[left->slotuse - shiftnum + i];
	    right->slotdata[i] = left->slotdata[left->slotuse - shiftnum + i];
	}
	left->slotuse -= shiftnum;

	parent->slotkey[parentslot] = left->slotkey[left->slotuse-1];
    }

    /// Balance two inner nodes. The function moves key/data pairs from left to
    /// right so that both nodes are equally filled. The parent node is updated
    /// if possible.
    static void shift_right_inner(inner_node *left, inner_node *right, inner_node *parent, unsigned int parentslot)
    {
	BTREE_ASSERT(left->level == right->level);
	BTREE_ASSERT(parent->level == left->level + 1);

	BTREE_ASSERT(left->slotuse > right->slotuse);
	BTREE_ASSERT(parent->childid[parentslot] == left);

	unsigned int shiftnum = (left->slotuse - right->slotuse) / 2;

	BTREE_PRINT("Shifting (leaf) " << shiftnum << " entries to right " << right << " from left " << left << " with common parent " << parent << "." << std::endl);

	if (selfverify)
	{
	    // find the left node's slot in the parent's children
	    unsigned int leftslot = 0;
	    while(leftslot <= parent->slotuse && parent->childid[leftslot] != left)
		++leftslot;

	    BTREE_ASSERT(leftslot < parent->slotuse);
	    BTREE_ASSERT(parent->childid[leftslot] == left);
	    BTREE_ASSERT(parent->childid[leftslot+1] == right);

	    BTREE_ASSERT(leftslot == parentslot);
	}

	// shift all slots in the right node

	BTREE_ASSERT(right->slotuse + shiftnum < innerslotmax);
    
	right->childid[right->slotuse + shiftnum] = right->childid[right->slotuse];
	for(int i = right->slotuse-1; i >= 0; i--)
	{
	    right->slotkey[i + shiftnum] = right->slotkey[i];
	    right->childid[i + shiftnum] = right->childid[i];
	}

	right->slotuse += shiftnum;

	// copy the parent's decision slotkey and childid to the last new key on the right
	right->slotkey[shiftnum - 1] = parent->slotkey[parentslot];
	right->childid[shiftnum - 1] = left->childid[left->slotuse];

	// copy the remaining last items from the left node to the first slot in the right node.
    	for(unsigned int i = 0; i < shiftnum - 1; i++)
	{
	    right->slotkey[i] = left->slotkey[left->slotuse - shiftnum + i + 1];
	    right->childid[i] = left->childid[left->slotuse - shiftnum + i + 1];
	}

	// copy the first to-be-removed key from the left node to the parent's decision slot
	parent->slotkey[parentslot] = left->slotkey[left->slotuse - shiftnum];

	left->slotuse -= shiftnum;
    }
#endif

public:
    // *** Debug Printing

    /// Print out the B+ tree structure with keys onto std::cout. This function
    /// requires that the header is compiled with BTREE_PRINT and that key_type
    /// is outputtable via std::ostream.
    void print()
    {
	print_node(info.root, 0, true);
    }

    /// Print out only the leaves via the double linked list.
    void print_leaves()
    {
	BTREE_PRINT("leaves:" << std::endl);

	pageid_type id = info.headleaf;

	while(id != 0)
	{
	    leaf_node *n = load_leaf(id);

	    BTREE_PRINT("  " << n << std::endl);

	    pageid_type nextid = n->nextleaf;

	    page_manager.release(id, n);

	    id = nextid;
	}
    }

private:

    /// Recursively descend down the tree and print out nodes.
    void print_node(pageid_type nodeid, unsigned int depth=0, bool recursive=false)
    {
	node *node = load_node(nodeid);

	for(unsigned int i = 0; i < depth; i++) BTREE_PRINT("  ");
	    
	BTREE_PRINT("node " << nodeid << " level " << node->level << " slotuse " << node->slotuse << std::endl);

	if (node->isleafnode())
	{
	    const leaf_node *leafnode = static_cast<const leaf_node*>(node);

	    for(unsigned int i = 0; i < depth; i++) BTREE_PRINT("  ");
	    BTREE_PRINT("  leaf prev " << leafnode->prevleaf << " next " << leafnode->nextleaf << std::endl);

	    for(unsigned int i = 0; i < depth; i++) BTREE_PRINT("  ");

	    for (unsigned int slot = 0; slot < leafnode->slotuse; ++slot)
	    {
		BTREE_PRINT(leafnode->slotkey[slot] << "  "); // << "(data: " << leafnode->slotdata[slot] << ") ";
	    }
	    BTREE_PRINT(std::endl);
	}
	else
	{
	    const inner_node *innernode = static_cast<const inner_node*>(node);

	    for(unsigned int i = 0; i < depth; i++) BTREE_PRINT("  ");

	    for (unsigned short slot = 0; slot < innernode->slotuse; ++slot)
	    {
		BTREE_PRINT("(" << innernode->childid[slot] << ") " << innernode->slotkey[slot] << " ");
	    }
	    BTREE_PRINT("(" << innernode->childid[innernode->slotuse] << ")");
	    BTREE_PRINT(std::endl);

	    if (recursive)
	    {
		for (unsigned short slot = 0; slot < innernode->slotuse + 1; ++slot)
		{
		    print_node(innernode->childid[slot], depth + 1, recursive);
		}
	    }
	}

	page_manager.release(nodeid, node);
    }

public:
    // *** Verification of B+ Tree Invariants

    /// Run a thorough verification of all B+ tree invariants. The program
    /// aborts via assert() if something is wrong.
    void verify()
    {
	key_type minkey, maxkey;
	treeinfo vinfo;
	
	if (info.root)
	{
	    verify_node(info.root, &minkey, &maxkey, vinfo);

	    assert( vinfo.itemcount == info.itemcount );
	    assert( vinfo.leaves == info.leaves );
	    assert( vinfo.innernodes == info.innernodes );
	}

	verify_leaflinks();
    }

private:

    /// Recursively descend down the tree and verify each node
    void verify_node(pageid_type nodeid, key_type* minkey, key_type* maxkey, treeinfo &vinfo)
    {
	BTREE_PRINT("verifynode " << nodeid << std::endl);

	node* node = load_node(nodeid);

	if (node->isleafnode())
	{
	    const leaf_node *leaf = static_cast<const leaf_node*>(node);

	    assert(nodeid == info.root || !leaf->isunderflow());

	    for(unsigned short slot = 0; slot < leaf->slotuse - 1; ++slot)
	    {
		assert(key_lessequal(leaf->slotkey[slot], leaf->slotkey[slot + 1]));
	    }

	    *minkey = leaf->slotkey[0];
	    *maxkey = leaf->slotkey[leaf->slotuse - 1];

	    vinfo.leaves++;
	    vinfo.itemcount += leaf->slotuse;
	}
	else // !node->isleafnode()
	{
	    const inner_node *inner = static_cast<const inner_node*>(node);
	    vinfo.innernodes++;

	    assert(nodeid == info.root || !inner->isunderflow());

	    for(unsigned short slot = 0; slot < inner->slotuse - 1; ++slot)
	    {
		assert(key_lessequal(inner->slotkey[slot], inner->slotkey[slot + 1]));
	    }

	    for(unsigned short slot = 0; slot <= inner->slotuse; ++slot)
	    {
		pageid_type subnode = inner->childid[slot];
		key_type subminkey = key_type();
		key_type submaxkey = key_type();

		// assert(subnode->level + 1 == inner->level);
		verify_node(subnode, &subminkey, &submaxkey, vinfo);

		BTREE_PRINT("verify subnode " << subnode << ": " << subminkey << " - " << submaxkey << std::endl);

		if (slot == 0)
		    *minkey = subminkey;
		else
		    assert(key_greaterequal(subminkey, inner->slotkey[slot-1]));

		if (slot == inner->slotuse)
		    *maxkey = submaxkey;
		else
		    assert(key_equal(inner->slotkey[slot], submaxkey));

		if (inner->level == 1 && slot < inner->slotuse)
		{
		    // children are leaves and must be linked together in the
		    // correct order
		    pageid_type idleafa = inner->childid[slot];
		    pageid_type idleafb = inner->childid[slot + 1];

		    leaf_node *leafa = load_leaf( idleafa );
		    leaf_node *leafb = load_leaf( idleafb );

		    assert(leafa->nextleaf == idleafb);
		    assert(idleafa == leafb->prevleaf);

		    page_manager.release(idleafa, leafa);
		    page_manager.release(idleafb, leafb);
		}
		if (inner->level == 2 && slot < inner->slotuse)
		{
		    // verify leaf links between the adjacent inner nodes
		    pageid_type idparenta = inner->childid[slot];
		    pageid_type idparentb = inner->childid[slot + 1];

		    inner_node *parenta = load_inner( idparenta );
		    inner_node *parentb = load_inner( idparentb );

		    pageid_type idleafa = parenta->childid[parenta->slotuse];
		    pageid_type idleafb = parentb->childid[0];

		    leaf_node *leafa = load_leaf( idleafa );
		    leaf_node *leafb = load_leaf( idleafb );

		    assert(leafa->nextleaf == idleafb);
		    assert(idleafa == leafb->prevleaf);

		    page_manager.release(idleafa, leafa);
		    page_manager.release(idleafb, leafb);

		    page_manager.release(idparenta, parenta);
		    page_manager.release(idparentb, parentb);
		}
	    }
	}

	page_manager.release(nodeid, node);
    }

    /// Verify the double linked list of leaves.
    void verify_leaflinks()
    {
	pageid_type nodeid = info.headleaf;

	unsigned int count = 0;

	while(nodeid)
	{
	    leaf_node *node = load_leaf(nodeid);

	    assert(node->level == 0);
	    assert(nodeid != info.headleaf || !node->prevleaf);

	    for(unsigned short slot = 0; slot < node->slotuse - 1; ++slot)
	    {
		assert(key_lessequal(node->slotkey[slot], node->slotkey[slot + 1]));
	    }

	    count += node->slotuse;

	    if (node->nextleaf)
	    {
		leaf_node *next = load_leaf(node->nextleaf);

		assert(key_lessequal(node->slotkey[node->slotuse-1], next->slotkey[0]));

		assert(nodeid == next->prevleaf);

		page_manager.release(node->nextleaf, next);
	    }
	    else
	    {
		assert(info.tailleaf == nodeid);
	    }

	    pageid_type nextid = node->nextleaf;
	    page_manager.release(nodeid, node);

	    nodeid = nextid;
	}

	assert(count == size());
    }

private:
    // *** Dump and Restore of B+ Trees

    /// \internal A header for the binary image containing the base properties
    /// of the B+ tree. These properties have to match the current template
    /// instantiation.
    struct dump_header
    {
	/// "stx-btree", just to stop the restore() function from loading garbage
	char 		signature[12];

	/// Currently 0
	unsigned short	version;

	/// sizeof(key_type)
	unsigned short	key_type_size;

	/// sizeof(data_type)
	unsigned short	data_type_size;

	/// Number of slots in the leaves
	unsigned short	leafslots;

	/// Number of slots in the inner nodes
	unsigned short	innerslots;

	/// Allow duplicates
	bool		allow_duplicates;

	/// The item count of the tree
	size_type	itemcount;

	/// Fill the struct with the current B+ tree's properties, itemcount is
	/// not filled.
	inline void fill()
	{
	    // don't want to include string.h just for this signature
	    *reinterpret_cast<unsigned int*>(signature+0) = 0x2d787473;
	    *reinterpret_cast<unsigned int*>(signature+4) = 0x65727462;
	    *reinterpret_cast<unsigned int*>(signature+8) = 0x00000065;

	    version = 0;
	    key_type_size = sizeof(typename btree_self::key_type);
	    data_type_size = sizeof(typename btree_self::data_type);
	    leafslots = btree_self::leafslotmax;
	    innerslots = btree_self::innerslotmax;
	    allow_duplicates = btree_self::allow_duplicates;
	}

	/// Returns true if the headers have the same vital properties
	inline bool same(const struct dump_header &o) const
	{
	    return (*reinterpret_cast<const unsigned int*>(signature+0) ==
		    *reinterpret_cast<const unsigned int*>(o.signature+0))
		&& (*reinterpret_cast<const unsigned int*>(signature+4) ==
		    *reinterpret_cast<const unsigned int*>(o.signature+4))
		&& (*reinterpret_cast<const unsigned int*>(signature+8) ==
		    *reinterpret_cast<const unsigned int*>(o.signature+8))

		&& (version == o.version)
		&& (key_type_size == o.key_type_size)
		&& (data_type_size == o.data_type_size)
		&& (leafslots == o.leafslots)
		&& (innerslots == o.innerslots)
		&& (allow_duplicates == o.allow_duplicates);
	}
    };

public:
#if 0

    /// Dump the contents of the B+ tree out onto an ostream as a binary
    /// image. The image contains memory pointers which will be fixed when the
    /// image is restored. For this to work your key_type and data_type must be
    /// integral types and contain no pointers or references.
    void dump(std::ostream &os) const
    {
	struct dump_header header;
	header.fill();
	header.itemcount = size();

	os.write(reinterpret_cast<char*>(&header), sizeof(header));

	if (info.root)
	    dump_node(os, info.root);
    }

    /// Restore a binary image of a dumped B+ tree from an istream. The B+ tree
    /// pointers are fixed using the dump order. For dump and restore to work
    /// your key_type and data_type must be integral types and contain no
    /// pointers or references. Returns true if the restore was successful.
    bool restore(std::istream &is)
    {
	struct dump_header fileheader;
	is.read(reinterpret_cast<char*>(&fileheader), sizeof(fileheader));
	if (!is.good()) return false;

	struct dump_header myheader;
	myheader.fill();
	myheader.itemcount = fileheader.itemcount;

	if (!myheader.same(fileheader))
	{
	    BTREE_PRINT("btree::restore: file header does not match instantiation signature." << std::endl);
	    return false;
	}

	clear();

	if (fileheader.itemcount > 0)
	{
	    info.root = restore_node(is);
	    if (info.root == NULL) return false;

	    info.itemcount = fileheader.itemcount;
	}

	if (debug) print();
	if (selfverify) verify();

	return true;
    }

private:

    /// Recursively descend down the tree and dump each node in a precise order
    void dump_node(std::ostream &os, const node* n) const
    {
	BTREE_PRINT("dump_node " << n << std::endl);

	if (n->isleafnode())
	{
	    const leaf_node *leaf = static_cast<const leaf_node*>(n);

	    os.write(reinterpret_cast<const char*>(leaf), sizeof(*leaf));
	}
	else // !n->isleafnode()
	{
	    const inner_node *inner = static_cast<const inner_node*>(n);

	    os.write(reinterpret_cast<const char*>(inner), sizeof(*inner));

	    for(unsigned short slot = 0; slot <= inner->slotuse; ++slot)
	    {
		const node *subnode = inner->childid[slot];
		
		dump_node(os, subnode);
	    }
	}
    }

    /// Read the dump image and construct a tree from the node order in the
    /// serialization.
    node* restore_node(std::istream &is)
    {
	union {
	    node 	top;
	    leaf_node 	leaf;
	    inner_node 	inner;
	} nu;

	// first read only the top of the node
	is.read(reinterpret_cast<char*>(&nu.top), sizeof(nu.top));
	if (!is.good()) return NULL;

	if (nu.top.isleafnode())
	{
	    // read remaining data of leaf node
	    is.read(reinterpret_cast<char*>(&nu.leaf) + sizeof(nu.top), sizeof(nu.leaf) - sizeof(nu.top));
	    if (!is.good()) return NULL;
	    
	    leaf_node *newleaf = allocate_leaf();

	    // copy over all data, the leaf nodes contain only their double linked list pointers
	    *newleaf = nu.leaf;

	    // reconstruct the linked list from the order in the file
	    if (info.headleaf == NULL) {
		BTREE_ASSERT(newleaf->prevleaf == NULL);
		info.headleaf = info.tailleaf = newleaf;
	    }
	    else {
		newleaf->prevleaf = info.tailleaf;
		info.tailleaf->nextleaf = newleaf;
		info.tailleaf = newleaf;
	    }

	    return newleaf;
	}
	else
	{
	    // read remaining data of inner node
	    is.read(reinterpret_cast<char*>(&nu.inner) + sizeof(nu.top), sizeof(nu.inner) - sizeof(nu.top));
	    if (!is.good()) return NULL;

	    inner_node *newinner = allocate_inner(0);

	    // copy over all data, the inner nodes contain only pointers to their children
	    *newinner = nu.inner;

	    for(unsigned short slot = 0; slot <= newinner->slotuse; ++slot)
	    {
		newinner->childid[slot] = restore_node(is);
	    }

	    return newinner;
	}
    }
#endif
};

} // namespace stx

#endif // _STX_BTREE_H_
