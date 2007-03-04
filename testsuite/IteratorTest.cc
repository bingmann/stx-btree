// $Id$

#include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include <vector>

#include "btree.h"

class IteratorTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IteratorTest );
    CPPUNIT_TEST(test_iterator1);
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

    void test_iterator1()
    {
	typedef btree<unsigned int, unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	std::vector< btree_type::value_type > vector;

	srand(34234235);
	for(unsigned int i = 0; i < 3200; i++)
	{
	    vector.push_back( btree_type::value_type(rand() % 1000, 0) );
	}

	CPPUNIT_ASSERT( vector.size() == 3200 );

	btree_type bt(vector.begin(), vector.end());

	CPPUNIT_ASSERT( bt.size() == 3200 );

	srand(34234235);
	for(unsigned int i = 0; i < 3200; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == 3200 - i);
	    CPPUNIT_ASSERT( bt.erase_one(rand() % 1000) );
	    CPPUNIT_ASSERT(bt.size() == 3200 - i - 1);
	}

	CPPUNIT_ASSERT( bt.empty() );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( IteratorTest );
