#ifndef VMAP_H
#define VMAP_H

#include <vector>
#include <string>
#include <list>

#include "lib2d/line.h"
//#include "options/options.h"
#include "libfig/fig.h"

//#include "libgeo_io/geofig.h"
//#include "libgeo/geo_convs.h"
#include "../libzn/zn.h"

// Class for vector map representation

// TODO - проверить стрелки
// TODO - проверить углы

namespace vmap {

/*****************************************************************/

extern const int label_type;
extern const double label_search_dist1;
extern const double label_search_dist2;
extern const double label_search_dist3;
extern const double label_len;
extern const double label_new_dist;
extern const bool fig_text_labels;

struct lpos {
  int    dir; // 0: left, 1: center, 2: right
  double ang; // angle, degrees
  dPoint pos;
};

struct lpos_full : public lpos { // for internal use
  dPoint ref;
  std::string text;
};

struct object: dMultiLine {
  int             type;  // MP type + 0x100000 for lines, 0x200000 for polygons
  int             dir;   // direction from mp, arrows from fig
  std::string     text;  // label text
  std::list<lpos> labels;
  Options                  opts; // some Key=Value fields
  std::vector<std::string> comm; // comments
};

struct world : std::list<object> {
  std::string style;
  double rscale;

  /*
   Процедура нахождения соответствия между подписями и объектами:
   1. Каждый объект берет по одной подписи с правильным текстом и с точкой
      привязки не далее label_search_dist1 от него.
   2. Каждый объект берет все остальные подписи с правильным текстом и с
      точкой привязки или положением не далее label_search_dist2 от него.
   3. Каждый объект берет все подписи, с точкой привязки не далее
      label_search_dist3 от него.
   4. Невостребованные подписи теряются.

    Считается что label_search_dist2 существенно больше label_search_dist3, так что
    для того, чтобы не потерять подпись, надо либо двигать, либо
    переименовывать объект, но не делать это одновременно.
  */
  // add labels to objects
  int add_labels(world & objects,  std::list<lpos_full> & labels);

  // create new labels
  void new_labels(world & objects, zn::zn_conv & zconverter);

  // add vmap objects and labels from fig
  int get_from_fig(const fig::fig_world & F);

  // add vmap objects and labels from fig
  int get_from_mp(const mp::mp_world & M);

  // put vmap to referenced fig
  int put_to_fig(fig::fig_world & F, bool put_labels=true);

  // put vmap to mp
  int put_to_mp(mp::mp_world & M, bool put_labels=true);

  // approximate distance in m
  double dist_pt(const dPoint & p, double lat);
  // approximate distance in m
  double dist_pt_pt(const dPoint & p1, const dPoint & p2);
  // Find point n in the line l which is nearest to the point p. Return distance in cm
  double dist_pt_l(const dPoint & p, const dMultiLine & l, dPoint & n);
  // convert shift
  dPoint v_cm2deg(const dPoint & p, const double lat);

};

} // namespace
#endif
