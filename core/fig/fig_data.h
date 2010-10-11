#ifndef FIG_DATA_H
#define FIG_DATA_H

#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "lib2d/line.h"
#include "options/options.h"

/// связанное с fig-файлами
namespace fig {


  extern const double                    cm2fig, fig2cm;   // fig units
  extern const std::map<int,int>         colors;           // fig colors
  extern const std::map<int,std::string> psfonts;          // ps fonts
  extern const std::map<int,std::string> texfonts;         // tex fonts

    /// fig object
    struct fig_object : iLine,
      public boost::equality_comparable<fig_object>,
      public boost::less_than_comparable<fig_object>,
      public boost::additive<fig_object, iPoint>
    {
        int     type;
        int     sub_type;
        int     line_style;          ///    (enumeration type, solid, dash, dotted, etc.)
	int     thickness;           ///    (1/80 inch)
	int     pen_color;           ///    (enumeration type, pen color)
	int     fill_color;          ///    (enumeration type, fill color)
	int     depth;               ///    (enumeration type)
        int     pen_style;           ///    (pen style, not used)
	int     area_fill;           ///    (enumeration type, -1 = no fill)
	float   style_val;           ///    (1/80 inch, specification for dash/dotted lines)
        int     join_style;          ///    (enumeration type)
        int     cap_style;           ///    (enumeration type)
        int     radius;              ///    (1/80 inch, radius of arc-boxes)
	int     direction;           ///    (0: clockwise, 1: counterclockwise)
        float   angle;               ///    (radians, the angle of the x-axis)
        int     forward_arrow;       ///    (0: no forward arrow, 1: on)
        int     backward_arrow;      ///    (0: no forward arrow, 1: on)
        float   center_x, center_y;  ///    (center of the arc)
        int     radius_x, radius_y;  ///    (Fig units)
        int     start_x, start_y;    ///    (Fig units; the 1st point entered)
        int     end_x, end_y;        ///    (Fig units; the last point entered)
        int     font;                ///    (enumeration type)
        float   font_size;           ///    (font size in points)
        int     font_flags;          ///    (bit vector)
        float   height, length;
        int     farrow_type, barrow_type;
        int     farrow_style, barrow_style;
        float   farrow_thickness, barrow_thickness;
	float   farrow_width, barrow_width;
	float   farrow_height, barrow_height;

	std::string  image_file;
	int     image_orient;
        std::string  text;
        std::vector<std::string> comment;
	std::vector<float>  f;
	Options opts;

	fig_object(){
	    type=0; sub_type=0; line_style=0; thickness=1; pen_color=0; fill_color=7; depth=50;
	    pen_style=0; area_fill=-1; style_val=0.0; join_style=0; cap_style=0; radius=0; 
            direction=1; angle=0.0; forward_arrow=0; backward_arrow=0; center_x=0; center_y=0;
            radius_x=0; radius_y=0; start_x=0; start_y=0; end_x=0; end_y=0; font=0; font_size=12;
            font_flags=0; image_orient=0;
            farrow_type=0; barrow_type=0;
            farrow_style=0; barrow_style=0;
            farrow_thickness=1; barrow_thickness=1;
	    farrow_width=60; barrow_width=60;
	    farrow_height=30; barrow_height=30;
            height=0; length=0;
	}
	bool operator== (const fig_object & o) const{
          // полное совпадение объектов
          return (
           (type == o.type) && (sub_type == o.sub_type) &&
           (line_style == o.line_style) && (thickness == o.thickness) &&
	   (pen_color == o.pen_color) && (fill_color == o.fill_color) &&
	   (depth == o.depth) && (pen_style == o.pen_style) &&
	   (area_fill == o.area_fill) && (style_val == o.style_val) &&
           (join_style == o.join_style) && (cap_style == o.cap_style) &&
           (radius == o.radius) && (direction == o.direction) && (angle == o.angle) && 
           (forward_arrow == o.forward_arrow) && (backward_arrow == o.backward_arrow) &&
           (center_x == o.center_x) && (center_y == o.center_y) &&
           (radius_x == o.radius_x) && (radius_y == o.radius_y) &&
           (start_x == o.start_x) && (start_y == o.start_y) &&
           (end_x == o.end_x) && (end_y == o.end_y) &&
           (font == o.font) && (font_size == o.font_size) && (font_flags == o.font_flags) &&
           (height == o.height) && (length == o.length) &&
           (farrow_type == o.farrow_type) && (barrow_type == o.barrow_type) &&
           (farrow_style == o.farrow_style) && (barrow_style == o.barrow_style) &&
           (farrow_thickness == o.farrow_thickness) && (barrow_thickness == o.barrow_thickness) &&
	   (farrow_width == o.farrow_width) && (barrow_width == o.barrow_width) &&
	   (farrow_height == o.farrow_height) && (barrow_height == o.barrow_height) &&
           (image_file == o.image_file) && (image_orient == o.image_orient) &&
           (text == o.text) && (comment == o.comment) && (opts == o.opts) &&
           (f == o.f) && iLine::operator==(o));
        }

	bool operator< (const fig_object & o) const{
          if (type != o.type) return (type < o.type);
          if (sub_type != o.sub_type) return (sub_type < o.sub_type);
          if (line_style != o.line_style) return (line_style < o.line_style);
	  if (thickness != o.thickness) return (thickness < o.thickness);
	  if (pen_color != o.pen_color) return (pen_color < o.pen_color);
	  if (fill_color != o.fill_color) return (fill_color < o.fill_color);
	  if (depth != o.depth) return (depth < o.depth);
	  if (pen_style != o.pen_style) return (pen_style < o.pen_style);
	  if (area_fill != o.area_fill) return (area_fill < o.area_fill);
          if (style_val != o.style_val) return (style_val < o.style_val);
          if (join_style != o.join_style) return (join_style < o.join_style);
          if (cap_style != o.cap_style) return (cap_style < o.cap_style);
          if (radius != o.radius) return (radius < o.radius);
          if (direction != o.direction) return (direction < o.direction);
          if (angle != o.angle) return (angle < o.angle);
          if (forward_arrow != o.forward_arrow) return (forward_arrow < o.forward_arrow);
          if (backward_arrow != o.backward_arrow) return (backward_arrow < o.backward_arrow);
          if (center_x != o.center_x) return (center_x < o.center_x);
          if (center_y != o.center_y) return (center_y < o.center_y);
          if (radius_x != o.radius_x) return (radius_x < o.radius_x);
          if (radius_y != o.radius_y) return (radius_y < o.radius_y);
          if (start_x != o.start_x) return (start_x < o.start_x);
          if (start_y != o.start_y) return (start_y < o.start_y);
          if (end_x != o.end_x) return (end_x < o.end_x);
          if (end_y != o.end_y) return (end_y < o.end_y);
          if (font != o.font) return (font < o.font);
          if (font_size != o.font_size) return (font_size < o.font_size);
          if (font_flags != o.font_flags) return (font_flags < o.font_flags);
          if (height != o.height) return (height < o.height);
          if (length != o.length) return (length < o.length);
          if (farrow_type != o.farrow_type) return (farrow_type < o.farrow_type);
          if (barrow_type != o.barrow_type) return (barrow_type < o.barrow_type);
          if (farrow_style != o.farrow_style) return (farrow_style < o.farrow_style);
          if (barrow_style != o.barrow_style) return (barrow_style < o.barrow_style);
          if (farrow_thickness != o.farrow_thickness) return (farrow_thickness < o.farrow_thickness);
          if (barrow_thickness != o.barrow_thickness) return (barrow_thickness < o.barrow_thickness);
	  if (farrow_width != o.farrow_width) return (farrow_width < o.farrow_width);
          if (barrow_width != o.barrow_width) return (barrow_width < o.barrow_width);
	  if (farrow_height != o.farrow_height) return (farrow_height < o.farrow_height);
          if (barrow_height != o.barrow_height) return (barrow_height < o.barrow_height);
          if (image_file != o.image_file) return (image_file < o.image_file);
          if (image_orient != o.image_orient) return (image_orient < o.image_orient);
          if (text != o.text) return (text < o.text);
          if (comment != o.comment) return (comment < o.comment);
          if (f != o.f) return (f < o.f);
          if (opts != o.opts) return (opts < o.opts); // ?!
          return iLine::operator<(o);
        }

        /// сместить на (x,y)
	fig_object & operator += (const iPoint & p) {
          for (iterator i=begin(); i!=end(); i++) *i += p;
          center_x+=p.x;
          center_y+=p.y;
	  return *this;
        }

        bool is_ellipse()  const {return (type==1);}
        bool is_polyline() const {return (type==2);}
        bool is_spline()   const {return (type==3);}
        bool is_text()     const {return (type==4);}
        bool is_arc()      const {return (type==5);}
        bool is_compound() const {return (type==6);}
        bool is_compound_end() const {return (type==-6);}

        bool is_closed() const {
          if (is_polyline()) return (sub_type>1);
          if (is_spline())   return (sub_type%2==1);
          if (is_ellipse())  return true;
          return false;
        }
        void close(){
          if (is_closed()) return;
          if (is_polyline()){ sub_type=3; return;}
          if (is_spline())  { sub_type++; return;}
          return;
        }
        void open(){
          if (!is_closed()) return;
          if (is_polyline()&&(sub_type==3)){ sub_type=1; return;}
          if (is_spline())  { sub_type--; return;}
          return;
        }

        // сделать из линии
        void set_points(const dLine & v);
        void set_points(const iLine & v);

	/// преобразовать в линию
        template <typename T>
        operator Line<T> () const {
          Line<T> ret;
          for (int i=0; i<size(); i++) 
            ret.push_back(Point<T>((*this)[i].x, (*this)[i].y));
          return ret;
        }

    };

    /// fig-file
    struct fig_world:std::list<fig_object>{
	std::string orientation;
	std::string justification;
	std::string units;
	std::string papersize;
	float magnification;
	std::string multiple_page;
	int transparent_color;
	int resolution; 
	int coord_system;

        std::vector<std::string> comment;
	Options opts;

        fig_world(){
          orientation="Portrait";
          justification="Center";
          units="Metric";
          papersize="A4";
          magnification=100.0;
          multiple_page="Single";
          transparent_color=-2;
          resolution=1200;
          coord_system=2;
        }

	// сместить на (x,y)
        fig_world & operator +=(const iPoint & p) {
          for (iterator i=begin(); i!=end(); i++) *i += p; 
          return *this;
        }

        iRect range() const{
          if (this->size()<1) return iRect(0,0,0,0);
          fig_world::const_iterator i=this->begin();
          iRect ret=i->range();
          while ((++i) != this->end())
            ret = rect_bounding_box(ret, i->range());
          return ret;
        }

        /// remove empty compounds
	void remove_empty_comp();

    };

}
#endif
