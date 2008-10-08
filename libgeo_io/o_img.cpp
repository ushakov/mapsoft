#define DEBUG_LAYER_GEOMAP

#include "../layers/layer_geomap.h"
#include "io.h"
#include "../libgeo/geo_convs.h"

#include "o_img.h"

namespace img{
using namespace std;

/// Supported options:
/// todo: geom proj (+proj options) datum rscale scale dpi factor geofig


/*

    Scale options: rscale scale dpi factor

Say, you have a map with scale 1:100'000 and image resolution 300dpi.
Without any scale option image will have the same resolution as
in original picture.
To get image two times smaller you may specify resolution explicitely:
    scale=1e-5 dpi=150
    rscale=100000 dpi=300
or you may set relative resolution
    factor=0.5
or even
    scale=1e-5 dpi=300 factor=0.5


    Geofig option

When geofig option is set to some filename, 
GEOFIG file will be also created with map reference
(and geodata?)

    Geometry option



    Default values:

* rscale scale dpi -- auto determined
* factor = 1
* proj   = tmerc
* datum  = wgs84
* geom   = ???
proj options (auto???)

*/

bool write_file (const char* filename, const geo_data & world, const Options & opt){

    double scale=0, rscale=0, dpi=0; // default values 0 - for auto
    double factor=1;
    int debug=0;
    string datum="pulkovo", proj="tmerc", fname="", geofig="";
    g_rect geom(0,0,0,0);

    opt.get("scale",  scale);
    opt.get("rscale", rscale);
    opt.get("dpi",    dpi);
    opt.get("factor", factor);
    opt.get("datum",  datum);
    opt.get("proj",   proj);
//    opt.get("geom",   geom);
    opt.get("fname",  fname);
    opt.get("geofig", geofig);

    opt.get("debug",  debug);

  if (debug) 
    cerr << "o_img:write_file -- writing rectangular region of map in raster format:\n" 
         << "  scale:  " << scale << "\n"
         << "  rscale: " << rscale << "\n"
         << "  dpi:    " << dpi << "\n"
         << "  factor: " << factor << "\n"
         << "  datum:  " << datum << "\n"
         << "  proj:   " << proj << "\n"
         << "  geom:   " << geom << "\n"
         << "  fname:  " << fname << "\n"
         << "  geofig: " << geofig << "\n"
         << "  debug:  " << debug << "\n"
         << "options:  " << opt << "\n";
/*
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
    ref.border.push_back(g_point(0,0));
    ref.border.push_back(g_point(0,0));
    ref.map_proj=Proj("tmerc");

    ml.set_ref(ref);

    int w=int((X2-X1)*k);
    int h=int((Y2-Y1)*k);
    Image<int> im = ml.get_image (Rect<int>(0,0,w,h));
    ml.dump_maps("out1.fig");
    image_r::save(im, "out.jpg", O);


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
    fig::write(f, W);
*/
}
} // namespace