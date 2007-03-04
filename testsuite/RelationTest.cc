// $Id$

#include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include "btree.h"

class RelationTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( RelationTest );
    CPPUNIT_TEST(test_relations);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
	static const bool	selfverify = true;
	static const bool	allow_duplicates = false;
	static const bool	debug = false;

	static const int 	leafslots = 8;
	static const int	innerslots = 8;
    };

    void test_relations()
    {
	typedef btree<unsigned int, unsigned int,
	    std::less<unsigned int>, struct traits_nodebug> btree_type;

	btree_type bt1, bt2;

	srand(34234236);
	for(unsigned int i = 0; i < 320; i++)
	{
	    unsigned int key = rand() % 1000;

	    bt1.insert2(key, 0);
	    bt2.insert2(key, 0);
	}

	CPPUNIT_ASSERT( bt1 == bt2 );
	
	bt1.insert2(499, 0);
	bt2.insert2(500, 0);

	CPPUNIT_ASSERT( bt1 != bt2 );
	CPPUNIT_ASSERT( bt1 < bt2 );
	CPPUNIT_ASSERT( !(bt1 > bt2) );

	bt1.insert2(500, 0);
	bt2.insert2(499, 0);

	CPPUNIT_ASSERT( bt1 == bt2 );
	CPPUNIT_ASSERT( bt1 <= bt2 );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( RelationTest );
