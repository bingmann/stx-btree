// $Id$

/*
 * STX B+ Tree Template Classes v0.7
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

#include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>
#include <time.h>

#include <stx/btree_multiset.h>
#include <set>

class LargeTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( LargeTest );
    CPPUNIT_TEST(test_3200_10);
    CPPUNIT_TEST(test_320_1000);
    CPPUNIT_TEST(test_320_10000);
    CPPUNIT_TEST(test_sequence);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
	static const bool	selfverify = true;
	static const bool	debug = false;

	static const int 	leafslots = 8;
	static const int	innerslots = 8;
    };

    void test_multi(const unsigned int insnum, const unsigned int modulo)
    {
	typedef stx::btree_multiset<unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	btree_type bt;

	typedef std::multiset<unsigned int> multiset_type;
	multiset_type set;

	// *** insert
	srand(34234235);
	for(unsigned int i = 0; i < insnum; i++)
	{
	    unsigned int k = rand() % modulo;

	    CPPUNIT_ASSERT( bt.size() == set.size() );
	    bt.insert(k);
	    set.insert(k);
	    CPPUNIT_ASSERT( bt.count(k) == set.count(k) );

	    CPPUNIT_ASSERT( bt.size() == set.size() );
	}

	CPPUNIT_ASSERT( bt.size() == insnum );

	// *** iterate
	btree_type::iterator bi = bt.begin();
	multiset_type::const_iterator si = set.begin();
	for(; bi != bt.end() && si != set.end(); ++bi, ++si)
	{
	    CPPUNIT_ASSERT( *si == bi.key() );
	}
	CPPUNIT_ASSERT( bi == bt.end() );
	CPPUNIT_ASSERT( si == set.end() );

	// *** existance
	srand(34234235);
	for(unsigned int i = 0; i < insnum; i++)
	{
	    unsigned int k = rand() % modulo;

	    CPPUNIT_ASSERT( bt.exists(k) );
	}

	// *** counting
	srand(34234235);
	for(unsigned int i = 0; i < insnum; i++)
	{
	    unsigned int k = rand() % modulo;

	    CPPUNIT_ASSERT( bt.count(k) == set.count(k) );
	}

	// *** deletion
	srand(34234235);
	for(unsigned int i = 0; i < insnum; i++)
	{
	    unsigned int k = rand() % modulo;

	    if (set.find(k) != set.end())
	    {
		CPPUNIT_ASSERT( bt.size() == set.size() );

		CPPUNIT_ASSERT( bt.exists(k) );
		CPPUNIT_ASSERT( bt.erase_one(k) );
		set.erase( set.find(k) );

		CPPUNIT_ASSERT( bt.size() == set.size() );
	    }
	}

	CPPUNIT_ASSERT( bt.empty() );
	CPPUNIT_ASSERT( set.empty() );
    }

    void test_3200_10()
    {
	test_multi(3200, 10);
    }

    void test_320_1000()
    {
	test_multi(320, 1000);
    }

    void test_320_10000()
    {
	test_multi(320, 10000);
    }

    void test_sequence()
    {
	typedef stx::btree_multiset<unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	btree_type bt;

	const unsigned int insnum = 10000;

	typedef std::multiset<unsigned int> multiset_type;
	multiset_type set;

	// *** insert
	srand(34234235);
	for(unsigned int i = 0; i < insnum; i++)
	{
	    unsigned int k = i;

	    CPPUNIT_ASSERT( bt.size() == set.size() );
	    bt.insert(k);
	    set.insert(k);
	    CPPUNIT_ASSERT( bt.count(k) == set.count(k) );

	    CPPUNIT_ASSERT( bt.size() == set.size() );
	}

	CPPUNIT_ASSERT( bt.size() == insnum );

	// *** iterate
	btree_type::iterator bi = bt.begin();
	multiset_type::const_iterator si = set.begin();
	for(; bi != bt.end() && si != set.end(); ++bi, ++si)
	{
	    CPPUNIT_ASSERT( *si == bi.key() );
	}
	CPPUNIT_ASSERT( bi == bt.end() );
	CPPUNIT_ASSERT( si == set.end() );

	// *** existance
	srand(34234235);
	for(unsigned int i = 0; i < insnum; i++)
	{
	    unsigned int k = i;

	    CPPUNIT_ASSERT( bt.exists(k) );
	}

	// *** counting
	srand(34234235);
	for(unsigned int i = 0; i < insnum; i++)
	{
	    unsigned int k = i;

	    CPPUNIT_ASSERT( bt.count(k) == set.count(k) );
	}

	// *** deletion
	srand(34234235);
	for(unsigned int i = 0; i < insnum; i++)
	{
	    unsigned int k = i;

	    if (set.find(k) != set.end())
	    {
		CPPUNIT_ASSERT( bt.size() == set.size() );

		CPPUNIT_ASSERT( bt.exists(k) );
		CPPUNIT_ASSERT( bt.erase_one(k) );
		set.erase( set.find(k) );

		CPPUNIT_ASSERT( bt.size() == set.size() );
	    }
	}

	CPPUNIT_ASSERT( bt.empty() );
	CPPUNIT_ASSERT( set.empty() );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( LargeTest );
