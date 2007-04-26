// $Id$

#include <stx/btree_fwd.h>

#include <stx/btree_map.h>
#include <stx/btree_multimap.h>
#include <stx/btree_set.h>
#include <stx/btree_multiset.h>

// forced instantiation

template class stx::btree_set<unsigned int>;
template class stx::btree_map<int, double>;
template class stx::btree_multiset<int>;
template class stx::btree_multimap<int, int>;
