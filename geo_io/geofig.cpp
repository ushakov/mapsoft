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

    // В комментарии к файлу может быть указано, в какой он проекции
    // default - tmerc
    string proj  = "tmerc";
    for (int n=0; n<w.comment.size(); n++){
      parse(w.comment[n].c_str(), str_p("proj:")  >> space_p >> (*anychar_p)[assign_a(proj)]); 
    }
    ret.map_proj=Proj(proj);

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

        convs::pt2pt c(Datum(O.get_string("datum","wgs84")), 
                       Proj(O.get_string("proj","lonlat")), O, Datum("wgs84"), Proj("lonlat"), O);
        c.frw(ref);
	ret.push_back(ref);
      }
    }
    // границы - довольно произвольные, чтобы не искали граф.файл.
    for (int i=0;i<3;i++) ret.border.push_back(g_point(0,0));
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
  void set_ref(fig_world & w, const g_map & m, const Options O){
    // Если хочется, можно записать точки привязки в нужной проекции
    Datum  datum(O.get_string("datum", "pulkovo"));
    Proj   proj(O.get_string("proj", "tmerc"));

    double lon0 = m.range().x + m.range().w/2;
    lon0 = floor( lon0/6.0 ) * 6 + 3;
    lon0 = O.get_double("lon0", lon0);

    convs::pt2pt cnv(datum,proj,O, Datum("wgs84"), Proj("lonlat"), Options());

    for (int n=0; n<m.size(); n++){
      fig::fig_object o = fig::make_object("2 1 0 4 4 7 1 -1 -1 0.000 0 1 -1 0 0 *");
      o.push_back(Point<int>( int(m[n].xr), int(m[n].yr) ));
      ostringstream comm;
      comm << "REF " << fixed << m[n].x << " " << m[n].y;
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
          >> +space_p >> (*anychar_p)[insert_at_a(pl,"comm")]).full)){
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

        g_map map(m);
        map.comm = comm;
        map.file = i->image_file;
        map.border.clear();
        for (int n=0; n<map.size(); n++){
          double xr = map[n].xr;
          double yr = map[n].yr;
          map[n].xr = a1*xr+b1*yr+c1;
          map[n].yr = a2*xr+b2*yr+c2;
        }
        d.maps.push_back(map);
      }
    }
  }

  // Добавить точки и треки в fig_world в соотв. с привязкой

  void set_wpts(fig_world & w, const g_map & m, const geo_data & world){
/*    vector<g_waypoint_list>::const_iterator wl;
    vector<g_waypoint>::const_iterator w;
    convs::map2pt cnv(m, Datum("wgs84"), Proj("lonlat"), Options());
    for (wl=world.wpts.begin();wl!=world.wpts.end();wl++){
      for (w=wl->begin();w!=wl->end();w++){

        fig_object f_pt;
        f_pt.pen_color=0;
        f_pt.depth=6;
        f_pt.width=2;
    
        fig_object f_txt;
        f_txt.pen_color=8;
        f_txt.depth=5;
        f_txt.font=18;
        f_txt.font_size=6;
      
        g_point p(w->x, w->y);
        g_waypoint def_pt;
        cnv.bck(p);
  
        
        out << "# WPT " << w->name << "\n";
        if (w->z   < 1e20)                      out << "# alt:        " << fixed << setprecision(1) << w->z << "\n";
        if (w->t != def_pt.t)                   out << "# time:       " << time2str(w->t) << "\n";
        if (w->comm != def_pt.comm)             out << "# comm:       " << w->comm << "\n";
        if (w->prox_dist != def_pt.prox_dist)   out << "# prox_dist:  " << fixed << setprecision(1) << w->prox_dist << "\n";
        if (w->symb != def_pt.symb)             out << "# symb:       " << wpt_symb_enum.int2str(w->symb) << "\n";
        if (w->displ != def_pt.displ)           out << "# displ:      " << w->displ << "\n";
        if (w->color != def_pt.color)           out << "# color:      #" << setbase(16) << setw(6) << setfill('0') << w->color <<
        if (w->bgcolor != def_pt.bgcolor)       out << "# bgcolor:    #" << setbase(16) << setw(6) << setfill('0') << w->bgcolor <      if (w->map_displ != def_pt.map_displ)   out << "# map_displ:  " << wpt_map_displ_enum.int2str(w->map_displ) << "\n";
        if (w->pt_dir != def_pt.pt_dir)         out << "# pt_dir:     " << wpt_pt_dir_enum.int2str(w->pt_dir) << "\n";
        if (w->font_size != def_pt.font_size)   out << "# font_size:  " << w->font_size << "\n";
        if (w->font_style != def_pt.font_style) out << "# font_style: " << w->font_style << "\n";
        if (w->size != def_pt.size)             out << "# size:       "  << w->size << "\n";
  
        out << "2 1 0 " << wpt_width << " " << wpt_color << " 7 " << wpt_depth
            << " 0 -1 1 1 1 -1 0 0 1\n\t"
            << int(p.x) << " " << int(p.y) << "\n";
  
        int td=0;
        // 15 = 1200/80
//                if (w->pt_dir == 0){tx=(int)p.x; ty=(int)p.y + 15*w->size; td=1;} // buttom
//                if (w->pt_dir == 1){tx=(int)p.x; ty=(int)p.y - 15*w->size - 15*w->font_size; td=1;} // top
//                if (w->pt_dir == 2){tx=(int)p.x + 15*w->size; ty=(int)p.y; td=0;} // left
//                if (w->pt_dir == 3){tx=(int)p.x - 15*w->size; ty=(int)p.y; td=2;} // right
//
        out << "4 " << td << " " << wpt_txt_color << " " << wpt_txt_depth
            << " -1 "<< wpt_txt_font << " " << wpt_txt_size << " 0.0000 4 105 150 "
            << int(p.x)+15*wpt_width << " " << int(p.y)+15*wpt_width << " " << w->name << "\\001\n";
      }
    }
*/
  }
  void set_trks(fig_world & w, const g_map & m, const geo_data & world){
/*    vector<g_track>::const_iterator tl;
    vector<g_trackpoint>::const_iterator t;
    convs::map2pt cnv(m, Datum("wgs84"), Proj("lonlat"), Options());
    for (tl=world.trks.begin();tl!=world.trks.end();tl++){
      int trk_color=1, trk_depth=60, trk_width=1;
  
      t=tl->begin();
      do {
        vector<Point<int> > pts;
        g_track def_t;
        do{
          g_point p(t->x, t->y);
          cnv.bck(p);
          pts.push_back(Point<int>(int(p.x),int(p.y)));
          t++;
        } while ((t!=tl->end())&&(!t->start));
  
        out << "# TRK " << tl->comm << "\n";
        if (tl->width != def_t.width) out << "# width: "  << tl->width << "\n";
        if (tl->displ != def_t.displ) out << "# displ: "  << tl->displ << "\n";
        if (tl->color != def_t.color) out << "# color: #" << setbase(16) << setw(6) << setfill('0') << tl->color << setbase(10) <<      if (tl->skip  != def_t.skip)  out << "# skip:  "  << tl->skip << "\n";
        if (tl->type  != def_t.type)  out << "# type:  "  << trk_type_enum.int2str(tl->type) << "\n";
        if (tl->fill  != def_t.fill)  out << "# fill:  "  << trk_fill_enum.int2str(tl->fill) << "\n";
        if (tl->cfill != def_t.cfill) out << "# cfill: #" << setbase(16) << setw(6) << setfill('0') << tl->cfill << setbase(10) <<
        out << "2 1 0 " << trk_width << " " << trk_color << " 7 " << trk_depth
            << " 0 -1 1 0 0 -1 0 0 " << pts.size() << "\n";
        for (vector<Point<int> >::const_iterator p1=pts.begin(); p1!=pts.end(); p1++)
          out << "\t" << p1->x << " " << p1->y << "\n";
      } while (t!=tl->end());
    }
*/
  }


}
