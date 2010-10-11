#ifndef VMAP_H
#define VMAP_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "../../core/2d/line.h"
#include "options/options.h"

// Class for vector map common representation

namespace vmap {

/*****************************************************************/

struct mobj_t: dMultiLine {
  Options                  opts; // some Key=Value fields
  std::vector<std::string> comm; // comments
};

struct lp_t : dPoint{ // single label position
  int    align; // 0: left, 1: center, 2: right
  double angle; // angle, degrees
};

struct lpos_t: std::multimap<std::string, lp_t> {
  Options                  opts; // some Key=Value fields
  std::vector<std::string> comm; // comments
};

struct rmap_t{
  Options           opts;        // some Key=Value fields
  std::vector<std::string> comm; // comments
};

struct vmap{
  std::map<std::string, mobj_t>   mobjs; // map objects
  std::map<std::string, lpos_t>   lpos;    // label positions
  std::map<std::string, rmap_t>   rmaps;   // raster maps
  void add(const vmap & M1);
  void cut_inner(const dRect & cutter, bool erase_lpos=true);

};



/*****************************************************************/

std::istream & operator>> (std::istream & s, vmap & o);
std::istream & operator<< (std::ostream & s, const vmap & o);

int vmap_read(const std::string & file, vmap & M);
int vmap_write(const std::string & file, const vmap & M);

/*****************************************************************/

// make unique id
std::string make_id(void);


Options read_fig(const std::string & fig_file, vmap & M); // read fig into new vmap

int put_fig(const std::string & fig_file, vmap & M, bool replace=true);

int get_fig(const std::string & fig_file, const vmap & M,
            const bool toedit=true, const std::string & rmap="DEFAULT");

} // namespace
#endif
