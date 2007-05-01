#ifndef FIG_H
#define FIG_H

#include <boost/operators.hpp>

#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "../utils/point.h"

namespace fig {

    // FIG units, 1.05/1200in 
    // (not 1/1200in, I don't know why...)
    const double cm2fig = 449.943757;
    const double fig2cm = 2.2225e-3;

    struct fig_colors : std::map<int,int> {
        fig_colors(){
	    insert(begin(), std::pair<int,int>(-1, 0x000000));
	    insert(begin(), std::pair<int,int>(0,  0x000000));
	    insert(begin(), std::pair<int,int>(1,  0x0000ff));
	    insert(begin(), std::pair<int,int>(2,  0x00ff00));
	    insert(begin(), std::pair<int,int>(3,  0x00ffff));
	    insert(begin(), std::pair<int,int>(4,  0xff0000));
	    insert(begin(), std::pair<int,int>(5,  0xff00ff));
	    insert(begin(), std::pair<int,int>(6,  0xffff00));
	    insert(begin(), std::pair<int,int>(7,  0xffffff));
	    insert(begin(), std::pair<int,int>(8,  0x000090));
	    insert(begin(), std::pair<int,int>(9,  0x0000b0));
	    insert(begin(), std::pair<int,int>(10, 0x0000d0));
	    insert(begin(), std::pair<int,int>(11, 0x87ceff));
	    insert(begin(), std::pair<int,int>(12, 0x009000));
	    insert(begin(), std::pair<int,int>(13, 0x00b000));
	    insert(begin(), std::pair<int,int>(14, 0x00d000));
	    insert(begin(), std::pair<int,int>(15, 0x009090));
	    insert(begin(), std::pair<int,int>(16, 0x00b0b0));
	    insert(begin(), std::pair<int,int>(17, 0x00d0d0));
	    insert(begin(), std::pair<int,int>(18, 0x900000));
	    insert(begin(), std::pair<int,int>(19, 0xb00000));
	    insert(begin(), std::pair<int,int>(20, 0xd00000));
	    insert(begin(), std::pair<int,int>(21, 0x900090));
	    insert(begin(), std::pair<int,int>(22, 0xb000b0));
	    insert(begin(), std::pair<int,int>(23, 0xd000d0));
	    insert(begin(), std::pair<int,int>(24, 0x803000));
	    insert(begin(), std::pair<int,int>(25, 0xa04000));
	    insert(begin(), std::pair<int,int>(26, 0xc06000));
	    insert(begin(), std::pair<int,int>(27, 0xff8080));
	    insert(begin(), std::pair<int,int>(28, 0xffa0a0));
	    insert(begin(), std::pair<int,int>(29, 0xffc0c0));
	    insert(begin(), std::pair<int,int>(30, 0xffe0e0));
	    insert(begin(), std::pair<int,int>(31, 0xffd700));
	}
    };
    struct fig_psfonts : std::map<int,std::string> {
        fig_psfonts(){
            insert(begin(), std::pair<int,std::string>(-1, "Default"));
            insert(begin(), std::pair<int,std::string>( 0, "Times-Roman"));
            insert(begin(), std::pair<int,std::string>( 1, "Times-Italic"));
            insert(begin(), std::pair<int,std::string>( 2, "Times-Bold"));
            insert(begin(), std::pair<int,std::string>( 3, "Times-BoldItalic"));
            insert(begin(), std::pair<int,std::string>( 4, "AvantGarde-Book"));
            insert(begin(), std::pair<int,std::string>( 5, "AvantGarde-BookOblique"));
            insert(begin(), std::pair<int,std::string>( 6, "AvantGarde-Demi"));
            insert(begin(), std::pair<int,std::string>( 7, "AvantGarde-DemiOblique"));
            insert(begin(), std::pair<int,std::string>( 8, "Bookman-Light"));
            insert(begin(), std::pair<int,std::string>( 9, "Bookman-LightItalic"));
            insert(begin(), std::pair<int,std::string>(10, "Bookman-Demi"));
            insert(begin(), std::pair<int,std::string>(11, "Bookman-DemiItalic"));
            insert(begin(), std::pair<int,std::string>(12, "Courier"));
            insert(begin(), std::pair<int,std::string>(13, "Courier-Oblique"));
            insert(begin(), std::pair<int,std::string>(14, "Courier-Bold"));
            insert(begin(), std::pair<int,std::string>(15, "Courier-BoldOblique"));
            insert(begin(), std::pair<int,std::string>(16, "Helvetica"));
            insert(begin(), std::pair<int,std::string>(17, "Helvetica-Oblique"));
            insert(begin(), std::pair<int,std::string>(18, "Helvetica-Bold"));
            insert(begin(), std::pair<int,std::string>(19, "Helvetica-BoldOblique"));
            insert(begin(), std::pair<int,std::string>(20, "Helvetica-Narrow"));
            insert(begin(), std::pair<int,std::string>(21, "Helvetica-Narrow-Oblique"));
            insert(begin(), std::pair<int,std::string>(22, "Helvetica-Narrow-Bold"));
            insert(begin(), std::pair<int,std::string>(23, "Helvetica-Narrow-BoldOblique"));
            insert(begin(), std::pair<int,std::string>(24, "NewCenturySchlbk-Roman"));
            insert(begin(), std::pair<int,std::string>(25, "NewCenturySchlbk-Italic"));
            insert(begin(), std::pair<int,std::string>(26, "NewCenturySchlbk-Bold"));
            insert(begin(), std::pair<int,std::string>(27, "NewCenturySchlbk-BoldItalic"));
            insert(begin(), std::pair<int,std::string>(28, "Palatino-Roman"));
            insert(begin(), std::pair<int,std::string>(29, "Palatino-Italic"));
            insert(begin(), std::pair<int,std::string>(30, "Palatino-Bold"));
            insert(begin(), std::pair<int,std::string>(31, "Palatino-BoldItalic"));
            insert(begin(), std::pair<int,std::string>(32, "Symbol"));
            insert(begin(), std::pair<int,std::string>(33, "ZapfChancery-MediumItalic"));
            insert(begin(), std::pair<int,std::string>(34, "ZapfDingbats"));
	}
    };
    struct fig_texfonts : std::map<int,std::string> {
        fig_texfonts(){
            insert(begin(), std::pair<int,std::string>( 0,"Default"));
            insert(begin(), std::pair<int,std::string>( 0,"Roman"));
            insert(begin(), std::pair<int,std::string>( 2,"Bold"));
            insert(begin(), std::pair<int,std::string>( 1,"Italic"));
            insert(begin(), std::pair<int,std::string>(16,"Sans Serif"));
            insert(begin(), std::pair<int,std::string>(12,"Typewriter"));
	}
    };


    struct fig_object : 
      std::vector<Point<int> >,
      public boost::equality_comparable<fig_object>{
        int     type;
        int     sub_type;
        int     line_style;          //    (enumeration type, solid, dash, dotted, etc.)
	int     thickness;           //    (1/80 inch)
	int     pen_color;           //    (enumeration type, pen color)
	int     fill_color;          //    (enumeration type, fill color)
	int     depth;               //    (enumeration type)
        int     pen_style;           //    (pen style, not used)
	int     area_fill;           //    (enumeration type, -1 = no fill)
	float   style_val;           //    (1/80 inch, specification for dash/dotted lines)
        int     join_style;          //    (enumeration type)
        int     cap_style;           //    (enumeration type)
        int     radius;              //    (1/80 inch, radius of arc-boxes)
	int     direction;           //    (0: clockwise, 1: counterclockwise)
        float   angle;               //    (radians, the angle of the x-axis)
        int     forward_arrow;       //    (0: no forward arrow, 1: on)
        int     backward_arrow;      //    (0: no forward arrow, 1: on)
        float   center_x, center_y;  //    (center of the arc)
        int     radius_x, radius_y;  //    (Fig units)
        int     start_x, start_y;    //    (Fig units; the 1st point entered)
        int     end_x, end_y;        //    (Fig units; the last point entered)
        int     font;                //    (enumeration type)
        float   font_size;           //    (font size in points)
        int     font_flags;          //    (bit vector)
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
          if (size()!=o.size()) return false;
          for (int i=0; i<size(); i++) if ((*this)[i]!=o[i]) return false;
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
           (text == o.text) && (comment == o.comment) && (f == o.f));
        }
        
	// 
        operator std::vector<Point<double> > () const {
          std::vector<Point<double> > ret;
          for (int i=0; i<size(); i++) 
            ret.push_back(Point<double>((*this)[i].x, (*this)[i].y));
          return ret;
        }

        // сместить на (x,y)
	fig_object & operator += (const Point<int> & p) {
          for (iterator i=begin(); i!=end(); i++) *i += p;
	  return *this;
        }

        void set_vector(const std::vector<Point<double> > & v);
    };

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
	fig_colors colors;

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
	// спроектировать точку pt на ближайшую линию, удовлетворяющую маске mask,
        // вернуть направление (единичный вектор vec) линии в этой точке и расстояние до линии
        // (нужно это все для изготовления знаков ж/д платформ, порогов, водопадов -
        // точечных объектов, привязанных к линии, которые хочется изображать 
        // ориентированно)
        double nearest_pt(Point<double> & vec, Point<double> & pt, const std::string & mask, double maxdist=100) const;

	// сместить на (x,y)
        fig_world & operator +=(const Point<int> & p) {
          for (iterator i=begin(); i!=end(); i++) *i += p; 
          return *this;
        }

    };


    fig_world read(const char* filename);
    bool write(std::ostream & out, const fig_world & world);

    // Построить fig-объект на основе obj, подставив все не-звездочки из маски
    fig_object make_object(const fig_object & obj, const std::string & mask);
    // Построить fig-объект на основе объекта по умолчанию
    fig_object make_object(const std::string & mask);
    // Проверить, соответствует ли объект маске
    bool test_object(const fig_object & obj, const std::string & mask);

    // Маска -- строчка информации об объекте в том виде, в каком она 
    // встречается в fig-файле (но информация о стрелках - на той же строке, 
    // а размера и координат текста и координат эллипсов - нет)
    // в маске могут присутствовать звездочки - при проверке эти значения
    // не проверяются, при создании объекта подставляются значения по умолчанию.
    // Вместо типа объекта нельзя подставлять звездочку

    // найти прямоугольники, накрывающие весь текст
//    std::vector<Rect<int> > text_bbxs(const fig_world & world);
 
}
#endif
