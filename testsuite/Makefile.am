# B+ Tree Test Cases

noinst_PROGRAMS = testsuite

TESTS = testsuite

testsuite_SOURCES = tpunit.cc tpunit.h

testsuite_SOURCES += InstantiationTest.cc SimpleTest.cc LargeTest.cc BoundTest.cc IteratorTest.cc StructureTest.cc DumpRestoreTest.cc RelationTest.cc

AM_CXXFLAGS = -W -Wall -Wold-style-cast -Wshadow -DBTREE_DEBUG -I$(top_srcdir)/include
