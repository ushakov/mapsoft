#include <vector>
#include <cmath>

#include "layer_wpt.h"
#include "utils/cairo_wrapper.h"

using namespace std;

void
draw_wpt(iImage & image, const iPoint & origin,
         const Conv & cnv, const g_waypoint_list & wpt,
         const Options & opt){

  CairoWrapper cr(image);

  g_waypoint_list::const_iterator pt;
  for (pt=wpt.begin(); pt!=wpt.end(); pt++){
    dPoint p(pt->x,pt->y); cnv.bck(p); p-=origin;

    Color color = pt->color;
    Color bgcolor = pt->bgcolor;

    double fs = pt->font_size * 1.5;
    double dot_width = pt->size/4.0;
    dPoint flag_shift(0,-fs);
    double text_pad=fs * 0.2;

    dPoint text_shift = flag_shift + dPoint(text_pad, -text_pad);
    cr->set_font_size(fs);

    cr->move_to(p + text_shift);
    dRect txt_rect = cr->get_text_extents(pt->name);
    txt_rect = rect_pump(txt_rect, text_pad);
    txt_rect = rect_pump(txt_rect, p + flag_shift);
    dRect pt_rect = rect_pump(txt_rect, p);
    pt_rect = rect_pump(pt_rect, dot_width+1);

    if (!rect_intersect(pt_rect, dRect(image.range())).empty()){
      cr->move_to(p);
      cr->circle(p, dot_width);
      cr->rectangle(txt_rect);
      cr->set_color(bgcolor.value);
      cr->fill();

      cr->move_to(p);
      cr->circle(p, dot_width);
      cr->move_to(p+dPoint(0,-dot_width));
      cr->rel_line_to(flag_shift);
      cr->rectangle(txt_rect);
      cr->set_color(color.value);
      cr->set_line_width(1);
      cr->stroke();

      cr->set_color(color.value);
      cr->move_to(p + text_shift);
      cr->show_text(pt->name);
    }
  }
}
