#include <list>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "2d/line_utils.h"
#include "2d/line_rectcrop.h"
#include "geo_io/geofig.h"
#include "geo/geo_convs.h"
#include "geo/geo_nom.h"
#include "vmap/zn.h"
#include "vmap.h"

namespace vmap {

using namespace std;

bool
skip_object(const Options & O, const object &o){
  if (O.get<int>("skip_all", 0)) return true;

  if (O.exists("select_type") && (o.type!=O.get<int>("select_type", 0))) return true;
  if (O.exists("skip_type") && (o.type==O.get<int>("skip_type", 0))) return true;

  string source = o.opts.get<string>("Source");
  if (O.exists("select_source") && (source!=O.get<string>("select_source"))) return true;
  if (O.exists("skip_source") && (source==O.get<string>("skip_source"))) return true;
  return false;
}

// remove empty lines and objects
void
remove_empty(world & W){
  world::iterator o = W.begin();
  while (o!=W.end()){
    dMultiLine::iterator l = o->begin();
    while (l != o->end()){
      if (l->size()==0) l=o->erase(l);
      else l++;
    }
    if (o->size()==0) o=W.erase(o);
    else o++;
  }
}

void
remove_dups(world & W, double dist){
  world::iterator oi;
  dMultiLine::iterator li;
  for (oi=W.begin(); oi!=W.end(); oi++){
    for (li=oi->begin(); li!=oi->end(); li++){
      if (li->size()<1) continue;
      dLine::iterator p1=li->begin(), p2=p1+1;
      while (p2!=li->end()){
        if (pdist(*p1,*p2) < dist) p2=li->erase(p2);
        else { p1++; p2++;}
      }
    }
  }
}

void
remove_tails(world & W, double dist, const dRect & cutter, Conv * cnv){
  if (dist<=0) return;

  world::iterator o;
  for (o=W.begin(); o!=W.end(); o++){
    dMultiLine::iterator l;
    for (l = o->begin(); l != o->end(); l++){
      if (l->size()<1) continue;
      // convert line to cutter coords
      dLine lc = cnv? cnv->line_frw(*l) : *l;

      // проверка, что все точки l лежат близко к границе
      bool close_to_brd = true;
      for (dLine::const_iterator pc=lc.begin(); pc!=lc.end(); pc++){
        if ((fabs(pc->x - cutter.x) > dist) &&
            (fabs(pc->y - cutter.y) > dist) &&
            (fabs(pc->x - cutter.x - cutter.w) > dist) &&
            (fabs(pc->y - cutter.y - cutter.h) > dist)){
          close_to_brd=false;
          break;
        }
      }

      // проверка, что есть другой такой объект, близко к сторонам
      // которого лежат все точки l
      bool close_to_obj = false;
      world::const_iterator o1;
      for (o1=W.begin(); !close_to_obj && (o1!=W.end()); o1++){
        // only objects with the same type and text.
        if ((o->type != o1->type) || (o->text != o1->text)) continue;
        dMultiLine::const_iterator l1;
        for (l1 = o1->begin(); !close_to_obj && (l1!=o1->end()); l1++){
          // skip l==l1
          if ((o==o1) && (l==l1)) continue;

          bool found=true;
          dLine l1c = cnv? cnv->line_frw(*l1) : *l1;
          for (dLine::const_iterator p=lc.begin(); p!=lc.end(); p++){
            dPoint v, pp(*p);

            if (nearest_pt(l1c, v, pp, dist) >= dist){
              found=false;
              break;
            }
          }
          if (found) close_to_obj=true;
        }
      }

      if (close_to_brd && close_to_obj) l->clear();
    }
  }
  remove_empty(W);
}

// crop/select/skip
void range_action(world & W, string action, const dRect & cutter, Conv * cnv){
  world::iterator o;
  for (o=W.begin(); o!=W.end(); o++){
    bool closed = (o->get_class() == POLYGON);
    dMultiLine::iterator l;

    for (l = o->begin(); l != o->end(); l++){
      if (l->size()<1) continue;
      dLine lc = cnv ? cnv->line_frw(*l) : *l;
      rect_crop(cutter, lc, closed);

      if (action == "skip"){
        if (lc.size()!=0) l->clear();
      }
      else if (action == "select"){
        if (lc.size()==0) l->clear();
      }
      else if (action == "crop"){
        *l = cnv? cnv->line_bck(lc) : lc;
      }
      else if (action == "crop_spl"){
        if (o->get_class()==POLYLINE){
          dMultiLine ML = rect_split_cropped(cutter, lc);
          for (dMultiLine::const_iterator i=ML.begin(); i!=ML.end(); i++){
            l = o->insert(l, cnv? cnv->line_bck(*i) : *i) + 1;
          }
          l->clear();
        }
        else{
           *l = cnv? cnv->line_bck(lc) : lc;
        }
      }
    }
  }
  remove_empty(W);
}

bool join_two_lines(const dLine & l1, const dLine & l2, dLine & res, const double dist){
  if (pdist(*(l1.rbegin()), *(l2.begin()))<dist){
    res.insert(res.end(), l1.begin(), l1.end());
    res.insert(res.end(), l2.begin()+1, l2.end());
    return true;
  }
  return false;
}

bool join_two_polygons(const dLine & l1, const dLine & l2, dLine & res, const double dist){
  dLine::const_iterator p1a,p1b,p2a,p2b;
  for (p1a = l1.begin(); p1a!=l1.end(); p1a++){
    p1b=p1a+1;
    if (p1b==l1.end()) p1b=l1.begin();

    for (p2a = l2.begin(); p2a!=l2.end(); p2a++){
      p2b=p2a+1;
      if (p2b==l2.end()) p2b=l2.begin();

      if ((pdist(*p1a,*p2b) < dist) && (pdist(*p1b,*p2a) < dist) &&
          (pdist(*p1a,*p1b) > dist) && (pdist(*p2a,*p2b) > dist)){
        res.insert(res.end(), l1.begin(), p1a);
        if (p2b!=l2.begin()) res.insert(res.end(), p2b, l2.end());
        res.insert(res.end(), l2.begin(), p2a);
        if (p1b!=l1.begin()) res.insert(res.end(), p1b, l1.end());
        return true;
      }

    }
  }
  return false;
}

void
join_objects(world & W, double dist){
  if (dist<=0) return;

  // TODO: join options and comments?

  // Идем по всем упорядоченным парам линий o1->l1, o2->l2.
  // Точками не интересуемся.
  // Для склейки должны совпадать тип и название объекта.

  // Склейка линий происходит только с сохранением направления.
  // При склейке мы помещаем результат в первую линию, удаляем вторую линию
  // (которая расположена где-то дальше) и возвращаем o2->l2 на o1->l1 (чтоб
  // можно было к уже сделанной линии подклеить что-то еще.

  // При склейке многоугольников находятся и склеиваются пары соседних сегментов.
  // Склеиваются только многоугольники с одинаковой ориентацией (либо по, либо против ч.с.)
  // После успешной склейки многоугольников требуется еще один проход, чтобы
  // отловить вещи, типа пересекающихся разрезов. (Подумать, как это оптимизировать!
  // Отделить ли циклы для линий и для многоугольников?)
  // Явная проблема: многоугольники, разрезанные на более двух (соединенных) частей

  world::iterator o1,o2;
  dMultiLine::iterator l1, l2;
  int lc=0,pc=0,pass=0;
  bool repeat;
  do {
    repeat=false; pass++;
    for (o1=W.begin(); o1!=W.end(); o1++){
      if (o1->get_class()==POI) continue;
      for (l1 = o1->begin(); l1 != o1->end(); l1++){
        if (l1->size()<1) continue;

        for (o2 = o1; o2 != W.end(); o2++){
          if (o1->type != o2->type) continue;
          if (o1->text != o2->text) continue;
          if ((o1->get_class()==POLYLINE) && (o1->dir != o2->dir)) continue;

          for (l2 = (o1==o2 ? l1+1 : o2->begin()); l2 != o2->end(); l2++){
            if (l2->size()<1) continue;

            // join lines
            if (o1->get_class()==POLYLINE){
              dLine tmp;
              if (join_two_lines(*l1, *l2, tmp, dist) ||
                  join_two_lines(*l2, *l1, tmp, dist)){
                l1->swap(tmp);
                o2->erase(l2);
                o2=o1;
                l2=l1;
                lc++;
              }
            }

            // join polygons
            else if (o1->get_class()==POLYGON){
              dLine tmp;
              if (join_two_polygons(*l1, *l2, tmp, dist)){
                l1->swap(tmp);
                o2->erase(l2);
                o2=o1;
                l2=l1;
                pc++;
                repeat=true;
              }
            }

          }
        }
      }
    }
  } while (repeat);
std::cerr << "Join: " << lc << " lines, " 
  << pc << " polygons, " << pass << " passes\n";
}

struct pt_in_cell: public dPoint{
  int type;
  pt_in_cell(const dPoint & p, const int t):dPoint(p), type(t){}
};

void
fix_diff(const world & WOLD, world & WNEW, double dist){
  // Плоскость разбивается на ячейки 4d x 4d с шагом 2d.
  // Ячейка задается центральной точкой.
  // В каждую ячейку помещаются точки из старой карты, которые в нее попадают.
  // Для точки новой карты ищется наиболее подходящая ячейка (округление до 2d)
  // Выбирается ближайшая точка того же типа, если расстояние до нее меньше d,
  // то координаты точки из новой карты меняются на координаты точки из старой.

  double cell_step=2.0*dist;

  multimap<iPoint, pt_in_cell> cells;

  world::const_iterator oci;
  dMultiLine::const_iterator lci;
  dLine::const_iterator pci;

  for (oci=WOLD.begin(); oci!=WOLD.end(); oci++){
    for (lci=oci->begin(); lci!=oci->end(); lci++){
      for (pci=lci->begin(); pci!=lci->end(); pci++){
        pt_in_cell p(*pci, oci->type);
        int x1 = int(floor(p.x / cell_step));
        int x2 = int(ceil(p.x / cell_step));
        int y1 = int(floor(p.y / cell_step));
        int y2 = int(ceil(p.y / cell_step));
        for (int x=x1; x<=x2; x++){
          for (int y=y1; y<=y2; y++){
            cells.insert(make_pair(iPoint(x,y), p));
          }
        }
      }
    }
  }

  world::iterator oi;
  dMultiLine::iterator li;
  dLine::iterator pi;

  for (oi=WNEW.begin(); oi!=WNEW.end(); oi++){
    for (li=oi->begin(); li!=oi->end(); li++){
      for (pi=li->begin(); pi!=li->end(); pi++){
        iPoint cell(int(round(pi->x / cell_step)),
                    int(round(pi->y / cell_step)));

        multimap<iPoint, pt_in_cell>::const_iterator ci, ci_min=cells.end();
        double min_dist = 2*dist;
        for (ci=cells.lower_bound(cell); ci!=cells.upper_bound(cell); ci++){
          if (oi->type!=ci->second.type) continue;
          if (pdist(ci->second, *pi) < min_dist){
            ci_min = ci;
            min_dist=pdist(ci->second, *pi);
          }
        }
        if (min_dist < dist) *pi = ci_min->second;
      }
    }
  }

  // the same for lbuf
  cells.clear();
  list<lpos_full>::const_iterator lbci;

  for (lbci=WOLD.lbuf.begin(); lbci!=WOLD.lbuf.end(); lbci++){
    for (int t = 0; t<2; t++){
      pt_in_cell p(t==0 ? lbci->pos:lbci->ref, t);
      int x1 = int(floor(p.x / cell_step));
      int x2 = int(ceil(p.x / cell_step));
      int y1 = int(floor(p.y / cell_step));
      int y2 = int(ceil(p.y / cell_step));
      for (int x=x1; x<=x2; x++){
        for (int y=y1; y<=y2; y++){
          cells.insert(make_pair(iPoint(x,y), p));
        }
      }
    }
  }

  list<lpos_full>::iterator lbi;

  for (lbi=WNEW.lbuf.begin(); lbi!=WNEW.lbuf.end(); lbi++){
    for (int t = 0; t<2; t++){
      dPoint p(t==0 ? lbi->pos:lbi->ref);
      iPoint cell(int(round(p.x / cell_step)),
                  int(round(p.y / cell_step)));

      multimap<iPoint, pt_in_cell>::const_iterator ci, ci_min=cells.end();
      double min_dist = 2*dist;
      for (ci=cells.lower_bound(cell); ci!=cells.upper_bound(cell); ci++){
        if (t!=ci->second.type) continue;
        if (pdist(ci->second, p) < min_dist){
          ci_min = ci;
          min_dist=pdist(ci->second, p);
        }
      }
      if (min_dist < dist){
        if (t==0) lbi->pos = ci_min->second;
        else      lbi->ref = ci_min->second;
      }
    }
  }
}


// crop/cut/select range, get statistics
struct RangeCutter{
  convs::pt2pt *cnv;
  int cnt_o, cnt_l, cnt_p;
  set<string> sources;
  dRect show_range;
  double tail_size;

  double lon0;
  Datum D;
  Proj P;
  dRect range;
  std::string action;
  dLine newbrd;

  RangeCutter(const Options & O){
    P = O.get<Proj>("range_proj", Proj("lonlat"));
    D = O.get<Datum>("range_datum", Datum("wgs84"));
    lon0 = O.get<double>("range_lon0", 0);
    range = O.get<dRect>("range");
    action = O.get<string>("range_action");


    cnv=NULL;

    if (action == "help"){
      std::cout << "range actions:\n"
                << "  skip     -- skip   objects, touching range\n"
                << "  select   -- select objects, touching range (complementary to skip)\n"
                << "  crop     -- crop\n"
                << "  crop_spl -- smartly crop lines to minimize extra segments on the borders\n"
                << "  help     -- show this help\n"
      ;
      return;
    }

    if ((action != "") &&
        (action != "skip") &&
        (action != "select") &&
        (action != "crop") &&
        (action != "crop_spl")){
      std::cerr << "warning: skipping unknown range action: " << action << "\n"
                << "         see --range_action help\n"
      ;
      return;
    }

    if (O.exists("range_nom")){
      range=convs::nom_to_range(O.get<string>("range_nom"));
      if (range.empty()){
        std::cerr << "bad name: " << O.get<string>("range_nom") << "\n";
        exit(1);
      }
      cnv = new convs::pt2pt(
          Datum("wgs84"), Proj("lonlat"), Options(),
          Datum("pulkovo"), Proj("lonlat"), Options());
    }
    else {
      Options ProjO;
      ProjO.put<double>("lon0", lon0);
      if ((P == Proj("tmerc")) && (range.x>=1e6)){
        double lon0p = convs::lon_pref2lon0(range.x);
        range.x = convs::lon_delprefix(range.x);
        ProjO.put<double>("lon0", lon0p);;
      }
      if (!range.empty())
        cnv = new convs::pt2pt(
          Datum("wgs84"), Proj("lonlat"), Options(), D, P, ProjO);
    }
    if ((cnv==NULL) && (action!="")){
      std::cerr << "warning: can't set geo conversion for range_action="
                << action << "\n";
      std::cerr << "         skipping action\n";
    }

    if (O.get<int>("set_brd_from_range", 0) && cnv && !range.empty()){
      newbrd=cnv->line_bck(rect2line(range), 1e-6);
      newbrd.resize(newbrd.size()-1);
    }

    tail_size = O.get<double>("remove_tails", 0);

    cnt_o=0; cnt_l=0; cnt_p=0;
  }

  ~RangeCutter(){
    if (cnv)   delete cnv;
  }

  void
  process(world & W){
    if ((range.empty()) || (action == "")) return;

    // do range action
    range_action(W, action, range, cnv);

    // remove tails and clear empty lines
    if (tail_size > 0) remove_tails(W, tail_size, range, cnv);
  }

};



/***************************************/

void
filter(world & W, const Options & O){

  // OPTION name
  if (O.exists("name")) W.name = O.get<string>("name");

  // OPTION mp_id
  if (O.exists("mp_id")){
    int mp_id = O.get<int>("mp_id", 0);
    if (mp_id<= 0){
      cerr << "error: bad mp_id value: " << O.get<string>("mp_id") << "\n";
      exit(1);
    }
    W.mp_id  = mp_id;
  }

  // OPTION rscale
  if (O.exists("rscale")){
    double rscale = O.get<double>("rscale", 0.0);
    if (rscale <= 0){
      cerr << "error: bad rscale value: " << O.get<string>("rscale") << "\n";
      exit(1);
    }
    W.rscale = rscale;
  }

  // OPTION style
  if (O.exists("style")){
    string style = O.get<string>("style");
    if (style == ""){
      cerr << "error: empty style value\n";
      exit(1);
    }
    W.style  = style;
  }

  // OPTION set_brd
  if (O.exists("set_brd")){
    W.brd = O.get<dLine>("set_brd");
  }

  // OPTION skip_labels 0
  // OPTION read_labels
  // OPTION split_labels
  int do_skip_labels = O.get<int>("skip_labels", 0);
  if (O.get<int>("read_labels", 0)) do_skip_labels=0;
  int do_split_labels = O.get<int>("split_labels", 0);

  if (do_skip_labels)  remove_labels(W);
  if (do_split_labels) split_labels(W);

  // OPTION range_datum lonlat
  // OPTION range_proj  wgs84
  // OPTION range_lon0  0
  // OPTION range
  // OPTION range_nom
  // OPTION range_action ""
  // OPTION set_brd_from_range 0
  // OPTION remove_tails
  RangeCutter RC(O);
  if (RC.newbrd.size()>0) W.brd=RC.newbrd;

  world::iterator o=W.begin();
  while (o!=W.end()){
    // OPTION skip_all
    // OPTION skip_type
    // OPTION select_type
    // OPTION select_source
    // OPTION skip_source
    if (skip_object(O, *o)){ o=W.erase(o); continue; }

    // OPTION set_source_from_name
    // OPTION set_source
    string source = o->opts.get<string>("Source"); // from original object
    if (O.get<int>("set_source_from_name", 0)) source = W.name; // from map name
    set_source(o->opts, O.get("set_source", source));
    o++;
  }
    // crop, skip, select range
  RC.process(W);
}

} // namespace
