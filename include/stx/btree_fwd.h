// $Id$
/** \file btree_fwd.h
 * Contains forward declarations of the B+ tree implementation classes.
 */

#ifndef _STX_BTREE_FWD_H_
#define _STX_BTREE_FWD_H_

namespace stx {

// *** Default Traits

template <typename _Key>
struct btree_default_set_traits;

template <typename _Key, typename _Data>
struct btree_default_map_traits;

// *** Base Class

template <typename _Key, typename _Data,
	  typename _Value,
	  typename _Compare,
	  typename _Traits,
	  bool _Duplicates>
class btree;

template <typename _Key,
	  typename _Compare,
	  typename _Traits>
class btree_set;

template <typename _Key,
	  typename _Compare,
	  typename _Traits>
class btree_multiset;

template <typename _Key, typename _Data,
	  typename _Compare,
	  typename _Traits>
class btree_map;

template <typename _Key, typename _Data,
	  typename _Compare,
	  typename _Traits>
class btree_multimap;

} // namespace stx

#endif // _STX_BTREE_FWD_H_
