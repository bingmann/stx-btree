// $Id$

/*
 * STX B+ Tree Template Classes v0.8.6
 * Copyright (C) 2008-2011 Timo Bingmann
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
#include <ext/hash_set>
#include <stx/btree_multiset.h>

#include <assert.h>

// *** Settings

/// starting number of items to insert
const unsigned int mininsertnum = 125;

/// maximum number of items to insert
const unsigned int maxinsertnum = 1024000 * 32;

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
struct Test_Set_Insert
{
    typedef std::multiset<unsigned int> multiset_type;

    Test_Set_Insert(unsigned int)
    {
    }

    void run(unsigned int insertnum)
    {
	multiset_type set;

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.insert( rand() );

	assert( set.size() == insertnum );
    }
};

/// Test the multiset hash from STL (only insert)
struct Test_Hashset_Insert
{
    typedef __gnu_cxx::hash_multiset<unsigned int> multiset_type;

    Test_Hashset_Insert(unsigned int)
    {
    }

    void run(unsigned int insertnum)
    {
	multiset_type set;

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.insert( rand() );

	assert( set.size() == insertnum );
    }
};

/// Test the B+ tree with a specific leaf/inner slots (only insert)
template <int _slots>
struct Test_Btree_Insert
{
    typedef stx::btree_multiset<unsigned int, std::less<unsigned int>,
				struct btree_traits_speed<_slots, _slots> > btree_type;

    Test_Btree_Insert(unsigned int)
    {
    }

    void run(unsigned int insertnum)
    {
	btree_type bt;

	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.insert( rand() );

	assert( bt.size() == insertnum );
    }
};

/// Test the multiset red-black tree from STL (insert, find and delete)
struct Test_Set_InsertFindDelete
{
    typedef std::multiset<unsigned int> multiset_type;

    Test_Set_InsertFindDelete(unsigned int)
    {
    }

    void run(unsigned int insertnum)
    {
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

/// Test the multiset hash from STL (insert, find and delete)
struct Test_Hashset_InsertFindDelete
{
    typedef __gnu_cxx::hash_multiset<unsigned int> multiset_type;

    Test_Hashset_InsertFindDelete(unsigned int)
    {
    }

    void run(unsigned int insertnum)
    {
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
struct Test_Btree_InsertFindDelete
{
    typedef stx::btree_multiset<unsigned int, std::less<unsigned int>,
				struct btree_traits_speed<Slots, Slots> > btree_type;

    Test_Btree_InsertFindDelete(unsigned int)
    {
    }

    void run(unsigned int insertnum)
    {
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

/// Test the multiset red-black tree from STL (find only)
struct Test_Set_Find
{
    typedef std::multiset<unsigned int> multiset_type;

    multiset_type set;

    Test_Set_Find(unsigned int insertnum)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.insert( rand() );

	assert( set.size() == insertnum );
    }

    void run(unsigned int insertnum)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.find(rand());
    }
};

/// Test the multiset hash from STL (find only)
struct Test_Hashset_Find
{
    typedef __gnu_cxx::hash_multiset<unsigned int> multiset_type;

    multiset_type set;

    Test_Hashset_Find(unsigned int insertnum)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.insert( rand() );

	assert( set.size() == insertnum );
    }

    void run(unsigned int insertnum)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    set.find(rand());
    }
};

/// Test the B+ tree with a specific leaf/inner slots (find only)
template <int Slots>
struct Test_Btree_Find
{
    typedef stx::btree_multiset<unsigned int, std::less<unsigned int>,
				struct btree_traits_speed<Slots, Slots> > btree_type;

    btree_type bt;

    Test_Btree_Find(unsigned int insertnum)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.insert(rand());

	assert( bt.size() == insertnum );
    }

    void run(unsigned int insertnum)
    {
	srand(randseed);
	for(unsigned int i = 0; i < insertnum; i++)
	    bt.exists(rand());
    }
};

unsigned int repeatuntil;

/// Repeat (short) tests until enough time elapsed and divide by the runs.
template <typename TestClass>
void testrunner_loop(std::ostream& os, unsigned int insertnum)
{
    unsigned int runs = 0;
    double ts1, ts2;

    do
    {
	runs = 0;	// count repetition of timed tests

	{
	    TestClass test(insertnum);	// initialize test structures

	    ts1 = timestamp();

	    for(unsigned int totaltests = 0; totaltests <= repeatuntil; totaltests += insertnum)
	    {
		test.run(insertnum);	// run timed test procedure
		++runs;
	    }

	    ts2 = timestamp();
	}

	std::cerr << "Insert " << insertnum << " repeat " << (repeatuntil / insertnum) << " time " << (ts2 - ts1) << "\n";

	// discard and repeat if test took less than one second.
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
        btree_range<functional, Low+2, High>()(os, insertnum);
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

/// Speed test them!
int main()
{
    { // speed test only insertion

	std::ofstream os("speed-insert.txt");

	repeatuntil = mininsertnum;

	for(unsigned int insertnum = mininsertnum; insertnum <= maxinsertnum; insertnum *= 2)
	{
	    std::cerr << "Insert " << insertnum << "\n";

	    os << insertnum << " " << std::flush;

	    testrunner_loop<Test_Set_Insert>(os, insertnum);

	    testrunner_loop<Test_Hashset_Insert>(os, insertnum);

	    btree_range<Test_Btree_Insert, min_nodeslots, max_nodeslots>()(os, insertnum);

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

	    testrunner_loop<Test_Set_InsertFindDelete>(os, insertnum);

	    testrunner_loop<Test_Hashset_InsertFindDelete>(os, insertnum);

	    btree_range<Test_Btree_InsertFindDelete, min_nodeslots, max_nodeslots>()(os, insertnum);

	    os << "\n" << std::flush;
	}
    }

    { // speed test find only

	std::ofstream os("speed-find.txt");
	
	repeatuntil = mininsertnum;

	for(unsigned int insertnum = mininsertnum; insertnum <= maxinsertnum; insertnum *= 2)
	{
	    std::cerr << "Find " << insertnum << "\n";

	    os << insertnum << " " << std::flush;

	    testrunner_loop<Test_Set_Find>(os, insertnum);

	    testrunner_loop<Test_Hashset_Find>(os, insertnum);

	    btree_range<Test_Btree_Find, min_nodeslots, max_nodeslots>()(os, insertnum);

	    os << "\n" << std::flush;
	}
    }
}
