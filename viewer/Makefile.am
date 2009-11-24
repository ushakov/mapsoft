packages = gtkmm-2.4,gthread-2.0

LDADD =\
 ../core/libgeo_io/libgeo_io.a\
 ../core/libmp/libmp.a\
 ../core/libfig/libfig.a\
 ../core/libgeo/libgeo.a\
 ../core/jeeps/libjeeps.a\
 ../core/loaders/libloaders.a\
 ../core/utils/libutils.a\
 ../core/lib2d/lib2d.a\
 -ltiff -ljpeg -lpng\
 -lgd -lcurl -lproj $(GTKMM_LIBS)

INCLUDES = $(GTKMM_CFLAGS)

bin_PROGRAMS = mapsoft_mapview

mapsoft_mapview_SOURCES =\
  mapsoft_mapview.cpp\
  generic_dialog.cpp\
  action_manager.cpp\
  viewer.cpp\
  workplane.cpp

