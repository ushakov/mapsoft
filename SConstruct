import os

subdirs = Split ("""
	  	core
		core/lib2d
		core/libfig
		core/libgeo
		core/libgeo_io
		core/jeeps
		core/layers
		core/loaders
		core/utils
		core/libmp
		programs
		viewer
		vector/libzn
		vector/libzn-utils
		vector/libzn-data
		vector/ps
		vector/vmap2
		""")

#SetOption('implicit_cache', 1)
env = Environment ()

if os.environ.has_key('GCCVER'):
   ver = os.environ['GCCVER']
   env.Replace (CC = ("gcc-%s" % ver))
   env.Replace (CXX = ("g++-%s" % ver))

env.Append (CCFLAGS=['-O2','-g','-fPIC'])
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

