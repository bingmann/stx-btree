// $Id$

/*
 * STX B+ Tree Template Classes v0.8.3
 * Copyright (C) 2008 Timo Bingmann
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

#include <stx/btree_multimap.h>
#include <set>

class BoundTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( BoundTest );
    CPPUNIT_TEST(test_3200_10);
    CPPUNIT_TEST(test_320_1000);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
        static const bool       selfverify = true;
        static const bool       debug = false;

        static const int        leafslots = 8;
        static const int        innerslots = 8;
    };

    void test_multi(const unsigned int insnum, const int modulo)
    {
        typedef stx::btree_multimap<unsigned int, unsigned int, std::less<unsigned int>, struct traits_nodebug> btree_type;
        btree_type bt;

        typedef std::multiset<unsigned int> multiset_type;
        multiset_type set;

        // *** insert
        srand(34234235);
        for(unsigned int i = 0; i < insnum; i++)
        {
            unsigned int k = rand() % modulo;
            unsigned int v = 234;

            CPPUNIT_ASSERT( bt.size() == set.size() );
            bt.insert2(k, v);
            set.insert(k);
            CPPUNIT_ASSERT( bt.count(k) == set.count(k) );

            CPPUNIT_ASSERT( bt.size() == set.size() );
        }

        CPPUNIT_ASSERT( bt.size() == insnum );

        // *** iterate
        {
            btree_type::iterator bi = bt.begin();
            multiset_type::const_iterator si = set.begin();
            for(; bi != bt.end() && si != set.end(); ++bi, ++si)
            {
                CPPUNIT_ASSERT( *si == bi.key() );
            }
            CPPUNIT_ASSERT( bi == bt.end() );
            CPPUNIT_ASSERT( si == set.end() );
        }

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

        // *** lower_bound
        for(int k = 0; k < modulo + 100; k++)
        {
            multiset_type::const_iterator si = set.lower_bound(k);
            btree_type::const_iterator bi = bt.lower_bound(k);

            if ( bi == bt.end() )
                CPPUNIT_ASSERT( si == set.end() );
            else if ( si == set.end() )
                CPPUNIT_ASSERT( bi == bt.end() );
            else
                CPPUNIT_ASSERT( *si == bi.key() );
        }

        // *** upper_bound
        for(int k = 0; k < modulo + 100; k++)
        {
            multiset_type::const_iterator si = set.upper_bound(k);
            btree_type::const_iterator bi = bt.upper_bound(k);

            if ( bi == bt.end() )
                CPPUNIT_ASSERT( si == set.end() );
            else if ( si == set.end() )
                CPPUNIT_ASSERT( bi == bt.end() );
            else
                CPPUNIT_ASSERT( *si == bi.key() );
        }

        // *** equal_range
        for(int k = 0; k < modulo + 100; k++)
        {
            std::pair<multiset_type::const_iterator, multiset_type::const_iterator> si = set.equal_range(k);
            std::pair<btree_type::const_iterator, btree_type::const_iterator> bi = bt.equal_range(k);

            if ( bi.first == bt.end() )
                CPPUNIT_ASSERT( si.first == set.end() );
            else if ( si.first == set.end() )
                CPPUNIT_ASSERT( bi.first == bt.end() );
            else
                CPPUNIT_ASSERT( *si.first == bi.first.key() );

            if ( bi.second == bt.end() )
                CPPUNIT_ASSERT( si.second == set.end() );
            else if ( si.second == set.end() )
                CPPUNIT_ASSERT( bi.second == bt.end() );
            else
                CPPUNIT_ASSERT( *si.second == bi.second.key() );
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
};

CPPUNIT_TEST_SUITE_REGISTRATION( BoundTest );
