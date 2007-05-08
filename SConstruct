import os

subdirs = Split ("""
		geo_io
		jeeps
		layers
		loaders
		programs
		utils
		vector
		viewer
		""")

SetOption('implicit_cache', 1)
env = Environment ()

env.ParseConfig('pkg-config --cflags --libs gtkmm-2.4,gthread-2.0')
env.Append (CPPPATH='#')
env.Append (LIBS=Split('m usb jpeg tiff png'))
env.Append (LIBPATH = map(lambda(s): "#"+s, subdirs))

if ARGUMENTS.get('debug', 0):
	env.Append (CCFLAGS='-ggdb')
	env.Append (LINKFLAGS='-ggdb')

Export('env')
SConscript (map (lambda(s): s+"/SConscript", subdirs))

