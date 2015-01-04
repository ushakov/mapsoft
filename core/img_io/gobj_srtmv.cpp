#include "gobj_srtmv.h"
#include <sstream>
#include <fstream>

#include "utils/cairo_wrapper.h"

using namespace std;

int
GObjSRTMv::draw(iImage & image, const iPoint & origin){
  if (S==NULL) return GOBJ_FILL_NONE;
  iRect range = image.range() + origin;

  int step1 = opt.get<int>("srtm_cnt_step",  10);
  int step2 = opt.get<int>("srtm_cnt_step2", 50);
  int col   = opt.get<int>("srtm_cnt_col",   0x0060C0);
  double w   = opt.get<double>("srtm_cnt_w",     1.5);
  double crv = opt.get<double>("srtm_cnt_crv",  20.0);

  CairoWrapper cr;
  cr.reset_surface(image);
  cr->set_line_width(w);
  cr->set_color(col);

  dRect wgs_range = rect_pump(cnv.bb_frw(range), 1/1200.0);
  map<short, dMultiLine> hors = S->find_contours(wgs_range, step1);

  for(map<short, dMultiLine>::iterator i = hors.begin(); i!=hors.end(); i++){
    cr->set_line_width( i->first%step2? w:2*w );

    dMultiLine l = i->second; cnv.line_bck_p2p(l);
    cr->mkpath_smline(l-origin, 0, crv*w);
    cr->stroke();
  }

  return GOBJ_FILL_PART;
}

