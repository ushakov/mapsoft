import os

subdirs = Split ("""
		lib2d
		libfig
		libmp
		libvmap
		libgeo
		libgeo_io
		jeeps
		layers
		loaders
		programs
		utils
		viewer
		libzn
		libzn-utils
		libzn-data
		""")

#SetOption('implicit_cache', 1)
env = Environment ()

if os.environ.has_key('GCCVER'):
   ver = os.environ['GCCVER']
   env.Replace (CC = ("gcc-%s" % ver))
   env.Replace (CXX = ("g++-%s" % ver))

env.Append (CCFLAGS=['-O2','-g'])
# env.Append (CPPPATH='#')
env.Append (LIBPATH = map(lambda(s): "#"+s, subdirs))

#env.ParseConfig('pkg-config --cflags --libs gtkmm-2.4,gthread-2.0')
# libgd is bad: its --libs does not include the library itself!
#env.ParseConfig('gdlib-config --cflags --ldflags --libs')
#env.Append (LIBS = '-lgd')
#env.Append (LIBS=Split('m curl'))
#
#env.Append (LINKFLAGS=['-O2', '-ggdb'])
#
#if ARGUMENTS.get('debug', 0):
#	env.Append (CCFLAGS='-ggdb')
#	env.Append (LINKFLAGS='-ggdb')
#
#if ARGUMENTS.get('profile', 0):
#	env.Append (CCFLAGS='-pg')
#	env.Append (LINKFLAGS='-pg')
#
if ARGUMENTS.get('gprofile', 0):
	env.Append (LINKFLAGS='-lprofiler')
if ARGUMENTS.get('gheapcheck', 0):
	env.Append (LINKFLAGS='-ltcmalloc')


Export('env')
SConscript (map (lambda(s): s+"/SConscript", subdirs))

