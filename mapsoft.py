import collections


class Target(object):
    def __init__(self, env, name):
        self.name = name
        self.compile_flags = ""
        self.link_flags = ""
        self.libs = ""
        self.srcs = ""
        self.env = env

    def Add(self, env):
        env.Prepend(LIBS = self.deps)
        env.Prepend(LINKFLAGS = self.link_flags)
        env.Prepend(CCFLAGS = self.compile_flags)


class LocalTarget(Target):
    def __init__(self, env, name):
        super(Target, self).__init__(env, name)


class ExternalLib(Target):
    def __init__(self, env, name):
        super(Target, self).__init__(env, name)

    def ParseFlags(self, arg):
        self.env.ParseFlags(arg)


class DepTracker(object):
    def __init__(self):
        self.deps = {}

    def RegisterTarget(self, target, deps):
        self.deps[target.name] = deps

    def GetDeps(self, target):
        if target.name not in self.deps:
            raise 'Unknown target %s' % target.name
        

def _ParseKW(target, kw):
    if 'deps' in kw:
        target.libs = kw['deps']
    if 'srcs' in kw:
        target.srcs = kw['srcs']
    if 'cflags' in kw:
        target.compile_flags = kw['cflags']
    if 'lflags' in kw:
        target.link_flags = kw['lflags']


def _ResolveDeps(env, target):
    def func(trg, src, env):
        print target
        raise "not implemented"
    return env.Action(func)


def MProg(env, name, **kw):
    target = LocalTarget(env, name)
    _ParseKW(target, kw)
    env.Program(name, kw['srcs'])
    env.PreAction(name, _ResolveDeps(env, target))


def MLib(env, name, srcs = None, deps = None):
    target = LocalTarget(env, name)
    _ParseKW(target, kw)
    env.Library(name, kw['srcs'])
    env.PreAction(name, _ResolveDeps(env, target))


def ExtLib(env, name, pkg_config_deps):
    target = ExternalLib(env, name)
    if isinstance(pkg_config_deps, str):
        pkg_config_str = pkg_config_deps
    else:
        pkg_config_str = ','.join(pkg_config_deps)
    target.ParseFlags('pkg-config --libs --cflags %s' % pkg_config_str)
    DEPS.RegisterTarget(target, [])

def Setup(env):
    env.AddMethod(MProg)
    env.AddMethod(MLib)
