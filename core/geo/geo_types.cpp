// типы данных, используемые в геоданных

#include "geo_types.h"
#include <sstream>
#include <cstdlib>
#include <cerrno>

const int Enum::fmt_mask = 0xFFFF;
const int Enum::xml_fmt  = 1<<16;
const int Enum::oe_fmt   = 1<<17;
const int Enum::proj_fmt = 1<<18;

int Enum::output_fmt = Enum::xml_fmt;


void Enum::set_from_string(const std::string & str){

  geo_types_table & data = get_table();
  geo_types_table::const_iterator i = data.find(str);

  if (i == data.end()){ // no entry in the table

    char *p;
    int n = strtol(str.c_str(), &p, 0); // number?
    if (*p==0){ // strtol reachs end of string
      val=n;
      return;
    }

    i = data.find(""); // default value
    if (i == data.end()){
      std::cerr << "Fatal error: no default value\n";
      exit(0);
    }
    std::cerr << "Unknown value: " << str << ". Using default: " << i->second << "\n";
  }
  val=i->second & Enum::fmt_mask;
}


std::ostream & operator<< (std::ostream & s, const Enum & e){
  geo_types_table & data = e.get_table();

  for (geo_types_table::const_iterator
         i=data.begin(); i!=data.end(); i++){
    if (((i->second & Enum::fmt_mask)   == e.val) &&
        ((i->second & Enum::output_fmt) != 0)) return s << i->first;
  }
  return s << e.val; // print number
}

std::istream & operator>> (std::istream & s, Enum & e){
  std::string str;
  getline(s, str);
  e.set_from_string(str);
  return s;
}

/***********************************************************/

geo_types_table Datum::names;

Datum::Datum(const int v){
  if (names.size()==0) create_table();
  val = v;
}

Datum::Datum(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void Datum::create_table(){
  // int values are from ../jeeps/gpsdatum.c
  names[""]       = 118;
  names["wgs84"]  = 118 | Enum::xml_fmt;
  names["wgs 84"] = 118;
  names["WGS84"]  = 118 | Enum::proj_fmt;
  names["WGS 84"] = 118 | Enum::oe_fmt;

  names["Pulkovo 1942"]     = 92 | Enum::oe_fmt;
  names["Pulkovo 1942 (1)"] = 92;
  names["Pulkovo 1942 (2)"] = 92;
  names["pulk"]             = 92;
  names["pulkovo"]          = 92 | Enum::xml_fmt;
  names["pulk42"]           = 92;
  names["krass"]            = 92 | Enum::proj_fmt;
  names["sphere"]        = 10001 | Enum::proj_fmt | Enum::xml_fmt;
  names["google_sphere"] = 10002 | Enum::xml_fmt;
  names["KKJ"]           = 10003;
}

/***********************************************************/

geo_types_table Proj::names;

Proj::Proj(const int v){
  if (names.size()==0) create_table();
  val=v;
}

Proj::Proj(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void Proj::create_table(){
  names[""]                    = 0;
  names["lonlat"]              = 0 | Enum::xml_fmt;
  names["latlong"]             = 0 | Enum::proj_fmt;
  names["Latitude/Longitude"]  = 0 | Enum::oe_fmt;
  names["tmerc"]               = 1 | Enum::xml_fmt | Enum::proj_fmt;
  names["Transverse Mercator"] = 1 | Enum::oe_fmt;
  names["utm"]                 = 2 | Enum::xml_fmt;
  names["UTM"]                 = 2 | Enum::oe_fmt;  // проверить, что в OE оно так называется!
  names["merc"]                = 3 | Enum::xml_fmt | Enum::proj_fmt;
  names["Mercator"]            = 3 | Enum::oe_fmt;  // проверить, что в OE оно так называется!
  names["google"]              = 4 | Enum::xml_fmt; // в Ozi что-то такое должно быть...
  names["ks"]                  = 5 | Enum::xml_fmt;
  names["lcc"]                     = 6 | Enum::xml_fmt | Enum::proj_fmt;
  names["Lambert Conformal Conic"] = 6 | Enum::oe_fmt;
  names["ch1904"]                  = 7 | Enum::xml_fmt;
}

/***********************************************************/

geo_types_table wptMapDispl::names;

wptMapDispl::wptMapDispl(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void wptMapDispl::create_table(){
  int m = Enum::xml_fmt;
  names[""] = 4;
  names["numb_only"] = 0 | m;
  names["name_only"] = 1 | m;
  names["numb_name"] = 2 | m;
  names["name_dot"]  = 3 | m;
  names["name_symb"] = 4 | m;
  names["symb_only"] = 5 | m;
  names["comm_symb"] = 6 | m;
  names["MOB"]       = 7 | m;
  names["marker"]    = 8 | m;
}

/***********************************************************/

geo_types_table wptPtDir::names;

wptPtDir::wptPtDir(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void wptPtDir::create_table(){
  int m = Enum::xml_fmt;
  names[""] = 1;
  names["buttom"] = 0 | m;
  names["top"]    = 1 | m;
  names["left"]   = 2 | m;
  names["right"]  = 3 | m;
}

/***********************************************************/

geo_types_table trkType::names;

trkType::trkType(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void trkType::create_table(){
  int m = Enum::xml_fmt;
  names[""] = 0;
  names["normal"] = 0 | m;
  names["closed"] = 1 | m;
  names["alarm"]  = 2 | m;
}

/***********************************************************/

geo_types_table trkFill::names;

trkFill::trkFill(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void trkFill::create_table(){
  int m = Enum::xml_fmt;
  names[""] = 1;
  names["solid"]  = 0 | m;
  names["clear"]  = 1 | m;
  names["bdiag"]  = 2 | m;
  names["fdiag"]  = 3 | m;
  names["cross"]  = 4 | m;
  names["dcross"] = 5 | m;
  names["horiz"]  = 6 | m;
  names["vert"]   = 7 | m;
}

/***********************************************************/

geo_types_table wptSymb::names;

wptSymb::wptSymb(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void wptSymb::create_table(){
  int m = Enum::xml_fmt;
// from http://www.wasab.dk/morten/2003/10/garmin_waypoint_symbols.py
  names[""] = 18 | m;
  names["anchor"]        = 0 | m; // white anchor symbol
  names["bell"]          = 1 | m; // white bell symbol
  names["diamond_grn"]   = 2 | m; // green diamond symbol
  names["diamond_red"]   = 3 | m; // red diamond symbol
  names["dive1"]         = 4 | m; // diver down flag 1
  names["dive2"]         = 5 | m; // diver down flag 2
  names["dollar"]        = 6 | m; // white dollar symbol
  names["fish"]          = 7 | m; // white fish symbol
  names["fuel"]          = 8 | m; // white fuel symbol
  names["horn"]          = 9 | m; // white horn symbol
  names["house"]        = 10 | m; // white house symbol
  names["knife"]        = 11 | m; // white knife & fork symbol
  names["light"]        = 12 | m; // white light symbol
  names["mug"]          = 13 | m; // white mug symbol
  names["skull"]        = 14 | m; // white skull and crossbones symbol
  names["square_grn"]   = 15 | m; // green square symbol
  names["square_red"]   = 16 | m; // red square symbol
  names["wbuoy"]        = 17 | m; // white buoy waypoint symbol
  names["wpt_dot"]      = 18 | m; // waypoint dot
  names["wreck"]        = 19 | m; // white wreck symbol
  names["null"]         = 20 | m; // null symbol (transparent)
  names["mob"]          = 21 | m; // man overboard symbol
// Marine navigational aid group
  names["buoy_ambr"]    = 22 | m; // amber map buoy symbol
  names["buoy_blck"]    = 23 | m; // black map buoy symbol
  names["buoy_blue"]    = 24 | m; // blue map buoy symbol
  names["buoy_grn"]     = 25 | m; // green map buoy symbol
  names["buoy_grn_red"] = 26 | m; // green/red map buoy symbol
  names["buoy_grn_wht"] = 27 | m; // green/white map buoy symbol
  names["buoy_orng"]    = 28 | m; // orange map buoy symbol
  names["buoy_red"]     = 29 | m; // red map buoy symbol
  names["buoy_red_grn"] = 30 | m; // red/green map buoy symbol
  names["buoy_red_wht"] = 31 | m; // red/white map buoy symbol
  names["buoy_violet"]  = 32 | m; // violet map buoy symbol
  names["buoy_wht"]     = 33 | m; // white map buoy symbol
  names["buoy_wht_grn"] = 34 | m; // white/green map buoy symbol
  names["buoy_wht_red"] = 35 | m; // white/red map buoy symbol
  names["dot"]          = 36 | m; // white dot symbol
  names["rbcn"]         = 37 | m; // radio beacon symbol
// Misc. group
  names["boat_ramp"]    = 150 | m; // boat ramp symbol
  names["camp"]         = 151 | m; // campground symbol
  names["restrooms"]    = 152 | m; // restrooms symbol
  names["showers"]      = 153 | m; // shower symbol
  names["drinking_wtr"] = 154 | m; // drinking water symbol
  names["phone"]        = 155 | m; // telephone symbol
  names["1st_aid"]      = 156 | m; // first aid symbol
  names["info"]         = 157 | m; // information symbol
  names["parking"]      = 158 | m; // parking symbol
  names["park"]         = 159 | m; // park symbol
  names["picnic"]       = 160 | m; // picnic symbol
  names["scenic"]       = 161 | m; // scenic area symbol
  names["skiing"]       = 162 | m; // skiing symbol
  names["swimming"]     = 163 | m; // swimming symbol
  names["dam"]          = 164 | m; // dam symbol
  names["controlled"]   = 165 | m; // controlled area symbol
  names["danger"]       = 166 | m; // danger symbol
  names["restricted"]   = 167 | m; // restricted area symbol
  names["null_2"]       = 168 | m; // null symbol
  names["ball"]         = 169 | m; // ball symbol
  names["car"]          = 170 | m; // car symbol
  names["deer"]         = 171 | m; // deer symbol
  names["shpng_cart"]   = 172 | m; // shopping cart symbol
  names["lodging"]      = 173 | m; // lodging symbol
  names["mine"]         = 174 | m; // mine symbol
  names["trail_head"]   = 175 | m; // trail head symbol
  names["truck_stop"]   = 176 | m; // truck stop symbol
  names["user_exit"]    = 177 | m; // user exit symbol
  names["flag"]         = 178 | m; // flag symbol
  names["circle_x"]     = 179 | m; // circle with x in the center
// Land group
  names["is_hwy"]       = 8192 | m; // interstate hwy symbol
  names["us_hwy"]       = 8193 | m; // us hwy symbol
  names["st_hwy"]       = 8194 | m; // state hwy symbol
  names["mi_mrkr"]      = 8195 | m; // mile marker symbol
  names["trcbck"]       = 8196 | m; // TracBack (feet) symbol
  names["golf"]         = 8197 | m; // golf symbol
  names["sml_cty"]      = 8198 | m; // small city symbol
  names["med_cty"]      = 8199 | m; // medium city symbol
  names["lrg_cty"]      = 8200 | m; // large city symbol
  names["freeway"]      = 8201 | m; // intl freeway hwy symbol
  names["ntl_hwy"]      = 8202 | m; // intl national hwy symbol
  names["cap_cty"]      = 8203 | m; // capitol city symbol (star)
  names["amuse_pk"]     = 8204 | m; // amusement park symbol
  names["bowling"]      = 8205 | m; // bowling symbol
  names["car_rental"]   = 8206 | m; // car rental symbol
  names["car_repair"]   = 8207 | m; // car repair symbol
  names["fastfood"]     = 8208 | m; // fast food symbol
  names["fitness"]      = 8209 | m; // fitness symbol
  names["movie"]        = 8210 | m; // movie symbol
  names["museum"]       = 8211 | m; // museum symbol
  names["pharmacy"]     = 8212 | m; // pharmacy symbol
  names["pizza"]        = 8213 | m; // pizza symbol
  names["post_ofc"]     = 8214 | m; // post office symbol
  names["rv_park"]      = 8215 | m; // RV park symbol
  names["school"]       = 8216 | m; // school symbol
  names["stadium"]      = 8217 | m; // stadium symbol
  names["store"]        = 8218 | m; // dept. store symbol
  names["zoo"]          = 8219 | m; // zoo symbol
  names["gas_plus"]     = 8220 | m; // convenience store symbol
  names["faces"]        = 8221 | m; // live theater symbol
  names["ramp_int"]     = 8222 | m; // ramp intersection symbol
  names["st_int"]       = 8223 | m; // street intersection symbol
  names["weigh_sttn"]   = 8226 | m; // inspection/weigh station symbol
  names["toll_booth"]   = 8227 | m; // toll booth symbol
  names["elev_pt"]      = 8228 | m; // elevation point symbol
  names["ex_no_srvc"]   = 8229 | m; // exit without services symbol
  names["geo_place_mm"] = 8230 | m; // Geographic place name, man-made
  names["geo_place_wtr"]= 8231 | m; // Geographic place name, water
  names["geo_place_lnd"]= 8232 | m; // Geographic place name, land
  names["bridge"]       = 8233 | m; // bridge symbol
  names["building"]     = 8234 | m; // building symbol
  names["cemetery"]     = 8235 | m; // cemetery symbol
  names["church"]       = 8236 | m; // church symbol
  names["civil"]        = 8237 | m; // civil location symbol
  names["crossing"]     = 8238 | m; // crossing symbol
  names["hist_town"]    = 8239 | m; // historical town symbol
  names["levee"]        = 8240 | m; // levee symbol
  names["military"]     = 8241 | m; // military location symbol
  names["oil_field"]    = 8242 | m; // oil field symbol
  names["tunnel"]       = 8243 | m; // tunnel symbol
  names["beach"]        = 8244 | m; // beach symbol
  names["forest"]       = 8245 | m; // forest symbol
  names["summit"]       = 8246 | m; // summit symbol
  names["lrg_ramp_int"] = 8247 | m; // large ramp intersection symbol
  names["lrg_ex_no_srvc"]= 8248 | m; // large exit without services smbl
  names["badge"]        = 8249 | m; // police/official badge symbol
  names["cards"]        = 8250 | m; // gambling/casino symbol
  names["snowski"]      = 8251 | m; // snow skiing symbol
  names["iceskate"]     = 8252 | m; // ice skating symbol
  names["wrecker"]      = 8253 | m; // tow truck (wrecker) symbol
  names["border"]       = 8254 | m; // border crossing (port of entry)
  names["8255"]         = 8255 | m; // closed box (Geko 201 only?)
  names["8256"]         = 8256 | m; // open box (Geko 201 only?)
// Aviation group
  names["airport"]      = 16384 | m; // airport symbol
  names["int"]          = 16385 | m; // intersection symbol
  names["ndb"]          = 16386 | m; // non-directional beacon symbol
  names["vor"]          = 16387 | m; // VHF omni-range symbol
  names["heliport"]     = 16388 | m; // heliport symbol
  names["private"]      = 16389 | m; // private field symbol
  names["soft_fld"]     = 16390 | m; // soft field symbol
  names["tall_tower"]   = 16391 | m; // tall tower symbol
  names["short_tower"]  = 16392 | m; // short tower symbol
  names["glider"]       = 16393 | m; // glider symbol
  names["ultralight"]   = 16394 | m; // ultralight symbol
  names["parachute"]    = 16395 | m; // parachute symbol
  names["vortac"]       = 16396 | m; // VOR/TACAN symbol
  names["vordme"]       = 16397 | m; // VOR-DME symbol
  names["faf"]          = 16398 | m; // first approach fix
  names["lom"]          = 16399 | m; // localizer outer marker
  names["map"]          = 16400 | m; // missed approach point
  names["tacan"]        = 16401 | m; // TACAN symbol
  names["seaplane"]     = 16402 | m; // Seaplane Base
}
