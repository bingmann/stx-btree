// $Id$

#include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include "btree.h"

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
	static const bool	allow_duplicates = true;
	static const bool	debug = false;

	static const int 	leafslots = 8;
	static const int	innerslots = 8;
    };

    void test_insert_erase_32()
    {
	typedef btree<unsigned int, unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	btree_type bt;

	srand(34234235);
	for(unsigned int i = 0; i < 32; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == i);
	    bt.insert2(rand() % 100, 0);
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
	typedef btree<unsigned int, unsigned int,
	    std::greater<unsigned int>, struct traits_nodebug> btree_type;

	btree_type bt;

	srand(34234235);
	for(unsigned int i = 0; i < 32; i++)
	{
	    CPPUNIT_ASSERT(bt.size() == i);
	    bt.insert2(rand() % 100, 0);
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
