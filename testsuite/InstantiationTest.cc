// $Id$

/*
 * STX B+ Tree Template Classes v0.8
 * Copyright (C) 2007 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stx/btree_map.h>
#include <stx/btree_multimap.h>
#include <stx/btree_set.h>
#include <stx/btree_multiset.h>

// forced instantiation

template class stx::btree_set<unsigned int>;
template class stx::btree_map<int, double>;
template class stx::btree_multiset<int>;
template class stx::btree_multimap<int, int>;
