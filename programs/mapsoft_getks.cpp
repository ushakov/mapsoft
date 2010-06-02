#define DEBUG_LAYER_KS

#include "loaders/image_r.h"
#include "layers/layer_ks.h"
#include "libgeo_io/io.h"
#include "libgeo/geo_convs.h"

#include "libgeo_io/geofig.h"

int zoom;
std::string lon0;
double X1;
double Y1;
double X2;
double Y2;

int
main(int argc, char **argv){
    if (argc<7) {
        std::cerr << "usage: " << argv[0] << " zoom lon0 X1 X2 Y1 Y2\n";
        exit(0);
    }
    zoom = atoi(argv[1]);
    lon0  = argv[2];
    X1    = atof(argv[3]);
    X2    = atof(argv[4]);
    Y1    = atof(argv[5]);
    Y2    = atof(argv[6]);

    LayerKS     ml("/d/MAPS/KS", zoom);
    ml.set_downloading(true);

    double width = 4 * 256*(1<<(zoom-2));
    double deg_per_pt = 180.0/width; // ~188
    double m_per_pt   = deg_per_pt * M_PI/180 * 6378137.0;

    g_point p1(X1,Y2), p2(X2,Y2), p3(X2,Y1), p4(X1,Y1);
    Options O;
    O["lon0"]=lon0;
    convs::pt2pt c(Datum("pulk42"), Proj("tmerc"), O, Datum("wgs84"), Proj("lonlat"), O);
    c.frw(p1); c.frw(p2); c.frw(p3); c.frw(p4);

    g_map ref;
    ref.push_back(g_refpoint(p1.x, p1.y, 0, 0));
    ref.push_back(g_refpoint(p2.x, p2.y, (X2-X1)/m_per_pt, 0));
    ref.push_back(g_refpoint(p3.x, p3.y, (X2-X1)/m_per_pt, (Y2-Y1)/m_per_pt));
    ref.push_back(g_refpoint(p4.x, p4.y, 0, (Y2-Y1)/m_per_pt));
    ref.border.push_back(g_point(0,0));
    ref.border.push_back(g_point(0,0));
    ref.border.push_back(g_point(0,0));
    ref.map_proj=Proj("tmerc");

    ml.set_ref(ref);

    int w=int((X2-X1)/m_per_pt);
    int h=int((Y2-Y1)/m_per_pt);
    iImage im = ml.get_image (iRect(0,0,w,h));
    image_r::save(im, "out_ks.jpg", O);

}
