#include "utils/spirit_utils.h"
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_insert_at_actor.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>

#include <iostream>
#include <iomanip>

#include <string>
#include <cstring>
#include <time.h>

#include "io_xml.h"
#include "options/options.h"
#include "utils/iconv_utils.h"
#include "err/err.h"
#include "geo/geo_convs.h"

namespace xml {

        const char *default_charset = "KOI8-R";

	using namespace std;
	using namespace boost::spirit::classic;

// function for reading objects from XML file
// into the world object
	void read_file(const char* filename, geo_data & world, const Options & opt){

		xml_point pt;
		xml_point_list pt_list;
                xml_map_list map_list, top_map_list;

                geo_data ret;

		// prefix -- директория, в которой лежит map-файл, на случай,
		// если в нем относительные названия граф.файлов
		std::string prefix("");

		char *sl = strrchr((char *)filename, '/');
		if (sl!=NULL){
		    int pos = sl-filename;
		    std::string fn1(filename);
  	            prefix=std::string(fn1.begin(),fn1.begin()+pos) + "/";
		}

                if (opt.exists("verbose")) cerr <<
                  "Reading data from Mapsoft XML file " << filename << endl;

		string aname, aval;
		const string oo="1";
		rule_t attr_name = (+(alnum_p | '-' | '_'))[assign_a(aname)][assign_a(aval, oo)];

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
		rule_t map_object = str_p("<map")[clear_a(pt_list)][insert_at_a(pt_list, "prefix", prefix)][clear_a(pt_list.points)]
			>> *attr[insert_at_a(pt_list, aname, aval)] >> ">" 
			>> *(*space_p >> point_object) >> *space_p 
			>> str_p("</map>")[push_back_a(map_list.maps, pt_list)];
                // topmap_object is for top-level maps in old-style files
		rule_t topmap_object = str_p("<map")[clear_a(pt_list)][insert_at_a(pt_list, "prefix", prefix)][clear_a(pt_list.points)]
			>> *attr[insert_at_a(pt_list, aname, aval)] >> ">" 
			>> *(*space_p >> point_object) >> *space_p 
			>> str_p("</map>")[push_back_a(top_map_list.maps, pt_list)];

		rule_t maps_object = str_p("<maps")[clear_a(map_list.maps)]
			>> *attr[insert_at_a(map_list, aname, aval)] >> ">" 
			>> *(*space_p >> map_object) >> *space_p 
			>> str_p("</maps>")[push_back_a(ret.maps, map_list)];

		rule_t main_rule = *(*space_p >> (wpt_object | trk_object | topmap_object | maps_object)) >> *space_p;

		if (!parse_file("xml::read", filename, main_rule))
                  throw Err()
                    << "Can't parse mapsoft XML file " << filename;

                if (top_map_list.maps.size()>0) ret.maps.push_back(top_map_list);

		// convert wpt names and comments to UTF-8
		IConv cnv(default_charset);
		for (vector<g_waypoint_list>::iterator l=ret.wpts.begin(); l!=ret.wpts.end(); l++){
                  l->comm = cnv.to_utf8(l->comm);
		  for (g_waypoint_list::iterator p=l->begin(); p!=l->end(); p++){
		    p->name = cnv.to_utf8(p->name);
		    p->comm = cnv.to_utf8(p->comm);
		  }
		}
		// convert track comments to UTF-8
		for (vector<g_track>::iterator l=ret.trks.begin(); l!=ret.trks.end(); l++){
		  l->comm = cnv.to_utf8(l->comm);
		}
		// convert map comments to UTF-8, add lon0 if needed
		for (vector<g_map_list>::iterator ll=ret.maps.begin(); ll!=ret.maps.end(); ll++){
		  ll->comm = cnv.to_utf8(ll->comm);
		  for (vector<g_map>::iterator l=ll->begin(); l!=ll->end(); l++){
		    l->comm = cnv.to_utf8(l->comm);
                    // old tmerc xml files does not have lon0 option!
                    if (l->map_proj==Proj("tmerc") && !l->proj_opts.exists("lon0"))
                      l->proj_opts.put("lon0", convs::map_lon0(*l));
		  }
		}



                world.wpts.insert(world.wpts.end(), ret.wpts.begin(), ret.wpts.end());
                world.trks.insert(world.trks.end(), ret.trks.begin(), ret.trks.end());
                world.maps.insert(world.maps.end(), ret.maps.begin(), ret.maps.end());
	}

/********************************************/

	bool write_track(ofstream & f, const g_track & tr, const Options & opt){

                if (!f.good()) return false;
                IConv cnv(default_charset);
		Enum::output_fmt=Enum::xml_fmt;

		g_trackpoint def_pt;
		g_track def_t;
		f << "<track points=" << tr.size();
		if (tr.width != def_t.width) f << " width="    << tr.width;
		if (tr.displ != def_t.displ) f << " displ="    << tr.displ;
                if (tr.color != def_t.color) f << " color=\"#" << hex << tr.color << "\"" << dec;
		if (tr.skip  != def_t.skip)  f << " skip="     << tr.skip;
		if (tr.type  != def_t.type)  f << " type="     << tr.type;
		if (tr.fill  != def_t.fill)  f << " fill="     << tr.fill;
		if (tr.cfill != def_t.cfill) f << " cfill=\"#"  << hex << tr.cfill << "\"" << dec;
		if (tr.comm  != def_t.comm)  f << " comm=\""   << cnv.from_utf8(tr.comm) << "\"";
		f << ">\n";
                vector<g_trackpoint>::const_iterator p, b=tr.begin(), e=tr.end();
		for (p = b; p != e; p++){
			f << "  <pt";
                        if (p->y != def_pt.y) f << " lat=" << fixed << setprecision(6) << p->y;
                        if (p->x != def_pt.x) f << " lon=" << fixed << setprecision(6) << p->x;
                        if (p->have_alt())    f << " alt=" << fixed << setprecision(1) << p->z;
                        if (p->have_depth())  f << " depth=" << fixed << setprecision(1) << p->depth;
			if (p->t != def_pt.t) f << " time=\"" << p->t << "\"";
                        if (p->start)         f << " start";
			f << "/>\n";
		}
		f << "</track>\n";
		return true;
	}

	bool write_waypoint_list(ofstream & f, const g_waypoint_list & wp, const Options & opt){

                if (!f.good()) return false;
                IConv cnv(default_charset);
		Enum::output_fmt=Enum::xml_fmt;

		g_waypoint def_pt;
		g_waypoint_list def_w;
		f << "<waypoints points=" << wp.size();
		if (wp.symbset != def_w.symbset) f << " symbset=" << wp.symbset;
		if (wp.comm    != def_w.comm)    f << " comm=\""   << cnv.from_utf8(wp.comm) << "\"";
		f << ">\n";
		vector<g_waypoint>::const_iterator p, b=wp.begin(), e=wp.end();
		for (p = b; p!=e; p++){
			f << "  <pt";
                        if (p->y != def_pt.y)       f << " lat=" << fixed << setprecision(6) << p->y;
                        if (p->x != def_pt.x)       f << " lon=" << fixed << setprecision(6) << p->x;
                        if (p->have_alt())          f << " alt=" << fixed << setprecision(1) << p->z;
			if (p->t != def_pt.t)       f << " time=\"" << p->t << "\"";
                        if (p->name != def_pt.name) f << " name=\"" << cnv.from_utf8(p->name) << "\"";
                        if (p->comm != def_pt.comm)             f << " comm=\"" << cnv.from_utf8(p->comm) << "\"";
                        if (p->prox_dist != def_pt.prox_dist)   f << " prox_dist="  << fixed << setprecision(1) << p->prox_dist;
                        if (p->symb != def_pt.symb)             f << " symb="       << p->symb;
                        if (p->displ != def_pt.displ)           f << " displ="      << p->displ;
                        if (p->color != def_pt.color)           f << " color=\""    << hex << p->color << "\"" << dec;
                        if (p->bgcolor != def_pt.bgcolor)       f << " bgcolor=\""  << hex << p->bgcolor << "\"" << dec;
                        if (p->map_displ != def_pt.map_displ)   f << " map_displ="  << p->map_displ;
                        if (p->pt_dir != def_pt.pt_dir)         f << " pt_dir="     << p->pt_dir;
                        if (p->font_size != def_pt.font_size)   f << " font_size="  << p->font_size;
                        if (p->font_style != def_pt.font_style) f << " font_style=" << p->font_style;
                        if (p->size != def_pt.size)             f << " size="       << p->size;
			f << "/>\n";
		}
		f << "</waypoints>\n";
		return true;
	}

	bool write_map(ofstream & f, const g_map & m, const Options & opt){

                if (!f.good()) return false;
                IConv cnv(default_charset);
		Enum::output_fmt=Enum::xml_fmt;

		g_refpoint def_pt;
		g_map def_m;
		f << "  <map points=" << m.size();
                if (m.comm != def_m.comm) f << " comm=\""   << cnv.from_utf8(m.comm) << "\"";
                if (m.file != def_m.file) f << " file=\""   << m.file << "\"";
                //if (m.proj_str  != def_m.proj_str)  f << "\n       proj_str=\"" << m.proj_str << "\"";
                if (m.map_proj  != def_m.map_proj)  f << "\n       map_proj=\"" << m.map_proj << "\"";
                if (m.map_datum != def_m.map_datum) f << " map_datum=\"" << m.map_datum << "\"";
                for (Options::const_iterator i=m.proj_opts.begin(); i!=m.proj_opts.end(); i++)
                  f << " " << i->first << "=\"" << i->second << "\"";

                if (m.tfmt  != def_m.tfmt)  f << " tile_fmt=\""   << m.tfmt << "\"";
                if (m.tsize != def_m.tsize) f << " tile_size=\""  << m.tsize << "\"";
                if (m.tswap != def_m.tswap) f << " tile_swapy=\"" << m.tswap << "\"";

		if (m.border.size()!=0){
			f << "\n       border=\"";
			for (size_t i = 0; i<m.border.size(); i++){
				if (i!=0) f << ",";
				f << m.border[i].x << "," << m.border[i].y;
			}
			f << "\"";
		}
		f << ">\n";
		vector<g_refpoint>::const_iterator p, b=m.begin(), e=m.end();
		for (p = b; p!=e; p++){
			f << "    <pt";
                        if (p->x    != def_pt.y)    f << " x="  << fixed << setprecision(6) << p->x;
                        if (p->y    != def_pt.x)    f << " y="  << fixed << setprecision(6) << p->y;
                        if (p->xr   != def_pt.xr)   f << " xr=" << fixed << setprecision(1) << p->xr;
                        if (p->yr   != def_pt.yr)   f << " yr=" << fixed << setprecision(1) << p->yr; 
			f << "/>\n";
		}
		f << "  </map>\n";
		return true;
	}

	bool write_map_list(ofstream & f, const g_map_list & m, const Options & opt){

                if (!f.good()) return false;
                IConv cnv(default_charset);

		g_map_list def_m;
		f << "<maps";
                if (m.comm != def_m.comm) f << " comm=\""   << cnv.from_utf8(m.comm) << "\"";
		f << ">\n";

		g_map_list::const_iterator map, b=m.begin(), e=m.end();
		for (map = b; map!=e; map++) write_map (f, *map, opt);
		f << "</maps>\n";
		return true;
	}



	void write_file(const char* filename, const geo_data & world, const Options & opt){
	  if (opt.exists("verbose")) cerr <<
	   "Writing data to Mapsoft XML file " << filename << endl;

	  ofstream f(filename);

	  for (vector<g_waypoint_list>::const_iterator i = world.wpts.begin(); i!=world.wpts.end(); i++){
	    if (!write_waypoint_list(f, *i, opt))
	      throw Err() << "Can't write data to Mapsoft XML file " << filename;
	  }
	  for (vector<g_track>::const_iterator i = world.trks.begin(); i!=world.trks.end(); i++){
	    if (!write_track(f, *i, opt))
	      throw Err() << "Can't write data to Mapsoft XML file " << filename;
	  }
	  for (vector<g_map_list>::const_iterator i = world.maps.begin(); i!=world.maps.end(); i++){
	    if (!write_map_list(f, *i, opt))
	      throw Err() << "Can't write data to Mapsoft XML file " << filename;
	  }
	}

/********************************************/

// Для всех типов точек - один map<string,string>, но разные преобразования
// Это все вынесено в отдельный h-файл, поскольку используется и при чтении точек из fig
	xml_point::operator g_waypoint () const {
          g_waypoint ret; ret.parse_from_options(*this);
	  return ret;
	}
	xml_point::operator g_trackpoint () const{
          g_trackpoint ret; ret.parse_from_options(*this);
	  return ret;
	}
	xml_point::operator g_refpoint () const{
          g_refpoint ret; ret.parse_from_options(*this);
	  return ret;
	}

	xml_point_list::operator g_waypoint_list () const {
	  g_waypoint_list ret; ret.parse_from_options(*this);
	  for (std::vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
	    ret.push_back(*i);
	  return ret;
	}
	xml_point_list::operator g_track () const {
	  g_track ret; ret.parse_from_options(*this);
	  for (std::vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
	    ret.push_back(*i);
	  return ret;
	}
	xml_point_list::operator g_map () const {
          g_map ret; ret.parse_from_options(*this);
          for (std::vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
            ret.push_back(*i);
          return ret;
        }

	xml_map_list::operator g_map_list () const {
	  g_map_list ret;
          ret.vector<g_map>::operator=(maps);
	  ret.parse_from_options(*this);
	  return ret;
	}
}
