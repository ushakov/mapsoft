import collections
from SCons.Script import *


class Target(object):
    def __init__(self, env, name):
        self.name = name
        self.compile_flags = ""
        self.link_flags = ""
        self.deps = []
        self.env = env
        self.add_dict = {}

    def Apply(self, env):
        env.Prepend(LINKFLAGS = self.link_flags)
        env.Prepend(CCFLAGS = self.compile_flags)
        env.Prepend(**self.add_dict)


class LocalTarget(Target):
    def __init__(self, env, name):
        super(LocalTarget, self).__init__(env, name)


class ExternalLib(Target):
    def __init__(self, env, name):
        super(ExternalLib, self).__init__(env, name)

    def ParseFlags(self, arg):
        self.add_dict = self.env.ParseFlags(arg)


class DepsTracker(object):
    def __init__(self):
        self.targets = {}
        self.deps = {}

    def RegisterTarget(self, target):
        self.targets[target.name] = target
        self.deps[target.name] = target.deps

    def GetDeps(self, target):
        if target.name not in self.deps:
            raise Exception('Unknown target %s' % target.name)
        need_resolving = set(self.deps[target.name])
        results = [target.name]
        seen = set(target.name)
        while need_resolving:
            next_need_resolving = []
            for trg in need_resolving:
                if trg not in self.deps:
                    raise Exception('Dep not found: %s' % trg)
                if trg in seen:
                    continue
                results.append(trg)
                seen.add(trg)
                new_deps = self.deps[trg]
                next_need_resolving.extend(new_deps)
            need_resolving = next_need_resolving
        return results


DEPS = DepsTracker()
        

def _ApplyFlags(env, deps):
    for name in deps:
        target = DEPS.targets[name]
        target.Apply(env)


def _ParseKW(target, kw):
    if 'deps' in kw:
        target.deps = kw['deps']
    if 'cflags' in kw:
        target.compile_flags = kw['cflags']
    if 'lflags' in kw:
        target.link_flags = kw['lflags']


def _ResolveDeps(target):
    deps = DEPS.GetDeps(target)
    _ApplyFlags(target.env, deps)
    target.env.Prepend(LIBS = deps)


def MProg(parent_env, name, **kw):
    env = parent_env.Clone()
    target = LocalTarget(env, name)
    _ParseKW(target, kw)
    env.Program(name, kw['srcs'])
    DEPS.RegisterTarget(target)


def MLib(parent_env, name, **kw):
    env = parent_env.Clone()
    target = LocalTarget(env, name)
    _ParseKW(target, kw)
    env.Library(name, kw['srcs'])
    DEPS.RegisterTarget(target)


def ExtLib(parent_env, name, pkg_config_deps):
    env = parent_env.Clone()
    target = ExternalLib(env, name)
    if isinstance(pkg_config_deps, str):
        pkg_config_str = pkg_config_deps
    else:
        pkg_config_str = ','.join(pkg_config_deps)
    target.ParseFlags('!pkg-config --libs --cflags %s' % pkg_config_str)
    DEPS.RegisterTarget(target)

def Setup(env):
    env.AddMethod(MProg)
    env.AddMethod(MLib)
    env.AddMethod(ExtLib)


def Finish():
    for name,  target in DEPS.targets.items():
        _ResolveDeps(target)
