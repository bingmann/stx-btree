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

#include <string>
#include <stdlib.h>
#include <sys/time.h>

#include <iostream>
#include <iomanip>

#include <set>
#include <stx/btree_multiset.h>

#include <assert.h>

// *** Settings

/// starting number of items to insert
const unsigned int mininsertnum = 125;

/// maximum number of items to insert
const unsigned int maxinsertnum = 1024000 * 4;

/// repeat each test until this number of items was inserted in total (loop
/// over very short tests)
const unsigned int repeatinsertuntil = maxinsertnum * 2;

const int randseed = 34234235;

const int min_nodeslots = 4;
const int max_nodeslots = 256;

const bool with_find = false;
const bool with_erase = false;

/// Time is measured using gettimeofday()
inline double timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 0.000001;
}

/// Traits used for the speed tests, BTREE_DEBUG is not defined.
template <int _innerslots, int _leafslots>
struct btree_traits_speed
{
    static const bool   selfverify = false;
    static const bool   debug = false;

    static const int    leafslots = _innerslots;
    static const int    innerslots = _leafslots;
};

/// Test the multiset red-black tree from STL
void test_set(unsigned int insertnum)
{
    typedef std::multiset<unsigned int> multiset_type;
    multiset_type set;

    srand(randseed);
    for(unsigned int i = 0; i < insertnum; i++)
	set.insert( rand() );

    assert( set.size() == insertnum );

    if (with_find)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.find(rand());
    }

    if (with_erase)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.erase( set.find(rand()) );

	assert( set.empty() );
    }
}

/// Test the B+ tree with a specific leaf/inner slots
template <int _slots>
void test_btree(unsigned int insertnum)
{
    typedef stx::btree_multiset<unsigned int, std::less<unsigned int>,
	struct btree_traits_speed<_slots, _slots> > btree_type;

    btree_type bt;

    srand(randseed);
    for(unsigned int i = 0; i < insertnum; i++)
	bt.insert(rand());

    assert( bt.size() == insertnum );

    if (with_find)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.exists(rand());
    }

    if (with_erase)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.erase_one(rand());

	assert(bt.empty());
    }
}

/// Repeat (short) tests until enough time elapsed and divide by the runs.
template <void (*func)(unsigned int)>
void testrunner_loop(unsigned int insertnum)
{
    unsigned int runs = 0;

    double ts1 = timestamp();

    for(unsigned int totaltests = 0; totaltests <= repeatinsertuntil; totaltests += insertnum)
    {
	func(insertnum);
	++runs;
    }

    double ts2 = timestamp();

    std::cout << std::fixed << std::setprecision(10) << ((ts2 - ts1) / runs) << " " << std::flush;
}

// Template magic to emulate a for_each slots. These templates will roll-out
// btree instantiations for each of the Low-High leaf/inner slot numbers.
template<int Low, int High>
struct btree_range
{
    inline void operator()(unsigned int insertnum)
    {
	testrunner_loop< test_btree<Low> >(insertnum);
	btree_range<Low+1, High>()(insertnum);
    }
};

template<int Low>
struct btree_range<Low, Low>
{
    inline void operator()(unsigned int insertnum) {
	testrunner_loop< test_btree<Low> >(insertnum);
    }
};

/// Speed test it!
int main()
{
    for(unsigned int insertnum = mininsertnum; insertnum <= maxinsertnum; insertnum *= 2)
    {
	std::cout << insertnum << " " << std::flush;

	testrunner_loop<test_set>(insertnum);

	btree_range<min_nodeslots, max_nodeslots>()(insertnum);

	std::cout << "\n" << std::flush;
    }
}
