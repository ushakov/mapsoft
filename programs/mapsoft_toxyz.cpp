#include <iostream>
#include <queue>

#include "../core/libgeo_io/io.h"
#include "../core/libgeo/geo_convs.h"

int main (int argc, char **argv) {
    geo_data world;
    for(int i = 1; i < argc; i++) {
	io::in(std::string(argv[i]), world, Options());
    }

    double speed, distance = 0;
    std::queue<std::pair<double, double> > timedist;

    g_trackpoint pp;
    int k = 0;

    double window = 120;

    convs::pt2pt pc(Datum("wgs84"), Proj("tmerc"), Options(),
                    Datum("wgs84"), Proj("lonlat"), Options());
    for (int i = 0; i < world.trks.size(); ++i) {
	for (int p = 0; p < world.trks[i].size(); ++p) {
	    g_trackpoint & tp = world.trks[i][p];
	    pc.bck(tp); // координаты -- в tmerc

	    if (k != 0) {
		// update values:
		// distance
		distance += hypot(tp.x - pp.x, tp.y - pp.y);
		timedist.push(std::make_pair(tp.t.value, distance));
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
	    std::cout << (int)tp.x << " " << (int)tp.y << " " << (int)tp.z << " " << tp.t << " " << distance << " " << speed << std::endl;
	    pp = tp;
	    ++k;
	}
    }
}
