# B+ Tree Speed Tests
# $Id$

if BUILD_SPEEDTEST

noinst_PROGRAMS = speedtest

endif

speedtest_SOURCES = speedtest.cc

AM_CXXFLAGS = -W -Wall -Wold-style-cast -DNDEBUG -I$(top_srcdir)/include

EXTRA_DIST = speed-all.trt speed-all.txt \
	speed-insert.trt speed-insert.txt \
	speed-find.trt speed-find.txt \
	speedtest.gnuplot transpose.awk \
	speedtest.pdf \
	speedtest-plot-01.png speedtest-plot-02.png \
	speedtest-plot-03.png speedtest-plot-04.png \
	speedtest-plot-05.png speedtest-plot-06.png \
	speedtest-plot-07.png speedtest-plot-08.png \
	speedtest-plot-09.png speedtest-plot-10.png \
	speedtest-plot-11.png speedtest-plot-12.png

plots:
	USER=tb ./speedtest.gnuplot
	pdftoppm speedtest.pdf speedtest-plot
	for f in *.ppm; do convert $$f $${f%%.ppm}.png; rm $$f; done
