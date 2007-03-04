# B+ Tree Test Cases
# $Id$

noinst_PROGRAMS = testsuite

TESTS = testsuite

if HAVE_CPPUNIT

testsuite_SOURCES = TestRunner.cc

testsuite_SOURCES += BoundTest.cc IteratorTest.cc StructureTest.cc LargeTest.cc SimpleTest.cc

else

testsuite_SOURCES = TestTrue.cc

endif

AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(top_srcdir)/include @CPPUNIT_CFLAGS@
LDADD = @CPPUNIT_LIBS@
