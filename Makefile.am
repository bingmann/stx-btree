# $Id$

SUBDIRS = . testsuite speedtest

include_HEADERS = \
	include/stx/btree.h \
	include/stx/btree_set.h \
	include/stx/btree_map.h \
	include/stx/btree_multiset.h \
	include/stx/btree_multimap.h \
	include/stx/btree \
	include/stx/btree_set \
	include/stx/btree_map \
	include/stx/btree_multiset \
	include/stx/btree_multimap \
	include/stx/btree_mainpage.h

AM_CFLAGS = -W -Wall -I$(srcdir)/include
AM_CXXFLAGS = -W -Wall -Wold-style-cast -DBTREE_DEBUG -I$(srcdir)/include

EXTRA_DIST = Doxyfile doxygen.css
