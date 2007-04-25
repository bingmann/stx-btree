# B+ Tree Speed Tests
# $Id$

if BUILD_SPEEDTEST

noinst_PROGRAMS = speedtest

endif

speedtest_SOURCES = speedtest.cc

AM_CXXFLAGS = -W -Wall -Wold-style-cast -O3 -DNDEBUG -I$(top_srcdir)/include

EXTRA_DIST = speed-all.trt speed-all.txt speed-insert.trt speed-insert.txt \
	speedtest.gnuplot speedtest.pdf transpose.awk
