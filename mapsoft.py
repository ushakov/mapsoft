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
    env.Prepend(LIBS = deps)

def Setup(env):
    env.AddMethod(MProg)
    env.AddMethod(MLib)
    env.AddMethod(MDeps)
