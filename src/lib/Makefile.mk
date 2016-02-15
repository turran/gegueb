
lib_LTLIBRARIES = src/lib/libgegueb.la

install_gegueb_headersdir = $(pkgincludedir)-@VMAJ@
dist_install_gegueb_headers_DATA = \
src/lib/Gegueb.h \
src/lib/gegueb_build.h \
src/lib/gegueb_main.h \
src/lib/gegueb_widget.h \
src/lib/gegueb_window.h

src_lib_libgegueb_la_CPPFLAGS = \
-DGEGUEB_BUILD \
@GEGUEB_CFLAGS@

src_lib_libgegueb_la_SOURCES = \
src/lib/gegueb_document.c \
src/lib/gegueb_main.c \
src/lib/gegueb_widget.c \
src/lib/gegueb_window.c

src_lib_libgegueb_la_LIBADD = @GEGUEB_LIBS@
src_lib_libgegueb_la_LDFLAGS = -no-undefined -version-info @version_info@

if BUILD_EGUEB_SMIL
src_lib_libgegueb_la_CPPFLAGS += @EGUEB_SMIL_CFLAGS@
src_lib_libgegueb_la_LIBADD += @EGUEB_SMIL_LIBS@
endif

if BUILD_EGUEB_SVG
src_lib_libgegueb_la_CPPFLAGS += @EGUEB_SVG_CFLAGS@
src_lib_libgegueb_la_LIBADD += @EGUEB_SVG_LIBS@
endif

