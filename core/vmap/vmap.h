#ifndef VMAP_H
#define VMAP_H

#include <iostream>
#include <vector>
#include <string>
#include <list>

#include "2d/line.h"
#include "options/options.h"
#include "fig/fig.h"

#include "vmap/zn.h"
#include "ocad/ocad_file.h"


// Class for vector map representation

// TODO - проверить стрелки
// TODO - проверить углы

namespace vmap {

/*****************************************************************/
extern const int label_type;
extern const int border_type;

extern const double label_search_dist1;
extern const double label_search_dist2;
extern const double label_search_dist3;
extern const double label_len;
extern const double label_new_dist;

extern const double default_rscale;
extern const std::string default_style;

struct lpos {
  int    dir; // 0: left, 1: center, 2: right
  double ang; // angle, degrees
  dPoint pos;
  bool   hor; // horisontal text
  bool operator< (const lpos & o) const;
  lpos();
};

struct lpos_full : public lpos { // for internal use
  dPoint ref;
  std::string text;
  bool operator< (const lpos_full & o) const;
};

typedef enum{
  POI = 0,
  POLYLINE = 1,
  POLYGON = 2
} object_class;

struct object: public dMultiLine {
  int             type;  // MP type + 0x100000 for lines, 0x200000 for polygons
  int             dir;   // direction from mp, arrows from fig
  std::string     text;  // label text
  std::list<lpos> labels;
  Options                  opts; // some Key=Value fields
  std::vector<std::string> comm; // comments

  object_class get_class() const;
  bool operator< (const object & o) const;
  object();
};

struct world : std::list<object> {
  int         mp_id;
  std::string name;
  std::string style;
  double      rscale;
  dLine       brd;
  std::list<lpos_full> lbuf; // buffer for ownerless labels

  world();

  dRect range() const;

  // Add objects and lbuf from the world W, set style etc from W
  void add(const world & W);
};


// Reading and writing fig (see vmap_fig.cpp):
world read(const fig::fig_world & F);
int write(fig::fig_world & F, const world & W, const Options & O = Options());

// Reading and writing mp (see vmap_mp.cpp):
world read(const mp::mp_world & M);
int write(mp::mp_world & M, const world & W, const Options & O = Options());

// Reading and writing ocad (see vmap_ocad.cpp) -- EXPERIMENTAL:
world read(const ocad::ocad_file & F);
int write(ocad::ocad_file & F, const world & W, const Options & O = Options());

// note: fig and mp write functions can't write lbuf yet. It is not
// a problem becouse vmap_copy always do add_labels

// Reading and writitng native format
world read(std::istream & IN);
int write(std::ostream & OUT, const world & W);

// Reading and writing any file (see vmap_file.cpp).
// Format is determined by extension (fig, mp).
// Options are passed to corresponding write() function.
world read(const char * fname);
int  write(const char * fname, const world & W, const Options & O = Options());


// Functions for labels and pics handling (see vmap_labels.cpp):
void join_labels(world & W);   ///< join labels from lbuf to objects
void split_labels(world & W);  ///< split labels from objects to lbuf
void create_labels(world & W); ///< create new labels
void remove_labels(world & W); ///< remove all lables
void move_pics(world & W);     ///< move and rotate some signs


// filtering and statistics (see vmap_filt.cpp):
void filter(world & W, const Options & O);
void join_objects(world & W, double dist);
void remove_tails(world & W, double dist, const dRect & cutter, Conv * cnv = NULL);
void range_action(world & W, std::string action, const dRect & cutter, Conv * cnv = NULL);
// remove empty lines and objects
void remove_empty(world & W);
//note: remove_tails and range_action do remove_empty
// remove inessential changes between WOLD and WNEW:
void fix_diff(const world & WOLD, world & WNEW, double dist);

/// create tmerc ref from brd or from map range (see vmap_ref.h):
g_map mk_tmerc_ref(const world & W, double u_per_cm, bool yswap=false);

// put source to options if it is not empty
void set_source(Options & o, const std::string & source);

// convert vector between meters and degrees (approximate)
// move to lib2d?
dPoint v_m2deg(const dPoint & v, const double lat);
dPoint v_deg2m(const dPoint & v, const double lat);

// approximate distance in m
double dist_pt(const dPoint & v, double lat);

// approximate distance in m
double dist_pt_pt(const dPoint & p1, const dPoint & p2);

// Find point n in the line l which is nearest to the point p.
// Return distance in m
double dist_pt_l(const dPoint & p, const dMultiLine & l, dPoint & n);


} // namespace
#endif
