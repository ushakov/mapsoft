#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

#include "geo_enums.h"
#include <stdarg.h>
#include <sstream>
#include <iostream>

using namespace std;
using namespace boost::spirit;

int geo_enum::str2int(const string & str) const{
        geo_enum::const_iterator it, b=begin(), e=end();
        // если нам известно про такую строку - возвращаем соответствующее число
        for (it=b;it!=e;it++) if (it->str == str) return it->i;
        // если в строчке записано число - его и возвращаем
        int ret;
        if (parse(str.c_str(), uint_p[assign_a(ret)]).full) return ret;
        // иначе - ругаемся и возвращаем значение по умолчанию
        if (str.length()!=0) cerr << "Unknown word: " << str << "\n";
        return def;
}
string geo_enum::int2str(const int i) const{
        geo_enum::const_iterator it, b=begin(), e=end();
        for (it=b;it!=e;it++) if (it->i == i) return it->str;
	stringstream s; s << i;
        return s.str();
}
geo_enum::geo_enum(int _def, int n, ...){
	def=_def;
        va_list ap;
        va_start(ap,n);
        for (int i=0;i<n;i++) {
	    char * str = va_arg(ap, char *);
	    int    i   = va_arg(ap, int);
	    geo_enum_element el; el.str=str; el.i=i;
            push_back(el);
        }
        va_end(ap);
}

const geo_enum wpt_map_displ_enum(4, 9,
"numb_only", 0,
"name_only", 1,
"numb_name", 2,
"name_dot",  3,
"name_symb", 4,
"symb_only", 5,
"comm_symb", 6,
"MOB",       7,
"marker",    8);

const geo_enum wpt_pt_dir_enum(0, 4,
"buttom", 0,
"top",    1,
"left",   2,
"right",  3);

const geo_enum trk_type_enum(0, 3,
"normal", 0,
"closed", 1,
"alarm",  2);

const geo_enum trk_fill_enum(0, 8,
"solid",  0,
"clear",  1,
"bdiag",  2,
"fdiag",  3,
"cross",  4,
"dcross", 5,
"horiz",  6,
"vert",   7);

// from http://www.wasab.dk/morten/2003/10/garmin_waypoint_symbols.py
const geo_enum wpt_symb_enum(18, 150,
"anchor",	 0, // white anchor symbol
"bell", 	 1, // white bell symbol
"diamond_grn", 	 2, // green diamond symbol
"diamond_red", 	 3, // red diamond symbol
"dive1", 	 4, // diver down flag 1
"dive2", 	 5, // diver down flag 2
"dollar", 	 6, // white dollar symbol
"fish", 	 7, // white fish symbol
"fuel", 	 8, // white fuel symbol
"horn", 	 9, // white horn symbol
"house", 	10, // white house symbol
"knife", 	11, // white knife & fork symbol
"light", 	12, // white light symbol
"mug", 		13, // white mug symbol
"skull", 	14, // white skull and crossbones symbol
"square_grn", 	15, // green square symbol
"square_red", 	16, // red square symbol
"wbuoy", 	17, // white buoy waypoint symbol
"wpt_dot", 	18, // waypoint dot
"wreck",	19, // white wreck symbol
"null", 	20, // null symbol (transparent)
"mob", 		21, // man overboard symbol
// Marine navigational aid group
"buoy_ambr", 	22, // amber map buoy symbol
"buoy_blck", 	23, // black map buoy symbol
"buoy_blue", 	24, // blue map buoy symbol
"buoy_grn", 	25, // green map buoy symbol
"buoy_grn_red", 26, // green/red map buoy symbol
"buoy_grn_wht", 27, // green/white map buoy symbol
"buoy_orng", 	28, // orange map buoy symbol
"buoy_red", 	29, // red map buoy symbol
"buoy_red_grn", 30, // red/green map buoy symbol
"buoy_red_wht", 31, // red/white map buoy symbol
"buoy_violet", 	32, // violet map buoy symbol
"buoy_wht", 	33, // white map buoy symbol
"buoy_wht_grn", 34, // white/green map buoy symbol
"buoy_wht_red", 35, // white/red map buoy symbol
"dot", 		36, // white dot symbol
"rbcn", 	37, // radio beacon symbol
// Misc. group
"boat_ramp", 	150, // boat ramp symbol
"camp", 	151, // campground symbol
"restrooms", 	152, // restrooms symbol
"showers", 	153, // shower symbol
"drinking_wtr", 154, // drinking water symbol
"phone", 	155, // telephone symbol
"1st_aid", 	156, // first aid symbol
"info", 	157, // information symbol
"parking",	158, // parking symbol
"park", 	159, // park symbol
"picnic", 	160, // picnic symbol
"scenic", 	161, // scenic area symbol
"skiing", 	162, // skiing symbol
"swimming", 	163, // swimming symbol
"dam", 		164, // dam symbol
"controlled", 	165, // controlled area symbol
"danger", 	166, // danger symbol
"restricted", 	167, // restricted area symbol
"null_2", 	168, // null symbol
"ball", 	169, // ball symbol
"car", 		170, // car symbol
"deer", 	171, // deer symbol
"shpng_cart", 	172, // shopping cart symbol
"lodging", 	173, // lodging symbol
"mine", 	174, // mine symbol
"trail_head", 	175, // trail head symbol
"truck_stop", 	176, // truck stop symbol
"user_exit", 	177, // user exit symbol
"flag", 	178, // flag symbol
"circle_x", 	179, // circle with x in the center
// Land group
"is_hwy", 	8192, // interstate hwy symbol
"us_hwy", 	8193, // us hwy symbol
"st_hwy", 	8194, // state hwy symbol
"mi_mrkr", 	8195, // mile marker symbol
"trcbck", 	8196, // TracBack (feet) symbol
"golf", 	8197, // golf symbol
"sml_cty", 	8198, // small city symbol
"med_cty", 	8199, // medium city symbol
"lrg_cty", 	8200, // large city symbol
"freeway", 	8201, // intl freeway hwy symbol
"ntl_hwy", 	8202, // intl national hwy symbol
"cap_cty", 	8203, // capitol city symbol (star)
"amuse_pk", 	8204, // amusement park symbol
"bowling", 	8205, // bowling symbol
"car_rental", 	8206, // car rental symbol
"car_repair", 	8207, // car repair symbol
"fastfood", 	8208, // fast food symbol
"fitness", 	8209, // fitness symbol
"movie", 	8210, // movie symbol
"museum", 	8211, // museum symbol
"pharmacy", 	8212, // pharmacy symbol
"pizza", 	8213, // pizza symbol
"post_ofc", 	8214, // post office symbol
"rv_park", 	8215, // RV park symbol
"school", 	8216, // school symbol
"stadium", 	8217, // stadium symbol
"store", 	8218, // dept. store symbol
"zoo", 		8219, // zoo symbol
"gas_plus", 	8220, // convenience store symbol
"faces", 	8221, // live theater symbol
"ramp_int", 	8222, // ramp intersection symbol
"st_int", 	8223, // street intersection symbol
"weigh_sttn", 	8226, // inspection/weigh station symbol
"toll_booth", 	8227, // toll booth symbol
"elev_pt", 	8228, // elevation point symbol
"ex_no_srvc", 	8229, // exit without services symbol
"geo_place_mm", 8230, // Geographic place name, man-made
"geo_place_wtr",8231, // Geographic place name, water
"geo_place_lnd",8232, // Geographic place name, land
"bridge", 	8233, // bridge symbol
"building", 	8234, // building symbol
"cemetery", 	8235, // cemetery symbol
"church", 	8236, // church symbol
"civil", 	8237, // civil location symbol
"crossing", 	8238, // crossing symbol
"hist_town", 	8239, // historical town symbol
"levee", 	8240, // levee symbol
"military", 	8241, // military location symbol
"oil_field", 	8242, // oil field symbol
"tunnel", 	8243, // tunnel symbol
"beach", 	8244, // beach symbol
"forest", 	8245, // forest symbol
"summit", 	8246, // summit symbol
"lrg_ramp_int", 8247, // large ramp intersection symbol
"lrg_ex_no_srvc",8248, // large exit without services smbl
"badge", 	8249, // police/official badge symbol
"cards", 	8250, // gambling/casino symbol
"snowski", 	8251, // snow skiing symbol
"iceskate", 	8252, // ice skating symbol
"wrecker", 	8253, // tow truck (wrecker) symbol
"border", 	8254, // border crossing (port of entry)
"8255", 	8255, // closed box (Geko 201 only?)
"8256", 	8256, // open box (Geko 201 only?)
// Aviation group
"airport", 	16384, // airport symbol
"int", 		16385, // intersection symbol
"ndb", 		16386, // non-directional beacon symbol
"vor", 		16387, // VHF omni-range symbol
"heliport", 	16388, // heliport symbol
"private", 	16389, // private field symbol
"soft_fld", 	16390, // soft field symbol
"tall_tower", 	16391, // tall tower symbol
"short_tower", 	16392, // short tower symbol
"glider", 	16393, // glider symbol
"ultralight", 	16394, // ultralight symbol
"parachute", 	16395, // parachute symbol
"vortac", 	16396, // VOR/TACAN symbol
"vordme", 	16397, // VOR-DME symbol
"faf", 		16398, // first approach fix
"lom", 		16399, // localizer outer marker
"map", 		16400, // missed approach point
"tacan", 	16401, // TACAN symbol
"seaplane", 	16402 // Seaplane Base
);
