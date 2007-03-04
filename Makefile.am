# $Id$

SUBDIRS = . testsuite

include_HEADERS = include/btree.h

noinst_PROGRAMS = testbt

testbt_SOURCES = testbt.cc

AM_CFLAGS = -W -Wall -I$(srcdir)/include
AM_CXXFLAGS = -W -Wall -Wold-style-cast -DBTREE_DEBUG -I$(srcdir)/include
