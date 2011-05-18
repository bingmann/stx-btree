#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(stx-btree, 0.8.6)
AC_CONFIG_SRCDIR([include/stx/btree.h])
AC_CONFIG_AUX_DIR(scripts)
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# enable full optimization by configure switch

AC_ARG_ENABLE(optimize,
	AS_HELP_STRING([--enable-optimize],
		       [Build with full optimization @<:@default=no@:>@]),
        [ case "${enableval}" in
          yes)
                CFLAGS="$CFLAGS -O3 -fomit-frame-pointer";
                CXXFLAGS="$CXXFLAGS -O3 -fomit-frame-pointer";
          ;;
          no) ;;
          *) AC_MSG_ERROR(bad value ${enableval} for --enable-optimize) ;;
          esac ],
        [ optimize=false ])

# set debug info flag if no optimization flags are set.

if test "$CFLAGS" == ""; then
    CFLAGS="-g"
fi
if test "$CXXFLAGS" == ""; then
    CXXFLAGS="-g"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_LANG([C++])

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Check whether to enable gcov coverage flags and macros

AC_ARG_ENABLE(gcov,
    AS_HELP_STRING([--enable-gcov],
                   [enable test coverage with gcov @<:@default=no@:>@]),
	[case "${enableval}" in
         yes) gcov=true ;;
         no)  gcov=false ;;
         *)   AC_MSG_ERROR([bad value ${enableval} for --enable-gcov]) ;;
         esac],
        [gcov=false])

AM_CONDITIONAL(GCOV, test x"$gcov" = "xtrue")

if test x"$gcov" = "xtrue"; then
    CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
    CXXFLAGS="$CXXFLAGS -fprofile-arcs -ftest-coverage"
    LIBS="$LIBS -lgcov"
fi

# check whether cppunit is available
AM_PATH_CPPUNIT(1.10.0)

AM_CONDITIONAL(HAVE_CPPUNIT, test x$CPPUNIT_CONFIG != xno)

# Conditional whether to compile the speedtest binary
AC_ARG_ENABLE(speedtest,
     [  --enable-speedtest	  Build the speedtest (takes lots of RAM) (default: no)],
     [case "${enableval}" in
       yes) speedtest=true ;;
       no)  speedtest=false ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-speedtest) ;;
     esac],[speedtest=false])
AM_CONDITIONAL(BUILD_SPEEDTEST, test x$speedtest = xtrue)

# Check for wxWidgets 2.6.0 or later
AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG(2.6.0, wxWin=1)

AM_CONDITIONAL(HAVE_WXWIDGETS, test "$wxWin" != 0)

if test "$wxWin" != 1; then
    AC_MSG_WARN([
	wxWidgets does not seem to be installed on your system.
	The wxParserDemo will not be built!

        If you think wxWidgets >= 2.6.0 is installed,
	please check that wx-config is in path, the directory
	where wxWidgets libraries are installed (returned by
	'wx-config --libs' command) is in LD_LIBRARY_PATH or
	equivalent variable.
    ])
fi

# Win32 tools to add icons to the programs
case "${host}" in
*-*-cygwin* | *-*-mingw32*)
    AC_CHECK_TOOL(RESCOMP, windres)
    ;;
esac
AM_CONDITIONAL(GOT_RESCOMP, test x$RESCOMP != x)

# Checks for library functions.

AC_CONFIG_FILES([Makefile
		 include/Makefile
		 testsuite/Makefile
		 speedtest/Makefile
                 wxbtreedemo/Makefile])
AC_OUTPUT
