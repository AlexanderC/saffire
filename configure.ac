#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([saffire], [0.1], [build@saffire-lang.org])
AM_INIT_AUTOMAKE([subdir-objects foreign])
AC_CONFIG_HEADERS([src/config.h])

# Store configure args
AC_DEFINE_UNQUOTED([CONFIGURE_ARGS],["$ac_configure_args"],[arguments passed to configure script])

# Checks for programs.
AC_LANG_C
AM_PROG_CC_C_O
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_YACC
if test "x$YACC" = xyacc; then
  AC_MSG_ERROR([Bison not found. Please install the 'bison'-package.])
fi
AC_PROG_LEX
if test "x$LEX" = x; then
  AC_MSG_ERROR([Flex not found. Please install the 'flex'-package.])
fi
AC_PROG_RANLIB


#debug
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")


#parsedebug
AC_ARG_ENABLE(parsedebug,
AS_HELP_STRING([--enable-parsedebug],
               [enable debugging of flex/bison, default: no]),
[case "${enableval}" in
             yes) parsedebug=true ;;
             no)  parsedebug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-parsedebug]) ;;
esac],
[parsedebug=false])

AM_CONDITIONAL(PARSEDEBUG, test x"$parsedebug" = x"true")


#gcov
AC_ARG_ENABLE(gcov,
AC_HELP_STRING([--enable-gcov],
               [turn on test coverage @<:@default=no@:>@]), [enable_coverage=$enableval],[enable_coverage="no"])

if test "x$enable_coverage" = "xyes"; then
	if test "x$ac_compiler_gnu" != "xyes"; then
		AC_MSG_ERROR([coverage testing only works if gcc is used])
	fi

	CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
	LDFLAGS="$LDFLAGS -lgcov"
	AC_CHECK_PROGS(GCOV, gcov, false)
	AC_CHECK_PROGS(LCOV, lcov, false)
	AC_CHECK_PROGS(GENHTML, genhtml, false)
fi

AM_CONDITIONAL(ENABLE_COVERAGE, test "x$enable_coverage" = "xyes")



dnl # check for icu-config
dnl AC_PATH_PROG(icu_config, icu-config, no)
dnl if test "$icu_config" = "no"; then
dnl 	AC_MSG_ERROR([Cannot find icu-config. ICU library is needed.])
dnl fi
dnl ICU_CPPFLAGS=`$icu_config --cppflags`
dnl ICU_LIBS=`$icu_config --ldflags`
dnl AC_SUBST([ICU_CPPFLAGS])
dnl AC_SUBST([ICU_LIBS])


# Checks for libraries.

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(edit, libedit)
PKG_CHECK_MODULES([libxml2], [libxml-2.0])

AC_CHECK_LIB([m], [main], [], [ AC_MSG_ERROR([*** Math library is required, install match development files (libm)])])
AC_CHECK_LIB([bz2], [BZ2_bzDecompressInit], [], [AC_MSG_ERROR([*** BZip2 is required, install bzip2 development files (libbz2)])])
AC_CHECK_LIB([fcgi], [FCGI_Accept], [], [AC_MSG_ERROR([*** FastCGI is required, install libfcgi development files (libfcgi)])])

# Check for PCRE
have_pcre_lib=no
have_pcre_h=no
AC_CHECK_LIB([pcre], [pcre_compile], [
    have_pcre_lib=yes
    LIBS="$LIBS -lpcre"
])
AC_CHECK_HEADER([pcre.h], [have_pcre_h=yes])
if test "$have_pcre_lib" = "no" -o "$have_pcre_h" = "no"; then
    PKG_PROG_PKG_CONFIG
    PKG_CHECK_MODULES([libpcre], [libpcre], [], [
        AC_MSG_ERROR([*** PCRE is required, install libpcre development files (libpcre3)])
    ])
    LIBS="$LIBS $libpcre_LIBS"
    CFLAGS="$CFLAGS $libpcre_CFLAGS"
fi


# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([inttypes.h libintl.h malloc.h stddef.h stdlib.h string.h unistd.h histedit.h prce.h])
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([floor memset strdup strtol])

AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    unittests/core/Makefile
                ])

AC_OUTPUT
