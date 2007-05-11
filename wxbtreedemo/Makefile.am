# $Id$

if HAVE_WXWIDGETS

bin_PROGRAMS = wxBTreeDemo

wxBTreeDemo_SOURCES = WMain.cpp WMain.h WMain_wxg.cpp \
	WTreeDrawing.h WTreeDrawing.cpp

AM_CFLAGS = -I/usr/include
AM_CXXFLAGS = -I/usr/include @WX_CXXFLAGS@ -I$(top_srcdir)/include -DBTREE_DEBUG
LDADD = @WX_LIBS@

endif
