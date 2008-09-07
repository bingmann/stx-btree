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

#include <stx/btree_multiset.h>
#include <stx/btree_multimap.h>
#include <stx/btree_map.h>

class SimpleTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SimpleTest );
    CPPUNIT_TEST(test_empty);
    CPPUNIT_TEST(test_set_insert_erase_320);
    CPPUNIT_TEST(test_set_insert_erase_320_descending);
    CPPUNIT_TEST(test_map_insert_erase_320);
    CPPUNIT_TEST(test_map_insert_erase_320_descending);
    CPPUNIT_TEST(test2_map_insert_erase_strings);
    CPPUNIT_TEST(test_set_100000_uint64);
    CPPUNIT_TEST_SUITE_END();

protected:

    struct traits_nodebug
    {
        static const bool       selfverify = true;
        static const bool       debug = false;

        static const int        leafslots = 8;
        static const int        innerslots = 8;
    };

    void test_empty()
    {
        typedef stx::btree_multiset<unsigned int,
            std::less<unsigned int>, struct traits_nodebug> btree_type;

        btree_type bt, bt2;
        bt.verify();

        CPPUNIT_ASSERT( bt.erase(42) == false );

        CPPUNIT_ASSERT( bt == bt2 );
    }

    void test_set_insert_erase_320()
    {
        typedef stx::btree_multiset<unsigned int,
            std::less<unsigned int>, struct traits_nodebug> btree_type;

        btree_type bt;
        bt.verify();

        srand(34234235);
        for(unsigned int i = 0; i < 320; i++)
        {
            CPPUNIT_ASSERT(bt.size() == i);
            bt.insert(rand() % 100);
            CPPUNIT_ASSERT(bt.size() == i + 1);
        }

        srand(34234235);
        for(unsigned int i = 0; i < 320; i++)
        {
            CPPUNIT_ASSERT(bt.size() == 320 - i);
            CPPUNIT_ASSERT( bt.erase_one(rand() % 100) );
            CPPUNIT_ASSERT(bt.size() == 320 - i - 1);
        }

        CPPUNIT_ASSERT( bt.empty() );
    }

    void test_set_insert_erase_320_descending()
    {
        typedef stx::btree_multiset<unsigned int,
            std::greater<unsigned int>, struct traits_nodebug> btree_type;

        btree_type bt;

        srand(34234235);
        for(unsigned int i = 0; i < 320; i++)
        {
            CPPUNIT_ASSERT(bt.size() == i);
            bt.insert(rand() % 100);
            CPPUNIT_ASSERT(bt.size() == i + 1);
        }

        srand(34234235);
        for(unsigned int i = 0; i < 320; i++)
        {
            CPPUNIT_ASSERT(bt.size() == 320 - i);
            CPPUNIT_ASSERT( bt.erase_one(rand() % 100) );
            CPPUNIT_ASSERT(bt.size() == 320 - i - 1);
        }

        CPPUNIT_ASSERT( bt.empty() );
    }

    void test_map_insert_erase_320()
    {
        typedef stx::btree_multimap<unsigned int, std::string,
            std::less<unsigned int>, struct traits_nodebug> btree_type;

        btree_type bt;

        srand(34234235);
        for(unsigned int i = 0; i < 320; i++)
        {
            CPPUNIT_ASSERT(bt.size() == i);
            bt.insert2(rand() % 100, "101");
            CPPUNIT_ASSERT(bt.size() == i + 1);
        }

        srand(34234235);
        for(unsigned int i = 0; i < 320; i++)
        {
            CPPUNIT_ASSERT(bt.size() == 320 - i);
            CPPUNIT_ASSERT( bt.erase_one(rand() % 100) );
            CPPUNIT_ASSERT(bt.size() == 320 - i - 1);
        }

        CPPUNIT_ASSERT( bt.empty() );
        bt.verify();
    }

    void test_map_insert_erase_320_descending()
    {
        typedef stx::btree_multimap<unsigned int, std::string,
            std::greater<unsigned int>, struct traits_nodebug> btree_type;

        btree_type bt;

        srand(34234235);
        for(unsigned int i = 0; i < 320; i++)
        {
            CPPUNIT_ASSERT(bt.size() == i);
            bt.insert2(rand() % 100, "101");
            CPPUNIT_ASSERT(bt.size() == i + 1);
        }

        srand(34234235);
        for(unsigned int i = 0; i < 320; i++)
        {
            CPPUNIT_ASSERT(bt.size() == 320 - i);
            CPPUNIT_ASSERT( bt.erase_one(rand() % 100) );
            CPPUNIT_ASSERT(bt.size() == 320 - i - 1);
        }

        CPPUNIT_ASSERT( bt.empty() );
        bt.verify();
    }

    void test2_map_insert_erase_strings()
    {
        typedef stx::btree_multimap<std::string, unsigned int,
            std::less<std::string>, struct traits_nodebug> btree_type;

        std::string letters = "abcdefghijklmnopqrstuvwxyz";

        btree_type bt;

        for(unsigned int a = 0; a < letters.size(); ++a)
        {
            for(unsigned int b = 0; b < letters.size(); ++b)
            {
                bt.insert2(std::string(1, letters[a]) + letters[b],
                           a * letters.size() + b);
            }
        }

        for(unsigned int b = 0; b < letters.size(); ++b)
        {
            for(unsigned int a = 0; a < letters.size(); ++a)
            {
                std::string key = std::string(1, letters[a]) + letters[b];

                CPPUNIT_ASSERT( bt.find(key)->second == a * letters.size() + b );
                CPPUNIT_ASSERT( bt.erase_one(key) );
            }
        }

        CPPUNIT_ASSERT( bt.empty() );
        bt.verify();
    }

    typedef unsigned char uint8_t;
    typedef unsigned long long uint64_t;

    void test_set_100000_uint64()
    {
        stx::btree_map<uint64_t, uint8_t> bt;

        for(uint64_t i = 10; i < 100000; ++i)
        {
            uint64_t key = i % 1000;

            if (bt.find(key) == bt.end())
            {
                bt.insert( std::make_pair(key, key % 100) );
            }
        }

        CPPUNIT_ASSERT( bt.size() == 1000 );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( SimpleTest );
