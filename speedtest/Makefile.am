# B+ Tree Speed Tests
# $Id$

if BUILD_SPEEDTEST

noinst_PROGRAMS = speedtest

endif

speedtest_SOURCES = speedtest.cc

AM_CXXFLAGS = -W -Wall -Wold-style-cast -DNDEBUG -I$(top_srcdir)/include

EXTRA_DIST = speed-all.trt speed-all.txt speed-insert.trt speed-insert.txt \
	speedtest.gnuplot transpose.awk \
	speedtest.pdf \
	speedtest-plot-1.png speedtest-plot-2.png \
	speedtest-plot-3.png speedtest-plot-4.png \
	speedtest-plot-5.png speedtest-plot-6.png \
	speedtest-plot-7.png speedtest-plot-8.png

plots:
	USER=tb ./speedtest.gnuplot
	pdftoppm speedtest.pdf speedtest-plot
	for f in *.ppm; do convert $$f $${f%%.ppm}.png; rm $$f; done
