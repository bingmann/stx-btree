# $Id$

SUBDIRS = . testsuite

include_HEADERS = include/stx/btree.h \
		include/stx/btree_set.h \
		include/stx/btree_map.h \
		include/stx/btree_multiset.h \
		include/stx/btree_multimap.h

noinst_PROGRAMS = testbt

testbt_SOURCES = testbt.cc

AM_CFLAGS = -W -Wall -I$(srcdir)/include
AM_CXXFLAGS = -W -Wall -Wold-style-cast -DBTREE_DEBUG -I$(srcdir)/include
