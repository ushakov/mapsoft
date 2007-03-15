#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include <iomanip>

#include "fig.h"

namespace fig {

using namespace std;
using namespace boost::spirit;

/****************************************************************/

typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;

int f_arr, b_arr;
bool no_f_arr(){return f_arr==0;}
bool no_b_arr(){return b_arr==0;}
int npoints, npoints_f;
bool point_counter(){npoints--; return npoints>=0;}
bool point_f_counter(){npoints_f--; return npoints_f>=0;}
int sub_type;
bool no_picture(){return sub_type!=5;}

// function for reading objects from file
fig_world read(const char* filename){

	fig_object o, o0;
	fig_world world;

	int color_num=0;

	// iterators for parsing
	iterator_t first(filename);
	if (!first) { cerr << "can't find file " << filename << '\n'; return world;}
	iterator_t last = first.make_end();

	rule_t ch = anychar_p - eol_p;

	rule_t comment = *(ch_p('#') >> *ch >> eol_p) ;

	rule_t header = str_p("#FIG 3.2") >> *ch >> eol_p >>
		(*ch)[assign_a(world.orientation)] >> eol_p >>       // orientation ("Landscape" or "Portrait")
		(*ch)[assign_a(world.justification)] >> eol_p >>     // justification ("Center" or "Flush Left")
		(*ch)[assign_a(world.units)] >> eol_p >>             // units ("Metric" or "Inches")
		(*ch)[assign_a(world.papersize)] >> eol_p >>         // papersize ("A4", etc.)
		ureal_p[assign_a(world.magnification)] >> eol_p >>   // magnification (export and print magnification, %)
		(*ch)[assign_a(world.multiple_page)] >> eol_p >>     // multiple-page ("Single" or "Multiple" pages)
		int_p[assign_a(world.transparent_color)] >> eol_p >> // transparent color
		comment[assign_a(world.comment)] >>                  // comments
		uint_p[assign_a(world.resolution)] >> +space_p >>    // resolution
                uint_p[assign_a(world.coord_system)] >> eol_p;       // coord_system
    
/****************************************************************/
rule_t r_sub_type       = +blank_p >> uint_p[assign_a(o.sub_type)][assign_a(sub_type)];
rule_t r_line_style     = +blank_p >> int_p[assign_a(o.line_style)];
rule_t r_thickness      = +blank_p >> int_p[assign_a(o.thickness)];
rule_t r_pen_color      = +blank_p >> int_p[assign_a(o.pen_color)];
rule_t r_fill_color     = +blank_p >> int_p[assign_a(o.fill_color)];
rule_t r_depth          = +blank_p >> uint_p[assign_a(o.depth)];
rule_t r_pen_style      = +blank_p >> int_p[assign_a(o.pen_style)];
rule_t r_area_fill      = +blank_p >> int_p[assign_a(o.area_fill)];
rule_t r_style_val      = +blank_p >> real_p[assign_a(o.style_val)];
rule_t r_cap_style      = +blank_p >> int_p[assign_a(o.cap_style)];
rule_t r_join_style     = +blank_p >> int_p[assign_a(o.join_style)];
rule_t r_direction      = +blank_p >> int_p[assign_a(o.direction)];
rule_t r_radius         = +blank_p >> int_p[assign_a(o.radius)];
rule_t r_arrows         = +blank_p >> uint_p[assign_a(f_arr)][assign_a(o.forward_arrow)] >> 
                          +blank_p >> uint_p[assign_a(b_arr)][assign_a(o.backward_arrow)];
rule_t r_arrows_p = (eps_p(&no_f_arr) | (
                     +space_p >> int_p[assign_a(o.farrow_type)]  
                  >> +blank_p >> int_p[assign_a(o.farrow_style)]   
                  >> +blank_p >> real_p[assign_a(o.farrow_thickness)]
                  >> +blank_p >> real_p[assign_a(o.farrow_width)]
                  >> +blank_p >> real_p[assign_a(o.farrow_height)]
                )) >> (eps_p(&no_b_arr) | (
                     +space_p >> int_p[assign_a(o.barrow_type)]    
                  >> +blank_p >> int_p[assign_a(o.barrow_style)]   
                  >> +blank_p >> real_p[assign_a(o.barrow_thickness)]
                  >> +blank_p >> real_p[assign_a(o.barrow_width)]
                  >> +blank_p >> real_p[assign_a(o.barrow_height)]
                ));
rule_t r_angle          = +blank_p >> real_p[assign_a(o.angle)];
rule_t r_center_xy      = +blank_p >> real_p[assign_a(o.center_x)] >>
                          +blank_p >> real_p[assign_a(o.center_y)];
rule_t r_radius_xy      = +blank_p >> int_p[assign_a(o.radius_x)] >>
                          +blank_p >> int_p[assign_a(o.radius_y)];
rule_t r_start_xy       = +blank_p >> int_p[assign_a(o.start_x)] >> 
                          +blank_p >> int_p[assign_a(o.start_y)];
rule_t r_end_xy         = +blank_p >> int_p[assign_a(o.end_x)] >> 
                          +blank_p >> int_p[assign_a(o.end_y)];
rule_t r_npoints        = +blank_p >> int_p[assign_a(npoints)][assign_a(npoints_f)];
rule_t r_push_xy        = +space_p >> int_p[push_back_a(o.x)] >>
                          +space_p >> int_p[push_back_a(o.y)];
rule_t r_image          = (eps_p(&no_picture) | (
                          +space_p >> uint_p[assign_a(o.image_orient)] >> // orientation = normal (0) or flipped (1)
                          +blank_p >> (+ch)[assign_a(o.image_file)]));
rule_t r_points         = *( eps_p(&point_counter) >> r_push_xy );
rule_t r_fpoints        = *( eps_p(&point_f_counter) >> +space_p >> real_p[push_back_a(o.f)] );

	/*******************************************/
	rule_t c0_color = ch_p('0')[assign_a(o.type,0)]
		>> +blank_p >> uint_p[assign_a(color_num)]
		>> +blank_p >> '#' >> hex_p[insert_at_a(world.colors,color_num)]
		>> eol_p;
	/*******************************************/
	rule_t c5_arc   = ch_p('5')[assign_a(o.type,5)]  // Arc
		>> r_sub_type >> r_line_style >> r_thickness
		>> r_pen_color >> r_fill_color >> r_depth
		>> r_pen_style >> r_area_fill >> r_style_val 
                >> r_cap_style >> r_direction >> r_arrows >> r_center_xy
                >> r_push_xy >> r_push_xy >> r_push_xy
		>> r_arrows_p >> eol_p;
	/*******************************************/
	rule_t c6_compound_start = ch_p('6')[assign_a(o.type,6)] // Compound
                >> r_push_xy >> r_push_xy >> eol_p;
	/*******************************************/
	rule_t c6_compound_end = str_p("-6")[assign_a(o.type,-6)] >> eol_p;
	/*******************************************/
	rule_t c1_ellipse = ch_p('1')[assign_a(o.type,1)] // Ellipse
		>> r_sub_type >> r_line_style >> r_thickness
		>> r_pen_color >> r_fill_color >> r_depth 
                >> r_pen_style >> r_area_fill >> r_style_val 
                >> r_direction /*(always 1)*/ >> r_angle /*(radians, the angle of the x-axis)*/
		>> r_center_xy >> r_radius_xy 
                >> r_start_xy >> r_end_xy >> eol_p;
	/*******************************************/
        rule_t c2_polyline = ch_p('2')[assign_a(o.type,2)] // Polyline
		>> r_sub_type >> r_line_style >> r_thickness
		>> r_pen_color >> r_fill_color >> r_depth
		>> r_pen_style >> r_area_fill >> r_style_val 
                >> r_join_style >> r_cap_style >> r_radius
		>> r_arrows >> r_npoints >> r_arrows_p
		>> r_image >> r_points >> eol_p;
	/*******************************************/
        rule_t c3_spline = ch_p('3')[assign_a(o.type,3)] // Spline
		>> r_sub_type >> r_line_style >> r_thickness
		>> r_pen_color >> r_fill_color
		>> r_depth >> r_pen_style >> r_area_fill
		>> r_style_val >> r_cap_style
		>> r_arrows >> r_npoints >> r_arrows_p
                >> r_points >> r_fpoints >> eol_p;
	/*******************************************/
        rule_t c4_text = ch_p('4')[assign_a(o.type,4)] // Text
		>> r_sub_type >> r_pen_color >> r_depth
		>> r_pen_style // (not used)
		>> +blank_p >> int_p[assign_a(o.font)]
		>> +blank_p >> real_p[assign_a(o.font_size)]
		>> r_angle
		>> +blank_p >> int_p[assign_a(o.font_flags)]
		>> +blank_p >> real_p[assign_a(o.height)]
		>> +blank_p >> real_p[assign_a(o.length)]
                >> r_push_xy
		>> blank_p  >> (*(~eps_p("\\001") >> anychar_p))[assign_a(o.text)] 
                >> str_p("\\001") >> eol_p;
	/*******************************************/

	if (!parse(first, last, header >> 
	  *( eps_p[assign_a(o,o0)] >> comment[assign_a(o.comment)] >> 
	    ( c0_color | c1_ellipse | c2_polyline | c3_spline | c4_text | c5_arc | 
            c6_compound_start | c6_compound_end) [push_back_a(world,o)] )).full)
        cerr << "Can't parse fig file!\n";

	return world;
}

/***********************************************************/
bool write(ostream & out, const fig_world & world){

  // запись заголовка
  out << "#FIG 3.2\n" 
      << world.orientation << "\n"
      << world.justification << "\n"
      << world.units << "\n"
      << world.papersize << "\n"
      << setprecision(2) << fixed 
      << world.magnification << "\n"
      << setprecision(3) 
      << world.multiple_page << "\n"
      << world.transparent_color << "\n"
      << world.comment << "\n"
      << world.resolution << " " 
      << world.coord_system << "\n";

  // запись цветов
  for (fig_colors::const_iterator 
       i  = world.colors.begin(); 
       i != world.colors.end(); i++){
    if (i->first > 31) 
      out << "0 " << i->first << " #" 
          << setbase(16) << setw(6) << setfill('0')
          << i->second << setbase(10) << "\n";
  }

  // запись разных объектов
  int n;
  for (fig_world::const_iterator
       i  = world.begin(); 
       i != world.end(); i++){
    out << i->comment << "\n";
    switch (i->type){
    case 0: // Color
      break;
    case 1: // Ellipse
      out << "1 "
	<< i->sub_type   << " "
        << i->line_style << " "
        << i->thickness  << " "
	<< i->pen_color  << " "
        << i->fill_color << " "
        << i->depth      << " "
	<< i->pen_style  << " " 
        << i->area_fill  << " " 
        << i->style_val  << " "
	<< i->direction  << " "
        << i->angle      << " "
        << int(i->center_x) << " " 
        << int(i->center_y) << " "  
        << i->radius_x   << " " 
        << i->radius_y   << " " 
	<< i->start_x    << " "
	<< i->start_y    << " "
        << i->end_x      << " "
        << i->end_y      << "\n";
        break;
    case 2: // Polyline
      if (i->x.size()!=i->y.size()){
        cerr << "fig::write (polyline): different amount of x and y values\n";
        return false;
      }
      out << "2 "
        << i->sub_type   << " "
        << i->line_style << " "
        << i->thickness  << " "
        << i->pen_color  << " " 
        << i->fill_color << " "
        << i->depth      << " "
        << i->pen_style  << " "
        << i->area_fill  << " "
        << i->style_val  << " "
        << i->join_style << " "
        << i->cap_style  << " "
        << i->radius     << " "
        << i->forward_arrow  << " "
        << i->backward_arrow << " "
        << i->x.size();
        if (i->forward_arrow) out << "\n\t"
           << i->farrow_type << " "
           << i->farrow_style << " "
           << i->farrow_thickness << " "
           << i->farrow_width << " "
           << i->farrow_height;
        if (i->backward_arrow) out << "\n\t"
           << i->barrow_type << " "
           << i->barrow_style << " "
           << i->barrow_thickness << " "
           << i->barrow_width << " "
           << i->barrow_height;
        if (i->sub_type==5) out << " "
          << i->image_orient << " "
          << i->image_file;
        for (n=0; n<i->x.size(); n++)
          out << ((n%6==0) ? "\n\t":" ")
              << i->x[n] << " " << i->y[n];
        out << "\n";
        break;
    case 3: // Spline
      if ((i->x.size()!=i->y.size())|| 
          (i->x.size()!=i->f.size())){
        cerr << "fig::write (spline): different amount of x,y and f values\n";
        return false;
      }
      out << "3 "
        << i->sub_type   << " "
        << i->line_style << " "
        << i->thickness  << " "
        << i->pen_color  << " " 
        << i->fill_color << " "
        << i->depth      << " "
        << i->pen_style  << " "
        << i->area_fill  << " "
        << i->style_val  << " " 
        << i->cap_style  << " "
        << i->forward_arrow  << " "
        << i->backward_arrow << " "
        << i->x.size();
        if (i->forward_arrow) out << "\n\t"
           << i->farrow_type << " "
           << i->farrow_style << " "
           << i->farrow_thickness << " "
           << i->farrow_width << " "
           << i->farrow_height;
        if (i->backward_arrow) out << "\n\t"
           << i->barrow_type << " "
           << i->barrow_style << " "
           << i->barrow_thickness << " "
           << i->barrow_width << " "
           << i->barrow_height;
        for (n=0; n<i->x.size(); n++)
          out << ((n%6==0) ? "\n\t":" ")
              << i->x[n] << " " << i->y[n];
        for (n=0; n<i->x.size(); n++)
          out << ((n%6==0) ? "\n\t":" ")
              << i->f[n];
        out << "\n";
        break;
    case 4: // Text
      // сделать пересчет размеров (height length)!
      if ((i->y.size()<1)||(i->x.size()<1)){
        cerr << "fig::write (text): can't get x and y values\n";
        return false;
      }
      out << "4 "
        << i->sub_type   << " "
        << i->pen_color  << " " 
        << i->depth      << " "
        << i->pen_style  << " "
        << i->font       << " "
        << i->font_size  << " "
        << i->angle      << " "
        << i->font_flags << " "
        << i->height     << " "
        << i->length     << " "
        << i->x[0]       << " "
        << i->y[0]       << " "
        << i->text       << "\\001\n";
        break;
    case 5: // Arc
      if ((i->y.size()<3)||(i->x.size()<3)){
        cerr << "fig::write (arc): can't get x and y values\n";
        return false;
      }
      out << "5 "
        << i->sub_type   << " "
        << i->line_style << " "
        << i->thickness  << " "
        << i->pen_color  << " "
        << i->fill_color << " "
        << i->depth      << " "
        << i->pen_style  << " "
        << i->area_fill  << " " 
        << i->style_val  << " "
        << i->cap_style  << " "
        << i->direction  << " "
        << i->forward_arrow  << " "
        << i->backward_arrow << " "
        << i->center_x   << " "
        << i->center_y   << " "
        << i->x[0]       << " "
        << i->y[0]       << " "
        << i->x[1]       << " "
        << i->y[1]       << " "
        << i->x[2]       << " "
        << i->y[2];
        if (i->forward_arrow) out << "\n\t"
           << i->farrow_type << " "
           << i->farrow_style << " "
           << i->farrow_thickness << " "
           << i->farrow_width << " "
           << i->farrow_height;
        if (i->backward_arrow) out << "\n\t"
           << i->barrow_type << " "
           << i->barrow_style << " "
           << i->barrow_thickness << " "
           << i->barrow_width << " "
           << i->barrow_height;
        out << "\n";
      break;
    case 6: // Compound begin
      // сделать пересчет размеров!
      if ((i->y.size()<2)||(i->x.size()<2)){
        cerr << "fig::write (compound): can't get x and y values\n";
        return false;
      }
      out << "6 " 
          << i->x[0] << " " 
          << i->y[0] << " "
          << i->x[1] << " " 
          << i->y[1] << "\n";
      break;
    case -6: // Compound end
      out << "-6\n"; 
      break;
    }
  }
}


/****************************************************************/
// создать новый объект на базе obj, подставив все не-звездочки
fig_object make_object(const fig_object & obj, const std::string & mask){

  fig_object o = obj;

  npoints=-1; 
  f_arr=0; b_arr=0; // на случай *
  rule<> ch = anychar_p - eol_p;

rule<> r_sub_type       = +blank_p >> (ch_p('*') | uint_p[assign_a(o.sub_type)]);
rule<> r_line_style     = +blank_p >> (ch_p('*') | int_p[assign_a(o.line_style)]);
rule<> r_thickness      = +blank_p >> (ch_p('*') | int_p[assign_a(o.thickness)]);
rule<> r_pen_color      = +blank_p >> (ch_p('*') | int_p[assign_a(o.pen_color)]);
rule<> r_fill_color     = +blank_p >> (ch_p('*') | int_p[assign_a(o.fill_color)]);
rule<> r_depth          = +blank_p >> (ch_p('*') | uint_p[assign_a(o.depth)]);
rule<> r_pen_style      = +blank_p >> (ch_p('*') | int_p[assign_a(o.pen_style)]);
rule<> r_area_fill      = +blank_p >> (ch_p('*') | int_p[assign_a(o.area_fill)]);
rule<> r_style_val      = +blank_p >> (ch_p('*') | real_p[assign_a(o.style_val)]);
rule<> r_cap_style      = +blank_p >> (ch_p('*') | int_p[assign_a(o.cap_style)]);
rule<> r_join_style     = +blank_p >> (ch_p('*') | int_p[assign_a(o.join_style)]);
rule<> r_direction      = +blank_p >> (ch_p('*') | int_p[assign_a(o.direction)]);
rule<> r_radius         = +blank_p >> (ch_p('*') | int_p[assign_a(o.radius)]);
rule<> r_arrows         = +blank_p >> (ch_p('*') | uint_p[assign_a(f_arr)][assign_a(o.forward_arrow)]) >> 
                          +blank_p >> (ch_p('*') | uint_p[assign_a(b_arr)][assign_a(o.backward_arrow)]);
rule<> r_arrows_p = (eps_p(&no_f_arr) | (
                     +blank_p >> (ch_p('*') | int_p[assign_a(o.farrow_type)])    
                  >> +blank_p >> (ch_p('*') | int_p[assign_a(o.farrow_style)])
                  >> +blank_p >> (ch_p('*') | real_p[assign_a(o.farrow_thickness)])
                  >> +blank_p >> (ch_p('*') | real_p[assign_a(o.farrow_width)])
                  >> +blank_p >> (ch_p('*') | real_p[assign_a(o.farrow_height)])
                )) >> (eps_p(&no_b_arr) | (
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
rule<> r_npoints        = +blank_p >> (ch_p('*') | int_p[assign_a(npoints)]);
rule<> r_push_xy        = +blank_p >> (ch_p('*') | int_p) >>
                          +blank_p >> (ch_p('*') | int_p);
rule<> r_font           = +blank_p >> (ch_p('*') | int_p[assign_a(o.font)]);
rule<> r_font_size      = +blank_p >> (ch_p('*') | real_p[assign_a(o.font_size)]);
rule<> r_font_flags     = +blank_p >> (ch_p('*') | int_p[assign_a(o.font_flags)]);

  /*******************************************/
  rule<> c5_arc   = ch_p('5')[assign_a(o.type,5)]  // Arc
	>> r_sub_type >> r_line_style >> r_thickness
	>> r_pen_color >> r_fill_color >> r_depth
	>> r_pen_style >> r_area_fill >> r_style_val 
        >> r_cap_style >> r_direction >> r_arrows >> r_center_xy
        >> r_push_xy >> r_push_xy >> r_push_xy
	>> r_arrows_p;
  /*******************************************/
  rule<> c6_compound_start = ch_p('6')[assign_a(o.type,6)] // Compound
        >> r_push_xy >> r_push_xy;
  /*******************************************/
  rule<> c6_compound_end = str_p("-6")[assign_a(o.type,-6)];
  /*******************************************/
  rule<> c1_ellipse = ch_p('1')[assign_a(o.type,1)] // Ellipse
	>> r_sub_type >> r_line_style >> r_thickness
	>> r_pen_color >> r_fill_color >> r_depth 
        >> r_pen_style >> r_area_fill >> r_style_val 
        >> r_direction /*(always 1)*/ >> r_angle /*(radians, the angle of the x-axis)*/
	>> r_center_xy >> r_radius_xy 
        >> r_start_xy >> r_end_xy;
  /*******************************************/
  rule<> c2_polyline = ch_p('2')[assign_a(o.type,2)] // Polyline
	>> r_sub_type >> r_line_style >> r_thickness
	>> r_pen_color >> r_fill_color >> r_depth
	>> r_pen_style >> r_area_fill >> r_style_val 
        >> r_join_style >> r_cap_style >> r_radius
	>> r_arrows >> r_npoints >> r_arrows_p;
  /*******************************************/
  rule<> c3_spline = ch_p('3')[assign_a(o.type,3)] // Spline
	>> r_sub_type >> r_line_style >> r_thickness
	>> r_pen_color >> r_fill_color
	>> r_depth >> r_pen_style >> r_area_fill
	>> r_style_val >> r_cap_style
	>> r_arrows >> r_npoints >> r_arrows_p;
  /*******************************************/
  rule<> c4_text = ch_p('4')[assign_a(o.type,4)] // Text
	>> r_sub_type >> r_pen_color >> r_depth
	>> r_pen_style // (not used)
	>> r_font >> r_font_size 
	>> r_angle >> r_font_flags;
  /*******************************************/

  if (!parse(mask.c_str(), *blank_p >> (c1_ellipse | c2_polyline | c3_spline | c4_text | c5_arc | 
      c6_compound_start | c6_compound_end) >> *blank_p ).full)
    cerr << "Can't parse fig mask: " << mask << "\n";
  if (npoints>=0) {o.x.resize(npoints); o.y.resize(npoints);}
  return o;
}

fig_object make_object(const std::string & mask){
  return make_object(fig_object(), mask);
}

bool test_object(const fig_object & o, const std::string & mask){
  return make_object(o, mask)==o;  
}

vector<Point<double> > fig_object::get_vector() const{
  vector<Point<double> > ret;
  for (int j=0; j<min(x.size(),y.size()); j++){
    ret.push_back(Point<double>(x[j],y[j]));
  }
  return ret;
}

void fig_object::set_vector(const vector<Point<double> > & v){
  x.clear(); y.clear();
  for (int i=0;i<v.size();i++){
    x.push_back(int(v[i].x));
    y.push_back(int(v[i].y));
  }
}


}
