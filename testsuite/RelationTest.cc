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

#include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include <stx/btree_multiset.h>

class RelationTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( RelationTest );
    CPPUNIT_TEST(test_relations);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
        static const bool       selfverify = true;
        static const bool       debug = false;

        static const int        leafslots = 8;
        static const int        innerslots = 8;
    };

    void test_relations()
    {
        typedef stx::btree_multiset<unsigned int,
            std::less<unsigned int>, struct traits_nodebug> btree_type;

        btree_type bt1, bt2;

        srand(34234236);
        for(unsigned int i = 0; i < 320; i++)
        {
            unsigned int key = rand() % 1000;

            bt1.insert(key);
            bt2.insert(key);
        }

        CPPUNIT_ASSERT( bt1 == bt2 );

        bt1.insert(499);
        bt2.insert(500);

        CPPUNIT_ASSERT( bt1 != bt2 );
        CPPUNIT_ASSERT( bt1 < bt2 );
        CPPUNIT_ASSERT( !(bt1 > bt2) );

        bt1.insert(500);
        bt2.insert(499);

        CPPUNIT_ASSERT( bt1 == bt2 );
        CPPUNIT_ASSERT( bt1 <= bt2 );

        // test assignment operator
        btree_type bt3;

        bt3 = bt1;
        CPPUNIT_ASSERT( bt1 == bt3 );
        CPPUNIT_ASSERT( bt1 >= bt3 );

        // test copy constructor
        btree_type bt4 = bt3;

        CPPUNIT_ASSERT( bt1 == bt4 );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( RelationTest );
