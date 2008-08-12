// $Id$

/*
 * STX B+ Tree Template Classes v0.8.1
 * Copyright (C) 2008 Timo Bingmann
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

#include <fstream>
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

const int randseed = 34234235;

/// b+ tree slot range to test
const int min_nodeslots = 4;
const int max_nodeslots = 256;

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

/// Test the multiset red-black tree from STL (only insert)
struct test_set_insert
{
    void operator()(unsigned int insertnum)
    {
	typedef std::multiset<unsigned int> multiset_type;
	multiset_type set;

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.insert( rand() );

	assert( set.size() == insertnum );
    }
};

/// Test the B+ tree with a specific leaf/inner slots (only insert)
template <int _slots>
struct test_btree_insert
{
    void operator()(unsigned int insertnum)
    {
	typedef stx::btree_multiset<unsigned int, std::less<unsigned int>,
	    struct btree_traits_speed<_slots, _slots> > btree_type;

	btree_type bt;

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.insert(rand());

	assert( bt.size() == insertnum );
    }
};

/// Test the multiset red-black tree from STL (insert, find and delete)
struct test_set_insert_find_delete
{
    void operator()(unsigned int insertnum)
    {
	typedef std::multiset<unsigned int> multiset_type;
	multiset_type set;

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.insert( rand() );

	assert( set.size() == insertnum );

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.find(rand());

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.erase( set.find(rand()) );

	assert( set.empty() );
    }
};

/// Test the B+ tree with a specific leaf/inner slots (insert, find and delete)
template <int Slots>
struct test_btree_insert_find_delete
{
    void operator()(unsigned int insertnum)
    {
	typedef stx::btree_multiset<unsigned int, std::less<unsigned int>,
	    struct btree_traits_speed<Slots, Slots> > btree_type;

	btree_type bt;

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.insert(rand());

	assert( bt.size() == insertnum );

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.exists(rand());

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.erase_one(rand());

	assert(bt.empty());
    }
};

unsigned int repeatuntil;

/// Repeat (short) tests until enough time elapsed and divide by the runs.
template <typename functional>
void testrunner_loop(std::ostream& os, unsigned int insertnum)
{
    unsigned int runs = 0;
    double ts1, ts2;

    do
    {
	runs = 0;

	ts1 = timestamp();

	for(unsigned int totaltests = 0; totaltests <= repeatuntil; totaltests += insertnum)
	{
	    functional()(insertnum);
	    ++runs;
	}

	ts2 = timestamp();

	std::cerr << "Insert " << insertnum << " repeat " << (repeatuntil / insertnum) << " time " << (ts2 - ts1) << "\n";

	if ((ts2 - ts1) < 1.0) repeatuntil *= 2;
    }
    while ((ts2 - ts1) < 1.0);

    os << std::fixed << std::setprecision(10) << ((ts2 - ts1) / runs) << " " << std::flush;
}

// Template magic to emulate a for_each slots. These templates will roll-out
// btree instantiations for each of the Low-High leaf/inner slot numbers.
template< template<int Slots> class functional, int Low, int High>
struct btree_range
{
    inline void operator()(std::ostream& os, unsigned int insertnum)
    {
        testrunner_loop< functional<Low> >(os, insertnum);
        btree_range<functional, Low+1, High>()(os, insertnum);
    }
};

template< template<int Slots> class functional, int Low>
struct btree_range<functional, Low, Low>
{
    inline void operator()(std::ostream& os, unsigned int insertnum)
    {
        testrunner_loop< functional<Low> >(os, insertnum);
    }
};

/// Speed test it!
int main()
{
    { // speed test only insertion

	std::ofstream os("speed-insert.txt");

	repeatuntil = mininsertnum;

	for(unsigned int insertnum = mininsertnum; insertnum <= maxinsertnum; insertnum *= 2)
	{
	    std::cerr << "Insert " << insertnum << "\n";

	    os << insertnum << " " << std::flush;

	    testrunner_loop<test_set_insert>(os, insertnum);

	    btree_range<test_btree_insert, min_nodeslots, max_nodeslots>()(os, insertnum);

	    os << "\n" << std::flush;
	}
    }

    { // speed test insert, find and delete

	std::ofstream os("speed-all.txt");
	
	repeatuntil = mininsertnum;

	for(unsigned int insertnum = mininsertnum; insertnum <= maxinsertnum; insertnum *= 2)
	{
	    std::cerr << "Insert, Find, Delete " << insertnum << "\n";

	    os << insertnum << " " << std::flush;

	    testrunner_loop<test_set_insert_find_delete>(os, insertnum);

	    btree_range<test_btree_insert_find_delete, min_nodeslots, max_nodeslots>()(os, insertnum);

	    os << "\n" << std::flush;
	}
    }
}
