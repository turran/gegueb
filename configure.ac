
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [1])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([gegueb], [v_ver], [enesim-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign 1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])

VMAJ=v_maj
AC_SUBST([VMAJ])

### Needed information

AC_CANONICAL_HOST

MODULE_ARCH="$host_os-$host_cpu-1.0.0"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

### Additional options to configure


### Checks for programs

AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if ! $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   AC_MSG_ERROR([pkg-config >= 0.22 is required])
fi


### Checks for libraries

requirements_pc="egueb-dom >= 0.0.23 enesim >= 0.0.15 gdk-2.0 glib-2.0"
AC_SUBST([requirements_pc])
PKG_CHECK_MODULES([GEGUEB], [${requirements_pc}])

requirements_bin_pc="egueb-dom >= 0.0.23 enesim >= 0.0.15 gdk-2.0 glib-2.0"
PKG_CHECK_MODULES([GEGUEB_BIN], [${requirements_bin_pc}])

### GDK X window
AC_ARG_ENABLE([gdk-x11], [AS_HELP_STRING([--enable-gdk-x11], [Enable GDK X window])],
  [
    if test "x$enableval" = "xyes" ; then
       want_gdk_x11="yes"
    else
       want_gdk_x11="no"
    fi
  ],
  [want_gdk_x11="yes"]
)
PKG_CHECK_MODULES([GEGUEB_X], [gdk-x11-2.0], [have_gdk_x11="yes"], [have_gdk_x11="no"])
build_gdk_x11="no"

if test "x${have_gdk_x11}" = "xyes" -a "x${want_gdk_x11}" = "xyes"; then
  build_gdk_x11="yes"
  requirements_pc="${requirements_pc} gdk-x11-2.0"
fi

if test "x${build_gdk_x11}" = "xyes"; then
  AC_DEFINE([BUILD_GDK_X11], [1], [Have support for gdk_x11])
fi
AM_CONDITIONAL([BUILD_GDK_X11], [test "x${build_gdk_x11}" = "xyes"])

### GDK Win32 window
AC_ARG_ENABLE([gdk-win32], [AS_HELP_STRING([--enable-gdk-win32], [Enable GDK Win32 window])],
  [
    if test "x$enableval" = "xyes" ; then
       want_gdk_win32="yes"
    else
       want_gdk_win32="no"
    fi
  ],
  [want_gdk_win32="yes"]
)
PKG_CHECK_MODULES([GEGUEB_WIN32], [gdk-win32-2.0], [have_gdk_win32="yes"], [have_gdk_win32="no"])
build_gdk_win32="no"

if test "x${have_gdk_win32}" = "xyes" -a "x${want_gdk_win32}" = "xyes"; then
  build_gdk_win32="yes"
  requirements_pc="${requirements_pc} gdk-win32-2.0"
fi

if test "x${build_gdk_win32}" = "xyes"; then
  AC_DEFINE([BUILD_GDK_WIN32], [1], [Have support for gdk_win32])
fi
AM_CONDITIONAL([BUILD_GDK_WIN32], [test "x${build_gdk_win32}" = "xyes"])

### SMIL based features
AC_ARG_ENABLE([egueb-smil], [AS_HELP_STRING([--enable-egueb-smil], [Enable SMIL animations support])],
  [
    if test "x$enableval" = "xyes" ; then
       want_egueb_smil="yes"
    else
       want_egueb_smil="no"
    fi
  ],
  [want_egueb_smil="yes"]
)

PKG_CHECK_MODULES([EGUEB_SMIL], [egueb-smil], [have_egueb_smil="yes"], [have_egueb_smil="no"])
build_egueb_smil="no"

if test "x${have_egueb_smil}" = "xyes" -a "x${want_egueb_smil}" = "xyes"; then
  build_egueb_smil="yes"
  requirements_pc="${requirements_pc} egueb-smil"
fi

if test "x${build_egueb_smil}" = "xyes"; then
  AC_DEFINE([BUILD_EGUEB_SMIL], [1], [Have support for egueb_smil])
fi
AM_CONDITIONAL([BUILD_EGUEB_SMIL], [test "x${build_egueb_smil}" = "xyes"])

### SVG based features
AC_ARG_ENABLE([egueb-svg], [AS_HELP_STRING([--enable-egueb-svg], [Enable SVG animations support])],
  [
    if test "x$enableval" = "xyes" ; then
       want_egueb_svg="yes"
    else
       want_egueb_svg="no"
    fi
  ],
  [want_egueb_svg="yes"]
)

PKG_CHECK_MODULES([EGUEB_SVG], [egueb-svg], [have_egueb_svg="yes"], [have_egueb_svg="no"])
build_egueb_svg="no"

if test "x${have_egueb_svg}" = "xyes" -a "x${want_egueb_svg}" = "xyes"; then
  build_egueb_svg="yes"
  requirements_pc="${requirements_pc} egueb-svg"
fi

if test "x${build_egueb_svg}" = "xyes"; then
  AC_DEFINE([BUILD_EGUEB_SVG], [1], [Have support for egueb_svg])
fi
AM_CONDITIONAL([BUILD_EGUEB_SVG], [test "x${build_egueb_svg}" = "xyes"])

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_C_O

### Checks for linker characteristics


### Checks for library functions

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo "  SMIL support..............................: ${build_egueb_smil}"
echo "  SVG support...............................: ${build_egueb_svg}"
echo "  Window support:"
echo "    GDK X.................................: ${build_gdk_x11}"
echo "    GDK Win32.............................: ${build_gdk_win32}"
echo
echo
echo "Compilation.................................: make (or gmake)"
echo "  CPPFLAGS..................................: $CPPFLAGS"
echo "  CFLAGS....................................: $CFLAGS"
echo "  LDFLAGS...................................: $LDFLAGS"
echo
echo "Installation................................: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix....................................: $prefix"
echo
