#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/clear_actor.hpp>
//#include <boost/spirit/utility/lists.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>

#include <vector>
#include <string>
#include <map>

#include "io.h"
#include "geo_enums.h"
#include "geo_names.h"
#include "geo_convs.h"
#include "../utils/mapsoft_options.h"

#include <math.h>

#include "../jeeps/gpsmath.h"
#include "../jeeps/gpsdatum.h"


namespace oe{

using namespace std;
using namespace boost::spirit;

typedef char                    char_t;
typedef file_iterator <char_t>  iterator_t;
typedef scanner<iterator_t>     scanner_t;
typedef rule <scanner_t>        rule_t;

// format-specific data types and conversions to geo_data.h types...

        struct oe_waypoint{

		string name, comm;
		double lat,lon,time;
		int symb, map_displ, displ, color, bgcolor, pt_dir;
		double prox_dist, alt;  // alt in feet
		int font_size, font_style, size;

                oe_waypoint(){
			// all default values -- in geo_data.h
			g_waypoint dflt;
                        lat=dflt.y; lon=dflt.x; time=dflt.t.value;
                        symb=dflt.symb; displ=dflt.displ; map_displ=dflt.map_displ;
			color=dflt.color.RGB().value; bgcolor=dflt.bgcolor.RGB().value; pt_dir=dflt.pt_dir;
			prox_dist=dflt.prox_dist; alt=dflt.z;
			font_size=dflt.font_size; font_style=dflt.font_style; size=dflt.size;
                }
                operator g_waypoint () const{
                        g_waypoint ret;
                        ret.name  = name;
                        ret.comm  = comm;
                        ret.y     = lat;
                        ret.x     = lon;
			if (time==0) ret.t = Time(0);
			else ret.t = Time(int(time* 3600.0 * 24.0 - 2209161600.0));
                        ret.symb  = symb;
                        ret.displ = displ;
                        ret.map_displ  = map_displ;
                        ret.color      = Color(0xFF, color);
                        ret.bgcolor    = Color(0xFF, bgcolor);
                        ret.pt_dir     = pt_dir;
                        ret.size       = size;
                        ret.font_size  = font_size;
                        ret.font_style = font_style;
			if (alt == -777) ret.z=1e24;
			else ret.z      = alt*0.3048;
			ret.prox_dist   = prox_dist;
                        return ret;
                } // здесь еще нет преобразования СК!
        };

        struct oe_trackpoint{
                double lat, lon, time, alt;
                bool start;
                oe_trackpoint(){
			g_trackpoint dflt;
                        lat=dflt.y; lon=dflt.x; start=dflt.start;
                        time=dflt.t.value; alt=-dflt.z;
                }
                operator g_trackpoint () const{
                        g_trackpoint ret;
                        ret.y     = lat;
                        ret.x     = lon;
                        ret.start = start;
			if (time==0) ret.t = Time(0);
			else ret.t     = Time(int(time* 3600.0 * 24.0 - 2209161600.0));
			if (alt == -777) ret.z=1e24;
			else ret.z   = alt*0.3048;
                        return ret;
                } // здесь еще нет преобразования СК!
        };


        struct oe_waypoint_list{
                string symbset;
                string datum;
                vector<oe_waypoint> points;
                operator g_waypoint_list () const{
			Datum D(datum);
			convs::ll2wgs cnv(D);
                        g_waypoint_list ret;
                        ret.symbset = symbset;
                        vector<oe_waypoint>::const_iterator i,
                                b=points.begin(), e=points.end();
                        for (i=b; i!=e; i++){
                                g_waypoint p = *i; // суровое преобразование типа
				cnv.frw(p);
                    		ret.push_back(p);
			}
                        return ret;
                }
        };

        struct oe_track{
                int width, color, skip, type, fill, cfill;
                string comm;
                string datum;
                vector<oe_trackpoint> points;
                oe_track (){
		  g_track dflt;
                  width=dflt.width; color=dflt.color.RGB().value; skip=dflt.skip;
                  type=dflt.type;
                  fill=dflt.fill;
                  cfill=dflt.cfill.RGB().value;
                }
                operator g_track () const{
                        g_track ret;
                        Datum D(datum);
                        convs::ll2wgs cnv(D);
                        ret.width = width;
                        ret.color = Color(0xFF, color);
                        ret.skip  = skip;
                        ret.type  = type;
                        ret.fill  = fill;
                        ret.cfill = cfill;
			ret.comm  = comm;
                        vector<oe_trackpoint>::const_iterator i,
                                b=points.begin(), e=points.end();
                        for (i=b; i!=e; i++){
				g_trackpoint p = *i; // суровое преобразование типа
                                cnv.frw(p);
                    		ret.push_back(p);
			}
                        return ret;
                }
        };

	struct oe_mappoint{
		int x,y;
		char   lat_h, lon_h;  // +/-1 for N/S, E/W
		int    lat_d, lon_d;
		double lat_m, lon_m;
		double grid_x, grid_y;
		oe_mappoint(){
			x=0;      y=0;
			lat_h=1;  lon_h=1;
			lat_d=0;  lon_d=0;
			lat_m=0;  lon_m=0;
			grid_x=0; grid_y=0;
                }
        };

	struct oe_map{
		string comm, file, prefix;
		string datum;
		char   mag_var_h; // not used
		int    mag_var_d;
		double mag_var_m;
		string map_proj;
		double proj_lat0, proj_lon0, proj_k;
		double proj_E0, proj_N0;
		double proj_lat1, proj_lat2, proj_hgt; // I don't know how to use this things
                vector<oe_mappoint> points;
                g_line border;

	        oe_map ()
		    : mag_var_h(0), mag_var_d(0), mag_var_m(0),
		      proj_lat0(0), proj_lon0(0), proj_k(0),
		      proj_E0(0), proj_N0(0),
		      proj_lat1(0), proj_lat2(0), proj_hgt(0)
	        { }
	    
                // map features etc are not supported. 
		operator g_map () const {
			g_map ret;
			ret.comm=comm;
			// если не абсолютный путь - добавим prefix
			if ((file.size()<1)||(file[0]!='/'))
			    ret.file=prefix+file;
			else ret.file=file;
			ret.map_proj=Proj(map_proj);
                        Datum D(datum);
                        convs::ll2wgs cnv(D);

                        double a = GPS_Ellipse[GPS_Datum[D.n].ellipse].a;
                        double f = 1/GPS_Ellipse[GPS_Datum[D.n].ellipse].invf;

                        vector<oe_mappoint>::const_iterator i,
                                b=points.begin(), e=points.end();
                        for (i=b; i!=e; i++){
				g_refpoint p;
                                p.xr = i->x; p.yr = i->y;
                                p.x = i->lon_d + i->lon_m/60.0;  if (i->lon_h<0) p.x=-p.x;
                                p.y = i->lat_d + i->lat_m/60.0;  if (i->lat_h<0) p.y=-p.y;
                                if ((p.y==0) && (p.x==0)){
                            	    if ((i->grid_x==0) && (i->grid_y==0)) continue;
					// преобразования
					// кажется, пока проще будет обойти mapsoft_geo.h
					// ну или надо хранить все параметры в виде текста...
                                        GPS_Math_EN_To_LatLon(i->grid_x, i->grid_y, &p.y, &p.x,
                                        proj_N0, proj_E0, proj_lat0, proj_lon0, proj_k, a, a*(1-f));
                                }
                                cnv.frw(p);
                    		ret.push_back(p);
			}
                        ret.border=border;
			return ret;
                }
        };



// function for reading objects from Ozi file
// into the world object
bool read_file(const char* filename, geo_data & world, const Options & opt){

  // iterators for parsing
  iterator_t first(filename);
  if (!first) { cerr << "can't find file " << filename << '\n'; return 0;}
  iterator_t last = first.make_end();

  oe_waypoint    wpt, wpt0; // wawpoint
  oe_trackpoint  tpt, tpt0; // trackpoint
  oe_mappoint    mpt, mpt0; // map ref point
  g_point        bpt, bpt0; // map border
  oe_waypoint_list w;
  oe_track         t;
  oe_map           m;

  // prefix -- директория, в которой лежит map-файл, на случай, 
  // если в нем относительные названия граф.файлов
  char *sl = rindex(filename, '/');
  if (sl!=NULL){
    int pos = sl-filename;
    std::string fn1(filename);
    m.prefix = std::string(fn1.begin(),fn1.begin()+pos) + "/";
  }
  else m.prefix="";

  // rules for parsing
  rule_t pr = anychar_p - ',' - eol_p;
  rule_t ch = anychar_p - eol_p;
  rule_t scs = *blank_p >> ',' >> *blank_p;

  rule_t wpt_head = 
    str_p("OziExplorer Waypoint File Version ") >> 
    str_p("1.1") >> eol_p >>
    (+ch)[assign_a(w.datum)] >> eol_p >> // Datum
    +ch >> eol_p >>                      // Reserved for future use
    (+ch)[assign_a(w.symbset)] >> eol_p; // Symbol set (unused)

  rule_t trk_head = 
    str_p("OziExplorer Track Point File Version ") >> 
    (str_p("2.0") || "2.1") >> eol_p >>
    (+ch)[assign_a(t.datum)] >> eol_p >>
    +ch >> eol_p >> // Altitude is in feet
    +ch >> eol_p >> // Reserved for future use
      ch_p('0') >> 
      !(scs >> !(int_p[assign_a(t.width)]) >> 
      !(scs >> !(int_p[assign_a(t.color)]) >> 
      !(scs >> !((*pr)[assign_a(t.comm)]) >> 
      !(scs >> !(int_p[assign_a(t.skip)]) >> 
      !(scs >> !(int_p[assign_a(t.type)]) >> 
      !(scs >> !(int_p[assign_a(t.fill)]) >> 
      !(scs >> !(int_p[assign_a(t.cfill)]) >> 
      !(scs >> *ch)))))))) >>
    eol_p >> 
    uint_p >> eol_p;

  rule_t wpt_point = 
    *blank_p >> int_p >> // Number, unused
    !(scs >> !((*pr)[assign_a(wpt.name)]) >> 
    !(scs >> !(real_p[assign_a(wpt.lat)]) >> 
    !(scs >> !(real_p[assign_a(wpt.lon)]) >> 
    !(scs >> !(real_p[assign_a(wpt.time)]) >> 
    !(scs >> !(int_p[assign_a(wpt.symb)]) >> 
    !(scs >> !(int_p) >>  // status, always 1, unused
    !(scs >> !(int_p[assign_a(wpt.map_displ)]) >> 
    !(scs >> !(int_p[assign_a(wpt.color)]) >> 
    !(scs >> !(int_p[assign_a(wpt.bgcolor)]) >> 
    !(scs >> !((*pr)[assign_a(wpt.comm)]) >> 
    !(scs >> !(int_p[assign_a(wpt.pt_dir)]) >> 
    !(scs >> !(int_p[assign_a(wpt.displ)]) >> 
    !(scs >> !(real_p[assign_a(wpt.prox_dist)]) >> 
    !(scs >> !(real_p[assign_a(wpt.alt)]) >> 
    !(scs >> !(int_p[assign_a(wpt.font_size)]) >> 
    !(scs >> !(int_p[assign_a(wpt.font_style)]) >> 
    !(scs >> !(int_p[assign_a(wpt.size)]) >> 
    !(scs >> *ch)))))))))))))))))) >> eol_p
	    [push_back_a(w.points, wpt)][assign_a(wpt, wpt0)];
    
  rule_t trk_point =
    !(*blank_p >> !(real_p[assign_a(tpt.lat)]) >> 
    !(scs >> !(real_p[assign_a(tpt.lon)]) >> 
    !(scs >> !(uint_p[assign_a(tpt.start)]) >> 
    !(scs >> !(real_p[assign_a(tpt.alt)]) >> 
    !(scs >> !(real_p[assign_a(tpt.time)]) >> 
    !(scs >> *ch)))))) >> eol_p
	    [push_back_a(t.points, tpt)][assign_a(tpt, tpt0)];


  rule_t map_point = 
    *blank_p >> "Point" >> uint_p >> 
    scs >> "xy" >> 
    scs >> !(uint_p[assign_a(mpt.x)]) >>
    scs >> !(uint_p[assign_a(mpt.y)]) >>    
    scs >> (str_p("in")|"ex") >> scs >> "deg" >> 
    scs >> !(uint_p[assign_a(mpt.lat_d)]) >>    
    scs >> !(ureal_p[assign_a(mpt.lat_m)]) >>    
    scs >> !(ch_p('S')[assign_a(mpt.lat_h,-1)] || 
             ch_p('N')[assign_a(mpt.lat_h,+1)]) >>    
    scs >> !(uint_p[assign_a(mpt.lon_d)]) >>    
    scs >> !(ureal_p[assign_a(mpt.lon_m)]) >>    
    scs >> !(ch_p('W')[assign_a(mpt.lat_h,-1)] ||
             ch_p('E')[assign_a(mpt.lat_h,+1)]) >>    
    scs >> "grid" >> scs >> *pr >> 
    scs >> !(uint_p[assign_a(mpt.grid_x)]) >>
    scs >> !(uint_p[assign_a(mpt.grid_y)]) >>
    scs >> *ch >> eol_p
            [push_back_a(m.points, mpt)][assign_a(mpt, mpt0)];

  rule_t MM = 
       // v2.2
       (str_p("MMPNUM") >> scs >> uint_p >> eol_p >>
       *("MMPXY" >> scs >> uint_p >> scs >> 
        uint_p[assign_a(bpt.x)] >> scs >> uint_p[assign_a(bpt.y)] >> eol_p
          [push_back_a(m.border,bpt)])) ||
       // v2.0
       (str_p("MM1A") >> /* why is it here? -- scs >> uint_p  >> */
       *(scs >> uint_p[assign_a(bpt.x)] >> scs >> uint_p[assign_a(bpt.y)]
          [push_back_a(m.border,bpt)] ) >> eol_p); 
	  

  rule_t map_rule = 
    str_p("OziExplorer Map Data File Version ") >>
    (str_p("2.0") || "2.1" || "2.2") >> eol_p >>
    (*ch)[assign_a(m.comm)] >> eol_p >>
    (*ch)[assign_a(m.file)] >> eol_p >>
    *ch >> eol_p >> // 1 TIFF scale factor -- ignored
    (*pr)[assign_a(m.datum)] >> !(scs >> *ch) >> eol_p >> 
    *ch >> eol_p >> // Reserved 1
    *ch >> eol_p >> // Reserved 2
    "Magnetic Variation" >> 
      scs >> !(uint_p[assign_a(m.mag_var_d)]) >>
      scs >> !(ureal_p[assign_a(m.mag_var_m)]) >>
      scs >> !(ch_p('W')[assign_a(m.mag_var_h,-1)] ||
               ch_p('E')[assign_a(m.mag_var_h,+1)]) >> eol_p >> 
    "Map Projection" >> 
    scs >> (*pr)[assign_a(m.map_proj)] >> !(scs >> *ch) >> eol_p >>
    +(map_point) >>
    "Projection Setup" >> 
      !(scs >> !(real_p[assign_a(m.proj_lat0)]) >>
      !(scs >> !(real_p[assign_a(m.proj_lon0)]) >>
      !(scs >> !(ureal_p[assign_a(m.proj_k)]) >>
      !(scs >> !(real_p[assign_a(m.proj_E0)]) >>
      !(scs >> !(real_p[assign_a(m.proj_N0)]) >>
      !(scs >> !(real_p[assign_a(m.proj_lat1)]) >>
      !(scs >> !(real_p[assign_a(m.proj_lat2)]) >>
      !(scs >> !(real_p[assign_a(m.proj_hgt)]) >>
      !(scs >> *ch))))))))) >> eol_p >>
    "Map Feature" >> *ch >> eol_p >>
    *( (str_p("MF") >> *ch >> eol_p >>   // MF (3 lines, unsupported)
       *ch >> eol_p >> *ch >> eol_p) ||
       (str_p("MC") >> *ch >> eol_p >>   // MC (2 lines, unsupported)
       *ch >> eol_p ) ) >> 
    "Track File" >> *ch >> eol_p >>
    *( str_p("TF") >> *ch >> eol_p ) >>  // TF (1 line, unsupported)
    "Moving Map" >> *ch >> eol_p >>
    "MM0" >> scs >> *ch >> eol_p >> // Yes or No, does not matter
    MM >> 
    *anychar_p;
    

  parse_info<iterator_t> info =
    parse(first, last,
           (wpt_head >> *(wpt_point))[push_back_a(world.wpts, w)] ||
           (trk_head >> *(trk_point))[push_back_a(world.trks, t)] || 
           map_rule[push_back_a(world.maps, m)]);
	   
  return info.full;
}



/***********************************************************/

	bool write_plt_file (ostream & f, const g_track & trk, const Options & opt){
		if (!f.good()) return false;
		int num = trk.size();
		f << "OziExplorer Track Point File Version 2.0\r\n"
		  << "WGS 84\r\n"
		  << "Altitude is in Feet\r\n"
		  << "Reserved 3\r\n" 
		  << "0,"
		  << trk.width << ',' 
		  << trk.color.RGB().value << ',' 
		  << trk.comm  << ',' 
		  << trk.skip  << ',' 
		  << trk.type  << ',' 
		  << trk.fill  << ',' 
		  << trk.cfill.RGB().value << "\r\n" 
		  << num << "\r\n";
		for (vector<g_trackpoint>::const_iterator p = trk.begin(); 
			 p!= trk.end(); p++){
			f << right << fixed << setprecision(6) << setfill(' ')
			  << setw(10)<< p->y << ','
			  << setw(11)<< p->x << ','
			  << ((p->start)? '1':'0') << ','
			  << setprecision(1) << setw(6) 
			  << (p->z >= 1e20 ? -777: p->z/0.3048) << ','
			  << setprecision(7) << setw(13)
			  << (p->t.value+2209161600.0)/3600.0/24.0 << ','
			  << setfill('0') << p->t << "\r\n";
		}
		return f.good();
	}

	bool write_wpt_file (ostream & f, const g_waypoint_list & wpt, const Options & opt){
		if (!f.good ()) return false;
		int num = wpt.size();
		int n=0;
		f << "OziExplorer Waypoint File Version 1.1\r\n"
		  << "WGS 84\r\n"
		  << "Reserved 2\r\n"
		  << wpt.symbset << "\r\n";
  
		for (vector<g_waypoint>::const_iterator p = wpt.begin(); 
			 p!= wpt.end(); p++){

			f << right << setw(4) << (++n) << ','
			  << left  << setw(6) << setfill(' ') << p->name << ','
			  << right << fixed << setprecision(6)
			  << setw(10) << p->y << ','
			  << setw(11) << p->x << ','
			  << setprecision(7) << setw(13)
			  << (p->t.value+2209161600.0)/3600.0/24.0 << ','
			  << p->symb       << ",1,"
			  << p->map_displ  << ','
			  << p->color.RGB().value   << ','
			  << p->bgcolor.RGB().value << ','
			  << p->comm       << ','
			  << p->pt_dir     << ','
			  << p->displ      << ','
			  << p->prox_dist  << ','
			  << (p->z >= 1e20 ? -777: p->z/0.3048) << ','
			  << p->font_size  << ','
			  << p->font_style << ','
			  << p->size       << "\r\n";
		}
		return f.good();
	}

	bool write_map_file (ostream & f, const g_map & m, const Options & opt){

		if (!f.good()) return false;

		// нам понадобятся координаты границы в wgs84
		// и m_per_pix
		// TODO -- заполнить их честно!

		g_line border_wgs;
		double m_per_pix=0;

		f << "OziExplorer Map Data File Version 2.2\r\n"
		  << m.comm << "\r\n"
		  << m.file << "\r\n"
		  << "1 ,Map Code,\r\n"
		  << "WGS 84,,   0.0000,   0.0000,WGS 84\r\n"
		  << "Reserved 1\r\n"
		  << "Reserved 2\r\n" 
		  << "Magnetic Variation,,,E\r\n"
		  << "Map Projection," 
		  << m.map_proj.oe_str()
		  << ",PolyCal,No,AutoCalOnly,No,BSBUseWPX,No\r\n";

		for (int n=1; n<=30; n++){
			f << "Point" << setw(2) << setfill('0') << n << ",xy,";

			if (n>m.size()){ 
				f <<"     ,     ,in, deg,    ,        ,N,    ,        ,W"
				  <<", grid,   ,           ,           ,N\r\n";
				continue;
			}

			int x = (int)m[n-1].xr;
			int y = (int)m[n-1].yr;
			double lat = m[n-1].y;
			double lon = m[n-1].x;

			f << setw(5) << setfill(' ') << x << ',' 
			  << setw(5) << setfill(' ') << y << ','
			  << "in, deg,"
			  << fixed << setw(4) << abs(int(lat)) << ',' 
			  << fixed << setw(8) << fabs(lat*60) - abs(int(lat))*60 << ',' 
			  << (lat<0? 'S':'N') << ','
			  << fixed << setw(4) << abs(int(lon)) 
			  << ',' << fixed << setw(8) << fabs(lon*60) - abs(int(lon))*60 << ',' 
			  << (lon<0? 'W':'E') << ','
			  << " grid,   ,           ,           ,N\r\n";
		}
		f << "Projection Setup,,,,,,,,,,\r\n"
		  << "Map Feature = MF ; Map Comment = MC     These follow if they exist\r\n"
		  << "Track File = TF      These follow if they exist\r\n"
		  << "Moving Map Parameters = MM?    These follow if they exist\r\n";


		if (m.border.size()>0){
			f << "MM0,Yes\r\n"
			  << "MMPNUM," << m.border.size() << "\r\n";
			int n=0;
			for (g_line::const_iterator it =m.border.begin();
				 it!=m.border.end(); it++){
				n++;
				f << "MMPXY," << n << "," 
                                  << right << fixed << setprecision(0) << setfill(' ')
                                  << it->x << "," << it->y << "\r\n"; 
			}
			n=0;
			f.precision(8);
			for (g_line::const_iterator it =border_wgs.begin();
				 it!=border_wgs.end(); it++){
				n++;
				f << "MMPLL," << n << "," 
				  << right << fixed << setprecision(6) << setfill(' ')
                                  << setw(10) << it->y << ','
                                  << setw(11) << it->x << "\r\n"; 
			}
			f << "MM1B," << m_per_pix << "\r\n";
		}
		return f.good();
	}
}

