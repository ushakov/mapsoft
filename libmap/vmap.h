#ifndef VMAP_H
#define VMAP_H

#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

#include "../lib2d/line.h"
#include "../utils/options.h"

//#include "legend.h"

// Classes for vector map common representation

namespace map {

typedef std::string id_t;

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
  * data is stored using one line per point in " <x> <y>" format
  * data segments are separated by empty lines
  * number of spaces between <key> and <value>, between <x> and <y>
    and before <x> does not matter (but must be >0).
*/

std::istream & operator>> (std::istream & s, object & o){
  Line<double> seg;
  char mode;
  while (s.get(mode)){
    std::string line, key, val;

    if (mode=='\n'){
      if (seg.size()>0) { o.data.push_back(seg); seg.clear();}
      continue;
    }

    if (s.peek()!=EOF){
      if (!getline(s, line)) break;
    }
    std::istringstream s1(line);

    switch (mode){
      case '\\': // key value pair
        s1 >> key >> std::ws;
        getline(s1, val);
        if (!s1) goto def;
        o.opts[key]=val;
        break;
      case '#':
        o.comm.push_back(line);
        break;
      case ' ':
        double x,y;
        s1 >> x >> y >> std::ws;
        if (!s1 || !s1.eof()) goto def;
        seg.push_back(Point<double>(x,y));
        break;
      default:
        def:
        std::cerr << "map::object: Skipping bad line on input: " << (char)mode << line << "\n";
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
  label position structure
  * shift in mm
  * angle in degrees
*/

struct label_pos{
  Point<double> shift;
  double angle;
};

/*
  information about raster representation
*/
struct rmap{
  Options opts;
  std::vector<std::string> comm;
  std::multimap<id_t, label_pos> positions;
};

/*
  Input/output.
  * key-value pairs are stored in "\<key> <value>" lines
  * comments are stored in "#<comment>" lines
  * label positions are stored in "+<id> <x> <y> <angle>" format
  * data segments are separated by empty lines
  * number of spaces between <key> and <value>, between <x> and <y>
    and before <x> does not matter (but must be >0).
*/

std::istream & operator>> (std::istream & s, rmap & o){
  char mode;
  while (s.get(mode)){
    std::string line, key, val;
    std::string id;
    label_pos pos;

    if (mode=='\n') continue;

    if (s.peek()!=EOF){
      if (!getline(s, line)) break;
    }
    std::istringstream s1(line);

    switch (mode){
      case '\\': // key value pair
        s1 >> key >> std::ws;
        getline(s1, val);
        if (!s1) goto def;
        o.opts[key]=val;
        break;
      case '#':
        o.comm.push_back(line);
        break;
      case '+':
        s1 >> id >> pos.shift.x >> pos.shift.y >> pos.angle >> std::ws;
        if (!s1 || !s1.eof()) goto def;
        o.positions.insert(std::pair<id_t, label_pos>(id, pos));
        break;
      default:
        def:
        std::cerr << "map::rmap: Skipping bad line on input: " << (char)mode << line << "\n";
        continue;
    }
  }
  return s;
}

std::istream & operator<< (std::ostream & s, const rmap & o){
  for (Options::const_iterator i=o.opts.begin(); i!=o.opts.end(); i++)
    s << '\\' << i->first << ' ' << i->second << '\n';
  for (std::vector<std::string>::const_iterator i=o.comm.begin(); i!=o.comm.end(); i++)
    s << '#' << *i << '\n';

  for (std::multimap<id_t, label_pos>::const_iterator i=o.positions.begin();
                                                      i!=o.positions.end(); i++)
    s << '+' << i->first << ' '
      << i->second.shift.x << ' ' << i->second.shift.y << ' '
      << i->second.angle <<'\n';
}

/*
  The whole map:
  + some Key=Value fields in Options
  + comments
  + objects
  + rmaps
*/
struct world{
  Options                  opts;
  std::vector<std::string> comm;
  std::map<id_t, object>   objects;
  std::map<id_t, rmap>     rmaps;
};

/*
  Input/output (without objects and rmaps!)
  * key-value pairs are stored in "\<key> <value>" lines
  * comments are stored in "#<comment>" lines
*/
std::istream & operator>> (std::istream & s, world & o){
  char mode;
  while (s.get(mode)){
    std::string line, key, val;
    std::string id;

    if (mode=='\n') continue;

    if (s.peek()!=EOF){
      if (!getline(s, line)) break;
    }
    std::istringstream s1(line);

    switch (mode){
      case '\\': // key value pair
        s1 >> key >> std::ws;
        getline(s1, val);
        if (!s1) goto def;
        o.opts[key]=val;
        break;
      case '#':
        o.comm.push_back(line);
        break;
      default:
        def:
        std::cerr << "map::world: Skipping bad line on input: " << (char)mode << line << "\n";
        continue;
    }
  }
  return s;
}

std::istream & operator<< (std::ostream & s, const world & o){
  for (Options::const_iterator i=o.opts.begin(); i!=o.opts.end(); i++)
    s << '\\' << i->first << ' ' << i->second << '\n';
  for (std::vector<std::string>::const_iterator i=o.comm.begin(); i!=o.comm.end(); i++)
    s << '#' << *i << '\n';
}

// i/o for dir format
// Directory structure:
// /dir/info
// /dir/objects/<id>
// /dir/rmaps/<id>
// 

bool write_dir(const std::string & dir, const world & w){

  // create new directories or fail
  if (mkdir(dir.c_str(), 0755)  ||
      mkdir((dir + "/objects").c_str(), 0755) ||
      mkdir((dir + "/rmaps").c_str(), 0755)){
    std::cerr << "map::write_dir: can't create map directory in " << dir << "\n";
    return false;
  }

  std::ofstream fi((dir + "/info").c_str());
  if (!fi){
    std::cerr << "map::write_dir: error while writing map info file\n";
    return false;
  }
  else fi<<w;
  fi.close();

  for (std::map<id_t, object>::const_iterator i=w.objects.begin(); i!=w.objects.end(); i++){
    std::ofstream f((dir + "/objects/" + i->first).c_str());
    if (!f){
      std::cerr << "map::write_dir: error while writing map object" << i->first << "\n";
    }
    else f << i->second;
    f.close();
  }

  for (std::map<id_t, rmap>::const_iterator i=w.rmaps.begin(); i!=w.rmaps.end(); i++){

    std::ofstream f((dir + "/rmaps/" + i->first).c_str());
    if (!f){
      std::cerr << "map::write_dir: error while writing rmap " << i->first << "\n";
    }
    else f << i->second;
    f.close();
  }
  return true;
}

bool read_dir(std::string dir, world & w){

  std::ifstream fi((dir+"/info").c_str());
  if (!fi){
    std::cerr << "map::read_dir: error while reading map info file\n";
    return false;
  }
  fi>>w;

  DIR *d;
  struct dirent *de;

  d=opendir((dir+"/objects").c_str());
  while ((de=readdir(d))!=NULL){
    if ((de->d_name==NULL) || (de->d_name[0]=='.')) continue;
    std::ifstream f((dir+"/objects/"+de->d_name).c_str());
    object o;
    if (!f){
      std::cerr << "map::read_dir: error while reading map object " << de->d_name << "\n";
    }
    else {
      f>>o;
      w.objects[de->d_name]=o;
    }
  }
  closedir(d);

  d=opendir((dir+"/rmaps").c_str());
  while ((de=readdir(d))!=NULL){
    if ((de->d_name==NULL) || (de->d_name[0]=='.')) continue;
    std::ifstream f((dir+"/rmaps/"+de->d_name).c_str());
    rmap m;
    if (!f){
      std::cerr << "map::read_dir: error while reading rmap " << de->d_name << "\n";
    }
    else {
      f>>m;
      w.rmaps[de->d_name]=m;
    }
  }
  closedir(d);
}

} // namespace
#endif
