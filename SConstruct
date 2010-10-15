import os

subdirs_min = Split("core programs viewer vector")
subdirs_max = subdirs_min + Split("tests misc")

#SetOption('implicit_cache', 1)
env = Environment ()

######################################

env.PREFIX = ARGUMENTS.get('prefix', '')
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

env.Prepend(LIBPATH=".")

Export('env')

## cairo env
env_cairo = env.Clone()
env_cairo.ParseConfig('pkg-config --cflags --libs cairomm-1.0,freetype2')
Export('env_cairo')

## cairo+gtk env
env_gtk = env_cairo.Clone()
env_gtk.ParseConfig('pkg-config --cflags --libs gtkmm-2.4,gthread-2.0')
Export('env_gtk')

env_list=[env, env_cairo, env_gtk]
Export('env_list')

if ARGUMENTS.get('minimal', 0):
	SConscript (map (lambda(s): s+"/SConscript", subdirs_min))
else:
	SConscript (map (lambda(s): s+"/SConscript", subdirs_max))

Help("""
You can build mapsoft with the following options:
  scons -Q debug=1          // build with -ggdb
  scons -Q profile=1        // build with -pg
  scons -Q gheapcheck=1     // build with -ltcmalloc
  scons -Q minimal=1        // skip misc and tests dirs
  scons -Q prefix=<prefix>  // set installation prefix
""")
