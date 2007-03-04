// $Id$

#include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include <sstream>
#include <iostream>

#include "btree.h"

class DumpRestoreTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DumpRestoreTest );
    CPPUNIT_TEST(test_dump_restore_3200);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
	static const bool	selfverify = true;
	static const bool	allow_duplicates = true;
	static const bool	debug = false;

	static const int 	leafslots = 8;
	static const int	innerslots = 8;
    };

    void test_dump_restore_3200()
    {
	typedef btree<unsigned int, unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	std::string dumpstr;

	{
	    btree_type bt;

	    srand(34234235);
	    for(unsigned int i = 0; i < 3200; i++)
	    {
		bt.insert2(rand() % 100, 0);
	    }

	    CPPUNIT_ASSERT(bt.size() == 3200);

	    std::ostringstream os;
	    bt.dump(os);
	
	    dumpstr = os.str();
	}

	// std::cerr << "dumpstr: size = " << dumpstr.size() << "\n";
	CPPUNIT_ASSERT( dumpstr.size() == 65388 );

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

	    typedef btree<unsigned int, long long,
		std::less<unsigned int>, struct traits_nodebug> otherbtree_type;

	    otherbtree_type bt3;

	    std::istringstream iss(dumpstr);
	    CPPUNIT_ASSERT( !bt3.restore(iss) );
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( DumpRestoreTest );
