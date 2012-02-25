#include "geo_convs.h"
#include <cmath>
#include <cstring>
#include <sstream>
#include <cassert>
#include <map>

#include "2d/line_utils.h"

#include "loaders/image_r.h" // определение размеров картинки (image_r::size)

namespace convs{
using namespace std;

/*******************************************************************/

/// Create PROJ4 projection object from our D, P and options.
projPJ
mkproj(const Datum & D, const Proj & P, const Options & o){

    int old_enum_fmt=Enum::output_fmt;
    Enum::output_fmt = Enum::proj_fmt;

    ostringstream projpar;

    if (P==Proj("google"))  projpar << " +proj=merc";
    else  projpar << " +proj=" << P;

    if (D==Datum("pulkovo"))
       projpar << " +ellps=krass +towgs84=+28,-130,-95";
    else if (D==Datum("sphere"))
       projpar << " +ellps=sphere";
    else if (D==Datum("google_sphere"))
       projpar << " +a=6378137 +b=6378137 +no_defs";
    else projpar << " +datum=" << D;

    Enum::output_fmt = old_enum_fmt;

    switch (P.val){

	case 0: // lonlat
          break;

	case 1: // tmerc
          if (o.count("lon0")==0){
            std::cerr << "mkproj: can't create proj: \""
                      << projpar.str() << "\" without lon0\n";
            exit(1);
          }
          projpar << " +lon_0=" << o.get("lon0", 0.0);
          projpar << " +lat_0=" << o.get("lat0", 0.0);
          projpar << " +k="     << o.get("k",    1.0);
          projpar << " +x_0="   << o.get("E0",   500000.0);
          projpar << " +y_0="   << o.get("N0",   0.0);
          break;

        case 3: // merc
          projpar << " +lon_0="  << o.get("lon0", 0.0);
          projpar << " +lat_ts=" << o.get("lat0", 0.0);
          projpar << " +x_0="    << o.get("E0",   0.0);
          projpar << " +y_0="    << o.get("N0",   0.0);
          break;

	case 4: // google
          break;

        default:
          std::cerr << "unknown proj: " << P.val << "\n";
          exit(1);
    }

    projPJ ret=pj_init_plus(projpar.str().c_str());
    if (!ret){
        std::cerr << "mkproj: Error: can't create proj: \"" << projpar.str() << "\"\n";
        exit(1);
    }
    return ret;
}

void
pjconv(const projPJ P1, const projPJ P2, dPoint & p){
  double z=0;
  if (pj_is_latlong(P1)) p*=M_PI/180.0;
  pj_transform(P1, P2, 1, 1, &p.x, &p.y, &z);
  if (pj_is_latlong(P2)) p*=180.0/M_PI;
}

/*******************************************************************/

pt2pt::pt2pt(const Datum & sD, const Proj & sP, const Options & sPo,
             const Datum & dD, const Proj & dP, const Options & dPo){
  pr_src = mkproj(sD, sP, sPo);
  pr_dst = mkproj(dD, dP, dPo);
  refcounter   = new int;
  *refcounter  = 1;
}

pt2pt::pt2pt(const pt2pt & other){
  copy(other);
}

pt2pt &
pt2pt::operator=(const pt2pt & other){
  if (this != &other){
    destroy();
    copy(other);
  }
  return *this;
}

pt2pt::~pt2pt(){
  destroy();
}

void
pt2pt::copy(const pt2pt & other){
   refcounter = other.refcounter;
   pr_src = other.pr_src;
   pr_dst = other.pr_dst;
   (*refcounter)++;
   assert(*refcounter >0);
}

void
pt2pt::destroy(void){
  (*refcounter)--;
  if (*refcounter<=0){
    delete refcounter;
    pj_free(pr_src);
    pj_free(pr_dst);
  }
}

void
pt2pt::frw(dPoint & p) const{
  pjconv(pr_src, pr_dst, p);
}

void
pt2pt::bck(dPoint & p) const{
  pjconv(pr_dst, pr_src, p);
}


/*******************************************************************/

map2pt::map2pt(const g_map & sM,
               const Datum & dD, const Proj & dP, const Options & dPo){

// Считается, что преобразование СК и замена осевого меридиана - линейны в пределах карты.
// Проекция карты берется из sM. Системы координат и параметры проекции
// (вроде lon0) берутся из dD и dPo.
//
// Практичекое следствие: если мы хотим работать с к-л определенным осевым
// меридианом - мы должны запихать его в dPo! (даже, если мы преобразуем из 
// карты с lon0 в lonlat!)
// преобразовать из карты с одним ос.м. в проекцию с другим - нельзя 
// (у нас же все линейно :)), надо использовать два последовательных преобразования...

// у нас точки привязки в lon-lat wgs84, карта нарисована в некоторой другой проекции
// а получить мы хотим третью проекцию.
// При этом в какой СК нарисована карта и какие параметры проекции
// используются - нам не важно - это станет частью лин.преобразования!

  // projection for reference points
  pr_ref = mkproj(Datum("WGS84"), Proj("lonlat"), Options()); // for ref points

  // destination projection
  pr_dst = mkproj(dD, dP, dPo);

  // "map" projection
  if (sM.map_proj == dP)
    pr_map = pr_dst;
  else
    pr_map = mkproj(dD, sM.map_proj, map_popts(sM));

  refcounter   = new int;
  *refcounter  = 1;

  // А теперь следите за руками...
  // Преобразуем (c подменой параметров) точки привязки в те координаты,
  // в которых карта линейна и найдем соответствующее линейное преобразование.
  map<dPoint,dPoint> points;
  for (g_map::const_iterator i=sM.begin(); i!=sM.end(); i++){
    dPoint pr(i->xr, i->yr), pl(*i);
    pjconv(pr_ref, pr_map, pl);
    points[pr] = pl;
  }
  lin_cnv.set_from_ref(points);
}

map2pt::map2pt(const map2pt & other){
  copy(other);
}

map2pt &
map2pt::operator=(const map2pt & other){
  if (this != &other){
    destroy();
    copy(other);
  }
  return *this;
}

map2pt::~map2pt(){
  destroy();
}

void
map2pt::copy(const map2pt & other){
   refcounter = other.refcounter;
   pr_ref = other.pr_ref;
   pr_map = other.pr_map;
   pr_dst = other.pr_dst;
   lin_cnv = other.lin_cnv;
   (*refcounter)++;
   assert(*refcounter >0);
}

void
map2pt::destroy(void){
  (*refcounter)--;
  if (*refcounter<=0){
    delete refcounter;
    pj_free(pr_ref);
    if (pr_map != pr_dst) pj_free(pr_map);
    pj_free(pr_dst);
  }
}

void
map2pt::frw(dPoint & p) const{
  lin_cnv.frw(p);
  if (pr_map!=pr_dst) pjconv(pr_map, pr_dst, p);
  return;
}

void
map2pt::bck(dPoint & p) const{
  if (pr_map!=pr_dst) pjconv(pr_dst, pr_map, p);
  lin_cnv.bck(p);
  return;
}


/*******************************************************************/
// преобразование из карты в карту
// здесь может быть суровое разбиение карты на куски и аппроксимация линейными преобразованиями...
// здесь же - преобразование картинок (с интерфейсом как у image loader'a)

map2map::map2map(const g_map & sM, const g_map & dM, bool test_brd_) :
    c1(sM, Datum("wgs84"), dM.map_proj, map_popts(dM)),
    c2(dM, Datum("wgs84"), dM.map_proj, map_popts(dM)),
    tst_frw(sM.border),
    tst_bck(sM.border),
    test_brd(test_brd_)
{

  if (sM.map_proj == Proj("google")){
    c1=map2pt(sM, Datum("sphere"), Proj("lonlat"), Options());
    c2=map2pt(dM, Datum("wgs84"), Proj("lonlat"), Options());
  }

  g_map M(sM);
  M.ensure_border();
  border_src = M.border;

  tst_bck = poly_tester(border_src);
  if (test_brd){
    border_dst = line_frw(border_src);
    tst_frw = poly_tester(border_dst);
  }
}

void
map2map::frw(dPoint & p) const {c1.frw(p); c2.bck(p);}

void
map2map::bck(dPoint & p) const {c2.frw(p); c1.bck(p);}

// ****************

int
map2map::image_frw(iImage & src_img, int src_scale, iRect cnv_rect,
                       iImage & dst_img, iRect dst_rect) const{

    if (cnv_rect.empty() || dst_rect.empty()) return 1;
    // во сколько раз придется растягивать картинку
    int xscale = int(floor(dst_rect.w/cnv_rect.w));
    int yscale = int(floor(dst_rect.h/cnv_rect.h));
    if (xscale<1) xscale=1;
    if (yscale<1) yscale=1;

    double x,y;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y+=yscale){
      // откуда мы хотим взять строчку
      y = cnv_rect.y + ((dst_y-dst_rect.y)*cnv_rect.h)/dst_rect.h;

      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x+=xscale){
        x = cnv_rect.x + ((dst_x-dst_rect.x)*cnv_rect.w)/dst_rect.w;
	dPoint p(x,y);
        bck(p);
        if (test_brd && !tst_bck.test(p)) continue;
	p/=src_scale;
	unsigned int c = src_img.safe_get(int(p.x),int(p.y));
	if (c != 0){
	  for (int jj=0;jj<yscale;jj++){
	    for (int ii=0;ii<xscale;ii++){
   	      dst_img.set_na(dst_x+ii, dst_y+jj, c);
            }
          }
	}
      }
    }
    return 0;
}

int
map2map::image_bck(iImage & src_img, int src_scale, iRect cnv_rect, 
                       iImage & dst_img, iRect dst_rect) const{
    cerr << "WARNING(ushakov): I suspect image_bck is broken" << endl;
    if (cnv_rect.empty() || dst_rect.empty()) return 1;
    // во сколько раз придется растягивать картинку
    int xscale = int(floor(dst_rect.w/cnv_rect.w));
    int yscale = int(floor(dst_rect.h/cnv_rect.h));
    if (xscale<1) xscale=1;
    if (yscale<1) yscale=1;

    double x,y;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
      // откуда мы хотим взять строчку
      y = cnv_rect.y + ((dst_y-dst_rect.y)*cnv_rect.h)/dst_rect.h;
      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
        x = cnv_rect.x + ((dst_x-dst_rect.x)*cnv_rect.w)/dst_rect.w;
        dPoint p(x,y);
        if (test_brd && !tst_bck.test(p)) continue;
        bck(p); p/=src_scale;
	unsigned int c = src_img.safe_get(int(p.x),int(p.y));
	if (c != 0){
	  for (int jj=0;jj<yscale;jj++){
	    for (int ii=0;ii<xscale;ii++){
   	      dst_img.set_na(dst_x+ii, dst_y+jj, c);
            }
          }
        }
      }
    }
    return 0;
}

/*******************************************************************/

Options
map_popts(const g_map & M, Options O){
  switch (M.map_proj.val){
  case 0: break; //lonlat
  case 1:        //tmerc
    if (O.count("lon0")==0){
      O.put("lon0", lon2lon0(M.center().x));
      O.put("E0",   500000.0);
      O.put("N0",   0.0);
    }
    break;
  case 2:        //UTM
    std::cerr << "utm map is not supported. fixme!\n";
    break;
  case 3:        // merc
    break;
  case 4:        // google
    break;
  case 5:        // ks
    break;
  case 6:        // cea
    break;
  default:
    std::cerr << "unknown map proj: " << M.map_proj << "\n";
    break;
  }
  return O;
}


g_map
mymap(const g_map_list & maplist){
  g_map ret;
  Options O;

  ret.map_proj=Proj("lonlat");
  if (maplist.size()>0){
    ret.map_proj=maplist[0].map_proj;
    O=map_popts(maplist[0]);
  }

  dRect rm=maplist.range();
  double lon0 = lon2lon0(rm.x+rm.w/2);
  O.put("lon0", lon0); // todo - use map_popts here?

  // масштаб -- соответствующий минимальному масштабу карт, если они есть,
  // или 1/3600 градуса на точку, если карт нет
  double mpp=1e99;
  g_map_list::const_iterator it;
  for (it = maplist.begin(); it != maplist.end(); it++){
    double tmp=map_mpp(*it, it->map_proj);
    if (mpp>tmp) mpp=tmp;
  }
  if ((mpp>1e90)||(mpp<1e-90)) mpp=1/3600.0;

  // точки привязки
  pt2pt cnv(Datum("WGS84"), ret.map_proj, O, Datum("WGS84"), Proj("lonlat"), O);
  dPoint p(lon0,0); cnv.bck(p);
  dPoint p1=p+dPoint(mpp*1000,0); cnv.frw(p1);
  dPoint p2=p+dPoint(0,mpp*1000); cnv.frw(p2);
  cnv.frw(p);

  ret.push_back(g_refpoint(p.x,p.y,   0,1000));
  ret.push_back(g_refpoint(p1.x,p1.y, 1000,1000));
  ret.push_back(g_refpoint(p2.x,p2.y, 0,0));
  return ret;
}

g_map
mymap(const geo_data & world){
  g_map ret;
  Options O;

  // put all maps into one map_list
  g_map_list maps;
  for (vector<g_map_list>::const_iterator ml = world.maps.begin();
       ml!=world.maps.end(); ml++){
    maps.insert(maps.end(), ml->begin(), ml->end());
  }

  return mymap(maps);
}


double
map_mpp(const g_map &map, Proj P){
  if (map.size()<3) return 0;
  double l1=0, l2=0;
  g_map map1=map; map1.map_proj=P;
  convs::pt2pt c(Datum("wgs84"), P, map_popts(map1), Datum("wgs84"), Proj("lonlat"), Options());
  for (int i=1; i<map.size();i++){
    dPoint p1(map[i-1].x,map[i-1].y);
    dPoint p2(map[i].x,  map[i].y);
    c.bck(p1); c.bck(p2);
    l1+=pdist(p1,p2);
    l2+=pdist(dPoint(map[i].xr, map[i].yr), dPoint(map[i-1].xr, map[i-1].yr));
  }
  if (l2==0) return 0;
  return l1/l2;
}

double
lon2lon0(const double lon){
  return floor( lon/6.0 ) * 6 + 3;
}

int
lon2pref(const double lon){
  return (lon2lon0(lon)-3)/6+1;
}

double
lon_pref2lon0(const double lon){
  return floor( lon/1e6 ) * 6 - 3;
}

double
lon_delprefix(const double lon){
  return lon - floor( lon/1e6 ) * 1e6;
}

}//namespace


