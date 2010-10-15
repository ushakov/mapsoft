import collections


class DepsInfo(object):
    def __init__(self):
        self.deps = collections.defaultdict(list)

    def GetDeps(self, target):
        """Returns full transitive deps"""

        # expanded is the set of targets which dependencies have been
        # fetched (no one can enter this set twice!)
        expanded = set([target])

        # inv.: resulting_deps are (transitive) dependencies of
        # target, all direct dependencies of those that are not in
        # resulting_deps are in new_targets.
        resulting_deps = set([target])
        new_targets = self.deps[target]
        while new_targets:
            next_new_targets = []
            for new_target in new_targets:
                if new_target in expanded:
                    raise Exception("Circular dependecy in expansion of %s" % new_target)
                deps = self.deps[new_target]
                resulting_deps.append(new_target)
                if deps:
                    expanded.append(new_target)
                    next_new_targets.extend(deps)
        return list(resulting_deps)

    def AddDeps(self, target, deps):
        assert target not in self.deps
        self.deps[target] = deps


GLOBAL_DEPS = DepsInfo()


def MProg(env, name, srcs = None, deps = None):
    if deps is not None:
        env.Prepend(LIBS = deps)
    if srcs is not None:
        env.Program(target = name, 
                    source = srcs)
    else:
        env.Program(name)


def MLib(env, name, srcs = None, deps = None):
    if deps is not None:
        env.Prepend(LIBS = deps)
    if srcs is not None:
        env.Library(target = name, 
                    source = srcs)
    else:
        env.Library(name)


def MDeps(env, deps):
    GLOBAL_DEPS.AddDeps(???
    

def Setup(env):
    env.AddMethod(MProg)
    env.AddMethod(MLib)
    env.AddMethod(MDeps)
