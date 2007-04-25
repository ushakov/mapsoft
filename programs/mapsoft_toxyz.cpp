#include <iostream>

#include "../geo_io/io.h"
#include "../geo_io/geo_convs.h"

int main (int argc, char **argv) {
    geo_data world;
    for(int i = 1; i < argc; i++) {
	io::in(std::string(argv[i]), world, Options());
    }

    convs::pt2ll pc(Datum("wgs84"), Proj("tmerc"), Options());
    for (int i = 0; i < world.trks.size(); ++i) {
	for (int p = 0; p < world.trks[i].size(); ++p) {
	    g_trackpoint & tp = world.trks[i][p];
	    pc.bck(tp); // координаты -- в tmerc
	    std::cout << (int)tp.x << " " << (int)tp.y << " " << (int)tp.z << " " << tp.t << " " << (tp.start ? 1 : 0) << std::endl;
	}
    }
}
