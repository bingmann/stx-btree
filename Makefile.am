# $Id$

SUBDIRS = . testsuite speedtest wxbtreedemo

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
	include/stx/btree_doxygen.h

AM_CFLAGS = -W -Wall -I$(srcdir)/include
AM_CXXFLAGS = -W -Wall -Wold-style-cast -DBTREE_DEBUG -I$(srcdir)/include

EXTRA_DIST = Doxyfile doxygen.css

if GCOV

clean-local:
	find -name "*.da" -o -name "*.gcov" -o -name "*.gcda" -o -name "*.gcno" | xargs rm || true

run-gcov: clean-gcov
	mkdir -p coverage/
	lcov --directory . --zerocounters
	-$(MAKE) check
	lcov --directory . --capture --output-file coverage/all.info
	lcov -e coverage/all.info "/tdata/*" -o coverage/testsuite.info
	genhtml -o coverage --num-spaces 8 coverage/testsuite.info --title "STX B+ Tree Testsuite" --prefix `pwd` --legend

clean-gcov:
	find -name "*.gcda" | xargs rm || true

endif
