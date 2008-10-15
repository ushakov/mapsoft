import os

subdirs = Split ("""
		lib2d
		libfig
		libmp
		libgeo
		libgeo_io
		jeeps
		layers
		loaders
		utils
		""")

#SetOption('implicit_cache', 1)
env = Environment ()
env.Append (CCFLAGS=['-O2','-g'])
env.Append (CPPPATH='#')
env.Append (LIBPATH = map(lambda(s): "#"+s, subdirs))

mapsoft_libs = Split ("geo_io geo loaders jeeps utils 2d")
other_libs   = Split ("usb tiff jpeg png gd")

env.Prepend(LIBS=[mapsoft_libs,other_libs])
env.ParseConfig('pkg-config --cflags --libs gtkmm-2.4,gthread-2.0')


env.Program('mview.cpp')

Export('env')
SConscript (map (lambda(s): s+"/SConscript", subdirs))

