bin_PROGRAMS = \
src/bin/gegueb_window_viewer \
src/bin/gegueb_widget_viewer

src_bin_gegueb_window_viewer_CPPFLAGS = -I$(top_srcdir)/src/lib @GEGUEB_BIN_CFLAGS@
src_bin_gegueb_window_viewer_SOURCES = src/bin/gegueb_window_viewer.c
src_bin_gegueb_window_viewer_LDADD = $(top_builddir)/src/lib/libgegueb.la @GEGUEB_BIN_LIBS@

src_bin_gegueb_widget_viewer_CPPFLAGS = -I$(top_srcdir)/src/lib @GEGUEB_BIN_CFLAGS@
src_bin_gegueb_widget_viewer_SOURCES = src/bin/gegueb_widget_viewer.c
src_bin_gegueb_widget_viewer_LDADD = $(top_builddir)/src/lib/libgegueb.la @GEGUEB_BIN_LIBS@
