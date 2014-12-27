#include <vector>
#include <cmath>

#include "draw_wpt.h"

using namespace std;

tmpl_wpts
make_wpts_tmpl(CairoWrapper & cr, const iPoint & origin,
                const Conv & cnv, const g_waypoint_list & wpt,
                const Options & opt, iRect & range){
  tmpl_wpts ret;
  g_waypoint_list::const_iterator pt;
  range=iRect();

  for (pt=wpt.begin(); pt!=wpt.end(); pt++){
    tmpl_wpt p;
    p.x=pt->x; p.y=pt->y; cnv.bck(p); p-=origin;
    p.color = pt->color;
    p.bgcolor = pt->bgcolor;
    p.name=pt->name;

    // dimensions
    p.font_size  = pt->font_size * 1.5;
    p.dot_rad    = pt->size      * 0.25;
    p.text_pad   = p.font_size * 0.2;
    double flag_shift = p.font_size * 0.5;

    dPoint text_shift(p.text_pad, -p.dot_rad - flag_shift - p.text_pad);

    // find text bbox
    cr->set_font_size(p.font_size);
    cr->move_to(p + text_shift);
    p.txt_rect = cr->get_text_extents(p.name);
    p.txt_rect = rect_pump(p.txt_rect, p.text_pad);

    // find point bbox
    dRect pt_rect(p - dPoint(p.dot_rad, p.dot_rad+flag_shift+p.txt_rect.h),
                  p + dPoint(max(p.dot_rad,p.txt_rect.w), p.dot_rad));
    pt_rect = rect_pump(pt_rect, 1.0); // linewidth

    // update range
    if (range.empty()) range=rect_pump_to_int(pt_rect);
    else range = rect_bounding_box(range, rect_pump_to_int(pt_rect));

    // add point to the cache if needed
    if (!rect_intersect(iRect(pt_rect), cr.range()).empty())
      ret.push_back(p);
  }
  return ret;
}

void
plot_wpts_tmpl(CairoWrapper & cr, const tmpl_wpts & wpts){
  std::vector<tmpl_wpt>::const_iterator i;
  for (i=wpts.begin(); i!=wpts.end(); i++){
    cr->move_to(*i);
    cr->circle(*i, i->dot_rad);
    cr->rectangle(i->txt_rect);

    cr->set_color(i->bgcolor);
    cr->fill_preserve();

    cr->set_line_width(1);
    cr->set_color(i->color);
    cr->stroke();

    cr->move_to(*i + dPoint(0,-i->dot_rad));
    cr->line_to(i->txt_rect.BLC());
    cr->stroke();

    cr->move_to(i->txt_rect.BLC());
    cr->rel_move_to(dPoint(i->text_pad, -i->text_pad));
    cr->set_font_size(i->font_size);
    cr->show_text(i->name);
  }
}

void
draw_wpt(iImage & image, const iPoint & origin,
         const Conv & cnv, const g_waypoint_list & wpt,
         const Options & opt){

  CairoWrapper cr(image);
  iRect range;
  tmpl_wpts cache =
    make_wpts_tmpl(cr, origin, cnv, wpt, opt, range);

  plot_wpts_tmpl(cr, cache);
}

iRect
wpts_range(const Conv & cnv, const g_waypoint_list & wpt, const Options & opt){
  CairoWrapper cr(iImage(2,2));
  iRect ret;
  make_wpts_tmpl(cr, iPoint(0,0), cnv, wpt, opt, ret);
  return ret;
}
