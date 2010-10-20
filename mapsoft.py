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

    def ShouldRemoveSelf(self):
        return False


class LocalTarget(Target):
    def __init__(self, env, name):
        super(LocalTarget, self).__init__(env, name)


class ExternalLib(Target):
    def __init__(self, env, name):
        super(ExternalLib, self).__init__(env, name)

    def ParseFlags(self, arg):
        self.add_dict = self.env.ParseFlags(arg)

    def ShouldRemoveSelf(self):
        return True


class RemainingVertices(object):
    def __init__(self):
        self.vertex = None
        self.remaining = None


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
        debug = False

        # Topologically sort the deps graph. Algorithm: run dfs, when
        # going back from each vertex, add it to start of
        # results. seen contain the set of already visited
        # vertices. stack is the backtrack stack (contains remaining
        # lists at each vertex).
        stack = []
        color = {}
        GREY = 'grey'
        BLACK = 'black'
        result = []
        rem = RemainingVertices()
        rem.vertex = target.name
        rem.remaining = target.deps
        stack.append(rem)
        color[target.name] = GREY
        while stack:
            rem = stack[-1]
            if debug:
                print 'looking at %s (%s remaining)' % (rem.vertex, len(rem.remaining))
            if len(rem.remaining) == 0:
                result = [rem.vertex] + result
                stack = stack[:-1]
                color[rem.vertex] = BLACK
                if debug:
                    print 'added %s' % rem.vertex
                continue
            to_visit = rem.remaining[0]
            rem.remaining = rem.remaining[1:]
            if to_visit not in color:
                if debug:
                    print '   visiting %s' % to_visit
                color[to_visit] = GREY
                new_rem = RemainingVertices()
                new_rem.vertex = to_visit
                if to_visit not in self.deps:
                    raise Exception('Dep not found: %s' % trg)
                new_rem.remaining = self.deps[to_visit]
                stack.append(new_rem)
            elif color[to_visit] == GREY:
                loop = []
                cur = len(stack) - 1
                while cur >= 0 and stack[cur].vertex != to_visit:
                    loop.append(stack[cur].vertex)
                    cur -= 1
                loop.append(to_visit)
                if cur < 0:
                    raise Exception("Logic error in unlooping %s -> %s" % (
                        target.name, to_visit))
                loop.reverse()
                raise Exception("loop! %s" % loop)
        assert result[0] == target.name
        result = result[1:]
        if debug:
            print '%s: %s' % (target.name, result)
        return result


DEPS = DepsTracker()
        

def _ApplyFlags(target, deps):
    outdeps = []
    target.Apply(target.env)
    for name in deps:
        dep = DEPS.targets[name]
        dep.Apply(target.env)
        if not dep.ShouldRemoveSelf():
            outdeps.append(name)
    return outdeps


def _ParseKW(target, kw):
    if 'deps' in kw:
        target.deps = kw['deps']
    if 'cflags' in kw:
        target.compile_flags = kw['cflags']
    if 'lflags' in kw:
        target.link_flags = kw['lflags']


def _ResolveDeps(target):
    deps = DEPS.GetDeps(target)
    deps = _ApplyFlags(target, deps)
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


def ExtLibPkgConfig(parent_env, name, pkg_config_deps):
    env = parent_env.Clone()
    target = ExternalLib(env, name)
    if isinstance(pkg_config_deps, str):
        pkg_config_str = pkg_config_deps
    else:
        pkg_config_str = ','.join(pkg_config_deps)
    target.ParseFlags('!pkg-config --libs --cflags %s' % pkg_config_str)
    DEPS.RegisterTarget(target)


def ExtLib(parent_env, name, cflags = "", lflags = ""):
    env = parent_env.Clone()
    target = ExternalLib(env, name)
    target.compile_flags = cflags
    target.link_flags = lflags
    DEPS.RegisterTarget(target)


def Setup(env):
    env.AddMethod(MProg)
    env.AddMethod(MLib)
    env.AddMethod(ExtLib)
    env.AddMethod(ExtLibPkgConfig)


def Finish():
    for name,  target in DEPS.targets.items():
        _ResolveDeps(target)
