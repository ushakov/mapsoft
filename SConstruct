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

# libgd is bad: its --libs does not include the library itself!
env.ParseConfig('gdlib-config --cflags --ldflags --libs')
env.Append (LIBS = '-lgd')

env.Append (CPPPATH='#')
env.Append (LIBS=Split('m usb jpeg tiff png'))
env.Append (LIBPATH = map(lambda(s): "#"+s, subdirs))

env.Append (CCFLAGS=['-O2', '-ggdb'])
env.Append (LINKFLAGS=['-O2', '-ggdb'])

if ARGUMENTS.get('debug', 0):
	env.Append (CCFLAGS='-ggdb')
	env.Append (LINKFLAGS='-ggdb')

if ARGUMENTS.get('profile', 0):
	env.Append (CCFLAGS='-pg')
	env.Append (LINKFLAGS='-pg')

if ARGUMENTS.get('googleprofile', 0):
	env.Append (LINKFLAGS='-lprofiler')


Export('env')
SConscript (map (lambda(s): s+"/SConscript", subdirs))

