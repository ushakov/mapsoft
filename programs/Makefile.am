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
 -lgd -lcurl -lproj

bin_PROGRAMS = mapsoft_convert mapsoft_add2fig

mapsoft_convert_SOURCES = mapsoft_convert.cpp
mapsoft_add2fig_SOURCES = mapsoft_add2fig.cpp



#===================================

#mapsoft_libs = Split ("geo_io geo loaders jeeps fig mp utils 2d")
#other_libs   = Split ("usb tiff jpeg png gd curl proj")

#env_gtk.Prepend(LIBS=["viewer", mapsoft_libs,other_libs])
#env_gtk.ParseConfig('pkg-config --cflags --libs gtkmm-2.4,gthread-2.0')

#env_geo.Prepend(LIBS=[mapsoft_libs,other_libs])


#env_geo.Program('mapsoft_add2fig.cpp')
#env_geo.Program('mapsoft_convert.cpp')
#env_geo.Program('mapsoft_pt2map.cpp')
## le.Program('mapsoft_imgview', 'mapsoft_imgview.cpp')
#env_geo.Program('mapsoft_mapcnv', 'mapsoft_mapcnv.cpp')
#env_geo.Program('mapsoft_mapcnv1', 'mapsoft_mapcnv1.cpp')
#env_geo.Program('mapsoft_getks.cpp')
#env_geo.Program('mapsoft_getks_ll.cpp')
#env_geo.Program('mapsoft_getks_nom.cpp')
#env_geo.Program('mapsoft_getgoogle.cpp')
#env_geo.Program('mapsoft_mkmap.cpp')
#env_geo.Program('mapsoft_ref.cpp')
#env_geo.Program('mapsoft_toxyz.cpp')
#env_geo.Program('mapsoft_map2fig.cpp')
#env_geo.Program('mapsoft_srtm2fig.cpp')
#env_geo.Program('mapsoft_fig2fig.cpp')
#env_geo.Program('mapsoft_calc_nom2ll.cpp')
##env_geo.Program('mapsoft_calc_wu2pg.cpp')
#env_geo.Program('mapsoft_measure.cpp')
#env_geo.Program('mapsoft_trackprofile.cpp')
#env_geo.Program('mapsoft_srtmprofile.cpp')

#env_geo.Program('mapsoft_pano.cpp')

#env_geo.Program("convs_pt2pt.cpp")
#env_geo.Program("convs_map2pt.cpp")
#env_geo.Program("convs_nom.cpp")

#env.Program('fig_crop.cpp', LIBS=['fig', 'utils'])
#env.Program('fig_sort.cpp', LIBS=['fig', 'utils'])
#env_geo.Program('fig_ref_mod.cpp')

#env.Alias('install', ['/usr/bin/', '/usr/share/man/man1/'])
#env.Install('/usr/bin/', ['mapsoft_mapview', 'mapsoft_convert'])
#env.Install('/usr/share/man/man1/', ['mapsoft_convert.1'])
#
#env.Install('/usr/bin/', 'convs_pt2pt')
#env.Install('/usr/bin/', 'convs_map2pt')
#env.Install('/usr/bin/', 'convs_nom')
