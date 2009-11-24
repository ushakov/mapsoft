AC_INIT([mapsoft], [0.1], [slazav@altlinux.org])

AM_INIT_AUTOMAKE()

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

PKG_CHECK_MODULES(GTKMM, gtkmm-2.4 gthread-2.0)
AC_SUBST(GTKMM_CFLAGS)
AC_SUBST(GTKMM_LIBS)

AC_CONFIG_FILES([
  Makefile
  core/Makefile
  core/jeeps/Makefile
  core/lib2d/Makefile
  core/utils/Makefile
  core/libgeo/Makefile
  core/libgeo_io/Makefile
  core/layers/Makefile
  core/loaders/Makefile
  core/libfig/Makefile
  core/libmp/Makefile
  programs/Makefile
  viewer/Makefile
])
AC_OUTPUT
