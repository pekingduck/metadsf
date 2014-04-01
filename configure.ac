#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_CONFIG_MACRO_DIR([/Users/gordonlo/dev/m4])
AC_PREREQ([2.69])
AC_INIT([metadsf], [0.1], [pekingducklabs@gmail.com])
AC_CONFIG_SRCDIR([src/main.cpp])
#AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AC_LANG(C++)
# Checks for libraries.
# taglib checks
SAVED_LDFLAGS=$LDFLAGS
SAVED_CXXFLAGS=$CXXFLAGS
LDFLAGS="$LDFLAGS -L/usr/local/lib -ltag -lz"
CXXFLAGS="-I/usr/local/include"
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include <taglib/tstring.h>],
    [TagLib::String dummy])],
  [TEST_LIBS="$TEST_LIBS -ltag -lz"] [HAVE_TAGLIB=1],
  [AC_MSG_FAILURE([TagLib is not installed.])])
LDFLAGS=$SAVED_LDFLAGS
CXXFLAGS=$SAVED_CXXFLAGS
# c++11 check
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
# zlib check
AC_CHECK_LIB([z], [gzwrite])

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
