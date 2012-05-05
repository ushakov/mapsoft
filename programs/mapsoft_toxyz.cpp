#include <iostream>
#include <queue>

#include "geo_io/io.h"
#include "geo/geo_convs.h"
#include "utils/err.h"

using namespace std;
int main (int argc, char **argv) {
    geo_data world;
    for(int i = 1; i < argc; i++) {
      try {io::in(string(argv[i]), world);}
      catch (MapsoftErr e) {cerr << e.str() << endl;}
    }

    double speed, distance = 0;
    queue<pair<double, double> > timedist;

    g_trackpoint pp;
    int k = 0;

    double window = 120;

    Options o;
    o.put("lon0", convs::lon2lon0(world.range().CNT().x));
    convs::pt2pt pc(Datum("wgs84"), Proj("tmerc"), o,
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
