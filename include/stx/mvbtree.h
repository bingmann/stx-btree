#define MAX_SIZE 512                  // The capacity of each node(block).
#define D 0.20f                       // The minimum number of live elements per node
#define EPSILON 0.50f                 // The fraction of live number 


// *** Required Headers from the STL
#include "btree.h" 
// #include <algorithm>
// #include <functional>
// #include <istream>
// #include <ostream>
// #include <iostream>
// #include <memory>


// *** Debugging Macros
#ifdef MVBTREE_DEBUG
#endif

/// The maximum of a and b. Used in some compile-time formulas.
#define MVBTREE_MAX(a, b)          ((a) < (b) ? (b) : (a))

/// STX - Some Template Extensions namespace. Notice that B+-Tree is also in STX.
namespace stx {

/** Generates default traits for a B+ tree used as a set. It estimates leaf and
 * inner node sizes by assuming a cache line size of MAX_SIZE bytes. */
template <typename _Key>
class mvbtree_default_set_traits
{
public:
    /// Number of slots in each leaf of the tree. Estimated so that each node
    /// has a size of about MAX_SIZE bytes.
    static const int leaf_slots = MVBTREE_MAX(8, MAX_SIZE / (sizeof(_Key)));

    /// Number of slots in each inner node of the tree. Estimated so that each node
    /// has a size of about MAX_SIZE bytes.
    static const int inner_slots = MVBTREE_MAX(8, MAX_SIZE / (sizeof(_Key) + sizeof(void*)));
};



/** Generates default traits for a B+ tree used as a map. It estimates leaf and
 * inner node sizes by assuming a cache line size of 256 bytes. */
template <typename _Key, typename _Data>
class mvbtree_default_map_traits
{
public:
    /// Number of slots in each leaf of the tree. Estimated so that each node
    /// has a size of about MAX_SIZE bytes.
    static const int leaf_slots = MVBTREE_MAX(8, MAX_SIZE / (sizeof(_Key)));

    /// Number of slots in each inner node of the tree. Estimated so that each node
    /// has a size of about MAX_SIZE bytes.
    static const int inner_slots = MVBTREE_MAX(8, MAX_SIZE / (sizeof(_Key) + sizeof(void*)));
};



/** @brief Basic class implementing a base MVB tree data structure in memory.
 */
template <typename _Time, typename _Key, typename _Data,
          typename _Value = std::pair<_Key, _Data>,
          typename _TimeCompare = std::less<_Time>,
          typename _KeyCompare = std::less<_Key>,
          typename _Traits = mvbtree_default_map_traits<_Key, _Data>,
          bool _Duplicates = false,
          typename _Alloc = std::allocator<_Value>,
          bool _UsedAsSet = false>
class mvbtree
{
public:
    // *** Template Parameter Types

    /// First template parameter: The timestamp type of the MVB TREE.
    typedef _Time time_type;

    /// First template parameter: The key type of the MVB tree. 
    // This is stored in inner nodes and leaves.
    typedef _Key key_type;

    /// Second template parameter: The data type associated with each
    /// key. Stored in the leaves
    typedef _Data data_type;

    /// Third template parameter: Composition pair of key and data types, this
    /// is required by the STL standard. The B+ tree does not store key and
    /// data together. If value_type == key_type then the B+ tree implements a
    /// set.
    typedef _Value value_type;

    /// Time comparison function object
    typedef _TimeCompare time_compare;

    /// Fourth template parameter: Key comparison function object
    typedef _KeyCompare key_compare; 
    

    /// Fifth template parameter: Traits object used to define more parameters
    /// of the B+ tree
    typedef _Traits traits;

    /// Sixth template parameter: Allow duplicate keys in the B+ tree. Used to
    /// implement multiset and multimap.
    static const bool allow_duplicates = _Duplicates;

    /// Seventh template parameter: STL allocator for tree nodes
    typedef _Alloc allocator_type;

    /// Eighth template parameter: boolean indicator whether the btree is used
    /// as a set. In this case all operations on the data arrays are
    /// omitted. This flag is kind of hacky, but required because
    /// sizeof(empty_struct) = 1 due to the C standard. Without the flag, lots
    /// of superfluous copying would occur.
    static const bool used_as_set = _UsedAsSet;


public:
    // *** Constructed Types

    /// Typedef of our own type
    typedef mvbtree<time_type, key_type, data_type, value_type, time_compare, key_compare, 
                  traits, allow_duplicates, allocator_type, used_as_set> self_type;

    /// Size type used to count keys
    typedef size_t size_type;

    /// The pair of key_type and data_type, this may be different from
    /// value_type.
    typedef std::pair<key_type, data_type> pair_type;

    /// unique id of nodes
    typedef int64_t id_type;

public:
    // *** Static Constant Options and Values of the MVB Tree

    /// Base MVB tree parameter: The number of key/data slots in each leaf
    /// Block Overflow: the maximum entries allowed in a node
    static const unsigned short max_leaf_slot = traits::leaf_slots;
    /// Weak Version Underflow: the minimum alive entries allowed in a node
    static const unsigned short min_alive_leaf_slot = (max_leaf_slot * D);
    /// Strong Version Overflow: the maximum alive entries allowed in a node
    static const unsigned short strong_max_alive_leaf_slot = (max_leaf_slot * (1.0f - D * EPSILON));
    /// Strong Version Underflow: the minimum alive entries allowed in a node
    static const unsigned short strong_min_alive_leaf_slot = (max_leaf_slot * D * (1.0f + EPSILON));

    /// Base MVB tree parameter: The number of slots in each inner node
    /// Block Overflow: the maximum entries allowed in a node
    static const unsigned short max_inner_slot = traits::inner_slots;
    /// Weak Version Underflow: the minimum alive entries allowed in a node
    static const unsigned short min_alive_inner_slot = (max_inner_slot * D);
    /// Strong Version Overflow: the maximum alive entries allowed in a node
    static const unsigned short strong_max_alive_inner_slot = (max_inner_slot * (1.0f - D * EPSILON));
    /// Strong Version Underflow: the minimum alive entries allowed in a node
    static const unsigned short strong_min_alive_inner_slot = (max_inner_slot * D * (1.0f + EPSILON));

private:
    // *** Crucial Classes for In-Memory the regions of key range and time interval
    struct Box
    {
        /// The key stored in this entry/node
        /// -- If this is an entry, the key is the stored info
        /// -- If this is a node, the key is the minimum key of this node
        key_type key;

        /// True if an entry/node is alive, which means that end_time is empty
        bool is_alive();

        /// [starttime, endtime)
        time_type start_time;
        
        /// [starttime, endtime)
        time_type end_time;        
    };

private:
    // *** Entries Classes for In-Memory Entries (Todo)

    /// The header structure of each entry in-memory. 
    /// This structure is extended by inner_entry or leaf_entry.    
    struct Entry
    {
        /// The key and time info inside it.
        Box box;  
    };

    /// Extended structure of a inner entry in-memory. 
    struct InnerEntry : public Entry
    {
        /// this is the pointer in the inner entry that points to the child node
        // Node* ptr_child_node;
    };

    /// Extended structure of a leaf entry in memory.
    struct LeafEntry : public Entry
    {
        /// The stored key
        key_type key;

        /// The stored payload
        data_type payload;
    };


private:
    // *** Node Classes for In-Memory Nodes (Todo)

    /// The header structure of each node in-memory. 
    /// This structure is extended by inner_node or leaf_node.
    struct Node
    {
        /// Level in the b-tree, if level == 0 -> leaf node
        /// To get the height of a tree, m_root->level + 1 
        unsigned short level;

        /// Number of key slotuse use, so number of valid children or data pointers
        unsigned short slot_use;

        /// Delayed initialisation of constructed node
        inline Node(const unsigned short l, const unsigned short s = 0)
            : level(l), slot_use(s)
        { }

        /// True if this is a leaf node
        inline bool is_leaf() const
        {
            return (level == 0);
        }

        /// unique id of this node
        id_type id;

        /// The time interval and the minimum key of this node.
        Box box;  
    };

    /// Extended structure of a inner node in-memory. 
    /// The node contains only keys and no data items.
    // struct inner_node : public stx::btree::inner_node
    // {
    // };
    struct InnerNode : public Node
    {
    };


    /// Extended structure of a leaf node in memory. Contains pairs of keys and
    /// data items. Key and data slots are kept in separate arrays, because the
    /// key array is traversed very often compared to accessing the data items.
    struct LeafNode : public Node
    {
    };


public:
    // *** Iterators and Reverse Iterators

    class iterator;
    class const_iterator;
    class reverse_iterator;
    class const_reverse_iterator;

public:
    // *** Small Statistics Structure

    /** A small struct containing basic statistics about the B+ tree. It can be
     * fetched using get_stats(). */
    struct tree_stats
    {
    };

private:
    // *** Tree Object Data Members (Todo)

    /// the additional B+-Tree to guide all the historical root nodes
    /// all the root nodes are the leaf entries in roots
    // stx::btree<key_type, data_type> m_roots;

    /// Pointer to the root node, either leaf or inner node
    Node* m_root;

    /// Other small statistics about the tree
    tree_stats m_stats;

    /// Time comparison object.
    time_compare m_time_less;

    /// Key comparison object. 
    key_compare m_key_less;

    /// Memory allocator.
    allocator_type m_allocator;


public:
    // *** Constructors and Destructor (Todo)

    /// Default constructor initializing an empty B+ tree with the standard key
    /// comparison function
    // explicit inline mvbtree(const allocator_type& alloc = allocator_type())
    //     : m_roots(NULL), m_root(NULL), m_allocator(alloc)
    // { }
    explicit inline mvbtree(const allocator_type& alloc = allocator_type())
        : m_root(NULL), m_allocator(alloc)
    { }


private:
    // *** Convenient Key Comparison Functions Generated From key_less

    /// True if a < b ? "constructed" from m_key_less()
    inline bool key_less(const key_type& a, const key_type& b) const
    {
        return m_key_less(a, b);
    }

    /// True if a <= b ? constructed from key_less()
    inline bool key_lessequal(const key_type& a, const key_type& b) const
    {
        return !m_key_less(b, a);
    }

    /// True if a > b ? constructed from key_less()
    inline bool key_greater(const key_type& a, const key_type& b) const
    {
        return m_key_less(b, a);
    }

    /// True if a >= b ? constructed from key_less()
    inline bool key_greaterequal(const key_type& a, const key_type& b) const
    {
        return !m_key_less(a, b);
    }

    /// True if a == b ? constructed from key_less(). This requires the <
    /// relation to be a total order, otherwise the B+ tree cannot be sorted.
    inline bool key_equal(const key_type& a, const key_type& b) const
    {
        return !m_key_less(a, b) && !m_key_less(b, a);
    }

private:
    // *** Convenient Time Comparison Functions Generated From time_less

    /// True if a < b ? "constructed" from m_time_less()
    inline bool time_less(const time_type& a, const time_type& b) const
    {
        return m_time_less(a, b);
    }

    /// True if a <= b ? constructed from time_less()
    inline bool time_lessequal(const time_type& a, const time_type& b) const
    {
        return !m_time_less(b, a);
    }

    /// True if a > b ? constructed from time_less()
    inline bool time_greater(const time_type& a, const time_type& b) const
    {
        return m_time_less(b, a);
    }

    /// True if a >= b ? constructed from time_less()
    inline bool time_greaterequal(const time_type& a, const time_type& b) const
    {
        return !m_time_less(a, b);
    }

    /// True if a == b ? constructed from time_less(). This requires the <
    /// relation to be a total order, otherwise the B+ tree cannot be sorted.
    inline bool time_equal(const time_type& a, const time_type& b) const
    {
        return !m_time_less(a, b) && !m_time_less(b, a);
    }

    

public:
    // *** Public Insertion Functions (Todo)



private:
    // *** Private Insertion Functions (Todo)

    /// Start the insertion descent at the current root and handle root
    /// splits. Returns true if the item was inserted
    /// insert_start
    /// 
    // void Node::insertData(TimeRegion& mbr, id_type id,, Node* oldVersion, std::stack<id_type>& pathBuffer)

    /**
     * @brief Insert an item into the B+ tree.
     *
     * Descend down the nodes to a leaf, insert the key/data pair in a free
     * slot. If the node overflows, then it must be split and the new split
     * node inserted into the parent. Unroll / this splitting up to the root.
    */
   /// insert_descend


    /// Split up a leaf node into two equally-filled sibling leaves. Returns
    /// the new nodes and it's insertion key in the two parameters.
    /// split_leaf_node

    
    /// Split up an inner node into two equally-filled sibling nodes. Returns
    /// the new nodes and it's insertion key in the two parameters. Requires
    /// the slot of the item will be inserted, so the nodes will be the same
    /// size after the insert.
    /// split_inner_node


    

};

}