// $Id$

/*
 * STX B+ Tree Template Classes v0.8.2
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

class StructureTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( StructureTest );
    CPPUNIT_TEST(test_insert_erase);
    CPPUNIT_TEST_SUITE_END();

public:

    struct testdata
    {
        unsigned int a, b;

        // required by the btree
        testdata()
            : a(0), b(0)
        {
        }

        // also used as implicit conversion constructor
        inline testdata(unsigned int _a)
            : a(_a), b(0)
        {
        }
    };

protected:

    struct testcomp
    {
        unsigned int somevalue;

        inline testcomp(unsigned int sv)
            : somevalue(sv)
        {
        }

        bool operator()(const struct testdata &a, const struct testdata &b) const
        {
            return a.a > b.a;
        }
    };

    struct traits_nodebug
    {
        static const bool       selfverify = true;
        static const bool       debug = false;

        static const int        leafslots = 8;
        static const int        innerslots = 8;
    };

    void test_insert_erase()
    {
        typedef stx::btree_multiset<struct testdata, struct testcomp, struct traits_nodebug> btree_type;

        btree_type bt( testcomp(42) );

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
    }
};

inline std::ostream& operator<< (std::ostream &o, const struct StructureTest::testdata &t)
{
    return o << t.a;
}

CPPUNIT_TEST_SUITE_REGISTRATION( StructureTest );
