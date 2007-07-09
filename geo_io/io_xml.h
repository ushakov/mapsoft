#ifndef IO_XML_H
#define IO_XML_H

#include <vector>

#include "geo_data.h"
#include "../utils/mapsoft_options.h"

namespace xml {
// Для всех типов точек - один map<string,string>, но разные преобразования
// Это все вынесено в отдельный h-файл, поскольку используется и при чтении точек из fig
    struct xml_point : Options{
	operator g_waypoint () const {
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
	operator g_trackpoint () const{
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
	operator g_refpoint () const{
		g_refpoint ret;
		get("x",  ret.x);
		get("y",  ret.y);
		get("xr", ret.xr);
		get("yr", ret.yr);
              const std::string used[] = {"x", "y", "xr", "yr", ""};
              warn_unused(used);
		return ret;
	}
    };
    struct xml_point_list : Options {
	std::vector<xml_point> points;

	operator g_waypoint_list () const {
		g_waypoint_list ret;
		ret.symbset = get_string("symbset", ret.symbset);
		const std::string used[] = {"symbset","points",""}; //points - только записывается, не читается.
                warn_unused(used);
		for (std::vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
			ret.push_back(*i);
		return ret;
	}
	operator g_track () const {
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
		for (std::vector<xml_point>::const_iterator i=points.begin(); i!=points.end();i++)
			ret.push_back(*i);
		return ret;
	}
    };
}
#endif
