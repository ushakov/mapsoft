#include <iostream>

#include <vector>
#include <string>
#include <cstring>

#include "io_gps.h"
#include "jeeps/gps.h"
#include "utils/iconv_utils.h"
#include "utils/err.h"

namespace gps {
	using namespace std;

// function for reading objects from gps into the world object

        void init_gps(const char* port, const Options &opt){
		// почему-то только такая процедура позволяет подключить usb-gps с первого раза
                if (opt.exists("verbose"))
                  cerr << "initialize GPS device " << port << endl;
                GPS_Init(port);
		sleep(1);
		if (GPS_Init(port) < 0)
                  throw MapsoftErr("GEO_IO_GPS_INIT")
                    << "Can't open GPS device";
        }

	void get_waypoints (const char* port, geo_data & world, const Options &opt){
		GPS_PWay   *wpt;

                if (opt.exists("verbose"))
                  cerr << "get waypoints from GPS device " << port << endl;

		int n = GPS_Command_Get_Waypoint (port, &wpt, NULL);
                if (n<0)
                  throw MapsoftErr("GEO_IO_GPS_WPT_GET")
                     << "Can't get waypoints from GPS device"; 

		g_waypoint_list new_w;

		for (int i=0; i<n; i++){
			g_waypoint new_w_pt;

			new_w_pt.name      = wpt[i]->ident;
			new_w_pt.comm      = wpt[i]->cmnt;
			new_w_pt.y         = wpt[i]->lat;
			new_w_pt.x         = wpt[i]->lon;
			new_w_pt.z         = wpt[i]->alt;
			new_w_pt.symb.val  = wpt[i]->smbl;
			new_w_pt.displ     = wpt[i]->dspl;
			new_w_pt.color     = Color(0xFF, wpt[i]->colour);
			if (wpt[i]->time_populated) new_w_pt.t
                                           = Time(wpt[i]->time);
			new_w.push_back(new_w_pt);
		}
		world.wpts.push_back(new_w);
	}

	void get_tracks (const char* port, geo_data & world, const Options &opt)
	{
		GPS_PTrack *trk;

                if (opt.exists("verbose"))
                  cerr << "get tracks from GPS device " << port << endl;

                int n = GPS_Command_Get_Track (port, &trk, 0);
                if (n<0)
                  throw MapsoftErr("GEO_IO_GPS_TRK_GET")
                    << "Can't get tracks from GPS device"; 

		g_track new_t;
		bool begin=true;

		for (int i=0; i<n; i++){
			if (trk[i]->ishdr){
				if (begin){
					begin = false;
				} else {
					world.trks.push_back(new_t);
					new_t = g_track();
				}
				new_t.displ = trk[i]->dspl;
				new_t.color = Color(0xFF, trk[i]->colour);
				new_t.comm  = trk[i]->trk_ident;
			} else {
				g_trackpoint new_t_pt;

				if (trk[i]->tnew) new_t_pt.start=true;
				new_t_pt.y     = trk[i]->lat;
				new_t_pt.x     = trk[i]->lon;
				new_t_pt.z     = trk[i]->alt;
				new_t_pt.depth = trk[i]->dpth;
				new_t_pt.t     = Time(trk[i]->Time);
				new_t.push_back(new_t_pt);
			}
		}
		world.trks.push_back(new_t);
	}

	void put_waypoints (const char * port, const g_waypoint_list & wp, const Options & opt){
		int num = wp.size();

                if (opt.exists("verbose"))
                  cerr << "put waypoints to GPS device " << port << endl;

		GPS_PWay *wpts = (GPS_PWay *) calloc(num, sizeof(GPS_PWay));

		IConv cnv("KOI8-R");

		int n=0;
		for (vector<g_waypoint>::const_iterator i =  wp.begin();
			 i != wp.end();
			 i++)
		{
			wpts[n] = GPS_Way_New();
			memccpy(wpts[n]->ident, cnv.from_utf8_7bit(i->name).c_str(), '\0', 255);
			memccpy(wpts[n]->cmnt,  cnv.from_utf8_7bit(i->comm).c_str(), '\0', 255);
			wpts[n]->lat  = i->y;
			wpts[n]->lon  = i->x;
			wpts[n]->smbl = i->symb.val;
			wpts[n]->dspl = i->displ;
			wpts[n]->colour = i->color.RGB().value;
			wpts[n]->alt  = i->z;
			wpts[n]->time = i->t.value;
			n++;
		}
		if (!GPS_Command_Send_Waypoint(port, wpts, n, NULL))
                  throw MapsoftErr("GEO_IO_GPS_WPT_SEND")
                    << "Can't send waypoints to GPS device";
	}

	void put_track (const char * port, const g_track & tr, const Options & opt){
		int num = tr.size()+1;

                if (opt.exists("verbose"))
                  cerr << "put tracks to GPS device " << port << endl;

		GPS_PTrack *trks = (GPS_PTrack *) calloc(num, sizeof(GPS_PTrack));

		IConv cnv("KOI8-R");

		trks[0] = GPS_Track_New();
		trks[0]->ishdr = 1;
		trks[0]->dspl   = tr.displ;
		trks[0]->colour = tr.color.RGB().value;
		memccpy(trks[0]->trk_ident, cnv.from_utf8_7bit(tr.comm).c_str(), '\0', 255);

		int n = 1;
		for (vector<g_trackpoint>::const_iterator i =  tr.begin();
			 i != tr.end(); i++){
			trks[n] = GPS_Track_New();
			trks[n]->ishdr = 0;
			trks[n]->lat  = i->y;
			trks[n]->lon  = i->x;
			trks[n]->Time = i->t.value;
			trks[n]->alt  = i->z;
			trks[n]->dpth = i->depth;
			trks[n]->tnew = i->start ? 1:0;
			n++;
		}
		if (!GPS_Command_Send_Track(port, trks, num))
                  throw MapsoftErr("GEO_IO_GPS_TRK_SEND")
                    << "Can't send tracks to GPS device";
	}

        void turn_off (const char* port, const Options &opt){
          if (opt.exists("verbose"))
            cerr << "turn off GPS device " << port << endl;
          if (!GPS_Command_Off(port))
            throw MapsoftErr("GEO_IO_GPS_OFF")
              << "Can't turn off GPS device";
        }

	void put_all (const char * port, const geo_data & world, const Options & opt)
	{
                init_gps(port);
		for (vector<g_waypoint_list>::const_iterator i = world.wpts.begin(); i!=world.wpts.end(); i++)
		  put_waypoints (port, *i, opt);
		for (vector<g_track>::const_iterator i = world.trks.begin(); i!=world.trks.end(); i++)
		  put_track (port, *i, opt);
		if (opt.find("gps_off")!=opt.end()) turn_off(port, opt);
	}

	void get_all (const char* port, geo_data & world, const Options &opt){
            init_gps(port, opt);
	    get_waypoints(port, world, opt);
	    get_tracks(port, world, opt);
	    if (opt.find("gps_off")!=opt.end()) turn_off(port, opt);
	}
}
