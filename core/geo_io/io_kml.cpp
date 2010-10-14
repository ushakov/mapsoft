#include "utils/spirit_utils.h"
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>

#include <vector>
#include <string>
#include <map>

#include "io_kml.h"

#include <math.h>

#include "jeeps/gpsmath.h"
#include "jeeps/gpsdatum.h"


namespace kml {
    using namespace boost::spirit::classic;
    
    // Записывает в KML-файл треки и точки
    // Не записывает карты! (хм, а может, надо?)
    bool write_file (const char* filename, const geo_data & world, const Options & opt){
	std::ofstream f(filename);
	if (!f.good()) return false;

	f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	f << "<kml xmlns=\"http://earth.google.com/kml/2.1\">" << std::endl;
	f << "  <Document>" << std::endl;

	for (int i = 0; i < world.wpts.size(); i++) {
	    f << "  <Folder>" << std::endl;
	    f << "    <name>WPTS_" << i << "</name>" << std::endl;
	    for (g_waypoint_list::const_iterator wp = world.wpts[i].begin();
		 wp != world.wpts[i].end(); ++wp) {
		f << "    <Placemark>" << std::endl;
		f << "      <name><![CDATA[" << wp->name << "]]></name>" << std::endl;
		f << "      <description><![CDATA[" << wp->comm << "]]></description>" << std::endl;
		f << "      <Point>" << std::endl;
		f << "        <coordinates>" << wp->x << "," << wp->y << "," << wp->z << "</coordinates>" << std::endl;
		f << "      </Point>" << std::endl;
		f << "    </Placemark>" << std::endl;
	    }
	    f << "  </Folder>" << std::endl;
	}

	for (int i = 0; i < world.trks.size(); ++i) {
	    f << "  <Placemark>" << std::endl;
	    f << "    <description><![CDATA[" << world.trks[i].comm << "]]></description>" << std::endl;
	    f << "    <MultiGeometry>" << std::endl;
	    for (g_track::const_iterator tp = world.trks[i].begin(); tp != world.trks[i].end(); ++tp) {
		if (tp->start || tp == world.trks[i].begin()) {
		    if (tp != world.trks[i].begin()) {
			f << "        </coordinates>" << std::endl;
			f << "      </LineString>" << std::endl;
		    }
		    f << "      <LineString>" << std::endl;
		    f << "        <tesselate>1</tesselate>" << std::endl;
		    f << "        <coordinates>" << std::endl;
		}	
		f << "          " << tp->x << "," << tp->y << "," << tp->z << std::endl;
	    }
	    f << "        </coordinates>" << std::endl;
	    f << "      </LineString>" << std::endl;
	    f << "    </MultiGeometry>" << std::endl;
	    f << "  </Placemark>" << std::endl;
	}
	
	f << "  </Document>" << std::endl;
	f << "</kml>" << std::endl;
	
	return f.good();
    }
/*
    struct kml_point : public Options {
	operator g_waypoint() const {
	    g_waypoint ret;

	    get("name", ret.name);
	    get("comm", ret.comm);
	    get("lon",  ret.x);
	    get("lat",  ret.y);
	    get("alt",  ret.z);
	    const std::string used[] = {
                "name", "comm", "lon", "lat", "alt", ""};
	    warn_unused(used);
	    return ret;
	}

	operator g_trackpoint() const {
	    g_trackpoint ret;

	    get("lon",  ret.x);
	    get("lat",  ret.y);
	    get("alt",  ret.z);
	    const std::string used[] = {
                "lon", "lat", "alt", ""};
	    warn_unused(used);
	    return ret;
	}
    };

    struct kml_point_list : public Options {
	std::vector<kml_point> points;

	operator g_waypoint_list () const {
	    g_waypoint_list ret;
	    const std::string used[] = {"points",""}; //points - только записывается, не читается.
	    warn_unused(used);
	    for (std::vector<kml_point>::const_iterator i=points.begin(); i!=points.end();i++)
		ret.push_back(*i);
	    return ret;
	}
	
	operator g_track () const {
	    g_track ret;
	    ret.comm  = get_string("comm",  ret.comm);
	    const std::string used[] = {"comm", "points", ""};
	    warn_unused(used);
	    for (std::vector<kml_point>::const_iterator i=points.begin(); i!=points.end();i++)
		ret.push_back(*i);
	    return ret;
	}
    };
*/

//     // incomplete!
//     bool read_file(const char* filename, geo_data & world, const Options & opt) {
// 	// iterators for parsing
// 	iterator_t first(filename);
// 	if (!first) { cerr << "can't find file " << filename << '\n'; return 0;}
// 	iterator_t last = first.make_end();

// 	kml_point pt;
// 	kml_point_list pt_list;

// 	string aname, aval;
// 	rule_t name = (+(alnum_p | '-' | '_'));
// 	rule_t attr_name = name[assign_a(aname)][assign_a(aval, "")];
// 	rule_t escaped_symb  = (ch_p('\\') >> ch_p('"')) | (ch_p('\\') >> ch_p('\\')) ;
// 	rule_t attr_value = ('"' >>
// 			     (*((anychar_p | escaped_symb ) - '"'))[assign_a(aval)] >> '"') |
// 	    (*(anychar_p - (space_p | ch_p('"') | '>' | '/' | '\\')))[assign_a(aval)];

// 	rule_t attr = +space_p >> attr_name >>
// 	    !(*space_p >> '=' >> *space_p >> attr_value);
	
// 	rule_t any_tag_open = *space_p >> ch_p('<') >> name >> *attr >> ch_p('>');
// 	rule_t any_tag_close = *space_p >> str_p("</") >> name >> ch_p('>');
// 	rule_t any_tag_standalone = *space_p >> ch_p('<') >> name >> *attr >> str_p("/>");

// 	string text;
// 	rule_t cdata = *space_p >> str_p("<![CDATA[") >> (*(anychar_p - ']'))[assign_a(text)] >> str_p("]]>");
// 	rule_t text_content = cdata | (*(anychar_p - '<'))[assign_a(text)];

// 	rule_t name_tag = *space_p >> str_p("<name>") >> text[insert_at_a(pt, "name", text)] >> str_p("</name>");
// 	rule_t comm_tag = *space_p >> str_p("<description>") >> text[insert_at_a(pt, "comm", text)] >> str_p("</description>");

// 	rule_t point = *space_p >> real_p[insert_at_a(pt, "lon")]
// 				>> *space_p >> "," >> *space_p >> real_p[insert_at_a(pt, "lat")]
// 				>> !(*space_p >> "," >> *space_p >> real_p);  // ignoring height

	
	
// 	rule_t placemark = *space_p >> str_p("<placemark") >> *attr >> str_p(">");
//     }
	

}
