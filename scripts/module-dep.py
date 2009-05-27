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

def AddToClusters(clusters, node, p):
    if len(p) == 0:
        if "." not in clusters:
            clusters["."] = []
        clusters["."].append(node)
        return
    if p[0] not in clusters:
        clusters[p[0]] = {}
    AddToClusters(clusters[p[0]], node, p[1:])

def GatherClusters(all_nodes):
    clusters = {}
    for n in all_nodes:
        p = n.split("/")
        AddToClusters(clusters, n, p)
    return clusters

def PrintClusters(clusters, prefix=""):
    for cname, cluster in clusters.iteritems():
        if cname == ".":
            for n in cluster:
                print '"%s";' % n
        else:
            print 'subgraph "cluster_%s_%s" {' % (prefix, cname)
            PrintClusters(cluster, "%s_%s" % (prefix, cname))
            print "}"

all_nodes = {}
print "digraph G {"
for (s, d, c) in GetUniqLinks("."):
    if s != d: print "  \"%s\" -> \"%s\" [label=\"%s\"]" % (s, d, c)
    all_nodes[s] = 1
    all_nodes[d] = 1

clusters = GatherClusters(all_nodes)
PrintClusters(clusters)
print "}"
