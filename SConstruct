import os

subdirs = Split ("""
		core
		core/2d
		core/fig
		core/geo
		core/geo_io
		core/jeeps
		core/layers
		core/loaders
		core/utils
		core/mp
		core/gred
		core/gred_tests
		core/options
		programs
		viewer
		vector/libzn
		vector/libzn-utils
		vector/data
		vector/ps
		vector/vmap2
		""")

#SetOption('implicit_cache', 1)
env = Environment ()

######################################

env.PREFIX = ''
env.bindir=env.PREFIX+'/usr/bin'
env.datadir=env.PREFIX+'/usr/share/mapsoft'
env.man1dir=env.PREFIX+'/usr/share/man/man1'

env.Alias('install', [env.bindir, env.man1dir, env.datadir])

######################################

if os.environ.has_key('GCCVER'):
   ver = os.environ['GCCVER']
   env.Replace (CC = ("gcc-%s" % ver))
   env.Replace (CXX = ("g++-%s" % ver))

env.Append (CCFLAGS=['-O2'])

if ARGUMENTS.get('debug', 0):
	env.Append (CCFLAGS='-ggdb')
	env.Append (LINKFLAGS='-ggdb')
if ARGUMENTS.get('profile', 0):
	env.Append (CCFLAGS='-pg')
	env.Append (LINKFLAGS='-pg')
if ARGUMENTS.get('gprofile', 0):
	env.Append (LINKFLAGS='-lprofiler')
if ARGUMENTS.get('gheapcheck', 0):
	env.Append (LINKFLAGS='-ltcmalloc')

env.Append (LIBPATH = map(lambda(s): "#"+s, subdirs))
env.Append (CPPPATH = "#core")

Export('env')

env_gtk = env.Clone()
env_gtk.ParseConfig('pkg-config --cflags --libs gtkmm-2.4,gthread-2.0')
Export('env_gtk')

SConscript (map (lambda(s): s+"/SConscript", subdirs))

