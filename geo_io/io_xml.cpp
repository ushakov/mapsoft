#include "../utils/spirit_utils.h"
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>

#include <iostream>
#include <iomanip>

#include <string>
#include <cstring>
#include <time.h>

#include "io_xml.h"
#include "../utils/mapsoft_options.h"
#include "../utils/iconv_utils.h"


namespace xml {

        const char *default_charset = "KOI8-R";

	using namespace std;
	using namespace boost::spirit;

// function for reading objects from XML file
// into the world object
	bool read_file(const char* filename, geo_data & world, const Options & opt){

		xml_point pt;
		xml_point_list pt_list;
                geo_data ret;

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
			>> str_p("</waypoints>")[push_back_a(ret.wpts, pt_list)];
		rule_t trk_object = str_p("<track")[clear_a(pt_list)][clear_a(pt_list.points)]
			>> *attr[insert_at_a(pt_list, aname, aval)] >> ">" 
			>> *(*space_p >> point_object) >> *space_p 
			>> str_p("</track>")[push_back_a(ret.trks, pt_list)];
		rule_t map_object = str_p("<map")[clear_a(pt_list)][clear_a(pt_list.points)][insert_at_a(pt_list, "prefix", prefix)]
			>> *attr[insert_at_a(pt_list, aname, aval)] >> ">" 
			>> *(*space_p >> point_object) >> *space_p 
			>> str_p("</map>")[push_back_a(ret.maps, pt_list)];

		rule_t main_rule = *(*space_p >> (wpt_object | trk_object | map_object)) >> *space_p;

		if (!parse_file("fig::read", filename, main_rule)) return false;

		//преобразование комментариев и названий точек в UTF-8
		IConv cnv(default_charset);
		for (vector<g_waypoint_list>::iterator l=ret.wpts.begin(); l!=ret.wpts.end(); l++){
		  for (g_waypoint_list::iterator p=l->begin(); p!=l->end(); p++){
		    p->name = cnv.to_utf(p->name);
		    p->comm = cnv.to_utf(p->comm);
		  }
		}
		//преобразование комментариев к трекам в UTF-8
		for (vector<g_track>::iterator l=ret.trks.begin(); l!=ret.trks.end(); l++){
		  l->comm = cnv.to_utf(l->comm);
		}
		//преобразование комментариев к картам в UTF-8
		for (vector<g_map>::iterator l=ret.maps.begin(); l!=ret.maps.end(); l++){
		  l->comm = cnv.to_utf(l->comm);
		}

                world.wpts.insert(world.wpts.end(), ret.wpts.begin(), ret.wpts.end());
                world.trks.insert(world.trks.end(), ret.trks.begin(), ret.trks.end());
                world.maps.insert(world.maps.end(), ret.maps.begin(), ret.maps.end());

                return true;
	}

/********************************************/

	bool write_track(ofstream & f, const g_track & tr, const Options & opt){

                if (!f.good()) return false;
                IConv cnv(default_charset);

		g_trackpoint def_pt;
		g_track def_t;
		f << "<track points=" << tr.size();
		if (tr.width != def_t.width) f << " width="    << tr.width;
		if (tr.displ != def_t.displ) f << " displ="    << tr.displ;
                if (tr.color != def_t.color) f << " color=\""  << tr.color << "\"";
		if (tr.skip  != def_t.skip)  f << " skip="     << tr.skip;
		if (tr.type  != def_t.type)  f << " type="     << trk_type_enum.int2str(tr.type);
		if (tr.fill  != def_t.fill)  f << " fill="     << trk_fill_enum.int2str(tr.fill);
		if (tr.cfill != def_t.cfill) f << " cfill=\""  << tr.cfill << "\"";
		if (tr.comm  != def_t.comm)  f << " comm=\""   << cnv.from_utf(tr.comm) << "\"";
		f << ">\n";
                vector<g_trackpoint>::const_iterator p, b=tr.begin(), e=tr.end();
		for (p = b; p != e; p++){
			f << "  <pt";
                        if (p->y != def_pt.y) f << " lat=" << fixed << setprecision(6) << p->y;
                        if (p->x != def_pt.x) f << " lon=" << fixed << setprecision(6) << p->x;
                        if (p->z   < 1e20)    f << " alt=" << fixed << setprecision(1) << p->z;
                        if (p->depth < 1e20)  f << " depth=" << fixed << setprecision(1) << p->depth;
			if (p->t != def_pt.t) f << " time=\"" << p->t << "\"";
                        if (p->start)         f << " start";
			f << "/>\n";
		}
		f << "</track>\n";
	}

	bool write_waypoint_list(ofstream & f, const g_waypoint_list & wp, const Options & opt){

                if (!f.good()) return false;
                IConv cnv(default_charset);

		g_waypoint def_pt;
		g_waypoint_list def_w;
		f << "<waypoints points=" << wp.size();
		if (wp.symbset != def_w.symbset) f << "symbset=" << wp.symbset;
		f << ">\n";
		vector<g_waypoint>::const_iterator p, b=wp.begin(), e=wp.end();
		for (p = b; p!=e; p++){
			f << "  <pt";
                        if (p->y != def_pt.y)       f << " lat=" << fixed << setprecision(6) << p->y;
                        if (p->x != def_pt.x)       f << " lon=" << fixed << setprecision(6) << p->x;
                        if (p->z   < 1e20)          f << " alt=" << fixed << setprecision(1) << p->z;
			if (p->t != def_pt.t)       f << " time=\"" << p->t << "\"";
                        if (p->name != def_pt.name) f << " name=\"" << cnv.from_utf(p->name) << "\"";
                        if (p->comm != def_pt.comm)             f << " comm=\"" << cnv.from_utf(p->comm) << "\"";
                        if (p->prox_dist != def_pt.prox_dist)   f << " prox_dist="  << fixed << setprecision(1) << p->prox_dist;
                        if (p->symb != def_pt.symb)             f << " symb="       << wpt_symb_enum.int2str(p->symb);
                        if (p->displ != def_pt.displ)           f << " displ="      << p->displ;
                        if (p->color != def_pt.color)           f << " color=\""    << p->color << "\"";
                        if (p->bgcolor != def_pt.bgcolor)       f << " bgcolor=\""  << p->bgcolor << "\"";
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

                if (!f.good()) return false;
                IConv cnv(default_charset);

		g_refpoint def_pt;
		g_map def_m;
		f << "<map points=" << m.size();
                if (m.comm != def_m.comm) f << " comm=\""   << cnv.from_utf(m.comm) << "\"";
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
		vector<g_refpoint>::const_iterator p, b=m.begin(), e=m.end();
		for (p = b; p!=e; p++){
			f << "  <pt";
                        if (p->x    != def_pt.y)    f << " x="  << fixed << setprecision(6) << p->x;
                        if (p->y    != def_pt.x)    f << " y="  << fixed << setprecision(6) << p->y;
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

/********************************************/

// Для всех типов точек - один map<string,string>, но разные преобразования
// Это все вынесено в отдельный h-файл, поскольку используется и при чтении точек из fig
	xml_point::operator g_waypoint () const {
		g_waypoint ret; // здесь уже возникли значения по умолчанию
		get("name", ret.name);
		get("comm", ret.comm);
		get("lon",  ret.x);
		get("lat",  ret.y);
		get("alt",  ret.z);
		get("prox_dist",  ret.prox_dist);
                ret.symb       = wpt_symb_enum.str2int(get_string("symb"));
		get("displ",    ret.displ);
		get("color",    ret.color);
		get("bgcolor",  ret.bgcolor);
                ret.map_displ  = wpt_map_displ_enum.str2int(get_string("map_displ"));
                ret.pt_dir     = wpt_pt_dir_enum.str2int(get_string("pt_dir"));
		get("font_size",  ret.font_size);
		get("font_style", ret.font_style);
		get("size",       ret.size);
		get("time", ret.t);
		const std::string used[] = {
                "name", "comm", "lon", "lat", "alt", "prox_dist", "symb", 
                "displ", "color", "bgcolor", "map_displ", "pt_dir", "font_size",
                "font_style", "size", "time", ""};
		warn_unused(used);
		return ret;
	}
	xml_point::operator g_trackpoint () const{
		g_trackpoint ret;
		get("lon",   ret.x);
		get("lat",   ret.y);
		get("alt",   ret.z);
		get("depth", ret.depth);
		get("start", ret.start);
		get("time",  ret.t);
              const std::string used[] = {"lon", "lat", "alt", "depth", "start", "time", ""};
              warn_unused(used);
		return ret;
	}
	xml_point::operator g_refpoint () const{
		g_refpoint ret;
		get("x",  ret.x);
		get("y",  ret.y);
		get("xr", ret.xr);
		get("yr", ret.yr);
              const std::string used[] = {"x", "y", "xr", "yr", ""};
              warn_unused(used);
		return ret;
	}

	xml_point_list::operator g_waypoint_list () const {
		g_waypoint_list ret;
		ret.symbset = get_string("symbset", ret.symbset);
		const std::string used[] = {"symbset","points",""}; //points - только записывается, не читается.
                warn_unused(used);
		for (std::vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
			ret.push_back(*i);
		return ret;
	}
	xml_point_list::operator g_track () const {
		g_track ret;
		get("comm",  ret.comm);
		get("width", ret.width);
		get("color", ret.color);
		get("skip",  ret.skip);
		get("displ", ret.displ);
                ret.type  = trk_type_enum.str2int(get_string("type"));
                ret.fill  = trk_fill_enum.str2int(get_string("fill"));
		get("cfill", ret.cfill);
                const std::string used[] = {"comm", "width", "color", "skip", "displ", "type", "fill", "cfill", "points", ""};
                warn_unused(used);
		for (std::vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
			ret.push_back(*i);
		return ret;
	}
	xml_point_list::operator g_map () const {
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
		for (std::vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
			ret.push_back(*i);
		return ret;
	}

}
