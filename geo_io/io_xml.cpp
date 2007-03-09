#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <vector>
#include <string>
#include <time.h>

#include "geo_data.h"
#include "geo_enums.h"
#include "io.h"
#include "geo_names.h"
#include "../utils/mapsoft_options.h"
#include "../utils/mapsoft_time.h"




namespace xml {

	using namespace std;
	using namespace boost::spirit;

	typedef char                    char_t;
	typedef file_iterator <char_t>  iterator_t;
	typedef scanner<iterator_t>     scanner_t;
	typedef rule <scanner_t>        rule_t;

// format-specific data types and conversions to geo_data.h types...
// Для всех типов точек - один map<string,string>, но разные преобразования

	struct xml_point : Options{
                
		operator g_waypoint () const {
			g_waypoint ret; // здесь уже возникли значения по умолчанию
			ret.name = get_string("name");
			ret.comm = get_string("comm");
			ret.x    = get_double("lon",  ret.x);
			ret.y    = get_double("lat",  ret.y);
			ret.z    = get_double("alt",  ret.z);
			ret.prox_dist  = get_double("prox_dist",  ret.prox_dist);
                        ret.symb       = wpt_symb_enum.str2int(get_string("symb"));
			ret.displ      = get_int("displ",    ret.displ);
			ret.color      = get_hex("color",    ret.color);
			ret.bgcolor    = get_hex("bgcolor",  ret.bgcolor);
                        ret.map_displ  = wpt_map_displ_enum.str2int(get_string("map_displ"));
                        ret.pt_dir     = wpt_pt_dir_enum.str2int(get_string("pt_dir"));
			ret.font_size  = get_int("font_size",  ret.font_size);
			ret.font_style = get_int("font_style", ret.font_style);
			ret.size       = get_int("size",       ret.size);
			ret.t          = get_time("time",      ret.t);
			const std::string used[] = {
                          "name", "comm", "lon", "lat", "alt", "prox_dist", "symb", 
                          "displ", "color", "bgcolor", "map_displ", "pt_dir", "font_size",
                          "font_style", "size", "time", ""};
			warn_unused(used);
			return ret;
		}
		operator g_trackpoint () const{
			g_trackpoint ret;
			ret.x = get_double("lon",  ret.x);
			ret.y = get_double("lat",  ret.y);
			ret.z = get_double("alt",  ret.z);
			ret.depth = get_double("depth",ret.depth);
			ret.start = get_bool("start");
			ret.t     = get_time("time", ret.t);
                        const std::string used[] = {"lon", "lat", "alt", "depth", "start", "time", ""};
                        warn_unused(used);
			return ret;
		}
		operator g_refpoint () const{
			g_refpoint ret;
			ret.x  = get_double("x",  ret.x);
			ret.y  = get_double("y",  ret.y);
			ret.xr = get_double("xr", ret.xr);
			ret.yr = get_double("yr", ret.yr);
                        const std::string used[] = {"x", "y", "xr", "yr", ""};
                        warn_unused(used);
			return ret;
		}
	};

	struct xml_point_list : Options {
		vector<xml_point> points;

		operator g_waypoint_list () const {
			g_waypoint_list ret;
			ret.symbset = get_string("symbset", ret.symbset);
			const std::string used[] = {"symbset","points",""}; //points - только записывается, не читается.
                        warn_unused(used);
			for (vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
				ret.points.push_back(*i);
			return ret;
		}
		operator g_track () const {
			g_track ret;
			ret.comm  = get_string("comm",  ret.comm);
			ret.width = get_int   ("width", ret.width);
			ret.color = get_hex   ("color", ret.color);
			ret.skip  = get_int   ("skip",  ret.skip);
			ret.displ = get_int   ("displ", ret.displ);
                        ret.type  = trk_type_enum.str2int(get_string("type"));
                        ret.fill  = trk_fill_enum.str2int(get_string("fill"));
			ret.cfill = get_hex   ("cfill", ret.cfill);
                        const std::string used[] = {"comm", "width", "color", "skip", "displ", "type", "fill", "cfill", "points", ""};
                        warn_unused(used);
			for (vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
				ret.points.push_back(*i);
			return ret;
		}
		operator g_map () const {
			g_map ret;
			ret.comm = get_string("comm",  ret.comm);
			ret.file = get_string("file",  ret.file);

			std::string prefix = get_string("prefix",  "");
                       // если не абсолютный путь - добавим prefix
                        if ((ret.file.size()<1)||(ret.file[0]!='/'))
                            ret.file=prefix+ret.file;

                        ret.map_proj = Proj(get_string("map_proj"));
			ret.border = get_poly("border");
                        const std::string used[] = {"comm", "file", "map_proj", "border", "points", "prefix", ""};
                        warn_unused(used);
			for (vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
				ret.points.push_back(*i);
			return ret;
		}
	};


// function for reading objects from XML file
// into the world object
	bool read_file(const char* filename, geo_data & world, const Options & opt){

		// iterators for parsing
		iterator_t first(filename);
		if (!first) { cerr << "can't find file " << filename << '\n'; return 0;}
		iterator_t last = first.make_end();

		xml_point pt;
		xml_point_list pt_list;

		// prefix -- директория, в которой лежит map-файл, на случай,
		// если в нем относительные названия граф.файлов
		std::string prefix("");

		char *sl = rindex(filename, '/');
		if (sl!=NULL){
		    int pos = sl-filename;
		    std::string fn1(filename);
  	            prefix=std::string(fn1.begin(),fn1.begin()+pos) + "/";
		}

		string aname, aval;

		rule_t attr_name = (+(alnum_p | '-' | '_'))[assign_a(aname)][assign_a(aval, "")];

		rule_t escaped_symb  = (ch_p('\\') >> ch_p('"')) | (ch_p('\\') >> ch_p('\\')) ;

		rule_t attr_value = ('"' >>
			(*((anychar_p | escaped_symb ) - '"'))[assign_a(aval)] >> '"') |
			(*(anychar_p - (space_p | ch_p('"') | '>' | '/' | '\\')))[assign_a(aval)];

		rule_t attr = +space_p >> attr_name >>
			!(*space_p >> '=' >> *space_p >> attr_value);

		rule_t point_object = str_p("<pt")[clear_a(pt)]
			>> *attr[insert_at_a(pt, aname, aval)] 
			>> str_p("/>")[push_back_a(pt_list.points, pt)];

		rule_t wpt_object = str_p("<waypoints")[clear_a(pt_list)][clear_a(pt_list.points)]
			>> *attr[insert_at_a(pt_list, aname, aval)] >> ">" 
			>> *(*space_p >> point_object) >> *space_p 
			>> str_p("</waypoints>")[push_back_a(world.wpts, pt_list)];
		rule_t trk_object = str_p("<track")[clear_a(pt_list)][clear_a(pt_list.points)] 
			>> *attr[insert_at_a(pt_list, aname, aval)] >> ">" 
			>> *(*space_p >> point_object) >> *space_p 
			>> str_p("</track>")[push_back_a(world.trks, pt_list)];
		rule_t map_object = str_p("<map")[clear_a(pt_list)][clear_a(pt_list.points)][insert_at_a(pt_list, "prefix", prefix)]
			>> *attr[insert_at_a(pt_list, aname, aval)] >> ">" 
			>> *(*space_p >> point_object) >> *space_p 
			>> str_p("</map>")[push_back_a(world.maps, pt_list)];

		parse_info<iterator_t> info = parse(first, last,
		   *(*space_p >> (wpt_object || trk_object || map_object)) >> *space_p);

		return info.full;
	}

/********************************************/

	bool write_track(ofstream & f, const g_track & tr, const Options & opt){
		g_trackpoint def_pt;
		g_track def_t;
		f << "<track points=" << tr.points.size();
		if (tr.width != def_t.width) f << " width=" << tr.width;
		if (tr.displ != def_t.displ) f << " displ=" << tr.displ;
                if (tr.color != def_t.color) f << " color=\"#" << setbase(16) << setw(6) << setfill('0') << tr.color << "\"" << setbase(10);
		if (tr.skip  != def_t.skip)  f << " skip="  << tr.skip;
		if (tr.type  != def_t.type)  f << " type="  << trk_type_enum.int2str(tr.type);
		if (tr.fill  != def_t.fill)  f << " fill="  << trk_fill_enum.int2str(tr.fill);
		if (tr.cfill != def_t.cfill) f << " cfill=\"#" << setbase(16) << setw(6) << setfill('0') << tr.cfill << "\"" << setbase(10);
		if (tr.comm  != def_t.comm)  f << " comm=\""  << tr.comm << "\"";
		f << ">\n";
                vector<g_trackpoint>::const_iterator p, b=tr.points.begin(), e=tr.points.end();
		for (p = b; p != e; p++){
			f << "  <pt";
                        if (p->y != def_pt.y) f << " lat=" << fixed << setprecision(6) << p->y;
                        if (p->x != def_pt.x) f << " lon=" << fixed << setprecision(6) << p->x;
                        if (p->z   < 1e20)    f << " alt=" << fixed << setprecision(1) << p->z;
                        if (p->depth < 1e20)  f << " depth=" << fixed << setprecision(1) << p->depth;
			if (p->t != def_pt.t) f << " time=\"" << time2str(p->t) << "\"";
                        if (p->start)         f << " start";
			f << "/>\n";
		}
		f << "</track>\n";
	}

	bool write_waypoint_list(ofstream & f, const g_waypoint_list & wp, const Options & opt){
		g_waypoint def_pt;
		g_waypoint_list def_w;
		f << "<waypoints points=" << wp.points.size();
		if (wp.symbset != def_w.symbset) f << "symbset=" << wp.symbset;
		f << ">\n";
		vector<g_waypoint>::const_iterator p, b=wp.points.begin(), e=wp.points.end();
		for (p = b; p!=e; p++){
			f << "  <pt";
                        if (p->y != def_pt.y)       f << " lat=" << fixed << setprecision(6) << p->y;
                        if (p->x != def_pt.x)       f << " lon=" << fixed << setprecision(6) << p->x;
                        if (p->z   < 1e20)          f << " alt=" << fixed << setprecision(1) << p->z;
			if (p->t != def_pt.t)       f << " time=\"" << time2str(p->t) << "\"";
                        if (p->name != def_pt.name) f << " name=\"" << p->name << "\"";
                        if (p->comm != def_pt.comm)             f << " comm=\"" << p->comm << "\"";
                        if (p->prox_dist != def_pt.prox_dist)   f << " prox_dist="  << fixed << setprecision(1) << p->prox_dist;
                        if (p->symb != def_pt.symb)             f << " symb="       << wpt_symb_enum.int2str(p->symb);
                        if (p->displ != def_pt.displ)           f << " displ="      << p->displ;
                        if (p->color != def_pt.color)           f << " color=\"#"   << setbase(16) << setw(6) << setfill('0') << p->color << "\"" << setbase(10);
                        if (p->bgcolor != def_pt.bgcolor)       f << " bgcolor=\"#" << setbase(16) << setw(6) << setfill('0') << p->bgcolor << "\"" << setbase(10);
                        if (p->map_displ != def_pt.map_displ)   f << " map_displ="  << wpt_map_displ_enum.int2str(p->map_displ);
                        if (p->pt_dir != def_pt.pt_dir)         f << " pt_dir="     << wpt_pt_dir_enum.int2str(p->pt_dir);
                        if (p->font_size != def_pt.font_size)   f << " font_size="  << p->font_size;
                        if (p->font_style != def_pt.font_style) f << " font_style=" << p->font_style;
                        if (p->size != def_pt.size)             f << " size="       << p->size;
			f << "/>\n";
		}
		f << "</waypoints>\n";
	}

	bool write_map(ofstream & f, const g_map & m, const Options & opt){
		g_refpoint def_pt;
		g_map def_m;
		f << "<map points=" << m.points.size();
                if (m.comm != def_m.comm) f << " comm=\""   << m.comm << "\"";
                if (m.file != def_m.file) f << " file=\""   << m.file << "\"";
                if (m.map_proj != def_m.map_proj) f << " map_proj=" << m.map_proj.xml_str();
		if (m.border.size()!=0){
			f << " border=\"";
			for (int i = 0; i<m.border.size(); i++){
				if (i!=0) f << ",";
				f << m.border[i].x << "," << m.border[i].y;
			}
			f << "\"";
		}
		f << ">\n";
		vector<g_refpoint>::const_iterator p, b=m.points.begin(), e=m.points.end();
		for (p = b; p!=e; p++){
			f << "  <pt";
                        if (p->y    != def_pt.y)    f << " x="  << fixed << setprecision(6) << p->x;
                        if (p->x    != def_pt.x)    f << " y="  << fixed << setprecision(6) << p->y;
                        if (p->xr   != def_pt.xr)   f << " xr=" << fixed << setprecision(1) << p->xr;
                        if (p->yr   != def_pt.yr)   f << " yr=" << fixed << setprecision(1) << p->yr;
			f << "/>\n";
		}
		f << "</map>\n";
	}



	bool write_file(const char* filename, const geo_data & world, const Options & opt){
		ofstream f(filename);

		for (vector<g_waypoint_list>::const_iterator i = world.wpts.begin(); i!=world.wpts.end(); i++)
			write_waypoint_list(f, *i, opt);
		for (vector<g_track>::const_iterator i = world.trks.begin(); i!=world.trks.end(); i++)
			write_track(f, *i, opt);
		for (vector<g_map>::const_iterator i = world.maps.begin(); i!=world.maps.end(); i++)
			write_map(f, *i, opt);
		return true;
	}
}
