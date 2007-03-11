#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <vector>
#include <string>
#include <time.h>

#include "geo_data.h"
#include "io.h"


namespace gu {

	using namespace std;
	using namespace boost::spirit;

	typedef char                    char_t;
	typedef file_iterator <char_t>  iterator_t;
	typedef scanner<iterator_t>     scanner_t;
	typedef rule <scanner_t>        rule_t;

// format-specific data types and conversions to geo_data.h types...
	struct gu_waypoint{
		double lat,lon;
		string name,comm;
		int displ,symb;
		gu_waypoint(){
			g_waypoint def;
			lat=def.y; 
			lon=def.x; 
			symb=def.symb;
			displ=def.displ;
		}
		operator g_waypoint () const{
			g_waypoint ret;
			ret.y     = lat;
			ret.x     = lon;
			ret.name  = name;
			ret.comm  = comm;
			ret.displ = displ;
			ret.symb  = symb;
			return ret;
		}
	};

	struct gu_trackpoint{
		double lat,lon;
		struct tm ts; // year from 0, month from 1!
		bool start;
		gu_trackpoint(){
			lat=0; lon=0; start=false;
			time_t t = time(NULL);
			struct tm * ts1 = localtime(&t);
			ts = *ts1;
			ts.tm_year+=1900;
			ts.tm_mon+=1;
		}
		operator g_trackpoint () const{
			g_trackpoint ret;
			ret.y     = lat;
			ret.x     = lon;
			ret.start = start;
			struct tm ts1 = ts;
			ts1.tm_mon-=1;
			if (ts1.tm_year>1900) ts1.tm_year-=1900;
			ret.t = mktime(&ts1);
			return ret;
		}
	};

	struct gu_waypoint_list{
		vector<gu_waypoint> points;
		operator g_waypoint_list () const{
			g_waypoint_list ret;
			vector<gu_waypoint>::const_iterator i, 
				b=points.begin(), e=points.end();
			for (i=b; i!=e; i++) ret.push_back(*i);
			return ret;
		}
	};

	struct gu_track{
		vector<gu_trackpoint> points;
		operator g_track () const{
			g_track ret;
			vector<gu_trackpoint>::const_iterator i, 
				b=points.begin(), e=points.end();
			for (i=b; i!=e; i++) ret.push_back(*i);
			return ret;
		}
	};

// function for reading objects from garmin-utils file
// into the world object
	bool read_file(const char* filename, geo_data & world, const Options & opt){

		// iterators for parsing
		iterator_t first(filename);
		if (!first) { cerr << "can't find file " << filename << '\n'; return 0;}
		iterator_t last = first.make_end();

		// нам здесь не требуется инициализация объектов, так как 
		// мы обязательно заполняем все поля
		// (только с полем t_pt.start возникает легкая проблема)
		// Когда мы будем работать с Ozi файлами - за этим придется следить...
		gu_waypoint wpt;
		gu_trackpoint tpt;
		gu_waypoint_list w;
		gu_track t;

		rule_t header = str_p("[product ") >> uint_p >> ", version " >>
			uint_p >> ": " >> +(print_p - ']') >>
			']' >> *blank_p;

		rule_t gu_time = 
			uint_p[assign_a(tpt.ts.tm_year)] >> '-' >> 
			uint_p[assign_a(tpt.ts.tm_mon)]  >> '-' >> 
			uint_p[assign_a(tpt.ts.tm_mday)] >> +blank_p >> 
			uint_p[assign_a(tpt.ts.tm_hour)] >> ':' >> 
			uint_p[assign_a(tpt.ts.tm_min)]  >> ':' >> 
			uint_p[assign_a(tpt.ts.tm_sec)];

		rule_t wpt_line = +ch_p('\n')
			>> *blank_p >> (*(alnum_p | '_' | '-'))[assign_a(wpt.name)]
			>> +blank_p >> real_p[assign_a(wpt.lat)]
			>> +blank_p >> real_p[assign_a(wpt.lon)]
			>> +blank_p >> uint_p[assign_a(wpt.symb)]
			>> '/' >> uint_p[assign_a(wpt.displ)]
			>> blank_p >> (*print_p)[assign_a(wpt.comm)];

		rule_t trk_line = +ch_p('\n')
			>> *blank_p >> real_p[assign_a(tpt.lat)]
			>> +blank_p >> real_p[assign_a(tpt.lon)][assign_a(tpt.start,false)]
			>> +blank_p >> gu_time
			>> !(+blank_p >> str_p("start")[assign_a(tpt.start,true)]
				)>> *blank_p;

		rule_t wpt_block = (+ch_p('\n') >> str_p("[waypoints,") >> +blank_p
							>> uint_p >> " records]"  >> *blank_p)
										>> *wpt_line[push_back_a(w.points, wpt)]
										>> (+ch_p('\n') >> "[end transfer," >> +blank_p >> uint_p >> '/'
											>> uint_p >> " records]" >> *blank_p)[push_back_a(world.wpts, w)];

		rule_t trk_block = (+ch_p('\n') >> str_p("[tracks,") >> +blank_p
							>> uint_p >> " records]"  >> *blank_p)
										>> *trk_line[push_back_a(t.points, tpt)]
										>> (+ch_p('\n') >> "[end transfer," >> +blank_p >> uint_p >> ch_p('/')
											>> uint_p >> " records]" >> *blank_p)[push_back_a(world.trks, t)];

		parse_info<iterator_t> info =
			parse(first, last,
				  header >> +( wpt_block | trk_block ) >> *space_p);

		return info.full;
	}

/********************************************/

	bool write_track(ofstream & f, const g_track & tr, const Options & opt){
		int num = tr.size();
		f << "[tracks, " << num << " records]\n";
		for (vector<g_trackpoint>::const_iterator p = tr.begin(); p != tr.end(); p++)
		{
			struct tm * ts = localtime(&p->t);
			if (ts == NULL) { time_t t = time(NULL);  ts = localtime(&t);}
			f << right << fixed << setprecision(6) << setfill(' ')
			  << setw(10)<< p->y << " "
			  << setw(11)<< p->x << " "
			  << setfill('0')
			  << setw(4) << ts->tm_year+1900 << "-"
			  << setw(2) << ts->tm_mon+1 << "-"
			  << setw(2) << ts->tm_mday  << " "
			  << setw(2) << ts->tm_hour  << ":"
			  << setw(2) << ts->tm_min   << ":"
			  << setw(2) << ts->tm_sec
			  << ((p->start)? " start":"") << "\n";
		}
		f << "[end transfer, " << num << "/" << num << " records]\n";
	}

	bool write_waypoint_list(ofstream & f, const g_waypoint_list & wp, const Options & opt){
		int num = wp.size();
		f << "[waypoints, " << num << " records]\n";
		for (vector<g_waypoint>::const_iterator p = wp.begin(); p!=wp.end(); p++){
			f << left << setw(6) << setfill(' ') << p->name << " " 
			  << right << fixed << setprecision(6)
			  << setw(10) << p->y << " "
			  << setw(11) << p->x << " "
			  << setw(5) << p->symb << "/"
			  << p->displ << " "
			  << p->comm  << "\n";
		}
		f << "[end transfer, " << num << "/" << num << " records]\n";
	}

	bool write_file(const char* filename, const geo_data & world, const Options & opt){
		ofstream f(filename);
		f << "[product 00, version 000: MAPSOFT]\n";
		for (vector<g_waypoint_list>::const_iterator i = world.wpts.begin(); i!=world.wpts.end(); i++)
			write_waypoint_list(f, *i, opt);
		for (vector<g_track>::const_iterator i = world.trks.begin(); i!=world.trks.end(); i++)
			write_track(f, *i, opt);
		return true;
	}
}
