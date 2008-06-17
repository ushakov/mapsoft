#include "../utils/spirit_utils.h"
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>

#include <iomanip>

#include "fig_io.h"
#include "../utils/iconv_utils.h"

namespace fig {

const char *default_charset = "KOI8-R";

using namespace std;
using namespace boost::spirit;

/****************************************************************/

int f_arr, b_arr;
bool no_f_arr(){return f_arr==0;}
bool no_b_arr(){return b_arr==0;}
int npoints, npoints_f;
bool point_counter(){npoints--; return npoints>=0;}
bool point_f_counter(){npoints_f--; return npoints_f>=0;}
int sub_type;
bool no_picture(){return sub_type!=5;}

// function for reading objects from file
bool read(const char* filename, fig_world & world){

  fig_world ret;

  fig_object o, o0;
  Point<int> p;
  std::vector<std::string> comment;

  int color_num=0;


  rule_t ch = anychar_p - eol_p;

  rule_t comm = eps_p[clear_a(comment)] >>
          *(str_p("# ") >> (*ch)[push_back_a(comment)] >> eol_p);

  rule_t header = str_p("#FIG 3.2") >> *ch >> eol_p >>
	(*ch)[assign_a(ret.orientation)] >> eol_p >>       // orientation ("Landscape" or "Portrait")
	(*ch)[assign_a(ret.justification)] >> eol_p >>     // justification ("Center" or "Flush Left")
	(*ch)[assign_a(ret.units)] >> eol_p >>             // units ("Metric" or "Inches")
	(*ch)[assign_a(ret.papersize)] >> eol_p >>         // papersize ("A4", etc.)
	ureal_p[assign_a(ret.magnification)] >> eol_p >>   // magnification (export and print magnification, %)
	(*ch)[assign_a(ret.multiple_page)] >> eol_p >>     // multiple-page ("Single" or "Multiple" pages)
	int_p[assign_a(ret.transparent_color)] >> eol_p >> // transparent color
	comm[assign_a(ret.comment, comment)] >>            // comments
	uint_p[assign_a(ret.resolution)] >> +space_p >>    // resolution
                uint_p[assign_a(ret.coord_system)] >> eol_p;       // coord_system

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
  rule_t r_push_xy        = +space_p >> int_p[assign_a(p.x)] >>
                            +space_p >> int_p[assign_a(p.y)][push_back_a(o,p)];
  rule_t r_image          = (eps_p(&no_picture) | (
                            +space_p >> uint_p[assign_a(o.image_orient)] >> // orientation = normal (0) or flipped (1)
                            +blank_p >> (+ch)[assign_a(o.image_file)]));
  rule_t r_points         = *( eps_p(&point_counter) >> r_push_xy );
  rule_t r_fpoints        = *( eps_p(&point_f_counter) >> +space_p >> real_p[push_back_a(o.f)] );

  /*******************************************/
  rule_t c0_color = ch_p('0')
	>> +blank_p >> uint_p[assign_a(color_num)]
	>> +blank_p >> '#' >> hex_p[insert_at_a(ret.colors,color_num)]
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

  rule_t main_rule = header >>
       *( eps_p[assign_a(o,o0)] >> comm[assign_a(o.comment, comment)] >>
        ( c0_color | (c1_ellipse | c2_polyline | c3_spline | c4_text | c5_arc |
        c6_compound_start | c6_compound_end) [push_back_a(ret,o)]) );

  if (!parse_file("fig::read", filename, main_rule)) return false;

  // преобразование символов из восьмиричного вида \??? 
  for (fig::fig_world::iterator i=ret.begin(); i!=ret.end(); i++){
    string t;
    for (int n=0;n<i->text.size();n++){
      if ((i->text[n] == '\\')&&(n+3<i->text.size())){
        t+=char((i->text[n+1]-'0')*64 + (i->text[n+2]-'0')*8 + (i->text[n+3]-'0'));
        n+=3;
      }
      else t+=i->text[n];
    }
    i->text=t;
  }

  //преобразование комментариев и текстов в UTF-8
  IConv cnv(default_charset);
  for (fig_world::iterator i=ret.begin(); i!=ret.end(); i++){
    i->text = cnv.to_utf(i->text);
    for (vector<string>::iterator
          c = i->comment.begin(); c != i->comment.end(); c++){
      *c = cnv.to_utf(*c);
    }
  }
  for (vector<string>::iterator
      c = ret.comment.begin(); c != ret.comment.end(); c++){
    *c = cnv.to_utf(*c);
  }

  // преобразование цветов
  for (fig::fig_world::iterator i=ret.begin(); i!=ret.end(); i++){
    if (i->pen_color > 31){
      if (ret.colors.find(i->pen_color)!=ret.colors.end()) i->pen_color = 0x1000000+ret.colors[i->pen_color];
      else std::cerr << "unknown fig-color " << i->pen_color << "\n";
    }
    if (i->fill_color > 31){
      if (ret.colors.find(i->fill_color)!=ret.colors.end()) i->fill_color = 0x1000000+ret.colors[i->fill_color];
      else std::cerr << "unknown fig-color " << i->fill_color << "\n";
    }
  }

  // merging world and ret
  world.swap(ret);
  world.insert(world.begin(), ret.begin(), ret.end());

  return true;
}

/***********************************************************/
bool write(ostream & out, const fig_world & world){

  IConv cnv(default_charset);

  int n;
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
      << world.transparent_color << "\n";
  for (n=0;n<world.comment.size();n++){
    if (n>99) {cerr << "fig comment contains > 100 lines! Cutting...\n"; break;}
    string s = cnv.from_utf(world.comment[n]);
    if (s.size()>1022){cerr << "fig comment line is > 1022 chars! Cutting...\n"; s.resize(1022);}
    out << "# " << s << "\n";
  }

  out << world.resolution << " " 
      << world.coord_system << "\n";

  std::map<int,int> color_tr;
  fig::fig_colors colors = world.colors;

  // поиск новых цветов
  for (fig::fig_world::const_iterator i=world.begin(); i!=world.end(); i++){

    if (i->pen_color >= 0x1000000){
      int maxc = 31;
      bool found_col = false;
      for (std::map<int,int>::const_iterator c = colors.begin(); c != colors.end(); c++){
        if (c->first <= 31) continue;
        if (c->first > maxc) maxc=c->first;
        if (c->second == i->pen_color-0x1000000) {color_tr[i->pen_color] = c->first; found_col = true;}
      }
      if (!found_col){
        color_tr[i->pen_color] = maxc+1;
        colors.insert(std::pair<int,int>(maxc+1, i->pen_color-0x1000000));
      }
    }

    if (i->fill_color >= 0x1000000){
      int maxc = 31;
      bool found_col = false;
      for (std::map<int,int>::const_iterator c = colors.begin(); c != colors.end(); c++){
        if (c->first <= 31) continue;
        if (c->first > maxc) maxc=c->first;
        if (c->second == i->fill_color-0x1000000) {color_tr[i->fill_color] = c->first; found_col = true;}
      }
      if (!found_col){
        color_tr[i->fill_color] = maxc+1;
        colors.insert(std::pair<int,int>(maxc+1, i->fill_color-0x1000000));
      }
    }

  }


  // запись цветов
  for (fig_colors::const_iterator 
       i  = colors.begin(); 
       i != colors.end(); i++){
    if (i->first > 31) 
      out << "0 " << i->first << " #" 
          << setbase(16) << setw(6) << setfill('0')
          << i->second << setbase(10) << "\n";
  }

  // запись разных объектов
  for (fig_world::const_iterator
       i  = world.begin(); 
       i != world.end(); i++){
    for (n=0;n<i->comment.size();n++){
      if (n>99) {cerr << "fig comment contains > 100 lines! Cutting...\n"; break;}
      string s = cnv.from_utf(i->comment[n]);
      if (s.size()>1022){cerr << "fig comment line is > 1022 chars! Cutting...\n"; s.resize(1022);}
      out << "# " << s << "\n";
    }

    int nn = i->size();
    int nn1=nn;
    std::vector<float> f = i->f;

    int pen_color = i->pen_color;
    int fill_color = i->fill_color;
    if (pen_color > 0x1000000) pen_color = color_tr[pen_color];
    if (fill_color > 0x1000000) fill_color = color_tr[fill_color];

    switch (i->type){
    case 0: // Color
      break;
    case 1: // Ellipse
      out << "1 "
	<< i->sub_type   << " "
        << i->line_style << " "
        << i->thickness  << " "
	<< pen_color     << " "
        << fill_color    << " "
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

      // в замкнутых многоугольниках последняя точка должна совпадать с первой
      if ((i->sub_type > 1) && (nn>0) && ((*i)[nn-1]!=(*i)[0])){
        nn1=nn+1;
      }

      out << "2 "
        << i->sub_type   << " "
        << i->line_style << " "
        << i->thickness  << " "
        << pen_color     << " " 
        << fill_color    << " "
        << i->depth      << " "
        << i->pen_style  << " "
        << i->area_fill  << " "
        << i->style_val  << " "
        << i->join_style << " "
        << i->cap_style  << " "
        << i->radius     << " "
        << i->forward_arrow  << " "
        << i->backward_arrow << " "
        << nn1;
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
      if (i->sub_type==5) out << "\n\t"
        << i->image_orient << " "
        << i->image_file;
      for (n=0; n<nn; n++)
        out << ((n%6==0) ? "\n\t":" ")
            << (*i)[n].x << " " << (*i)[n].y;
      if (nn1>nn)  out << " " << (*i)[0].x << " " << (*i)[0].y;
      out << "\n";
      break;
    case 3: // Spline
      if (nn!=f.size()){
        f.resize(nn, 1);
        cerr << "fig::write (spline): different amount of x,y and f values\n";
      }
      if ((i->sub_type%2==1) && (nn<3)){ 
        cerr << "fig::write (spline): closed spline with <3 points\n";
        break;
      }
      if ((i->sub_type%2==0) && (nn<2)){ 
        cerr << "fig::write (spline): spline with <2 points\n";
        break;
      }
      out << "3 "
        << i->sub_type   << " "
        << i->line_style << " "
        << i->thickness  << " "
        << pen_color     << " " 
        << fill_color    << " "
        << i->depth      << " "
        << i->pen_style  << " "
        << i->area_fill  << " "
        << i->style_val  << " " 
        << i->cap_style  << " "
        << i->forward_arrow  << " "
        << i->backward_arrow << " "
        << nn;
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
        for (n=0; n<nn; n++)
          out << ((n%6==0) ? "\n\t":" ")
              << (*i)[n].x << " " << (*i)[n].y;
        for (n=0; n<nn; n++)
          out << ((n%6==0) ? "\n\t":" ")
              << f[n];
        out << "\n";
        break;
    case 4: // Text
      // сделать пересчет размеров (height length)!
      if (nn<1){
        cerr << "fig::write (text): can't get x and y values\n";
        break;
      }
      out << "4 "
        << i->sub_type   << " "
        << pen_color     << " " 
        << i->depth      << " "
        << i->pen_style  << " "
        << i->font       << " "
        << i->font_size  << " "
        << i->angle      << " "
        << i->font_flags << " "
        << i->height     << " "
        << i->length     << " "
        << (*i)[0].x     << " "
        << (*i)[0].y     << " "
        << cnv.from_utf(i->text) << "\\001\n";
        break;
    case 5: // Arc
      if (nn<3){
        cerr << "fig::write (arc): can't get x and y values\n";
        break;
      }
      out << "5 "
        << i->sub_type   << " "
        << i->line_style << " "
        << i->thickness  << " "
        << pen_color     << " "
        << fill_color    << " "
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
        << (*i)[0].x     << " "
        << (*i)[0].y     << " "
        << (*i)[1].x     << " "
        << (*i)[1].y     << " "
        << (*i)[2].x     << " "
        << (*i)[2].y;
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
      int x1,y1,x2,y2;
      if (nn<2){
        cerr << "fig::write (compound): can't get x and y values\n";
        x1=y1=x2=y2=0;
      } else {
        x1 = (*i)[0].x;
        y1 = (*i)[0].y;
        x2 = (*i)[1].x;
        y2 = (*i)[1].y;
      }
      out << "6 " 
          << x1 << " " 
          << y1 << " "
          << x2 << " " 
          << y2 << "\n";
      break;
    case -6: // Compound end
      out << "-6\n"; 
      break;
    default:
      cerr << "fig::write: bad object type!\n";
      break;
    }
  }
  return true;
}

} //namespace
