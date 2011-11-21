// Привязки специальных карт и снимков

#include "geo_refs.h"
#include "geo_convs.h"
#include "geo_nom.h"
#include "loaders/image_google.h"
#include "loaders/image_ks.h"
#include "2d/line_utils.h"
#include <sstream>
#include <iomanip>

using namespace std;

g_map
mk_tmerc_ref(const dLine & points, double u_per_m, bool yswap){
  g_map ref;

  if (points.size()<3){
    cerr << "error in mk_tmerc_ref: number of points < 3\n";
    return g_map();
  }
  // Get refs.
  // Reduce border to 5 points, remove last one.
  dLine refs = points;
  refs.push_back(*refs.begin()); // to assure last=first
  refs = generalize(refs, -1, 5);
  refs.resize(4);

  // Create lonlat -> tmerc conversion with wanted lon0,
  // convert refs to our map coordinates.
  Options PrO;
  PrO.put<double>("lon0", convs::lon2lon0(points.center().x));
  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), PrO,
                   Datum("wgs84"), Proj("tmerc"), PrO);
  dLine refs_c(refs);
  cnv.line_frw_p2p(refs_c);
  refs_c *= u_per_m; // to out units
  refs_c -= refs_c.range().TLC();
  double h = refs_c.range().h;

  // swap y if needed
  if (yswap){
    for (int i=0;i<refs_c.size();i++)
      refs_c[i].y = h - refs_c[i].y;
  }

  // add refpoints to our map
  for (int i=0;i<refs.size();i++){
    ref.push_back(g_refpoint(refs[i], refs_c[i]));
  }

  ref.map_proj=Proj("tmerc");

  // Now we need to convert border to map units.
  // We can convert them by the same way as refs, but
  // this is unnecessary duplicating of non-trivial code.
  // So we constract map2pt conversion from our map.

  // Set map border
  convs::map2pt brd_cnv(ref, Datum("wgs84"), Proj("lonlat"));
  ref.border = brd_cnv.line_bck(points);
  ref.border.push_back(*ref.border.begin()); // to assure last=first
  ref.border = generalize(ref.border, 1000, -1); // 1 unit accuracy
  ref.border.resize(ref.border.size()-1);

  return ref;
}

void
incompat_warning(const Options & o, const string & o1, const string & o2){
  if (o.exists(o2))
    cerr << "make reference warning: "
      << "skipping option --" << o2 << "\n"
      <<" which is incompatable with --" << o1 << "\n";
}

vector<int>
read_int_vec(const string & str){
  istringstream s(str);
  char sep;
  int x;
  vector<int> ret;

  while (1){
    s >> ws >> x;
    if (!s.fail()){
      ret.push_back(x);
      s >> ws;
    }

    if (s.eof()) return ret;

    s >> ws >> sep;
    if (sep!=','){
      cerr << "error while reading comma separated values\n";
      return ret;
    }
  }
}

g_map
mk_ref(Options & o){
  g_map ret;

  // default values
  double dpi=300;
  double google_dpi=-1;
  double rscale=100000;
  double rs_factor=1.0; // proj units/m
  Datum datum("wgs84");
  Proj  proj("tmerc");
  bool verbose=o.exists("verbose");

  // first step: process geom, nom, google options
  // -- create border and 4 refpoints in wgs84 lonlat
  // -- set map_proj and proj options
  // -- change defauld dpi and rscale if needed
  dRect geom;
  dLine refs;
  Options proj_opts;

  // rectangular map
  if (o.exists("geom")){
    incompat_warning (o, "geom", "wgs_brd");
    incompat_warning (o, "geom", "wgs_geom");
    incompat_warning (o, "geom", "nom");
    incompat_warning (o, "geom", "google");\

    dRect geom = o.get<dRect>("geom");
    if (geom.empty()){
     cerr << "error: bad geometry\n";
      exit(1);
    }

    datum = o.get<Datum>("datum", Datum("pulkovo"));
    proj = o.get<Proj>("proj", Proj("tmerc"));

    if ((proj == Proj("tmerc")) && (geom.x>=1e6)){
      double lon0 = convs::lon_pref2lon0(geom.x);
      geom.x = convs::lon_delprefix(geom.x);
      proj_opts.put<double>("lon0", lon0);
    }
    if (o.exists("lon0"))
    proj_opts.put("lon0", o.get<double>("lon0"));
    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     datum, proj, proj_opts);

    if (verbose) cerr << "mk_ref: geom = " << geom << "\n";
    refs = rect2line(geom);
    ret.border = cnv.line_bck(refs, 1e-6);
    refs.resize(4);
    cnv.line_bck_p2p(refs);
  }
  else if (o.exists("wgs_geom")){
    incompat_warning (o, "wgs_geom", "wgs_brd");
    incompat_warning (o, "wgs_geom", "geom");
    incompat_warning (o, "wgs_geom", "nom");
    incompat_warning (o, "wgs_geom", "google");\

    dRect geom = o.get<dRect>("wgs_geom");
    if (geom.empty()){
     cerr << "error: bad geometry\n";
      exit(1);
    }

    proj = o.get<Proj>("proj", Proj("tmerc"));
    proj_opts.put<double>("lon0", convs::lon2lon0(geom.x+geom.w/2));

    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     Datum("wgs84"), proj, proj_opts);

    if (verbose) cerr << "mk_ref: geom = " << geom << "\n";
    refs = rect2line(geom);
    refs.resize(4);
    // border is set to be rectanglar in proj:
    ret.border =
      cnv.line_bck(rect2line(cnv.bb_frw(geom, 1e-6)), 1e-6);
  }
  else if (o.exists("wgs_brd")){
    incompat_warning (o, "wgs_geom", "wgs_geom");
    incompat_warning (o, "wgs_geom", "geom");
    incompat_warning (o, "wgs_geom", "nom");
    incompat_warning (o, "wgs_geom", "google");\

    dLine brd = o.get<dLine>("wgs_brd");
    if (brd.size()<3){
     cerr << "error: bad border line\n";
      exit(1);
    }
    brd.push_back(brd[0]);

    proj = o.get<Proj>("proj", Proj("tmerc"));
    proj_opts.put<double>("lon0", convs::lon2lon0(brd.range().CNT().x));

    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     Datum("wgs84"), proj, proj_opts);

    if (verbose) cerr << "mk_ref: brd = " << brd << "\n";
    refs = brd;
    refs.resize(4);
    // border is set to be rectanglar in proj:
    ret.border =
      cnv.line_bck(cnv.line_frw(brd, 1e-6), 1e-6);
  }
  // nom map
  else if (o.exists("nom")){
    incompat_warning (o, "nom", "wgs_brd");
    incompat_warning (o, "nom", "wgs_geom");
    incompat_warning (o, "nom", "geom");
    incompat_warning (o, "nom", "google");

    proj=Proj("tmerc");
    datum=Datum("pulkovo");

    int rs;
    string name=o.get<string>("nom", string());
    dRect geom = convs::nom_to_range(name, rs);
    if (geom.empty()){
      cerr << "error: can't get geometry for map name \"" << name << "\"\n";
      exit(1);
    }
    rscale = rs;
    double lon0 = convs::lon2lon0(geom.x+geom.w/2.0);
    proj_opts.put("lon0", lon0);
    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     datum, Proj("lonlat"), proj_opts);

    if (verbose) cerr << "mk_ref: geom = " << geom << "\n";
    refs = rect2line(geom);
    ret.border = cnv.line_bck(refs, 1e-6);
    refs.resize(4);
    cnv.line_bck_p2p(refs);
  }
  // google tile
  else if (o.exists("google")){
    incompat_warning (o, "googlr", "wgs_brd");
    incompat_warning (o, "googlr", "wgs_geom");
    incompat_warning (o, "google", "geom");
    incompat_warning (o, "google", "nom");

    proj=Proj("merc");
    datum=Datum("sphere");

    vector<int> crd = read_int_vec(o.get<string>("google"));
    if (crd.size()!=3){
      cerr << "error: wrong --google coordinates\n";
      exit(1);
    }
    int x=crd[0];
    int y=crd[1];
    int z=crd[2];
    //
    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     datum, proj, Options());
    dPoint p(180,0);
    cnv.frw(p);
    double sc1 = 1.0/(2<<(z-2));
    double sc2 = p.x; // merc units (equator meters) per z=1 tile
    int x1 = (x * sc1 - 1)*sc2;
    int x2 = ((x+1) * sc1 - 1)*sc2;
    int y1 = (1 - (y+1) * sc1)*sc2;
    int y2 = (1 - y * sc1)*sc2;
    dRect geom(dPoint(x1,y1), dPoint(x2,y2));
    if (geom.empty()){
      cerr << "error: empty geometry\n";
      exit(1);
    }

    if (verbose) cerr << "mk_ref: geom = " << geom << "\n";
    refs = rect2line(geom);
    ret.border = cnv.line_bck(refs, 1e-6);
    refs.resize(4);
    cnv.line_bck_p2p(refs);

    rscale=o.get<double>("rscale", rscale);

    double lat=refs.range().CNT().y;
    rs_factor = 1/cos(M_PI*lat/180.0);
    dpi = 256 * 2.54/100.0 * rscale * rs_factor / geom.w;
  }
  else {
    cerr << "error: can't make map reference without\n"
         << "--geom or --nom or --google setting\n";
    exit(1);
  }
  ret.map_proj=proj;

  // step 2: calculate conversion coeff between map proj units and
  // output map points

  rscale=o.get<double>("rscale", rscale);
  if (o.get<string>("dpi", "") == "fig") dpi= 1200/1.05;
  else dpi=o.get<double>("dpi", dpi);

  dpi*=o.get<double>("mag", 1.0);

  // put real dpi and rscale back to options
  o.put("dpi", dpi);
  o.put("rscale", rscale);

  double k = 100.0/2.54 * dpi / rscale / rs_factor;

  if (verbose) cerr << "mk_ref: rscale = " << rscale
    << ", dpi = " << dpi << ", k = " << k << "\n";

  // step 3:  setting refpoints

  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                   datum, proj, proj_opts);
  dLine refs_r(refs);
  cnv.line_frw_p2p(refs_r);

  refs_r *= k; // to out units
  refs_r -= refs_r.range().TLC();
  double h = refs_r.range().h;

  // swap y if needed
  if (!o.exists("swap_y"))
    for (int i=0;i<refs_r.size();i++)
      refs_r[i].y = h - refs_r[i].y;

  // add refpoints to our map
  for (int i=0;i<refs.size();i++)
    ret.push_back(g_refpoint(refs[i], refs_r[i]));

  // step 3:  converting border
  convs::map2pt brd_cnv(ret, Datum("wgs84"), Proj("lonlat"));
  ret.border = brd_cnv.line_bck(ret.border);
  ret.border = generalize(ret.border, 1, -1); // 1 unit accuracy
  ret.border.resize(ret.border.size()-1);

  return ret;
}


g_map
ref_google(int scale){
   g_map ret;
   ret.map_proj = Proj("google");
   ret.comm = "maps.google.com";

   if (scale<google::google_scale_min) scale=google::google_scale_min;
   if (scale>google::google_scale_max) scale=google::google_scale_max;

   double maxlat = 360/M_PI*atan(exp(M_PI)) - 90;
   double width = 1<<(8+scale-1);

   ret.push_back(g_refpoint(-180,maxlat,  0,0));
   ret.push_back(g_refpoint( 180,maxlat,  width,0));
   ret.push_back(g_refpoint( 180,-maxlat, width,width));
   ret.push_back(g_refpoint(-180,-maxlat, 0,width));

   ret.border.push_back(dPoint(0,0));
   ret.border.push_back(dPoint(width,0));
   ret.border.push_back(dPoint(width,width));
   ret.border.push_back(dPoint(0,width));
   return ret;
}

g_map ref_ks_old(int scale){
   g_map ret;
   ret.map_proj = Proj("ks");
   ret.comm = "new.kosmosnimki.ru";

   if (scale<ks::ks_scale_min) scale=ks::ks_scale_min;
   if (scale>ks::ks_scale_max) scale=ks::ks_scale_max;
 
   // Datum тут должен быть wgs, 
   // в Proj(ks) параметры эллипсоида записаны жестко
   convs::pt2pt c1(Datum("wgs84"), Proj("ks"), Options(), Datum("wgs84"), Proj("lonlat"), Options());

   dPoint p(5 * (1<<14) * 256, 5 * (1<<14) * 256);
   c1.frw(p);
   double width = 5*256*(1<<(scale-3));
   double maxlat=p.y;
   double maxlon=p.x;

   ret.push_back(g_refpoint(0,  p.y, 0,0));
   ret.push_back(g_refpoint(p.x,p.y, width,0));
   ret.push_back(g_refpoint(p.x,0,   width,width));
   ret.push_back(g_refpoint(0,  0,   0,width));

   ret.border.push_back(dPoint(0,0));
   ret.border.push_back(dPoint(width,0));
   ret.border.push_back(dPoint(width,width));
   ret.border.push_back(dPoint(0,width));
   return ret;
}

g_map
ref_ks(int scale){
   g_map ret;
   ret.map_proj = Proj("merc");
   ret.comm = "new.kosmosnimki.ru";

   if (scale<ks::ks_scale_min) scale=ks::ks_scale_min;
   if (scale>ks::ks_scale_max) scale=ks::ks_scale_max;

   double width = 256*(1<<(scale-1));
   double maxlat = 85.08405905;

   ret.push_back(g_refpoint(0,maxlat,0,0));
   ret.push_back(g_refpoint(180,maxlat,width,0));
   ret.push_back(g_refpoint(180,0,width,width));
   ret.push_back(g_refpoint(0,0,0,width));

   // ручная поправка :(((
   ret += dPoint(6.0/(1<<(14-scale)),0);

   ret.border.push_back(dPoint(0,0));
   ret.border.push_back(dPoint(width,0));
   ret.border.push_back(dPoint(width,width));
   ret.border.push_back(dPoint(0,width));

   return ret;
}
