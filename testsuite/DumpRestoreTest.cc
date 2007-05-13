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

#include <sstream>
#include <iostream>

#include <stx/btree_multiset.h>

class DumpRestoreTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DumpRestoreTest );
    CPPUNIT_TEST(test_dump_restore_3200);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
	static const bool	selfverify = true;
	static const bool	debug = false;

	static const int 	leafslots = 8;
	static const int	innerslots = 8;
    };

    void test_dump_restore_3200()
    {
	typedef stx::btree_multiset<unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	std::string dumpstr;

	{
	    btree_type bt;

	    srand(34234235);
	    for(unsigned int i = 0; i < 3200; i++)
	    {
		bt.insert(rand() % 100);
	    }

	    CPPUNIT_ASSERT(bt.size() == 3200);

	    std::ostringstream os;
	    bt.dump(os);
	
	    dumpstr = os.str();
	}

        // std::cerr << "dumpstr: size = " << dumpstr.size() << "\n";
	CPPUNIT_ASSERT( dumpstr.size() == 47772 );

	// cannot check the string with a hash function, because it contains
	// memory pointers

	{ // restore the btree image
	    btree_type bt2;

	    std::istringstream iss(dumpstr);
	    CPPUNIT_ASSERT( bt2.restore(iss) );

	    CPPUNIT_ASSERT( bt2.size() == 3200 );

	    srand(34234235);
	    for(unsigned int i = 0; i < 3200; i++)
	    {
		CPPUNIT_ASSERT( bt2.exists(rand() % 100) );
	    }
	}

	{ // try restore the btree image using a different instantiation

	    typedef stx::btree_multiset<long long,
		std::less<long long>, struct traits_nodebug> otherbtree_type;

	    otherbtree_type bt3;

	    std::istringstream iss(dumpstr);
	    CPPUNIT_ASSERT( !bt3.restore(iss) );
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( DumpRestoreTest );
