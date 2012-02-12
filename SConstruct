import os


def UseLibs(env, libs):
   if isinstance(libs, list):
      libs = " ".join(libs)
   env.ParseConfig('pkg-config --cflags --libs %s' % libs)

# Arguments target and linkname follow standard order (look at ln(1))
# wd is optional base directory for both
def SymLink(env, target, linkname, wd=None):
   if wd: 
      env.Command(wd+'/'+linkname, wd+'/'+target, "ln -s %s %s/%s" % (target, wd, linkname))
   else:
      env.Command(linkname, target, "ln -s %s %s" % (target, linkname))

subdirs_min = Split("core programs viewer vector")
subdirs_max = subdirs_min + Split("tests misc")

#SetOption('implicit_cache', 1)
env = Environment ()
env.AddMethod(UseLibs)
env.AddMethod(SymLink)

######################################

env.PREFIX = ARGUMENTS.get('prefix', '')
env.bindir=env.PREFIX+'/usr/bin'
env.datadir=env.PREFIX+'/usr/share/mapsoft'
env.man1dir=env.PREFIX+'/usr/share/man/man1'
env.figlibdir=env.PREFIX+'/usr/share/xfig/Libraries'

env.Alias('install', [env.bindir, env.man1dir, env.datadir, env.figlibdir])

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

# vim: ft=python tw=0
