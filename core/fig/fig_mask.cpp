#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_insert_at_actor.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>

#include "fig_mask.h"

namespace fig {

using namespace std;
using namespace boost::spirit::classic;

// to protect from fig_io.cpp
namespace mask {
  int f_arr, b_arr;
  bool no_f_arr(){return f_arr==0;}
  bool no_b_arr(){return b_arr==0;}
}

/****************************************************************/
// создать новый объект на базе obj, подставив все не-звездочки
fig_object make_object(const fig_object & obj, const std::string & mask){

  fig_object o = obj;
  int pen_color = o.pen_color, fill_color = o.fill_color;

  mask::f_arr=0; mask::b_arr=0; // на случай *
  rule<> ch = anychar_p - eol_p;

  rule<> r_sub_type       = +blank_p >> (ch_p('*') | uint_p[assign_a(o.sub_type)]);
  rule<> r_line_style     = +blank_p >> (ch_p('*') | int_p[assign_a(o.line_style)]);
  rule<> r_thickness      = +blank_p >> (ch_p('*') | int_p[assign_a(o.thickness)]);
  rule<> r_pen_color      = +blank_p >> (ch_p('*') | int_p[assign_a(pen_color)]);
  rule<> r_fill_color     = +blank_p >> (ch_p('*') | int_p[assign_a(fill_color)]);
  rule<> r_depth          = +blank_p >> (ch_p('*') | uint_p[assign_a(o.depth)]);
  rule<> r_pen_style      = +blank_p >> (ch_p('*') | int_p[assign_a(o.pen_style)]);
  rule<> r_area_fill      = +blank_p >> (ch_p('*') | int_p[assign_a(o.area_fill)]);
  rule<> r_style_val      = +blank_p >> (ch_p('*') | real_p[assign_a(o.style_val)]);
  rule<> r_cap_style      = +blank_p >> (ch_p('*') | int_p[assign_a(o.cap_style)]);
  rule<> r_join_style     = +blank_p >> (ch_p('*') | int_p[assign_a(o.join_style)]);
  rule<> r_direction      = +blank_p >> (ch_p('*') | int_p[assign_a(o.direction)]);
  rule<> r_radius         = +blank_p >> (ch_p('*') | int_p[assign_a(o.radius)]);
  rule<> r_arrows         = +blank_p >> (ch_p('*') | uint_p[assign_a(mask::f_arr)][assign_a(o.forward_arrow)]) >> 
                            +blank_p >> (ch_p('*') | uint_p[assign_a(mask::b_arr)][assign_a(o.backward_arrow)]);
  rule<> r_arrows_p = (eps_p(&mask::no_f_arr) | (
                       +blank_p >> (ch_p('*') | int_p[assign_a(o.farrow_type)])    
                    >> +blank_p >> (ch_p('*') | int_p[assign_a(o.farrow_style)])
                    >> +blank_p >> (ch_p('*') | real_p[assign_a(o.farrow_thickness)])
                    >> +blank_p >> (ch_p('*') | real_p[assign_a(o.farrow_width)])
                    >> +blank_p >> (ch_p('*') | real_p[assign_a(o.farrow_height)])
                  )) >> (eps_p(&mask::no_b_arr) | (
                       +blank_p >> (ch_p('*') | int_p[assign_a(o.barrow_type)])
                    >> +blank_p >> (ch_p('*') | int_p[assign_a(o.barrow_style)])
                    >> +blank_p >> (ch_p('*') | real_p[assign_a(o.barrow_thickness)])
                    >> +blank_p >> (ch_p('*') | real_p[assign_a(o.barrow_width)])
                    >> +blank_p >> (ch_p('*') | real_p[assign_a(o.barrow_height)])
                  ));
  rule<> r_angle          = +blank_p >> (ch_p('*') | real_p[assign_a(o.angle)]);
  rule<> r_center_xy      = +blank_p >> (ch_p('*') | real_p[assign_a(o.center_x)]) >>
                            +blank_p >> (ch_p('*') | real_p[assign_a(o.center_y)]);
  rule<> r_radius_xy      = +blank_p >> (ch_p('*') | int_p[assign_a(o.radius_x)]) >>
                            +blank_p >> (ch_p('*') | int_p[assign_a(o.radius_y)]);
  rule<> r_start_xy       = +blank_p >> (ch_p('*') | int_p[assign_a(o.start_x)]) >> 
                            +blank_p >> (ch_p('*') | int_p[assign_a(o.start_y)]);
  rule<> r_end_xy         = +blank_p >> (ch_p('*') | int_p[assign_a(o.end_x)]) >> 
                            +blank_p >> (ch_p('*') | int_p[assign_a(o.end_y)]);
  rule<> r_npoints        = +blank_p >> (ch_p('*') | int_p);
  rule<> r_push_xy        = +blank_p >> (ch_p('*') | int_p) >>
                            +blank_p >> (ch_p('*') | int_p);
  rule<> r_font           = +blank_p >> (ch_p('*') | int_p[assign_a(o.font)]);
  rule<> r_font_size      = +blank_p >> (ch_p('*') | real_p[assign_a(o.font_size)]);
  rule<> r_font_flags     = +blank_p >> (ch_p('*') | int_p[assign_a(o.font_flags)]);

    /*******************************************/
    const int type5=5;
    rule<> c5_arc   = ch_p('5')[assign_a(o.type,type5)]  // Arc
  	>> r_sub_type >> r_line_style >> r_thickness
  	>> r_pen_color >> r_fill_color >> r_depth
  	>> r_pen_style >> r_area_fill >> r_style_val 
          >> r_cap_style >> r_direction >> r_arrows >> r_center_xy
          >> r_push_xy >> r_push_xy >> r_push_xy
  	>> r_arrows_p;
    /*******************************************/
    const int type6=6;
    rule<> c6_compound_start = ch_p('6')[assign_a(o.type,type6)] // Compound
          >> r_push_xy >> r_push_xy;
    /*******************************************/
    const int typem6=-6;
    rule<> c6_compound_end = str_p("-6")[assign_a(o.type,typem6)];
    /*******************************************/
    const int type1=1;
    rule<> c1_ellipse = ch_p('1')[assign_a(o.type,type1)] // Ellipse
  	>> r_sub_type >> r_line_style >> r_thickness
  	>> r_pen_color >> r_fill_color >> r_depth 
          >> r_pen_style >> r_area_fill >> r_style_val 
          >> r_direction /*(always 1)*/ >> r_angle /*(radians, the angle of the x-axis)*/
  	>> r_center_xy >> r_radius_xy 
          >> r_start_xy >> r_end_xy;
    /*******************************************/
    const int type2=2;
    rule<> c2_polyline = ch_p('2')[assign_a(o.type,type2)] // Polyline
  	>> r_sub_type >> r_line_style >> r_thickness
  	>> r_pen_color >> r_fill_color >> r_depth
  	>> r_pen_style >> r_area_fill >> r_style_val 
          >> r_join_style >> r_cap_style >> r_radius
  	>> r_arrows >> r_npoints >> r_arrows_p;
    /*******************************************/
    const int type3=3;
    rule<> c3_spline = ch_p('3')[assign_a(o.type,type3)] // Spline
  	>> r_sub_type >> r_line_style >> r_thickness
  	>> r_pen_color >> r_fill_color
  	>> r_depth >> r_pen_style >> r_area_fill
  	>> r_style_val >> r_cap_style
  	>> r_arrows >> r_npoints >> r_arrows_p;
    /*******************************************/
    const int type4=4;
    rule<> c4_text = ch_p('4')[assign_a(o.type,type4)] // Text
  	>> r_sub_type >> r_pen_color >> r_depth
  	>> r_pen_style // (not used)
  	>> r_font >> r_font_size 
  	>> r_angle >> r_font_flags;
    /*******************************************/

  if (!parse(mask.c_str(), *blank_p >> (c1_ellipse | c2_polyline | c3_spline | c4_text | c5_arc | 
       c6_compound_start | c6_compound_end) >> *blank_p ).full)
    cerr << "Can't parse fig mask: " << mask << "\n";

  // convert colors to rgb
  map<int,int>::const_iterator i;
  i=colors.find(pen_color);
  o.pen_color = (i!=colors.end())? i->second : pen_color;
  i=colors.find(fill_color);
  o.fill_color = (i!=colors.end())? i->second : fill_color;
  // backward compat:
  if (o.pen_color > 0x1000000) o.pen_color-=0x1000000;
  if (o.fill_color > 0x1000000) o.fill_color-=0x1000000;

  return o;
}

fig_object make_object(const std::string & mask){
  return make_object(fig_object(), mask);
}

bool test_object(const fig_object & o, const std::string & mask){
  return make_object(o, mask)==o;
}

}//namespace

