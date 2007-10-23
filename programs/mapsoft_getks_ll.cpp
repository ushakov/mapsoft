#define DEBUG_LAYER_KS

#include "../loaders/image_r.h"
#include "../utils/point.h"
#include "../layers/layer_ks.h"
#include "../geo_io/io.h"
#include "../geo_io/geo_convs.h"

#include "../geo_io/geofig.h"

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
    Image<int> im = ml.get_image (Rect<int>(0,0,w,h));
    image_r::save(im, "out.jpg", O);

    geo_data W;
    W.maps.push_back(ref);
    W.maps.push_back(ml.mymap);
    W.maps.push_back(ml.mymap0);
    xml::write_file("out.xml", W, Options());
/*
    for (g_map::iterator i=ref.begin(); i!=ref.end(); i++){
      i->xr *= 2.54/dpi * fig::cm2fig;
      i->yr *= 2.54/dpi * fig::cm2fig;
    }
    fig::fig_world W;
    fig::set_ref(W, ref, Options());
    fig::fig_object o = fig::make_object("2 5 0 1 0 -1 500 -1 -1 0.000 0 0 -1 0 0 *");

    for (g_map::iterator i=ref.begin(); i!=ref.end(); i++){
      o.push_back(Point<int>(int(i->xr), int(i->yr)));
    }
    o.push_back(Point<int>(int(ref[0].xr), int(ref[0].yr)));
    o.image_file = "out.jpg";
    o.comment.push_back("MAP AAA");
    W.push_back(o);
    std::ofstream f("out2.fig");
    fig::write(f, W);*/
}
