#define DEBUG_LAYER_KS

#include "../core/loaders/image_r.h"
#include "../core/layers/layer_ks.h"
#include "../core/libgeo_io/io.h"
#include "../core/libgeo/geo_convs.h"

#include "../core/libgeo_io/geofig.h"
#include <iostream>

int zoom;
double X1;
double Y1;
double X2;
double Y2;

int
main(int argc, char **argv){
    if (argc<6) {
        std::cerr << "usage: " << argv[0] << " zoom lon1 lon2 lat1 lat2\n";
        exit(0);
    }
    zoom = atoi(argv[1]);
    X1    = atof(argv[2]);
    X2    = atof(argv[3]);
    Y1    = atof(argv[4]);
    Y2    = atof(argv[5]);

    LayerKS     ml("/d/MAPS/KS", zoom);
    ml.set_downloading(true);

    double deg_per_pt = 360.0/256.0/(2 << (zoom-1));

    g_map ref;
    ref.push_back(g_refpoint(X1, Y2, 0,                  0));
    ref.push_back(g_refpoint(X2, Y2, (X2-X1)/deg_per_pt, 0));
    ref.push_back(g_refpoint(X2, Y1, (X2-X1)/deg_per_pt, (Y2-Y1)/deg_per_pt));
    ref.push_back(g_refpoint(X1, Y1, 0,                  (Y2-Y1)/deg_per_pt));
    ref.border.push_back(g_point(0,0));
    ref.border.push_back(g_point(0,0));
    ref.border.push_back(g_point(0,0));
    ref.map_proj=Proj("lonlat");

    ml.set_ref(ref);

    int w=int((X2-X1)/deg_per_pt);
    int h=int((Y2-Y1)/deg_per_pt);
    iImage im = ml.get_image (iRect(0,0,w,h));
    image_r::save(im, "out.jpg", Options());

}
