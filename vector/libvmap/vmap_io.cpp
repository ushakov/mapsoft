#include <sstream>
#include <fstream>
#include <iomanip>

#include "vmap.h"

using namespace std;
namespace vmap {

// OBJECT
istream & operator>> (istream & s, object & o){
  Line<double> seg;
  char mode;
  while (s.get(mode)){
    string line, key, val;

    if (mode=='\n'){
      if (seg.size()>0) { o.data.push_back(seg); seg.clear();}
      continue;
    }

    if (s.peek()!=EOF){
      if (!getline(s, line)) break;
    }
    istringstream s1(line);

    switch (mode){
      case '\\': // key value pair
        s1 >> key >> ws;
        getline(s1, val);
        if (!s1) goto def;
        o.opts[key]=val;
        break;
      case '#':
        o.comm.push_back(line);
        break;
      case ' ':
        double x,y;
        s1 >> x >> y >> ws;
        if (!s1 || !s1.eof()) goto def;
        seg.push_back(Point<double>(x,y));
        break;
      default:
        def:
        cerr << "map::object: Skipping bad line on input: " << (char)mode << line << "\n";
        continue;
    }
  }
  if (seg.size()>0) { o.data.push_back(seg); seg.clear();}
  return s;
}

// OBJECT
istream & operator<< (ostream & s, const object & o){
  for (vector<string>::const_iterator i=o.comm.begin(); i!=o.comm.end(); i++)
    if (i->size()>0) s << '#' << *i << '\n';
  for (Options::const_iterator i=o.opts.begin(); i!=o.opts.end(); i++)
    s << '\\' << i->first << ' ' << i->second << '\n';
  for (MultiLine<double>::const_iterator l=o.data.begin(); l!=o.data.end(); l++){
    for (Line<double>::const_iterator i=l->begin(); i!=l->end(); i++)
      s << ' ' << setprecision(6) << fixed << i->x << ' ' << i->y << '\n';
    s << '\n';
  }
}

// RMAP
istream & operator>> (istream & s, rmap & o){
  char mode;
  while (s.get(mode)){
    string line, key, val;
    string id;
    label_pos pos;

    if (mode=='\n') continue;

    if (s.peek()!=EOF){
      if (!getline(s, line)) break;
    }
    istringstream s1(line);

    switch (mode){
      case '\\': // key value pair
        s1 >> key >> ws;
        getline(s1, val);
        if (!s1) goto def;
        o.opts[key]=val;
        break;
      case '#':
        o.comm.push_back(line);
        break;
      case '+':
        s1 >> id >> pos.x >> pos.y >> pos.angle >> ws;
        if (!s1 || !s1.eof()) goto def;
        o.positions.insert(pair<id_t, label_pos>(id, pos));
        break;
      default:
        def:
        cerr << "map::rmap: Skipping bad line on input: " << (char)mode << line << "\n";
        continue;
    }
  }
  return s;
}

// RMAP
istream & operator<< (ostream & s, const rmap & o){
  for (vector<string>::const_iterator i=o.comm.begin(); i!=o.comm.end(); i++)
    if (i->size()>0) s << '#' << *i << '\n';
  for (Options::const_iterator i=o.opts.begin(); i!=o.opts.end(); i++)
    s << '\\' << i->first << ' ' << i->second << '\n';

  for (multimap<id_t, label_pos>::const_iterator i=o.positions.begin();
                                                      i!=o.positions.end(); i++)
    s << '+' << i->first << ' '
      << setprecision(6) << fixed << i->second.x << ' ' << i->second.y << ' '
      << setw(5) << setprecision(1) << fixed << i->second.angle <<'\n';
}

// WORLD
istream & operator>> (istream & s, world & o){
  char mode;
  while (s.get(mode)){
    string line, key, val;
    string id;

    if (mode=='\n') continue;

    if (s.peek()!=EOF){
      if (!getline(s, line)) break;
    }
    istringstream s1(line);

    switch (mode){
      case '\\': // key value pair
        s1 >> key >> ws;
        getline(s1, val);
        if (!s1) goto def;
        o.opts[key]=val;
        break;
      case '#':
        o.comm.push_back(line);
        break;
      default:
        def:
        cerr << "map::world: Skipping bad line on input: " << (char)mode << line << "\n";
        continue;
    }
  }
  return s;
}

// WORLD
istream & operator<< (ostream & s, const world & o){
  for (vector<string>::const_iterator i=o.comm.begin(); i!=o.comm.end(); i++)
    if (i->size()>0) s << '#' << *i << '\n';
  for (Options::const_iterator i=o.opts.begin(); i!=o.opts.end(); i++)
    s << '\\' << i->first << ' ' << i->second << '\n';
}

} // namespace
