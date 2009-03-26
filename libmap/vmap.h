#ifndef VMAP_H
#define VMAP_H

#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "../lib2d/line.h"
#include "../utils/options.h"

//#include "legend.h"

// Classes for vector map common representation

namespace map {

typedef unsigned long long id_t;

/*
  Single object:
  + some Key=Value fields in Options
  + data
  + comments
  Supports multiple data segments
*/
struct object{
  Options                  opts;
  std::list<Line<double> > data;
  std::vector<std::string> comm;
};

/*
  Input/output.
  * key-value pairs are stored in "\<key> <value>" lines
  * comments are stored in "#<comment>" lines
  * data is stored using lone line per point in " <x> <y>" format
  * data segments are separated by empty lines
  * number of spaces between <key> and <value>, between <x> and <y>
    and before <x> does not matter (but must be >0).
*/

std::istream & operator>> (std::istream & s, object & o){
  s.unsetf(std::ios::skipws);
  Line<double> seg;
  while (!s.eof()){
    std::string line, key, val;
    int mode=s.get();
    if (!getline(s, line)) break;
     std::istringstream s1(line);

    switch (mode){
      case '\\': // key value pair
        s1 >> key >> std::ws;
        getline(s1, val);
        o.opts[key]=val;
        break;
      case '#':
        o.comm.push_back(line);
        break;
      case ' ':
        double x,y;
        s1 >> x >> y;
        if (!s1 || !s1.eof()) goto def;
        seg.push_back(Point<double>(x,y));
        break;
      case '\n':
        if (seg.size()>0) { o.data.push_back(seg); seg.clear();}
        break;
      case EOF:
        break;
      default:
        def:
        std::cerr << "map::object: Skipping bad line: " << (char)mode << line << "\n";
        continue;
    }
  }
  if (seg.size()>0) { o.data.push_back(seg); seg.clear();}
  return s;
}

std::istream & operator<< (std::ostream & s, const object & o){
  for (Options::const_iterator i=o.opts.begin(); i!=o.opts.end(); i++)
    s << '\\' << i->first << ' ' << i->second << '\n';
  for (std::vector<std::string>::const_iterator i=o.comm.begin(); i!=o.comm.end(); i++)
    s << '#' << *i << '\n';
  for (std::list<Line<double> >::const_iterator l=o.data.begin(); l!=o.data.end(); l++){
    for (Line<double>::const_iterator i=l->begin(); i!=l->end(); i++)
      s << ' ' << i->x << ' ' << i->y << '\n';
    s << '\n';
  }
}


/*
  The whole map:
  + list of objects
  + some Key=Value fields in Options
  + comments
*/
struct world : std::list<object>{
  Options                  opts;
  std::vector<std::string> comm;


/* TODO
  bool read_mp(const char* filename,
    const Legend & legend, const Options & opts);
  bool write_mp(const char* filename,
    const Legend & legend, const Options & opts) const;

  bool read_fig(const char* filename,
    const Legend & legend, const Options & opts);
  bool write_fig(const char* filename,
    const Legend & legend, const Options & opts) const;
*/
};

} // namespace
#endif
