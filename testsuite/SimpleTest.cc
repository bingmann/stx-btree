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

#include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include <stx/btree_multiset.h>

class SimpleTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SimpleTest );
    CPPUNIT_TEST(test_insert_erase_32);
    CPPUNIT_TEST(test_insert_erase_32_descending);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
	static const bool	selfverify = true;
	static const bool	debug = false;

	static const int 	leafslots = 8;
	static const int	innerslots = 8;
    };

    void test_insert_erase_32()
    {
	typedef stx::btree_multiset<unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	btree_type bt;

	srand(34234235);
	for(unsigned int i = 0; i < 32; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == i);
	    bt.insert(rand() % 100);
	    CPPUNIT_ASSERT(bt.size() == i + 1);
	}

	srand(34234235);
	for(unsigned int i = 0; i < 32; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == 32 - i);
	    CPPUNIT_ASSERT( bt.erase_one(rand() % 100) );
	    CPPUNIT_ASSERT(bt.size() == 32 - i - 1);
	}
    }

    void test_insert_erase_32_descending()
    {
	typedef stx::btree_multiset<unsigned int,
	    std::greater<unsigned int>, struct traits_nodebug> btree_type;

	btree_type bt;

	srand(34234235);
	for(unsigned int i = 0; i < 32; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == i);
	    bt.insert(rand() % 100);
	    CPPUNIT_ASSERT(bt.size() == i + 1);
	}

	srand(34234235);
	for(unsigned int i = 0; i < 32; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == 32 - i);
	    CPPUNIT_ASSERT( bt.erase_one(rand() % 100) );
	    CPPUNIT_ASSERT(bt.size() == 32 - i - 1);
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( SimpleTest );
