#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <vector>
#include <string>
#include <map>
#include <sys/stat.h>

#include "io.h"
#include "libgeo/geo_convs.h"
#include "utils/options.h"
#include "layers/layer_geomap.h"

#include <math.h>


namespace html{

// функция для записи html (аналогично fig)
// - в директории cache лежат плитки карт tsize x tsize (м),
//   соответствующих масштабу scale c разрешением dpi,
//   в СК datum, в проекции proj, с соответствующими параметрами
// - если плитки нет - создаем ее из наших map-файлов
// - создаем html, использую эти плитки
// - если данные больше msize x msize (м) -- отрезаем левый нижний угол
// - параметр marg -- поле вокруг данных, которое надо оставлять (м)

/***********************************************************/
using namespace std;
bool write(std::ostream & out, const geo_data & world, const Options & opt){

  int tsize = opt.get("tsize", 2000);
  int msize = opt.get("msize", 100000);
  int dpi   = opt.get("dpi",   150);
  double scale = opt.get("scale", 1e-5);
  string cache = opt.get("cache", "./");
  double marg  = opt.get("marg",  100.0);

  Datum datum(opt.get("datum", "pulkovo"));
  Proj  proj(opt.get("proj", "tmerc"));

  // найдем габариты данных в нужных нам координатах, 
  // посмотрим, не надо ли обрезать данные,
  // определим, какие плитки нам нужны:
  convs::pt2pt cnv(datum,proj,opt, Datum("wgs84"), Proj("lonlat"), Options());

  double minx(1e99), miny(1e99), maxx(-1e99), maxy(-1e99);

  vector<g_waypoint_list>::const_iterator wl;
  vector<g_track>::const_iterator tl;
  vector<g_waypoint>::const_iterator w;
  vector<g_trackpoint>::const_iterator t;

  for (wl=world.wpts.begin();wl!=world.wpts.end();wl++){
    for (w=wl->begin();w!=wl->end();w++){
      g_point p(w->x, w->y);
      cnv.bck(p);
      if (p.x > maxx) maxx = p.x;
      if (p.y > maxy) maxy = p.y;
      if (p.x < minx) minx = p.x;
      if (p.y < miny) miny = p.y;
    }
  }
  for (tl=world.trks.begin();tl!=world.trks.end();tl++){
    for (t=tl->begin();t!=tl->end();t++){
      g_point p(t->x, t->y);
      cnv.bck(p);
      if (p.x > maxx) maxx = p.x;
      if (p.y > maxy) maxy = p.y;
      if (p.x < minx) minx = p.x;
      if (p.y < miny) miny = p.y;
    }
  }
  if ((maxx<minx)||(maxy<miny)){
    std::cerr << "o_htm: empty data\n";
    return false;
  }

  minx-=marg;
  miny-=marg;
  maxx+=marg;
  maxy+=marg;
  if (maxx-minx > msize) maxx = minx+msize;
  if (maxy-miny > msize) maxy = miny+msize;

  int tminx = int(floor(minx/tsize));
  int tminy = int(floor(miny/tsize));

  int tmaxx = int(ceil(maxx/tsize));
  int tmaxy = int(ceil(maxy/tsize));

  minx = tminx*tsize;
  maxx = tmaxx*tsize;
  miny = tminy*tsize;
  maxy = tmaxy*tsize;

  std::cerr << "o_html: xt = " << int(minx) << " - " << int(maxx) << 
                      " yt = " << int(miny) << " - " << int(maxy) << "\n";

  // сделаем привязку нашей карты (в координатах растрового изображения!)
  g_map ref;

  g_point rp1(minx, miny), rp2(minx, maxy), rp3(maxx, miny), rp4(maxx, maxy);
  cnv.frw(rp1); cnv.frw(rp2); cnv.frw(rp3); cnv.frw(rp4);

  double k = scale/2.54e-2*dpi;
  int S = int(tsize*k);
  double W = (maxx-minx)*k;
  double H = (maxy-miny)*k;
  ref.push_back(g_refpoint(rp1.x, rp1.y, 0, H));
  ref.push_back(g_refpoint(rp2.x, rp2.y, 0, 0));
  ref.push_back(g_refpoint(rp3.x, rp3.y, W, H));
  ref.push_back(g_refpoint(rp4.x, rp4.y, W, 0));
  ref.border.push_back(g_point(0,0));
  ref.border.push_back(g_point(0,0));
  ref.border.push_back(g_point(0,0));
  ref.map_proj=proj;

  // если даны какие-то карты - пополним кэш
  // (если карт нет - считаем, что кэш сделан до нас и менять его не надо):
  if (!world.maps.empty()){

    LayerGeoMap ml(&world);
    ml.set_ref(ref);
    ml.dump_maps("out.fig");

    for (int y = tminy; y<tmaxy; y++){
    for (int x = tminx; x<tmaxx; x++){
      ostringstream file, dir;
      if (cache != "") dir << cache << "/";
      dir  << y << "/";
      file << x << ".jpg";
      struct stat st;
      stat(dir.str().c_str(), &st);
      mkdir(dir.str().c_str(), 0755);
      stat((dir.str()+file.str()).c_str(), &st);
      if (S_ISREG(st.st_mode)) continue; //файл существует

      iImage im(S,S);
      ml.draw (iPoint(S*(x-tminx),S*(tmaxy-y-1)), im);
      image_r::save(im, (dir.str()+file.str()).c_str(), Options());
    }
    }
  }
  
  // выводим файл 
  out << "<html><head>\n" 
      << "<LINK type=\"text/css\" rel=\"stylesheet\" href=\"lines.css\">\n"
      << "</head>\n"
      << "<body>\n"
      << "<script src=\"lines.js\"></script>\n";

  // картинки
  for (int y = tminy; y<tmaxy; y++){
  for (int x = tminx; x<tmaxx; x++){
    ostringstream file, dir;
    if (cache != "") dir << cache << "/";
    dir  << y << "/";
    file << x << ".jpg";
    struct stat st;
    stat((dir.str()+file.str()).c_str(), &st);
    if (!S_ISREG(st.st_mode)) continue; //файл не существует

    int x1 = int(S*(x-tminx));
    int y1 = int(S*(tmaxy-y-1));
    int x2 = int(x1+W);
    int y2 = int(y1+H);

    out << "<img src=\""<< dir.str() << file.str() << "\" class=\"map\"" 
        << " style=\"position: absolute; "
        << "left: " << x1 << "px; top: " << y1 << "px; " 
        << "width: " << S << "px; height: "<< S << "px\">\n";
  }
  }

  // подписи сетки
  int dd=5;
  for (int y = tminy; y<=tmaxy; y++){
    out << "<b style=\"position: absolute; "
        << " left: " << (tmaxx-tminx)*S+dd << "px; top: " << (tmaxy-y)*S-dd << "px; "
        << "\">" << int(y*tsize) << "</b>\n";
  }
  for (int x = tminx; x<tmaxx; x++){
    out << "<b style=\"position: absolute; "
        << " left: " << (x-tminx)*S << "px; top: " <<  (tmaxy-tminy)*S+dd<< "px; "
        << "\">| " << int(x*tsize) << "</b>\n";
  }


  // треки
  for (tl=world.trks.begin();tl!=world.trks.end();tl++){

    t=tl->begin();
    do {
      vector<g_trackpoint> pts;
//      vector<string> pts_t;
      g_track def_t;
      do{
        g_trackpoint p(*t);
        cnv.bck(p);
	pts.push_back(p);
        t++;
      } while ((t!=tl->end())&&(!t->start));

      out << "<script id=\"t0\">draw_track(new Array(\n";    

      for (int i = 0; i< pts.size(); i++){
        if (i!=0) out << ",\n";
        out << int((pts[i].x-minx)*k) << "," << int((maxy-pts[i].y)*k) 
        << ",\"h: " << pts[i].z << "; t: " << pts[i].t << "\"";
     }
      out << "),0);</script>\n";    
    } while (t!=tl->end());
  }
  // точки
  for (wl=world.wpts.begin();wl!=world.wpts.end();wl++){
    out << "<script id=\"w\">draw_wpts(new Array(\n";    
    for (int i=0; i<wl->size(); i++){
      g_point p((*wl)[i].x, (*wl)[i].y);
      g_waypoint def_pt;
      cnv.bck(p);
      int tx=int((p.x-minx)*k), ty=int((maxy-p.y)*k);
      if (i!=0) out << ",\n";
      out << tx << "," << ty << ",\"" << (*wl)[i].name << "\",\"h: " << (*wl)[i].z << "; comm: " << (*wl)[i].comm << "\"";
    }
    out << "));</script>\n";    
  }

}
}
