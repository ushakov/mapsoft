#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <vector>
#include <string>
#include <time.h>

#include "geo_data.h"
#include "io.h"
#include "../jeeps/gpsmath.h"
#include "../jeeps/gpsdatum.h"
#include "geo_convs.h"
#include "geo_enums.h"



namespace fig {

	using namespace std;
	using namespace boost::spirit;


        struct xy_range{  // такое уже есть в o_fig!
            g_point min, max;
            xy_range():min(1e99,1e99),max(-1e99,-1e99){}
            void test(double x, double y){
                if (min.x>x) min.x=x;
                if (max.x<x) max.x=x;
                if (min.y>y) min.y=y;
                if (max.y<y) max.y=y;
            }
            void test(g_point p){
                if (min.x>p.x) min.x=p.x;
                if (max.x<p.x) max.x=p.x;
                if (min.y>p.y) min.y=p.y;
                if (max.y<p.y) max.y=p.y;
            }
        };

	struct fig_colors:map<int,int>{
	    fig_colors(){
		insert(begin(), pair<int,int>(-1, 0x000000));
		insert(begin(), pair<int,int>(0,  0x000000));
		insert(begin(), pair<int,int>(1,  0x0000ff));
		insert(begin(), pair<int,int>(2,  0x00ff00));
		insert(begin(), pair<int,int>(3,  0x00ffff));
		insert(begin(), pair<int,int>(4,  0xff0000));
		insert(begin(), pair<int,int>(5,  0xff00ff));
		insert(begin(), pair<int,int>(6,  0xffff00));
		insert(begin(), pair<int,int>(7,  0xffffff));
		insert(begin(), pair<int,int>(8,  0x000090));
		insert(begin(), pair<int,int>(9,  0x0000b0));
		insert(begin(), pair<int,int>(10, 0x0000d0));
		insert(begin(), pair<int,int>(11, 0x87ceff));
		insert(begin(), pair<int,int>(12, 0x009000));
		insert(begin(), pair<int,int>(13, 0x00b000));
		insert(begin(), pair<int,int>(14, 0x00d000));
		insert(begin(), pair<int,int>(15, 0x009090));
		insert(begin(), pair<int,int>(16, 0x00b0b0));
		insert(begin(), pair<int,int>(17, 0x00d0d0));
		insert(begin(), pair<int,int>(18, 0x900000));
		insert(begin(), pair<int,int>(19, 0xb00000));
		insert(begin(), pair<int,int>(20, 0xd00000));
		insert(begin(), pair<int,int>(21, 0x900090));
		insert(begin(), pair<int,int>(22, 0xb000b0));
		insert(begin(), pair<int,int>(23, 0xd000d0));
		insert(begin(), pair<int,int>(24, 0x803000));
		insert(begin(), pair<int,int>(25, 0xa04000));
		insert(begin(), pair<int,int>(26, 0xc06000));
		insert(begin(), pair<int,int>(27, 0xff8080));
		insert(begin(), pair<int,int>(28, 0xffa0a0));
		insert(begin(), pair<int,int>(29, 0xffc0c0));
		insert(begin(), pair<int,int>(30, 0xffe0e0));
		insert(begin(), pair<int,int>(31, 0xffd700));
	    }
	};

	struct fig_object{
    	    int     type;
    	    int     sub_type;
    	    int     line_style;          //    (enumeration type, solid, dash, dotted, etc.)
    	    int     thickness;           //    (1/80 inch)
    	    int     pen_color;           //    (enumeration type, pen color)
    	    int     fill_color;          //    (enumeration type, fill color)
    	    int     depth;               //    (enumeration type)
    	    int     area_fill;           //    (enumeration type, -1 = no fill)
    	    float   style_val;           //    (1/80 inch, specification for dash/dotted lines)
	    string  image_file;
	    int     image_orient;
	    string  comment;
	    vector<int>    x,y;

	    fig_object(){
		type=0; line_style=0; thickness=1; pen_color=0; fill_color=7; depth=50;
		area_fill=-1; style_val=0.0;
 	    }
	};

	struct fig_world:vector<fig_object>{
	    string filename;
	    string comment;
	    fig_colors colors;
	};


/////////////////////////////////////////////////////////////////////////

	typedef char                    char_t;
	typedef file_iterator <char_t>  iterator_t;
	typedef scanner<iterator_t>     scanner_t;
	typedef rule <scanner_t>        rule_t;

	int f_arr, b_arr;
	bool is_f_arr(){return f_arr!=0;}
	bool is_b_arr(){return b_arr!=0;}
	int npoints, npoints_f;
	bool point_counter(){npoints--; return npoints>=0;}
	bool point_f_counter(){npoints_f--; return npoints_f>=0;}
	int sub_type;
    	bool is_picture(){return sub_type==5;}

// function for reading objects from file
	fig_world parse_fig_file(const char* filename){

		fig_object o, o0;
		fig_world world;
		world.filename=filename;
	
		int color_num=0;

		// iterators for parsing
		iterator_t first(filename);
		if (!first) { cerr << "can't find file " << filename << '\n'; return world;}
		iterator_t last = first.make_end();

		rule_t ch = anychar_p - eol_p;

		rule_t comment = *(ch_p('#') >> *ch >> eol_p) ;

		rule_t header = str_p("#FIG 3.2") >> *ch >> eol_p >>
			*ch >> eol_p >>     // orientation ("Landscape" or "Portrait")
			*ch >> eol_p >>     // justification ("Center" or "Flush Left")
			*ch >> eol_p >>     // units ("Metric" or "Inches")
			*ch >> eol_p >>     // papersize ("A4", etc.)
			ureal_p >> eol_p >> // magnification (export and print magnification, %) // Оно нам надо?
			*ch >> eol_p >>     // multiple-page ("Single" or "Multiple" pages)
			int_p >> eol_p >>   // transparent color
			comment[assign_a(world.comment)] >>         // comments
			uint_p >> +space_p >> uint_p >> eol_p;  //resolution coord_system
	    
		/*******************************************/
		rule_t c0_color = ch_p('0')[assign_a(o.type,1)]
			>> +space_p >> uint_p[assign_a(color_num)]
			>> +space_p >> '#' >> hex_p[insert_at_a(world.colors,color_num)]
			>> eol_p;

		rule_t c5_arc   = ch_p('5')[assign_a(o.type,5)] 
			>> +space_p >> uint_p[assign_a(o.sub_type)]  // sub_type
			>> +space_p >> int_p   // line_style
			>> +space_p >> int_p   // line_thickness
			>> +space_p >> int_p   // pen_color
			>> +space_p >> int_p   // fill_color
			>> +space_p >> uint_p  // depth
			>> +space_p >> int_p   // pen_style
			>> +space_p >> int_p   // area_fill
			>> +space_p >> real_p  // style_val
			>> +space_p >> int_p   // cap_style
			>> +space_p >> int_p   // direction
			>> +space_p >> uint_p[assign_a(f_arr)]
			>> +space_p >> uint_p[assign_a(b_arr)]
			>> +space_p >> real_p  // center_x
			>> +space_p >> real_p  // center_x
			>> +space_p >> int_p   // x1
			>> +space_p >> int_p   // y1
			>> +space_p >> int_p   // x2
			>> +space_p >> int_p   // y2
			>> +space_p >> int_p   // x3
			>> +space_p >> int_p   // y3
			>> !( eps_p(&is_f_arr)
			>> +space_p >> int_p   // arrow_type
			>> +space_p >> int_p   // arrow_style
			>> +space_p >> real_p  // arrow_thickness
			>> +space_p >> real_p  // arrow_width
			>> +space_p >> real_p  // arrow_height
			)
			>> !( eps_p(&is_b_arr) 
			>> +space_p >> int_p   // arrow_type
			>> +space_p >> int_p   // arrow_style
			>> +space_p >> real_p  // arrow_thickness
			>> +space_p >> real_p  // arrow_width
			>> +space_p >> real_p  // arrow_height
			) >> eol_p;

		/*******************************************/
		rule_t c6_compound_start = ch_p('6')[assign_a(o.type,6)]
			>> +space_p >> int_p   // upperleft_corner_x
			>> +space_p >> int_p   // upperleft_corner_y
			>> +space_p >> int_p   // lowerright_corner_x
			>> +space_p >> int_p   // lowerright_corner_y
			>> eol_p;
		rule_t c6_compound_end = str_p("-6")[assign_a(o.type,-6)] >> eol_p;
		/*******************************************/
		rule_t c1_ellipse = ch_p('1')[assign_a(o.type,1)]
			>> +space_p >> uint_p[assign_a(o.sub_type)]  // sub_type
			>> +space_p >> int_p   // line_style
			>> +space_p >> int_p   // thickness
			>> +space_p >> int_p   // pen_color
			>> +space_p >> int_p   // fill_color
			>> +space_p >> uint_p  // depth
			>> +space_p >> int_p   // pen_style
			>> +space_p >> int_p   // area_fill
			>> +space_p >> real_p  // style_val
			>> +space_p >> int_p   // direction (always 1)
			>> +space_p >> real_p  // angle (radians, the angle of the x-axis)
			>> +space_p >> int_p   // center_x
			>> +space_p >> int_p   // center_y
			>> +space_p >> int_p   // radius_x
			>> +space_p >> int_p   // radius_y
			>> +space_p >> int_p   // start_x
			>> +space_p >> int_p   // start_y
			>> +space_p >> int_p   // end_x
			>> +space_p >> int_p   // end_y
			>> eol_p;
		/*******************************************/
                rule_t c2_polyline = ch_p('2')[assign_a(o.type,2)]
			>> +space_p >>uint_p[assign_a(o.sub_type)][assign_a(sub_type)] // sub_type
			>> +space_p >> int_p[assign_a(o.line_style)]   // line_style
			>> +space_p >> int_p[assign_a(o.thickness)]    // thickness
			>> +space_p >> int_p[assign_a(o.pen_color)]    // pen_color
			>> +space_p >> int_p[assign_a(o.fill_color)]   // fill_color
			>> +space_p >>uint_p[assign_a(o.depth)]        // depth
			>> +space_p >> int_p   // pen_style
			>> +space_p >> int_p[assign_a(o.area_fill)]    // area_fill
			>> +space_p >>real_p[assign_a(o.style_val)]    // style_val
			>> +space_p >> int_p   // join_style
			>> +space_p >> int_p   // cap_style
			>> +space_p >> int_p   // radius
			>> +space_p >> int_p[assign_a(f_arr)]   // forward_arrow
			>> +space_p >> int_p[assign_a(b_arr)]   // backward_arrow
			>> +space_p >> int_p[assign_a(npoints)] // npoints
			>> !( eps_p(&is_f_arr)
			>> +space_p >> int_p   // arrow_type
			>> +space_p >> int_p   // arrow_style
			>> +space_p >> real_p   // arrow_thickness
			>> +space_p >> real_p   // arrow_width
			>> +space_p >> real_p   // arrow_height
			)
			>> !( eps_p(&is_b_arr) 
			>> +space_p >> int_p   // arrow_type
			>> +space_p >> int_p   // arrow_style
			>> +space_p >> real_p   // arrow_thickness
			>> +space_p >> real_p   // arrow_width
			>> +space_p >> real_p   // arrow_height
			)
			>> !( eps_p(&is_picture) 
			>> +space_p >> uint_p[assign_a(o.image_orient)]  // image_orient  // orientation = normal (0) or flipped (1)
			>> space_p >> (+ch)[assign_a(o.image_file)] >> eol_p  // image_file
			) 
			>> *( eps_p(&point_counter)
                        >> +space_p >> int_p[push_back_a(o.x)]   // x
                        >> +space_p >> int_p[push_back_a(o.y)]   // y
			)
			>> eol_p;
		/*******************************************/
                rule_t c3_spline = ch_p('3')[assign_a(o.type,3)]
			>> +space_p >> uint_p[assign_a(o.sub_type)]  // sub_type
			>> +space_p >> int_p   // line_style
			>> +space_p >> int_p   // thickness
			>> +space_p >> int_p   // pen_color
			>> +space_p >> int_p   // fill_color
			>> +space_p >> uint_p  // depth
			>> +space_p >> int_p   // pen_style
			>> +space_p >> int_p   // area_fill
			>> +space_p >> real_p  // style_val
			>> +space_p >> int_p   // cap_style
			>> +space_p >> int_p[assign_a(f_arr)] // forward_arrow
			>> +space_p >> int_p[assign_a(b_arr)] // backward_arrow
			>> +space_p >> int_p[assign_a(npoints)] // npoints
			>> !( eps_p(&is_f_arr)
			>> +space_p >> int_p   // arrow_type
			>> +space_p >> int_p   // arrow_style
			>> +space_p >> real_p  // arrow_thickness
			>> +space_p >> real_p  // arrow_width
			>> +space_p >> real_p  // arrow_height
			)
			>> !( eps_p(&is_b_arr) 
			>> +space_p >> int_p   // arrow_type
			>> +space_p >> int_p   // arrow_style
			>> +space_p >> real_p  // arrow_thickness
			>> +space_p >> real_p  // arrow_width
			>> +space_p >> real_p  // arrow_height
			)
			>> *( eps_p(&point_counter)
                        >> +space_p >> int_p   // x
                        >> +space_p >> int_p   // y
			)
			>> *( eps_p(&point_f_counter)
                        >> +space_p >> real_p  // f
			)
			>> eol_p;
		/*******************************************/
                rule_t c4_text = ch_p('4')[assign_a(o.type,4)]
			>> +space_p >> uint_p  // sub_type
			>> +space_p >> int_p   // color
			>> +space_p >> uint_p  // depth
			>> +space_p >> int_p   // pen_style (not used)
			>> +space_p >> int_p   // font
			>> +space_p >> real_p  // font_size
			>> +space_p >> real_p  // angle
			>> +space_p >> int_p   // font_flags
			>> +space_p >> real_p  // height
			>> +space_p >> real_p  // length
			>> +space_p >> int_p   //  x
			>> +space_p >> int_p   //  y
			>> space_p  >> *(~eps_p("\\001") >> anychar_p) >> str_p("\\001")   //  text
			>> eol_p;

		/*******************************************/

		// We don't care about compound structure
		parse(first, last, header >> 
		  *( eps_p[assign_a(o,o0)] >> comment[assign_a(o.comment)] >> 
		    ( c0_color | c1_ellipse | c2_polyline | c3_spline | c4_text | c5_arc | 
                    c6_compound_start | c6_compound_end) [push_back_a(world,o)] ));

		return world;
	}

/////////////////////////////////////////////////////////////////////////
        // привязка файла
	g_map get_canvas(const fig_world & world){
		
		g_map canvas;
		canvas.file = world.filename;

		// В комментарии к файлу может быть указано, в какой он проекции
		// default - tmerc
		string proj  = "tmerc";
		parse(world.comment.c_str(), 
		  *( (str_p("# proj:")  >> space_p >> (*(anychar_p-eol_p))[assign_a(proj)]) | 
		     (anychar_p-eol_p) 
		      >> eol_p));
		canvas.map_proj=Proj(proj);

		xy_range range;

		vector<fig_object>::const_iterator i, b=world.begin(), e=world.end();
		for (i=b;i!=e;i++){
		    if (i->type!=2) continue; // polylines only
		    if (i->x.size()<1) continue;

		    double x,y;
		    string datum = "wgs84";
		    string proj  = "latlon";
		    string lon0  = "";
		    
                    if (parse(i->comment.c_str(), str_p("# REF") 
		        >> +space_p >> real_p[assign_a(x)]
			>> +space_p >> real_p[assign_a(y)] >> eol_p 
			>> *(
			    (str_p("# datum:") >> +space_p >> (*(anychar_p-eol_p))[assign_a(datum)]) |
			    (str_p("# proj:")  >> +space_p >> (*(anychar_p-eol_p))[assign_a(proj)]) |
			    (str_p("# lon0:")  >> +space_p >> (*(anychar_p-eol_p))[assign_a(lon0)]) |
			    *(anychar_p-eol_p)
			    >> eol_p
			)).full){

			g_refpoint ref;
			ref.xr = i->x[0];
			ref.yr = i->y[0];
                        ref.x  = x; 
                        ref.y  = y; 
                        Options O;
                        O["lon0"] = lon0;

                        convs::pt2pt c1(Datum(datum), Proj(proj), O, Datum("wgs84"), Proj("lonlat"), O);
                        c1.frw(ref);
			range.test(ref.xr,ref.yr);
			canvas.points.push_back(ref);
		    }
		}
		// границы - довольно произвольные, но без них не
		// инициализируешь g_map
		if (canvas.points.size() < 1) return g_map();
		canvas.border.push_back(range.min);
		canvas.border.push_back(g_point(range.min.x, range.max.y));
		canvas.border.push_back(range.max);
		canvas.border.push_back(g_point(range.max.x, range.min.y));

		return canvas;
	}



/////////////////////////////////////////////////////////////////////////
        // точки
	g_waypoint_list get_waypoint_list(const fig_world & world, const g_map & canvas){
		
		g_waypoint_list ret;
                Options O;
                convs::map2pt cnv(canvas, Datum("wgs"), Proj("tmerc"), O);

		vector<fig_object>::const_iterator i, b=world.begin(), e=world.end();
		for (i=b;i!=e;i++){
		    if (i->type!=2) continue; // polylines only
		    if (i->x.size()<1) continue;

		    Options attrs;
		    string tmp_aname;
		    
                    if (parse(i->comment.c_str(), str_p("# WPT") 
		        >> +space_p >> (*(anychar_p-eol_p))[insert_at_a(attrs, "name")] >> eol_p 
			>> *(str_p("# ") >> (*(anychar_p-eol_p-':'))[assign_a(tmp_aname)]
					 >> ch_p(':') >> *blank_p >> 
			                    (*(anychar_p-eol_p))[insert_at_a(attrs, tmp_aname)] >> eol_p)
			).full){

			g_waypoint wp;
                        wp.x=double(i->x[0]);
                        wp.y=double(i->y[0]);
			cnv.frw(wp);

                	wp.name = attrs.get_string("name");
                 	wp.comm = attrs.get_string("comm");
                 	wp.x    = attrs.get_double("lon",  wp.x);
                 	wp.y    = attrs.get_double("lat",  wp.y);
                 	wp.z    = attrs.get_double("alt",  wp.z);
                 	wp.prox_dist  = attrs.get_double("prox_dist",  wp.prox_dist);
                 	wp.symb       = wpt_symb_enum.str2int(attrs.get_string("symb"));
                 	wp.displ      = attrs.get_int("displ",    wp.displ);
                 	wp.color      = attrs.get_hex("color",    wp.color);
                 	wp.bgcolor    = attrs.get_hex("bgcolor",  wp.bgcolor);
                 	wp.map_displ  = wpt_map_displ_enum.str2int(attrs.get_string("map_displ"));
                 	wp.pt_dir     = wpt_pt_dir_enum.str2int(attrs.get_string("pt_dir"));
                	wp.font_size  = attrs.get_int("font_size",  wp.font_size);
                 	wp.font_style = attrs.get_int("font_style", wp.font_style);
                 	wp.size       = attrs.get_int("size",       wp.size);
                 	wp.t          = attrs.get_time("time",      wp.t);
                 	const std::string used[] = {
                 	  "name", "comm", "lon", "lat", "alt", "prox_dist", "symb",
                 	  "displ", "color", "bgcolor", "map_displ", "pt_dir", "font_size",
                 	  "font_style", "size", "time", ""};
                 	attrs.warn_unused(used);

			ret.points.push_back(wp);
		    }
		}
		return ret;
	}

/////////////////////////////////////////////////////////////////////////
        // треки
	vector<g_track> get_tracks(const fig_world & world, const g_map & canvas){
		
		vector<g_track> ret;
                Options O;
                convs::map2pt cnv(canvas, Datum("wgs"), Proj("tmerc"), O);

		vector<fig_object>::const_iterator i, b=world.begin(), e=world.end();
		for (i=b;i!=e;i++){
		    if (i->type!=2) continue; // polylines only
		    if (i->x.size()<1) continue;

		    Options attrs;
		    string tmp_aname;
		    
                    if (parse(i->comment.c_str(), str_p("# TRK") 
		        >> +space_p >> (*(anychar_p-eol_p))[insert_at_a(attrs, "comm")] >> eol_p 
			>> *(str_p("# ") >> (*(anychar_p-eol_p-':'))[assign_a(tmp_aname)]
					 >> ch_p(':') >> *blank_p >> 
			                    (*(anychar_p-eol_p))[insert_at_a(attrs, tmp_aname)] >> eol_p)
			).full){

			g_track tr;
			for (int j=0; j<i->x.size(); j++){
			    g_trackpoint tp;
                            tp.x=double(i->x[j]);
                            tp.y=double(i->y[j]);
                            cnv.frw(tp);
			    tr.points.push_back(tp);
			}

                        tr.comm = attrs.get_string("comm");                
                        tr.width = attrs.get_uint("width", tr.width);
                        tr.color = attrs.get_hex("color", tr.color);
                        tr.skip  = attrs.get_uint("skip", tr.skip);
                        tr.displ = attrs.get_uint("displ", tr.displ);
                        tr.type  = trk_type_enum.str2int(attrs.get_string("type"));
                        tr.fill  = trk_fill_enum.str2int(attrs.get_string("fill"));
                        tr.cfill = attrs.get_hex("cfill", tr.cfill);
                        const std::string used[] = {"comm", "width", "color", "skip", "displ", "type", "fill", "cfill", "points", ""};
                        attrs.warn_unused(used);
 
			ret.push_back(tr);
		    }
		}
		return ret;
	}

////////////////////////////////////////////////////////
	bool parse_file(const char* filename, geo_data & world, const Options & opt){
		fig_world w = parse_fig_file(filename);
		g_map canvas = get_canvas(w);

		world.maps.push_back(canvas);
		world.wpts.push_back(get_waypoint_list(w, canvas));
		vector<g_track> tracks = get_tracks(w, canvas);

		for (vector<g_track>::const_iterator i = tracks.begin(); i!=tracks.end(); i++)
		    world.trks.push_back(*i);
		return true;
	}
}
