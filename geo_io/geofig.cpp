#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include <sstream>
#include "fig.h"
#include "geo_convs.h"
#include "io_xml.h"
#include "../utils/mapsoft_options.h"

namespace fig {

using namespace std;
using namespace xml;
using namespace boost::spirit;

  // Извлечь привязку из fig-картинки
  g_map get_map(const fig_world & w){
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
      if (i->x.size()<1) continue;
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
 	g_refpoint ref(x,y,i->x[0],i->y[0]);

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
  void rem_map(fig_world & w){
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
  void set_map(fig_world & w, const g_map & m, const Options O){
    // Если хочется, можно записать точки привязки в нужной проекции
    Datum  datum(O.get_string("datum", "pulkovo"));
    Proj   proj(O.get_string("proj", "tmerc"));

    double lon0 = m.range().x + m.range().w/2;
    lon0 = floor( lon0/6.0 ) * 6 + 3;
    lon0 = O.get_double("lon0", lon0);

    convs::pt2pt cnv(datum,proj,O, Datum("wgs84"), Proj("lonlat"), Options());

    for (int n=0; n<m.size(); n++){
      fig::fig_object o = fig::make_object("2 1 0 4 4 7 1 -1 -1 0.000 0 1 -1 0 0 *");
      o.x.push_back( int(m[n].xr) );
      o.y.push_back( int(m[n].yr) );
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
  void get_wpts(const fig_world & w, geo_data & d){
    // сделаем привязку
    g_map map=get_map(w);
    convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"), Options());

    fig_world::const_iterator i;

    g_waypoint_list pl;

    for (i=w.begin();i!=w.end();i++){
      if ((i->type!=2)&&(i->type!=3)) continue;
      if (i->x.size()<1) continue;
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
        wp.x = i->x[0];
        wp.y = i->y[0];
        cnv.frw(wp);
        pl.push_back(wp);
      }
    }
        d.wpts.push_back(pl);
  }

  void get_trks(const fig_world & w, geo_data & d){
    // сделаем привязку
    g_map map=get_map(w);
    convs::map2pt cnv(map, Datum("wgs84"), Proj("lonlat"), Options());

    fig_world::const_iterator i;
    for (i=w.begin();i!=w.end();i++){
      if ((i->type!=2)&&(i->type!=3)) continue;
      if (i->x.size()<1) continue;
      xml_point_list pl;
      string comm;
      if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("TRK")
          >> +space_p >> (*anychar_p)[insert_at_a(pl,"comm")]).full)){
        string key;
        for (int n=1; n<i->comment.size(); n++){
          parse(i->comment[n].c_str(), (*(anychar_p-':'-space_p))[assign_a(key)] >>
            *space_p >> ch_p(':') >> *space_p >>
            (*(anychar_p-':'-space_p))[insert_at_a(pl, key)]);
        }
        g_track tr(pl);
        for (int n = 0; n<min(i->x.size(), i->y.size());n++){
          g_trackpoint p;
          p.x=i->x[n];
          p.y=i->y[n];
          cnv.frw(p);
          tr.push_back(p);
        }
        d.trks.push_back(tr);
      }
    }
  }

  // Добавить точки и треки в fig_world в соотв. с привязкой
  void set_wpts(fig_world & w, const g_map & m, const geo_data & d){
  }
  void set_trks(fig_world & w, const g_map & m, const geo_data & d){
  }


}
