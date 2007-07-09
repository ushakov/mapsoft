#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <vector>
#include <string>
#include <map>
#include <sys/stat.h>

#include "io.h"
#include "geo_enums.h"
#include "geo_names.h"
#include "geo_convs.h"
#include "../utils/mapsoft_options.h"
#include "../layers/layer_geomap.h"

#include <math.h>


namespace fig{

// функция для записи fig
// - в директории cache лежат плитки карт tsize x tsize (м),
//   соответствующих масштабу scale c разрешением dpi,
//   в СК datum, в проекции proj, с соответствующими параметрами
// - если плитки нет - создаем ее из наших map-файлов
// - создаем fig, использую эти плитки
// - если данные больше msize x msize (м) -- отрезаем левый нижний угол
// - параметр marg -- поле вокруг данных, которое надо оставлять (м)

/***********************************************************/
using namespace std;
bool write(std::ostream & out, const geo_data & world, const Options & opt){

  int tsize = 2000;    opt.get("tsize", tsize);
  int msize = 100000;  opt.get("msize", msize);
  int dpi   = 150;     opt.get("dpi",   dpi);
  double scale = 1e-5; opt.get("scale", scale);
  string cache = "./"; opt.get("cache", cache);
  double marg = 100;   opt.get("marg", marg);

  string datum_str("pulkovo"); opt.get("datum", datum_str);
  string proj_str("tmerc");    opt.get("proj", proj_str);

  Datum datum(datum_str);
  Proj  proj(proj_str);

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
    std::cerr << "o_fig: empty data\n";
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

  std::cerr << "o_fig: xt = " << int(minx) << " - " << int(maxx) << 
                     " yt = " << int(miny) << " - " << int(maxy) << "\n";

  // сделаем привязку нашей карты (в координатах растрового изображения!)
  g_map ref;

  g_point rp1(minx, miny), rp2(minx, maxy), rp3(maxx, miny), rp4(maxx, maxy);
  cnv.frw(rp1); cnv.frw(rp2); cnv.frw(rp3); cnv.frw(rp4);

  double k = scale/2.54e-2*dpi;
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

      int S = int(tsize*k);
      Image<int> im(S,S);
      ml.draw (Point<int>(S*(x-tminx),S*(tmaxy-y-1)), im);
      image_r::save(im, (dir.str()+file.str()).c_str(), Options());
    }
    }
  }
  
  // выводим файл 
  out << "#FIG 3.2\nPortrait\nCenter\nMetric\nA4\n100.0\nSingle\n-2\n1200 2\n";
  double cm2fig = 449.943757;
  double kf = scale*100*cm2fig;
  int Sf = int(kf*tsize);

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

    int x1 = Sf*(x-tminx);
    int y1 = Sf*(tmaxy-y-1);
    int x2 = x1+Sf;
    int y2 = y1+Sf;

    out << "2 5 0 1 4 -1 400 -1 20 0.000 0 0 -1 0 0 5\n"
        << "\t0 " << dir.str() << file.str() << "\n\t"
        << x1 << " " << y1 << "  "
        << x2 << " " << y1 << "  "
        << x2 << " " << y2 << "  "
        << x1 << " " << y2 << "  "
        << x1 << " " << y1 << "\n";
  }
  }

  // подписи сетки
  int dd=50;
  for (int y = tminy; y<=tmaxy; y++){
    out << "4 2 0 400 -1 18 10 0.0000 4 0 0 "
        << -dd << " " << (tmaxy-y)*Sf+dd << " " << int(y*tsize) << "\\001\n";
    out << "4 0 0 400 -1 18 10 0.0000 4 0 0 "
        << (tmaxx-tminx)*Sf+dd << " " << (tmaxy-y)*Sf+dd << " " << int(y*tsize) << "\\001\n";
  }
  for (int x = tminx; x<=tmaxx; x++){
    out << "4 0 0 400 -1 18 10 1.5708 4 0 0 " 
        << (x-tminx)*Sf+dd << " " << -dd << " " << int(x*tsize) << "\\001\n";
    out << "4 2 0 400 -1 18 10 1.5708 4 0 0 "
        << (x-tminx)*Sf+dd << " " << (tmaxy-tminy)*Sf+dd << " " << int(x*tsize) << "\\001\n";
  }

  // привязка
  int ref_color=4, ref_depth=1, ref_width=3;
  for (vector<g_refpoint>::const_iterator i = ref.begin(); i!=ref.end(); i++){
    out << "# REF " << i->x << " " << i->y << "\n"
        << "2 1 0 " << ref_width << " " << ref_color << " 7 " << ref_depth << " 0 -1 1 0 0 -1 0 0 1\n"
        << "\t" << int(i->xr /k*kf) << " " << int(i->yr /k*kf) << "\n";
  }



  // точки
  for (wl=world.wpts.begin();wl!=world.wpts.end();wl++){
    int wpt_color=0, wpt_depth=59, wpt_width=2;
    int wpt_txt_color=22, wpt_txt_depth=58, wpt_txt_font=18, wpt_txt_size=8;
    for (w=wl->begin();w!=wl->end();w++){
      g_point p(w->x, w->y);
      g_waypoint def_pt;
      cnv.bck(p);
      int tx=int((p.x-minx)*kf), ty=int((maxy-p.y)*kf);

      out << "# WPT " << w->name << "\n";
      if (w->z   < 1e20)                      out << "# alt:        " << fixed << setprecision(1) << w->z << "\n";
      if (w->t != def_pt.t)                   out << "# time:       " << w->t << "\n";
      if (w->comm != def_pt.comm)             out << "# comm:       " << w->comm << "\n";
      if (w->prox_dist != def_pt.prox_dist)   out << "# prox_dist:  " << fixed << setprecision(1) << w->prox_dist << "\n";
      if (w->symb != def_pt.symb)             out << "# symb:       " << wpt_symb_enum.int2str(w->symb) << "\n";
      if (w->displ != def_pt.displ)           out << "# displ:      " << w->displ << "\n";
      if (w->color != def_pt.color)           out << "# color:      #" << setbase(16) << setw(6) << setfill('0') << w->color << setbase(10)   << "\n";
      if (w->bgcolor != def_pt.bgcolor)       out << "# bgcolor:    #" << setbase(16) << setw(6) << setfill('0') << w->bgcolor << setbase(10) << "\n";
      if (w->map_displ != def_pt.map_displ)   out << "# map_displ:  " << wpt_map_displ_enum.int2str(w->map_displ) << "\n";
      if (w->pt_dir != def_pt.pt_dir)         out << "# pt_dir:     " << wpt_pt_dir_enum.int2str(w->pt_dir) << "\n";
      if (w->font_size != def_pt.font_size)   out << "# font_size:  " << w->font_size << "\n";
      if (w->font_style != def_pt.font_style) out << "# font_style: " << w->font_style << "\n";
      if (w->size != def_pt.size)             out << "# size:       "  << w->size << "\n";

      out << "2 1 0 " << wpt_width << " " << wpt_color << " 7 " << wpt_depth 
          << " 0 -1 1 1 1 -1 0 0 1\n\t"
          << tx << " " << ty << "\n";

      int td=0;
      // 15 = 1200/80
/*            if (w->pt_dir == 0){tx=(int)p.x; ty=(int)p.y + 15*w->size; td=1;} // buttom
              if (w->pt_dir == 1){tx=(int)p.x; ty=(int)p.y - 15*w->size - 15*w->font_size; td=1;} // top
              if (w->pt_dir == 2){tx=(int)p.x + 15*w->size; ty=(int)p.y; td=0;} // left
              if (w->pt_dir == 3){tx=(int)p.x - 15*w->size; ty=(int)p.y; td=2;} // right
*/
      out << "4 " << td << " " << wpt_txt_color << " " << wpt_txt_depth
          << " -1 "<< wpt_txt_font << " " << wpt_txt_size << " 0.0000 4 105 150 "
          << tx+15*wpt_width << " " << ty+15*wpt_width << " " << w->name << "\\001\n";
    }
  }

  // треки
  for (tl=world.trks.begin();tl!=world.trks.end();tl++){
    int trk_color=1, trk_depth=60, trk_width=1;

    t=tl->begin();
    do {
      vector<Point<int> > pts;
      g_track def_t;
      do{
        g_point p(t->x, t->y);
        cnv.bck(p);
	pts.push_back(Point<int>(int((p.x-minx)*kf),int((maxy-p.y)*kf)));
        t++;
      } while ((t!=tl->end())&&(!t->start));

      out << "# TRK " << tl->comm << "\n";
      if (tl->width != def_t.width) out << "# width: "  << tl->width << "\n";
      if (tl->displ != def_t.displ) out << "# displ: "  << tl->displ << "\n";
      if (tl->color != def_t.color) out << "# color: #" << setbase(16) << setw(6) << setfill('0') << tl->color << setbase(10) << "\n";
      if (tl->skip  != def_t.skip)  out << "# skip:  "  << tl->skip << "\n";
      if (tl->type  != def_t.type)  out << "# type:  "  << trk_type_enum.int2str(tl->type) << "\n";
      if (tl->fill  != def_t.fill)  out << "# fill:  "  << trk_fill_enum.int2str(tl->fill) << "\n";
      if (tl->cfill != def_t.cfill) out << "# cfill: #" << setbase(16) << setw(6) << setfill('0') << tl->cfill << setbase(10) << "\n";
  
      out << "2 1 0 " << trk_width << " " << trk_color << " 7 " << trk_depth 
          << " 0 -1 1 0 0 -1 0 0 " << pts.size() << "\n";
      for (vector<Point<int> >::const_iterator p1=pts.begin(); p1!=pts.end(); p1++)
        out << "\t" << p1->x << " " << p1->y << "\n";
    } while (t!=tl->end());
  }

}
}
