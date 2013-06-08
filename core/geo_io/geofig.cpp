#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_insert_at_actor.hpp>

#include <sstream>
#include <iomanip>
#include "geofig.h"
#include "geo/geo_convs.h"
#include "options/options.h"
#include "loaders/image_r.h"

namespace fig {

using namespace std;
using namespace boost::spirit::classic;

// Get geo-reference from fig
g_map
get_ref(const fig_world & w){
  g_map ret;
  fig::fig_object brd; // border

  string proj="tmerc"; // tmerc is the default proj

  // old-style options -- to be removed
  for (int n=0; n<w.comment.size(); n++){
    parse(w.comment[n].c_str(), str_p("proj:")  >> space_p >> (*anychar_p)[assign_a(proj)]);
  }

  Options O = w.opts;
  if (!O.exists("map_proj")) O.put<string>("map_proj", proj);

  dPoint min(1e99,1e99), max(-1e99,-1e99);
  fig_world::const_iterator i;
  for (i=w.begin();i!=w.end();i++){
    if ((i->type!=2)&&(i->type!=3)) continue;
    if (i->size()<1) continue;
    double x,y;
    if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("REF") 
	  >> +space_p >> real_p[assign_a(x)]
	  >> +space_p >> real_p[assign_a(y)]).full)){

      Options O=i->opts;
      string key;

      // old-style options -- to be removed
      for (int n=1; n<i->comment.size(); n++){
        parse(i->comment[n].c_str(), (*(anychar_p-':'-space_p))[assign_a(key)] >> 
          *space_p >> ch_p(':') >> *space_p >> 
          (*(anychar_p-':'-space_p))[insert_at_a(O, key)]);
        cerr << "Geofig: old-style option: " << key << ": " << O[key] << "\n";
      }

      O.put<double>("x", x);
      O.put<double>("y", y);
      O.put<double>("xr", (*i)[0].x);
      O.put<double>("yr", (*i)[0].y);
      g_refpoint ref;
      ref.parse_from_options(O);
      ret.push_back(ref);
      continue;
    }

    if ((i->comment.size()>0)&&(i->comment[0].size()>3) &&
        (i->comment[0].compare(0,3,"BRD")==0)) ret.border = *i; 

  }

  // set lon0 if it id not set
  if (O.get<Proj>("map_proj") == Proj("tmerc") &&
     !O.exists("lon0")){
    O.put<double>("lon0", convs::lon2lon0(ret.center().x));
  }

  ret.parse_from_options(O, false); // new-style options

  return ret;
}

void
rem_ref(fig_world & w){
  vector<string>::iterator i = w.comment.begin();
  while (i!=w.comment.end()){
    if (i->find("map_proj",0)==0) i=w.comment.erase(i);
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


void
set_ref(fig_world & w, const g_map & m, const Options & O){

  double lon0 = m.range().x + m.range().w/2;
  lon0 = floor( lon0/6.0 ) * 6 + 3;
  lon0 = m.proj_opts.get("lon0", lon0);
  Enum::output_fmt=Enum::xml_fmt;

  // Если хочется, можно записать точки привязки в нужной проекции
  string pproj=O.get<string>("proj", "lonlat");
  string pdatum=O.get<string>("datum", "wgs84");
  convs::pt2wgs cnv(Datum(pdatum), Proj(pproj), O);

  for (int n=0; n<m.size(); n++){
    fig::fig_object o = fig::make_object("2 1 0 4 4 7 1 -1 -1 0.000 0 1 -1 0 0 *");
    o.push_back(iPoint( int(m[n].xr), int(m[n].yr) ));
    dPoint p(m[n]); cnv.bck(p);
    Enum::output_fmt=Enum::xml_fmt;
    ostringstream comm;
    comm << "REF " << fixed << p.x << " " << p.y;
    o.comment.push_back(comm.str()); comm.str("");
    o.opts=O;
    w.push_back(o);
  }
  // добавим в заголовок fig-файла информацию о проекции.
  w.opts.insert(m.proj_opts.begin(), m.proj_opts.end());
  w.opts.put("map_proj", m.map_proj);
}


void
get_wpts(const fig_world & w, const g_map & m, geo_data & d){
  // сделаем привязку
  convs::map2wgs cnv(m);

  fig_world::const_iterator i;

  g_waypoint_list pl;

  for (i=w.begin();i!=w.end();i++){
    if ((i->type!=2)&&(i->type!=3)) continue;
    if (i->size()<1) continue;

    Options O=i->opts;

    if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("WPT")
        >> +space_p >> (*anychar_p)[insert_at_a(O,"name")]).full)){

      string key;

      // old-style options -- to be removed
      for (int n=1; n<i->comment.size(); n++){
        // Удалил ограничение не позволявшее использовать ' ' и ':' в
        // комментариях. Тим
        parse(i->comment[n].c_str(), (*(anychar_p-':'-space_p))[assign_a(key)] >>
          *space_p >> ch_p(':') >> *space_p >>
          (*(anychar_p))[insert_at_a(O, key)]);
      }

      g_waypoint wp;
      wp.parse_from_options(O);
      wp.x=(*i)[0].x; wp.y=(*i)[0].y;
      cnv.frw(wp);
      pl.push_back(wp);
    }
  }
  d.wpts.push_back(pl);
}


void
get_trks(const fig_world & w, const g_map & m, geo_data & d){
  convs::map2wgs cnv(m);

  fig_world::const_iterator i;
  for (i=w.begin();i!=w.end();i++){
    if ((i->type!=2)&&(i->type!=3)) continue;
    if (i->size()<1) continue;


    Options O=i->opts;

    if ((i->comment.size()>0)&&(parse(i->comment[0].c_str(), str_p("TRK")
        >> !(+space_p >> (*anychar_p)[insert_at_a(O,"comm")])).full)){
      string key;

      // old-style options -- to be removed
      for (int n=1; n<i->comment.size(); n++){
        parse(i->comment[n].c_str(), (*(anychar_p-':'-space_p))[assign_a(key)] >>
          *space_p >> ch_p(':') >> *space_p >>
          (*(anychar_p-':'-space_p))[insert_at_a(O, key)]);
      }

      g_track tr;
      tr.parse_from_options(O);
      for (int n = 0; n<i->size();n++){
        g_trackpoint p;
        p.x=(*i)[n].x; p.y=(*i)[n].y;
        cnv.frw(p);
        tr.push_back(p);
      }
      d.trks.push_back(tr);
    }
  }
}


void
get_maps(const fig_world & w, const g_map & m, geo_data & d){
  convs::map2wgs cnv(m);
  fig_world::const_iterator i;
  g_map_list maplist;

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
      iPoint WH = image_r::size(i->image_file.c_str());
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
            map.border.push_back(dPoint(
              a1*(*j)[n].x + b1*(*j)[n].y + c1, 
              a2*(*j)[n].x + b2*(*j)[n].y + c2
            ));
          }
        }
      }
      if (map.border.empty()){
        map.border.push_back(dPoint(0,0));
        map.border.push_back(dPoint(WH.x,0));
        map.border.push_back(dPoint(WH.x,WH.y));
        map.border.push_back(dPoint(0,WH.y));
      }

      maplist.push_back(map);
    }
  }
  d.maps.push_back(maplist);
}


void
rem_wpts(fig_world & w){
  fig_world::iterator obj = w.begin();
  while (obj!=w.end()){
    if ((obj->comment.size()>0) &&
        (obj->comment[0].find("WPT ",0)==0))
      obj=w.erase(obj);
    else obj++;
  }
}


void
rem_trks(fig_world & w){
  fig_world::iterator obj = w.begin();
  while (obj!=w.end()){
    if ((obj->comment.size()>0) &&
        (obj->comment[0].find("TRK ",0)==0))
      obj=w.erase(obj);
    else obj++;
  }
}


void
rem_maps(fig_world & w){
  fig_world::iterator obj = w.begin();
  while (obj!=w.end()){
    if ((obj->comment.size()>0) &&
        (obj->comment[0].find("MAP ",0)==0))
      obj=w.erase(obj);
    else obj++;
  }
}

void
rem_brds(fig_world & w){
  fig_world::iterator obj = w.begin();
  while (obj!=w.end()){
    if ((obj->comment.size()>0) &&
        (obj->comment[0].find("BRD ",0)==0))
      obj=w.erase(obj);
    else obj++;
  }
}


#define ADDCOMM(x) {ostringstream s; s << x; f.comment.push_back(s.str());}

void
put_wpts(fig_world & F, const g_map & m, const geo_data & world){
  vector<g_waypoint_list>::const_iterator wl;
  vector<g_waypoint>::const_iterator w;
  convs::map2wgs cnv(m);
  Enum::output_fmt=Enum::xml_fmt;

  for (wl=world.wpts.begin();wl!=world.wpts.end();wl++){
    for (w=wl->begin();w!=wl->end();w++){

      dPoint p(w->x, w->y);
      cnv.bck(p);

	fig::fig_object f = fig::make_object("2 1 0 2 0 7 6 0 -1 1 1 1 -1 0 0 *");
      ADDCOMM("WPT " << w->name);
      f.opts=to_options_skipdef(*w);
      f.opts.erase("name");
      f.opts.erase("lon");
      f.opts.erase("lat");
	f.push_back(iPoint(int(p.x), int(p.y)));
	F.push_back(f);

	fig::fig_object ft = fig::make_object("4 0 8 5 -1 18 6 0.0000 4");
	ft.push_back(iPoint(int(p.x)+30, int(p.y)+30));
	ft.text = w->name;
	F.push_back(ft);
    }
  }

}


void
put_trks(fig_world & F, const g_map & m, const geo_data & world){
  vector<g_track>::const_iterator tl;
  vector<g_trackpoint>::const_iterator t;
  convs::map2wgs cnv(m);
  Enum::output_fmt=Enum::xml_fmt;
  g_track def;

  for (tl=world.trks.begin();tl!=world.trks.end();tl++){
    t=tl->begin();
    do {
      vector<iPoint> pts;
      do{
        dPoint p(t->x, t->y);
        cnv.bck(p);
        pts.push_back(iPoint(int(p.x),int(p.y)));
        t++;
      } while ((t!=tl->end())&&(!t->start));

      fig::fig_object f = fig::make_object("2 1 0 1 1 7 7 0 -1 1 1 1 -1 0 0 *"); 
      ADDCOMM("TRK " << tl->comm);
      f.opts=to_options_skipdef(*tl);
      f.opts.erase("comm");

      for (vector<iPoint>::const_iterator p1=pts.begin(); p1!=pts.end(); p1++) f.push_back(*p1);
	F.push_back(f);

    } while (t!=tl->end());
  }
}

} // namespace
