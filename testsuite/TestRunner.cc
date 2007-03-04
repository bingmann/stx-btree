// $Id$

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
