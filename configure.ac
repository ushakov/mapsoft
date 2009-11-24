AC_INIT([mapsoft], [0.1], [slazav@altlinux.org])

AM_INIT_AUTOMAKE()

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

#libs = $(subst %, -l%,\
#  geo_io geo loaders jeeps fig mp utils 2d)
#libdirs = $(subst %, -L../core/%,\
#  libgeo_io libgeo loaders jeeps libfig libmp utils lib2d)

#AC_LIBOBJ([core/lib2d/lib2d.a])
#AC_LIBOBJ([core/libgeo_io/libgeo_io.a])

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
])
AC_OUTPUT
