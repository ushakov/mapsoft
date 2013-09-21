#include "geo_convs.h"
#include <cmath>
#include <cstring>
#include <sstream>
#include <cassert>
#include <map>

#include "2d/line_utils.h"

namespace convs{
using namespace std;

/*******************************************************************/

/// Create PROJ4 projection object from our D, P and options.
string
mkprojstr(const Datum & D, const Proj & P, const Options & o){

    int old_enum_fmt=Enum::output_fmt;
    Enum::output_fmt = Enum::proj_fmt;

    ostringstream projpar;

    // proj setings

    // special google case, see http://trac.osgeo.org/proj/wiki/FAQ#ChangingEllipsoidWhycantIconvertfromWGS84toGoogleEarthVirtualGlobeMercator
    if (P==Proj("google")) // use google_sphere instead of wgs
       projpar << " +proj=merc +a=6378137 +b=6378137 +nadgrids=@null +no_defs";
    else if (P==Proj("ch1904"))
       projpar << " +proj=somerc +lat_0=46.95240555555556"
                  " +lon_0=7.439583333333333 +x_0=600000 +y_0=200000"
                  " +ellps=bessel +towgs84=674.374,15.056,405.346,0,0,0,0"
                  " +units=m +no_defs";
    else
      projpar << " +proj=" << P;

    // datum and ellps settings
    if (D==Datum("pulkovo"))
       projpar << " +ellps=krass +towgs84=+28,-130,-95";
    // Finnish coords, see http://www.kolumbus.fi/eino.uikkanen/geodocsgb/ficoords.htm
    // http://lists.maptools.org/pipermail/proj/2005-December/001944.html
    else if (D==Datum("KKJ"))
       projpar << " +ellps=intl +towgs84=-90.7,-106.1,-119.2,4.09,0.218,-1.05,1.37";
    else if (D==Datum("sphere"))
       projpar << " +ellps=sphere";
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
        case 5: // google
          break;

        case 6: // lcc
          projpar << " +lat_1="  << o.get("lat1", 30.0);
          projpar << " +lat_2="  << o.get("lat2", 60.0);
          projpar << " +lon_0="  << o.get("lon0", 0.0);
          projpar << " +k_0="    << o.get("k",    1.0);
          projpar << " +x_0="   << o.get("E0",   500000.0);
          projpar << " +y_0="   << o.get("N0",   0.0);
          break;

        case 7: // swiss
          break;

        default:
          std::cerr << "unknown proj: " << P.val << "\n";
          exit(1);
    }

    return projpar.str();
}

projPJ
mkproj(const Datum & D, const Proj & P, const Options & o){
    string str=mkprojstr(D,P,o);
    projPJ ret=pj_init_plus(str.c_str());
    if (!ret){
        std::cerr << "mkproj: Error: can't create proj: \"" << str << "\"\n";
        exit(1);
    }
    return ret;
}

/*******************************************************************/

pt2pt::pt2pt(const Datum & sD, const Proj & sP, const Options & sPo,
             const Datum & dD, const Proj & dP, const Options & dPo){
  pr_src = mkproj(sD, sP, sPo);

  if (sD==dD && sP==dP && sPo ==dPo)
    pr_dst=pr_src;
  else
    pr_dst = mkproj(dD, dP, dPo);

  if (pj_is_latlong(pr_src)) sc_src=180.0/M_PI;
  if (pj_is_latlong(pr_dst)) sc_dst=180.0/M_PI;

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
   this->Conv::operator=(other);
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
    if (pr_dst != pr_src) pj_free(pr_dst);
    pj_free(pr_src);
  }
}

/*******************************************************************/

map2pt::map2pt(const g_map & sM,
               const Datum & dD, const Proj & dP, const Options & dPo){

  // proj for reference points (coords in radians!)
  pr_ref = mkproj(Datum("WGS84"), Proj("lonlat"), Options()); // for ref points

  // destination projection
  pr_dst = mkproj(dD, dP, dPo);

  // map projection
  if (sM.map_datum==dD && sM.map_proj==dP && sM.proj_opts ==dPo)
    pr_map = pr_dst;
  else{
    Options o = sM.proj_opts;
    if (sM.map_proj == Proj("tmerc") && !o.exists("lon0"))
      o.put<double>("lon0", map_lon0(sM));
    pr_map = mkproj(sM.map_datum, sM.map_proj, o);
  }

  if (pj_is_latlong(pr_dst)) sc_dst=180.0/M_PI;

  refcounter   = new int;
  *refcounter  = 1;

  // convert refpoints into map coords and find linear conversion
  // to raster points.
  map<dPoint,dPoint> points;
  for (g_map::const_iterator i=sM.begin(); i!=sM.end(); i++){
    dPoint pr(i->xr, i->yr), pl(*i * M_PI/180.0);
    pj_transform(pr_ref, pr_map, 1, 1, &pl.x, &pl.y, NULL);
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
   this->Conv::operator=(other);
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
map2pt::rescale_src(const double s){
  lin_cnv.rescale_src(s);
}

/*******************************************************************/
/*
input: M1(D1,P1,O1) --ref--> WGS
       M2(D2,P2,O2) --ref--> WGS


     ref1    c1                   c2      ref2
  M1 --> WGS --> P1 --cnv2--> P2 <-- WGS <--- M2
   \             /             \             /
    -->cnv1(Aff)-               -cnv3(aff)<--

*/
map2map::map2map(const Datum & sD, const Proj & sP, const Options & sPo,
                 const Datum & dD, const Proj & dP, const Options & dPo,
                std::map<dPoint, dPoint> ref1,
                std::map<dPoint, dPoint> ref2 ) :
                   cnv2(sD, sP, sPo, dD, dP, dPo){

  convs::pt2wgs c1(sD, sP, sPo);
  convs::pt2wgs c2(dD, dP, dPo);
  std::map<dPoint, dPoint>::iterator i;
  for (i=ref1.begin(); i!=ref1.end(); i++) c1.bck(i->second);
  for (i=ref2.begin(); i!=ref2.end(); i++) c2.bck(i->second);
  cnv1.set_from_ref(ref1);
  cnv3.set_from_ref(ref2);
}

Options ref2opt(const std::map<dPoint, dPoint> & ref){
  double lon0=0;
  std::map<dPoint, dPoint>::const_iterator i;
  for (i=ref.begin(); i!=ref.end(); i++) lon0+=i->second.x;
  if (ref.size()) lon0/=ref.size();
  Options ret;
  ret.put<double>("lon0", lon0);
  return ret;
}

map2map::map2map(const Proj & sP, const Proj & dP,
                 std::map<dPoint, dPoint> ref1,
                 std::map<dPoint, dPoint> ref2 ):
                   cnv2(Datum("wgs84"), sP, ref2opt(ref1),
                        Datum("wgs84"), dP, ref2opt(ref2) ){

  convs::pt2wgs c1(Datum("wgs84"), sP, ref2opt(ref1));
  convs::pt2wgs c2(Datum("wgs84"), dP, ref2opt(ref2));
  std::map<dPoint, dPoint>::iterator i;
  for (i=ref1.begin(); i!=ref1.end(); i++) c1.bck(i->second);
  for (i=ref2.begin(); i!=ref2.end(); i++) c2.bck(i->second);
  cnv1.set_from_ref(ref1);
  cnv3.set_from_ref(ref2);
}

map2map::map2map(const g_map & sM, const g_map & dM):
     cnv2(Datum("wgs84"), sM.map_proj, sM.proj_opts,
          Datum("wgs84"), dM.map_proj, dM.proj_opts){

  convs::pt2wgs c1(Datum("wgs84"), sM.map_proj, sM.proj_opts);
  convs::pt2wgs c2(Datum("wgs84"), dM.map_proj, dM.proj_opts);
  std::map<dPoint, dPoint> ref1, ref2;
  g_map::const_iterator i;
  for (i=sM.begin(); i!=sM.end(); i++){
    dPoint p1(i->xr, i->yr);
    dPoint p2(i->x, i->y);
    c1.bck(p2);
    ref1[p1]=p2;
  }
  for (i=dM.begin(); i!=dM.end(); i++){
    dPoint p1(i->xr, i->yr);
    dPoint p2(i->x, i->y);
    c2.bck(p2);
    ref2[p1]=p2;
  }
  cnv1.set_from_ref(ref1);
  cnv3.set_from_ref(ref2);
}

void
map2map::rescale_src(const double s){ cnv1.rescale_src(s); }
void
map2map::rescale_dst(const double s){ cnv3.rescale_src(s); }


/*******************************************************************/

// we can't use range() here, because this function
// can be used in map2pt constructor.
double map_lon0(const g_map &M){
  dRect rg = M.range_ref();
  dRect rr = M.range_ref_r();
  dRect br = M.border.range();
  if (br.empty()) return lon2lon0(rg.CNT().x);
  return lon2lon0( rg.x + rg.w/rr.w * (br.CNT().x-rr.x) );
}

g_map
mymap(const g_map_list & maplist){
  g_map ret;
  Options O;

  // proj and proj_options - from the first map
  ret.map_proj=Proj("lonlat");
  if (maplist.size()>0){
    ret.map_proj=maplist[0].map_proj;
    ret.proj_opts=maplist[0].proj_opts;
  }

  // set scale from map with minimal scale
  // or 1/3600 degree if no map exists
  double mpp=1e99;
  g_map_list::const_iterator it;
  for (it = maplist.begin(); it != maplist.end(); it++){
    double tmp=map_mpp(*it, it->map_proj);
    if (mpp>tmp) mpp=tmp;
  }
  if ((mpp>1e90)||(mpp<1e-90)) mpp=1/3600.0;

  // create refpoints
  pt2pt cnv(Datum("WGS84"), ret.map_proj, ret.proj_opts, Datum("WGS84"), Proj("lonlat"), Options());
  dPoint p(maplist.range().CNT()); cnv.bck(p);
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
  convs::pt2wgs c(Datum("wgs84"), P, map1.proj_opts);
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


