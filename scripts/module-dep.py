#!/usr/bin/python

import os
import sys
import re

def GetModules(topdir):
    for (d, dirs, files) in os.walk(topdir):
        srcs = []
        for f in files:
            if f.endswith(".cpp") or f.endswith(".h") or f.endswith(".c"):
                srcs.append(f)
        yield (d, srcs)

def GetIncludes(file):
    f = open(file, "r")
    include = re.compile("^ *#include \"(.*)\"")
    for line in f:
        m = include.search(line)
        if m:
            yield m.group(1)
    f.close()

def GetLinks(top):
    cd = re.compile("\./(.*)")
    for (module, files) in GetModules("."):
        m = cd.match(module)
        if m:
            module = m.group(1)
        for f in files:
            fn = os.path.join(module, f)
            for n in GetIncludes(fn):
                p = os.path.normpath(os.path.join(module, n))
                target_module = os.path.dirname(p)
                yield (module, target_module)

def GetUniqLinks(top):
    t = {}
    for (s, d) in GetLinks(top):
        key = "%s %s" % (s, d)
        t[key] = t.get(key, 0) + 1
    for (key, count) in t.iteritems():
        (s, d) = key.split()
        yield s, d, count

clean = re.compile("-")

print "digraph G {"
for (s, d, c) in GetUniqLinks("."):
    print "  \"%s\" -> \"%s\" [label=\"%s\"]" % (s, d, c) 
print "}"
