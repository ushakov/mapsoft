import os
import mapsoft

subdirs = Split ("""
                core
		tests
                misc
		programs
		viewer
		vector
		""")

#SetOption('implicit_cache', 1)
env = Environment()
mapsoft.Setup(env)

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

env.Prepend(LIBPATH=".")

env.ExtLib('cairo', 'cairomm-1.0,freetype2')

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

SConscript (map (lambda(s): s+"/SConscript", subdirs))

mapsoft.Finish()
