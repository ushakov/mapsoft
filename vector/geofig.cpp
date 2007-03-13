#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#include <boost/spirit/actor/insert_at_actor.hpp>

#include "fig.h"
#include "../geo_io/geo_convs.h"


namespace fig {

using namespace std;
using namespace boost::spirit;

  // Извлечь привязку из fig-картинки
  g_map get_map(const fig_world & w){
    g_map ret;

    // В комментарии к файлу может быть указано, в какой он проекции
    // default - tmerc
    string proj  = "tmerc";
    parse(w.comment.c_str(), 
      *( (str_p("# proj:")  >> space_p >> (*(anychar_p-eol_p))[assign_a(proj)]) | 
       (anychar_p-eol_p) >> eol_p));
    ret.map_proj=Proj(proj);

    fig_world::const_iterator i;
    for (i=w.begin();i!=w.end();i++){
      if (i->type!=2) continue; // ref in polylines only
      if (i->x.size()<1) continue;
      double x,y;
        string datum = "wgs84";
        string proj  = "latlon";
        string lon0  = "";
      if (parse(i->comment.c_str(), str_p("# REF") 
	  >> +space_p >> real_p[assign_a(x)]
	  >> +space_p >> real_p[assign_a(y)] >> eol_p 
  	  >> *(
            (str_p("# datum:") >> +space_p >> (*(anychar_p-eol_p))[assign_a(datum)]) |
	    (str_p("# proj:")  >> +space_p >> (*(anychar_p-eol_p))[assign_a(proj)]) |
	    (str_p("# lon0:")  >> +space_p >> (*(anychar_p-eol_p))[assign_a(lon0)]) |
	   *(anychar_p-eol_p)
	  >> eol_p )).full){
 	g_refpoint ref(x,y,i->x[0],i->y[0]);
        Options O; O["lon0"] = lon0;

        convs::pt2pt c(Datum(datum), Proj(proj), O, Datum("wgs84"), Proj("lonlat"), O);
        c.frw(ref);
	ret.push_back(ref);
      }
    }
    // границы - довольно произвольные, чтобы не искали граф.файл.
    for (int i=0;i<3;i++) ret.border.push_back(g_point(0,0));
    return ret;
  }


/*
  // Удалить старые точки привязки, если они есть,
  // сделать новые
  void  set_map(fig_world & w, const g_map & map){
    for (fig_world::iterator i=w.begin(); i!=w.end(); i++){
      if (parse(i->comment.c_str(), str_p("# REF") >> +space_p >> real_p >> +space_p >> real_p >> eol_p).full){
        fig_world::iterator j=i; j--; w.erase(i); i=j;
      }
    }
    for (g_map::const_iterator i=map.begin(); i!=map.end(); i++){
      fig_object o = make_object("2 1 0 4 4 7 1 -1 -1 0.000 0 1 -1 0 0 1");
      o.comment="# REF "
    }  
  }

  // пересчитать точки объекта в lon-lat wgs84
  vector<g_point> get_geo_crds(const fig_object & o, const g_map & map){
    vector<g_point> v1;
    if (i->x.size()!=i->y.size()){
      cerr << "fig::get_geo_crds: different amount of x and y values\n";
      return v1;
    }
    for (int n=0; n<i->x.size(); n++)
      v1.push_back(g_point(i->x[n],i->y[n]);
    convs::map2pt c(map, Datum("wgs84"), Proj("lonlat"), Options());
    return c.frw(v1);
  }

  // изменить точки объекта
  void set_geo_crds(fig_object & o, const g_map & map, const vector<g_point> crds){
  }
  // при пересчете линий могут добавляться новые точки!
*/

}
