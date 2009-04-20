#ifndef VMAP_H
#define VMAP_H

#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "../lib2d/line.h"
#include "../utils/options.h"

#include "legend.h"

// Classes for vector map common representation

namespace vmap {

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
  MultiLine<double>        data;
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

std::istream & operator>> (std::istream & s, object & o);
std::istream & operator<< (std::ostream & s, const object & o);

/*
  label position structure
  * coordinates
  * angle in degrees
*/

struct label_pos : Point<double>{
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

std::istream & operator>> (std::istream & s, rmap & o);
std::istream & operator<< (std::ostream & s, const rmap & o);

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
std::istream & operator>> (std::istream & s, world & o);
std::istream & operator<< (std::ostream & s, const world & o);

// make unique id
id_t make_id(void);

// i/o for dir format
// Directory structure:
// /dir/info
// /dir/objects/<id>
// /dir/rmaps/<id>

bool write_dir(const std::string & dir, const world & w);
bool read_dir(const std::string & dir, world & w);

bool write_mp(const std::string & file, const legend & leg, const world & w);
bool read_mp(const std::string & file, const legend & leg, world & w);

bool write_fig(const std::string & file, const legend & leg, const world & w);
bool read_fig(const std::string & file, const legend & leg, world & w);


} // namespace
#endif
