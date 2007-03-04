#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(stx-btree, 0.6)
AC_CONFIG_SRCDIR([include/stx/btree_base.h])
AC_CONFIG_AUX_DIR(scripts)

AM_INIT_AUTOMAKE([foreign])
#AM_MAINTAINER_MODE

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

# check whether cppunit is available
AM_PATH_CPPUNIT(1.10.0)

AM_CONDITIONAL(HAVE_CPPUNIT, test x$CPPUNIT_CONFIG != xno)

# Checks for library functions.

AC_CONFIG_FILES([Makefile
		 testsuite/Makefile])
AC_OUTPUT
