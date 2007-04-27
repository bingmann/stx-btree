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

#include <vector>

#include <stx/btree_multiset.h>
#include <stx/btree_multimap.h>

class IteratorTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IteratorTest );
    CPPUNIT_TEST(test_iterator1);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
	static const bool	selfverify = true;
	static const bool	debug = false;

	static const int 	leafslots = 8;
	static const int	innerslots = 8;
    };

    void test_iterator1()
    {
	typedef stx::btree_multiset<unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	std::vector<unsigned int> vector;

	srand(34234235);
	for(unsigned int i = 0; i < 3200; i++)
	{
	    vector.push_back( rand() % 1000 );
	}

	CPPUNIT_ASSERT( vector.size() == 3200 );

	// test construction and insert(iter, iter) function
        btree_type bt(vector.begin(), vector.end());

	CPPUNIT_ASSERT( bt.size() == 3200 );

	// copy for later use
	btree_type bt2 = bt;

	// empty out the first bt
	srand(34234235);
	for(unsigned int i = 0; i < 3200; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == 3200 - i);
	    CPPUNIT_ASSERT( bt.erase_one(rand() % 1000) );
	    CPPUNIT_ASSERT(bt.size() == 3200 - i - 1);
	}

	CPPUNIT_ASSERT( bt.empty() );

	// copy btree values back to a vector

	std::vector<unsigned int> vector2;
	vector2.assign( bt2.begin(), bt2.end() );

	// afer sorting the vector, the two must be the same
	std::sort(vector.begin(), vector.end());

	CPPUNIT_ASSERT( vector == vector2 );

	// test reverse iterator
	vector2.clear();
	vector2.assign( bt2.rbegin(), bt2.rend() );

	std::reverse(vector.begin(), vector.end());

	btree_type::reverse_iterator ri = bt2.rbegin();
	for(unsigned int i = 0; i < vector2.size(); ++i)
	{
	    CPPUNIT_ASSERT( vector[i] == vector2[i] );
	    CPPUNIT_ASSERT( vector[i] == *ri );

	    ri++;
	}

	CPPUNIT_ASSERT( ri == bt2.rend() );
    }

    void test_iterator2()
    {
	typedef stx::btree_multimap<unsigned int, unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	std::vector< btree_type::value_type > vector;

	srand(34234235);
	for(unsigned int i = 0; i < 3200; i++)
	{
	    vector.push_back( btree_type::value_type(rand() % 1000, 0) );
	}

	CPPUNIT_ASSERT( vector.size() == 3200 );

	// test construction and insert(iter, iter) function
	btree_type bt(vector.begin(), vector.end());

	CPPUNIT_ASSERT( bt.size() == 3200 );

	// copy for later use
	btree_type bt2 = bt;

	// empty out the first bt
	srand(34234235);
	for(unsigned int i = 0; i < 3200; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == 3200 - i);
	    CPPUNIT_ASSERT( bt.erase_one(rand() % 1000) );
	    CPPUNIT_ASSERT(bt.size() == 3200 - i - 1);
	}

	CPPUNIT_ASSERT( bt.empty() );

	// copy btree values back to a vector

	std::vector< btree_type::value_type > vector2;
	vector2.assign( bt2.begin(), bt2.end() );

	// afer sorting the vector, the two must be the same
	std::sort(vector.begin(), vector.end());

	CPPUNIT_ASSERT( vector == vector2 );

	// test reverse iterator
	vector2.clear();
	vector2.assign( bt2.rbegin(), bt2.rend() );

	std::reverse(vector.begin(), vector.end());

	btree_type::reverse_iterator ri = bt2.rbegin();
	for(unsigned int i = 0; i < vector2.size(); ++i)
	{
	    CPPUNIT_ASSERT( vector[i].first == vector2[i].first );
	    CPPUNIT_ASSERT( vector[i].first == ri->first );

	    // there are some undetermined problems with the second value
	    // std::cout << vector[i].second << " " << vector2[i].second << " " << ri->second << "\n";
	    ri++;
	}

	CPPUNIT_ASSERT( ri == bt2.rend() );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( IteratorTest );
