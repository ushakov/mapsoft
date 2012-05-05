#include <iostream>
#include <queue>

#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "utils/err.h"
#include "options/m_getopt.h"

using namespace std;

static struct ext_option options[] = {
  {"lon0",         1,'o', 1, "set lon0 (default: auto)"},
  {0,0,0,0}
};

void usage(bool pod=false){
  const char * fname = "mapsoft_toxyz";
  cerr << fname << " -- write track data in text form\n"
       << "Usage: " << fname << " <options> <input files>\n";
  cerr << "Options:\n";
  print_options(options, 1, cerr);
  exit(1);
}

int main (int argc, char **argv) {

  if (argc==1) usage();
  vector<string> infiles;
  Options O = parse_options_all(&argc, &argv, options, 1, infiles);
  if (O.exists("help")) usage();

  geo_data world;
  for (vector<string>::const_iterator i = infiles.begin(); i!=infiles.end(); i++){
    try { io::in(*i, world, O);}
    catch (MapsoftErr e) {cerr << e.str() << endl;}
  }

  double speed, distance = 0;
  queue<pair<double, double> > timedist;

  g_trackpoint pp;
  int k = 0;

  double window = 120;

  double lon0 = O.get("lon0", convs::lon2lon0(world.range().CNT().x));

  Options geo_opts;
  geo_opts.put("lon0", lon0);
  convs::pt2pt pc(Datum("wgs84"), Proj("tmerc"), geo_opts,
                    Datum("wgs84"), Proj("lonlat"), Options());

    for (int i = 0; i < world.trks.size(); ++i) {
	for (int p = 0; p < world.trks[i].size(); ++p) {
	    g_trackpoint & tp = world.trks[i][p];
	    pc.bck(tp); // координаты -- в tmerc

	    if (k != 0) {
		// update values:
		// distance
		distance += hypot(tp.x - pp.x, tp.y - pp.y);
		timedist.push(make_pair(tp.t.value, distance));
		while (timedist.front().first < tp.t.value - window && timedist.size() > 2) {
		    timedist.pop();
		}
		double traveled = timedist.back().second - timedist.front().second;
		double time = timedist.back().first - timedist.front().first;
		speed = traveled/time * 3.6;
	    } else {
		distance = 0;
		speed = 0;
	    }
	    cout << (int)tp.x << " " << (int)tp.y << " " << (int)tp.z << " " << tp.t << " " << distance << " " << speed << endl;
	    pp = tp;
	    ++k;
	}
    }
}
