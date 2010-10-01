#define DEBUG_LAYER_KS

#include <fstream>

#include "loaders/image_r.h"
#include "layers/layer_ks.h"
#include "libgeo_io/io.h"
#include "libgeo/geo_convs.h"
#include "libgeo/geo_nom.h"

#include "libgeo_io/geofig.h"
#include "lib2d/line_utils.h"

#include "utils/image_gd.h"
#include <boost/lexical_cast.hpp>

/** Изготовление номенклатурного листа с космоснимком */
int
main(int argc, char **argv){
    if (argc<3) {
        std::cerr << "usage: " << argv[0] << " zoom name\n";
        exit(0);
    }
    int zoom = atoi(argv[1]);
    std::string map_name  = argv[2];


    // разрешение снимка:
    double width = 4 * 256*(1<<(zoom-2));
    double deg_per_pt = 180.0/width; // ~188
    double m_per_pt   = deg_per_pt * M_PI/180 * 6378137.0;

    // определим диапазон карты в координатах lonlat (в СК Пулково!)
    dRect r0 = convs::nom_range(map_name);

    // определим осевой меридиан
    double lon0 = (r0.TLC().x + r0.TRC().x)/2;
    if (r0.w > 11.9) lon0 = floor( lon0/3 ) * 3; // сдвоенные десятки
    else lon0 = floor( lon0/6.0 ) * 6 + 3;
    Options O;
    O.put("lon0", lon0);

    // определим диапазон карты в координатах lonlat (в СК wgs84)
    convs::pt2pt c0(Datum("pulkovo"), Proj("lonlat"), Options(), Datum("wgs84"), Proj("lonlat"), Options());
    dPoint p01(r0.TLC()), p02(r0.BRC());
    c0.frw(p01); c0.frw(p02);
    dRect r = dRect(p01, p02);

    // граница карты в СК wgs84
    dLine border_ll = rect2line(r);

    // углы в координытах lonlat -> углы в координатах карты
    convs::pt2pt cnv(Datum("pulkovo"), Proj("tmerc"), O, Datum("pulkovo"), Proj("lonlat"), Options());
    dPoint p1(r0.TLC()), p2(r0.TRC()), p3(r0.BRC()), p4(r0.BLC());
    cnv.bck(p1); cnv.bck(p2); cnv.bck(p3); cnv.bck(p4);

    p1/=m_per_pt;
    p2/=m_per_pt;
    p3/=m_per_pt;
    p4/=m_per_pt;

    dPoint f_min(std::min(p1.x, p4.x), std::min(p1.y, p2.y));
    dPoint f_max(std::max(p2.x, p3.x), std::max(p3.y, p4.y));

    p1-=f_min; p1.y = f_max.y-f_min.y-p1.y;
    p2-=f_min; p2.y = f_max.y-f_min.y-p2.y;
    p3-=f_min; p3.y = f_max.y-f_min.y-p3.y;
    p4-=f_min; p4.y = f_max.y-f_min.y-p4.y;

    // построим привязку
    std::string basename= map_name + "_ks" + boost::lexical_cast<std::string>(zoom);
    g_map ref;
    ref.map_proj=Proj("tmerc");
    ref.file = basename+".jpg";
    ref.push_back(g_refpoint(r.TLC(), p1));
    ref.push_back(g_refpoint(r.TRC(), p2));
    ref.push_back(g_refpoint(r.BRC(), p3));
    ref.push_back(g_refpoint(r.BLC(), p4));
    convs::map2pt cnv_f(ref, Datum("wgs84"), Proj("lonlat"));
    ref.border = cnv_f.line_bck(border_ll);

    LayerKS     ml("/d/MAPS/KS", zoom);
    ml.set_downloading(true);
    ml.set_ref(ref);

    int w=int(f_max.x-f_min.x);
    int h=int(f_max.y-f_min.y);
    iImage im = ml.get_image (iRect(0,0,w,h));

    ImageDrawContext * ctx(ImageDrawContext::Create(&im));
    for (dLine::iterator i = ref.border.begin(), j=i+1; j!=ref.border.end(); i++,j++)
      ctx->DrawLine(*i, *j, 1, 0xC0000000);
    ctx->StampAndClear();
    O.put("quality", 85);
    image_r::save(im, ref.file.c_str(), O);

    std::ofstream map_stream(std::string(basename+".map").c_str());
    oe::write_map_file(map_stream, ref, Options());
}
