#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include <sstream>
#include <iomanip>
#include "fig.h"
#include "geo_convs.h"
#include "io_xml.h"
#include "../utils/mapsoft_options.h"
#include "../loaders/image_r.h"

namespace fig {

using namespace std;
using namespace xml;
using namespace boost::spirit;

  // Извлечь привязку из fig-картинки
  g_map get_ref(const fig_world & w){
    g_map ret;
    fig::fig_object brd; // граница

    // В комментарии к файлу может быть указано, в какой он проекции
    // default - tmerc
    string proj  = "tmerc";
    for (int n=0; n<w.comment.size(); n++){
      parse(w.comment[n].c_str(), str_p("proj:")  >> space_p >> (*anychar_p)[assign_a(proj)]); 
    }
    ret.map_proj=Proj(proj);

    g_point min(1e99,1e99), max(-1e99,-1e99);
    fig_world::const_iterator i;
    for (i=w.begin();i!=w.end();i++){
      if ((i->type!=2)&&(i->type!=3)) continue;
      if (i->size()<1) continue;
      double x,y;
      if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("REF") 
	  >> +space_p >> real_p[assign_a(x)]
	  >> +space_p >> real_p[assign_a(y)]).full)){
        Options O; 
        string key;
        for (int n=1; n<i->comment.size(); n++){
          parse(i->comment[n].c_str(), (*(anychar_p-':'-space_p))[assign_a(key)] >> 
            *space_p >> ch_p(':') >> *space_p >> 
            (*(anychar_p-':'-space_p))[insert_at_a(O, key)]);
        }
 	g_refpoint ref(x,y,(*i)[0].x,(*i)[0].y);
        if (min.x>(*i)[0].x) min.x = (*i)[0].x;
        if (min.y>(*i)[0].y) min.y = (*i)[0].y;
        if (max.x<(*i)[0].x) max.x = (*i)[0].x;
        if (max.y<(*i)[0].y) max.y = (*i)[0].y;

        convs::pt2pt c(Datum(O.get_string("datum","wgs84")), 
                       Proj(O.get_string("proj","lonlat")), O, Datum("wgs84"), Proj("lonlat"), O);
        c.frw(ref);
	ret.push_back(ref);
        continue;
      }
      if ((i->comment.size()>0)&&(i->comment[0].size()>3) &&
          i->comment[0].compare(0,3,"BRD")) brd = *i; 

    }
    // границы - по точкам привязки или по объекту BRD
    if (brd.size() < 3) {
      ret.border.push_back(min);
      ret.border.push_back(g_point(min.x,max.y));
      ret.border.push_back(max);
      ret.border.push_back(g_point(max.x,max.y));
    } else {
      ret.border.insert(ret.border.end(), brd.begin(), brd.end());
    }
    return ret;
  }

  // Убрать информацию о привязке из fig-файла
  void rem_ref(fig_world & w){
    vector<string>::iterator i = w.comment.begin();
    while (i!=w.comment.end()){
      if (i->find("proj",0)==0) i=w.comment.erase(i);
      else i++;
    }
    fig_world::iterator obj = w.begin();
    while (obj!=w.end()){
      if ((obj->comment.size()>0) &&
          (obj->comment[0].find("REF ",0)==0))
        obj=w.erase(obj);
      else obj++;
    }
  }

  // Добавить привязку в fig_world
  void set_ref(fig_world & w, const g_map & m, const Options & O){
    // Если хочется, можно записать точки привязки в нужной проекции
    Datum  datum(O.get_string("datum", "pulkovo"));
    Proj   proj(O.get_string("proj", "tmerc"));

    double lon0 = m.range().x + m.range().w/2;
    lon0 = floor( lon0/6.0 ) * 6 + 3;
    O.get("lon0", lon0);

    convs::pt2pt cnv(datum,proj,O, Datum("wgs84"), Proj("lonlat"), Options());

    for (int n=0; n<m.size(); n++){
      fig::fig_object o = fig::make_object("2 1 0 4 4 7 1 -1 -1 0.000 0 1 -1 0 0 *");
      o.push_back(Point<int>( int(m[n].xr), int(m[n].yr) ));
      g_point p(m[n]);
      cnv.bck(p);
      ostringstream comm;
      comm << "REF " << fixed << p.x << " " << p.y;
      o.comment.push_back(comm.str()); comm.str("");
      if (datum != Datum("wgs84"))
         comm << "datum: " << datum.xml_str();
      o.comment.push_back(comm.str()); comm.str("");
      if (proj != Proj("lonlat"))
         comm << "proj: " << proj.xml_str();
      o.comment.push_back(comm.str()); comm.str("");
      if ((proj == Proj("tmerc")) && (lon0!=0))
         comm << "lon0: " << lon0;
      o.comment.push_back(comm.str()); comm.str("");
      w.push_back(o);
    }
    // добавим в заголовок fig-файла информацию о проекции.
    // по умолчанию - tmerc
    if (m.map_proj != Proj("tmerc")){
      ostringstream comm;
      comm << "proj: " << m.map_proj.xml_str();
      w.comment.push_back(comm.str());
    }
  }


  // Извлечь треки и точки и поместить их в geo_data
  void get_wpts(const fig_world & w, const g_map & m, geo_data & d){
    // сделаем привязку
    convs::map2pt cnv(m, Datum("wgs84"), Proj("lonlat"), Options());

    fig_world::const_iterator i;

    g_waypoint_list pl;

    for (i=w.begin();i!=w.end();i++){
      if ((i->type!=2)&&(i->type!=3)) continue;
      if (i->size()<1) continue;
      xml_point p;
      if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("WPT")
          >> +space_p >> (*anychar_p)[insert_at_a(p,"name")]).full)){
        string key;
        for (int n=1; n<i->comment.size(); n++){
          parse(i->comment[n].c_str(), (*(anychar_p-':'-space_p))[assign_a(key)] >>
            *space_p >> ch_p(':') >> *space_p >>
            (*(anychar_p-':'-space_p))[insert_at_a(p, key)]);
        }
        g_waypoint wp(p);
        wp.x = (*i)[0].x;
        wp.y = (*i)[0].y;
        cnv.frw(wp);
        pl.push_back(wp);
      }
    }
        d.wpts.push_back(pl);
  }

  void get_trks(const fig_world & w, const g_map & m, geo_data & d){
    convs::map2pt cnv(m, Datum("wgs84"), Proj("lonlat"), Options());

    fig_world::const_iterator i;
    for (i=w.begin();i!=w.end();i++){
      if ((i->type!=2)&&(i->type!=3)) continue;
      if (i->size()<1) continue;
      xml_point_list pl;
      if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("TRK")
          >> !(+space_p >> (*anychar_p)[insert_at_a(pl,"comm")])).full)){
        string key;
        for (int n=1; n<i->comment.size(); n++){
          parse(i->comment[n].c_str(), (*(anychar_p-':'-space_p))[assign_a(key)] >>
            *space_p >> ch_p(':') >> *space_p >>
            (*(anychar_p-':'-space_p))[insert_at_a(pl, key)]);
        }
        g_track tr(pl);
        for (int n = 0; n<i->size();n++){
          g_trackpoint p;
          p.x=(*i)[n].x;
          p.y=(*i)[n].y;
          cnv.frw(p);
          tr.push_back(p);
        }
        d.trks.push_back(tr);
      }
    }
  }

  // Извлечь привязку растровых картинок
  void get_maps(const fig_world & w, const g_map & m, geo_data & d){
    convs::map2pt cnv(m, Datum("wgs84"), Proj("lonlat"), Options());
    fig_world::const_iterator i;
    for (i=w.begin();i!=w.end();i++){
      if ((i->type!=2)||(i->sub_type!=5)) continue;
      if (i->size()<3) continue;
      string comm;
      if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("MAP")
          >> !(+space_p >> (*anychar_p)[assign_a(comm)])).full)){
        // откопируем m и заменим координаты xfig на координаты в растровом файле.
        // 4 точки fig-объекта соответствуют углам картинки (0,0) (W,0) (W,H) (0,H*)
        g_map map(m);
        map.comm = comm;
        map.file = i->image_file;
        Point<int> WH = image_r::size(i->image_file.c_str());
        double x1 = (*i)[0].x;
        double y1 = (*i)[0].y;
        double x2 = (*i)[1].x;
        double y2 = (*i)[1].y;
        double x4 = (*i)[3].x;
        double y4 = (*i)[3].y;

        double a1 = -WH.x*(y4-y1)/(x4*(y2-y1)+x1*(y4-y2)+x2*(y1-y4));
        double b1 = WH.x*(x4-x1)/(x4*(y2-y1)+x1*(y4-y2)+x2*(y1-y4));
        double c1 = -a1*x1-b1*y1;

        double a2 = -WH.y*(y2-y1)/(x2*(y4-y1)+x1*(y2-y4)+x4*(y1-y2));
        double b2 = WH.y*(x2-x1)/(x2*(y4-y1)+x1*(y2-y4)+x4*(y1-y2));
        double c2 = -a2*x1-b2*y1;

        for (int n=0; n<map.size(); n++){
          double xr = map[n].xr;
          double yr = map[n].yr;
          map[n].xr = a1*xr+b1*yr+c1;
          map[n].yr = a2*xr+b2*yr+c2;
        }
        // граница: если есть ломаная с комментарием "BRD <имя карты>" - 
        // сделаем границу из нее. Иначе - из размеров растрового файла.
        map.border.clear();
        fig_world::const_iterator j;
        for (j=w.begin();j!=w.end();j++){
          if (j->type!=2) continue;
          int nn = j->size();
          if (nn<3) continue;
          // если последняя точка совпадает с первой
          if ((*j)[0]==(*j)[nn-1]) {nn--; if (nn<3) continue;}
          string brd_comm;
          if ((j->comment.size()>0)&&(parse(j->comment[0].c_str(), str_p("BRD")
            >> !(+space_p >> (*anychar_p)[assign_a(brd_comm)])).full)&&(brd_comm==comm)){
            for (int n=0;n<nn;n++){
              map.border.push_back(g_point(
                a1*(*j)[n].x + b1*(*j)[n].y + c1, 
                a2*(*j)[n].x + b2*(*j)[n].y + c2
              ));
            }
          }
        }
        if (map.border.empty()){
          map.border.push_back(g_point(0,0));
          map.border.push_back(g_point(WH.x,0));
          map.border.push_back(g_point(WH.x,WH.y));
          map.border.push_back(g_point(0,WH.y));
        }

        d.maps.push_back(map);
      }
    }
  }

  // Добавить точки и треки в fig_world в соотв. с привязкой

#define ADDCOMM(x) {ostringstream s; s << x; f.comment.push_back(s.str());}

  void put_wpts(fig_world & F, const g_map & m, const geo_data & world){
    vector<g_waypoint_list>::const_iterator wl;
    vector<g_waypoint>::const_iterator w;
    convs::map2pt cnv(m, Datum("wgs84"), Proj("lonlat"), Options());
    for (wl=world.wpts.begin();wl!=world.wpts.end();wl++){
      for (w=wl->begin();w!=wl->end();w++){

        g_point p(w->x, w->y);
        g_waypoint def;
        cnv.bck(p);
  
	fig::fig_object f = fig::make_object("2 1 0 2 0 7 6 0 -1 1 1 1 -1 0 0 *"); 
                                             ADDCOMM("# WPT " << w->name);
        if (w->z   < 1e20)                   ADDCOMM("# alt:        " << fixed << setprecision(1) << w->z);
        if (w->t != def.t)                   ADDCOMM("# time:       " << w->t);
        if (w->comm != def.comm)             ADDCOMM("# comm:       " << w->comm);
        if (w->prox_dist != def.prox_dist)   ADDCOMM("# prox_dist:  " << fixed << setprecision(1) << w->prox_dist);
        if (w->symb != def.symb)             ADDCOMM("# symb:       " << wpt_symb_enum.int2str(w->symb));
        if (w->displ != def.displ)           ADDCOMM("# displ:      " << w->displ);
        if (w->color != def.color)           ADDCOMM("# color:      #" << setbase(16) << setw(6) << setfill('0') << w->color);
        if (w->bgcolor != def.bgcolor)       ADDCOMM("# bgcolor:    #" << setbase(16) << setw(6) << setfill('0') << w->bgcolor);
        if (w->map_displ != def.map_displ)   ADDCOMM("# map_displ:  " << wpt_map_displ_enum.int2str(w->map_displ));
        if (w->pt_dir != def.pt_dir)         ADDCOMM("# pt_dir:     " << wpt_pt_dir_enum.int2str(w->pt_dir));
        if (w->font_size != def.font_size)   ADDCOMM("# font_size:  " << w->font_size);
        if (w->font_style != def.font_style) ADDCOMM("# font_style: " << w->font_style);
        if (w->size != def.size)             ADDCOMM("# size:       "  << w->size);
	f.push_back(Point<int>(int(p.x), int(p.y)));
	F.push_back(f);

	fig::fig_object ft = fig::make_object("4 0 8 5 -1 18 6 0.0000 4");
	ft.push_back(Point<int>(int(p.x)+30, int(p.y)+30));
	ft.text = w->name;
	F.push_back(ft);
      }
    }

  }
  void put_trks(fig_world & F, const g_map & m, const geo_data & world){
    vector<g_track>::const_iterator tl;
    vector<g_trackpoint>::const_iterator t;
    convs::map2pt cnv(m, Datum("wgs84"), Proj("lonlat"), Options());
    g_track def;

    for (tl=world.trks.begin();tl!=world.trks.end();tl++){
      t=tl->begin();
      do {
        vector<Point<int> > pts;
        do{
          g_point p(t->x, t->y);
          cnv.bck(p);
          pts.push_back(Point<int>(int(p.x),int(p.y)));
          t++;
        } while ((t!=tl->end())&&(!t->start));
  
	fig::fig_object f = fig::make_object("2 1 0 1 1 7 7 0 -1 1 1 1 -1 0 0 *"); 
                                    ADDCOMM("# TRK " << tl->comm);
        if (tl->width != def.width) ADDCOMM("# width: "  << tl->width);
        if (tl->displ != def.displ) ADDCOMM("# displ: "  << tl->displ);
        if (tl->color != def.color) ADDCOMM("# color: #" << setbase(16) << setw(6) << setfill('0') << tl->color << setbase(10));
        if (tl->skip  != def.skip)  ADDCOMM("# skip:  "  << tl->skip);
        if (tl->type  != def.type)  ADDCOMM("# type:  "  << trk_type_enum.int2str(tl->type));
        if (tl->fill  != def.fill)  ADDCOMM("# fill:  "  << trk_fill_enum.int2str(tl->fill));
        if (tl->cfill != def.cfill) ADDCOMM("# cfill: #" << setbase(16) << setw(6) << setfill('0') << tl->cfill << setbase(10));

        for (vector<Point<int> >::const_iterator p1=pts.begin(); p1!=pts.end(); p1++) f.push_back(*p1);
	F.push_back(f);

      } while (t!=tl->end());
    }
  }

}
