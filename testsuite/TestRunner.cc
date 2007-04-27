// $Id$

/*
 * STX B+ Tree Template Classes v0.7
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>

#include <cppunit/extensions/TestFactoryRegistry.h>

#include <cppunit/TestResult.h>

int main()
{
    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextTestRunner runner;
    runner.addTest( suite );

    // add brief output before running each test
    runner.eventManager().addListener( new CppUnit::BriefTestProgressListener() );

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter( new CppUnit::TextOutputter( &runner.result(), std::cout ) );
    
    // Run the tests.
    bool wasSucessful = runner.run();

    // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;
}
