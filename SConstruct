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

######################################

AddOption('--prefix',
          dest='prefix',
          nargs=1, type='string',
          action='store',
          metavar='DIR',
          help='installation prefix')

env.PREFIX = GetOption('prefix')
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

Export('env')
SConscript (map (lambda(s): s+"/SConscript", subdirs))

