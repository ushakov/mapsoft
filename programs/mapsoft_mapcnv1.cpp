#define DEBUG_LAYER_GEOMAP

#include "../layers/layer_geomap.h"
#include "../libgeo_io/io.h"
#include "../libgeo/geo_convs.h"

#include "../libgeo_io/geofig.h"

double scale;
double dpi;
std::string lon0;
double X1;
double Y1;
double X2;
double Y2;

int
main(int argc, char **argv){
    if (argc<9) {
        std::cerr << "usage: " << argv[0] << " scale dpi lon0 X1 X2 Y1 Y2 <files>\n";
        exit(0);
    }
    scale = atof(argv[1]);
    dpi   = atof(argv[2]);
    lon0  = argv[3];
    X1    = atof(argv[4]);
    X2    = atof(argv[5]);
    Y1    = atof(argv[6]);
    Y2    = atof(argv[7]);

    geo_data world;
    LayerGeoMap     ml(&world, false);
    //чтение файлов из командной строки:
    for(int i=8;i<argc;i++){
      io::in(std::string(argv[i]), world, Options());
    }

    double k = scale/2.54e-2*dpi;
    g_point p1(X1,Y2), p2(X2,Y2), p3(X2,Y1), p4(X1,Y1);
    Options O;
    O["lon0"]=lon0;
    convs::pt2pt c(Datum("pulk42"), Proj("tmerc"), O, Datum("wgs84"), Proj("lonlat"), O);
    c.frw(p1); c.frw(p2); c.frw(p3); c.frw(p4);

    g_map ref;
    ref.push_back(g_refpoint(p1.x, p1.y, 0, 0));
    ref.push_back(g_refpoint(p2.x, p2.y, (X2-X1)*k, 0));
    ref.push_back(g_refpoint(p3.x, p3.y, (X2-X1)*k, (Y2-Y1)*k));
    ref.push_back(g_refpoint(p4.x, p4.y, 0, (Y2-Y1)*k));
    ref.border.push_back(g_point(0,0));
    ref.border.push_back(g_point((X2-X1)*k, 0));
    ref.border.push_back(g_point((X2-X1)*k, (Y2-Y1)*k));
    ref.border.push_back(g_point(0, (Y2-Y1)*k));
    ref.map_proj=Proj("tmerc");

    ml.set_ref(ref);

    int w=int((X2-X1)*k);
    int h=int((Y2-Y1)*k);
    Image<int> im = ml.get_image (Rect<int>(0,0,w,h));
    image_r::save(im, "out.jpg", O);
    ref.file = "out.jpg";
    std::ofstream f("out.map");
    oe::write_map_file(f, ref, Options());
}
