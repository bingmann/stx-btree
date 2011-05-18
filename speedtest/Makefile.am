# B+ Tree Speed Tests
# $Id$

if BUILD_SPEEDTEST

noinst_PROGRAMS = speedtest

endif

speedtest_SOURCES = speedtest.cc

AM_CXXFLAGS = -W -Wall -Wold-style-cast -DNDEBUG -I$(top_srcdir)/include

EXTRA_DIST = \
	speedtest.gnuplot transpose.awk \
	results-2007/speedtest.pdf \
	results-2007/speedtest-2007-??.png \
	results-2007/speed-*.txt \
	results-2007/speed-*.trt \
	results-2011/speedtest.pdf \
	results-2011/speedtest-2011-??.png \
	results-2011/speed-*.txt \
	results-2011/speed-*.trt

plots:
	USER=tb ./speedtest.gnuplot
	pdftoppm speedtest.pdf speedtest-plot
	for f in *.ppm; do convert $$f $${f%%.ppm}.png; rm $$f; done
