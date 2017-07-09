// Привязки специальных карт и снимков

#include "tiles/tiles.h"

#include "geo_refs.h"
#include "geo/geo_convs.h"
#include "geo/geo_nom.h"
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

  // Create tmerc->lonlat tmerc conversion with wanted lon0,
  // convert refs to our map coordinates.
  Options PrO;
  PrO.put<double>("lon0", convs::lon2lon0(points.center().x));
  convs::pt2wgs cnv(Datum("wgs84"), Proj("tmerc"), PrO);
  dLine refs_c(refs);
  cnv.line_bck_p2p(refs_c);
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
  ref.proj_opts.put("lon0", convs::lon2lon0(refs.range().CNT().x));

  // Now we need to convert border to map units.
  // We can convert them by the same way as refs, but
  // this is unnecessary duplicating of non-trivial code.
  // So we constract map2pt conversion from our map.

  // Set map border
  convs::map2wgs brd_cnv(ref);
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
  bool sw=!o.exists("swap_y");

  // first step: process geom, nom, google options
  // -- create border and 4 refpoints in wgs84 lonlat
  // -- set map_proj and proj options
  // -- change defauld dpi and rscale if needed
  dRect geom;
  dLine refs;
  Options proj_opts;

  // rectangular map
  if (o.exists("geom")){
    incompat_warning (o, "geom", "wgs_geom");
    incompat_warning (o, "geom", "wgs_brd");
    incompat_warning (o, "geom", "trk_brd");
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
    refs = rect2line(geom, true, sw);
    ret.border = cnv.line_bck(refs, 1e-6);
    refs.resize(4);
    cnv.line_bck_p2p(refs);
  }
  else if (o.exists("wgs_geom")){
    incompat_warning (o, "wgs_geom", "geom");
    incompat_warning (o, "wgs_geom", "wgs_brd");
    incompat_warning (o, "wgs_geom", "trk_brd");
    incompat_warning (o, "wgs_geom", "nom");
    incompat_warning (o, "wgs_geom", "google");\

    dRect geom = o.get<dRect>("wgs_geom");
    if (geom.empty()){
     cerr << "error: bad geometry\n";
      exit(1);
    }

    proj = o.get<Proj>("proj", Proj("tmerc"));
    proj_opts.put<double>("lon0",
      o.get("lon0", convs::lon2lon0(geom.x+geom.w/2)));

    convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                     Datum("wgs84"), proj, proj_opts);

    if (verbose) cerr << "mk_ref: geom = " << geom << "\n";
    refs = rect2line(geom, true, sw);
    refs.resize(4);
    // border is set to be rectanglar in proj:
    ret.border =
      cnv.line_bck(rect2line(cnv.bb_frw(geom, 1e-6), true, sw), 1e-6);
  }
  else if (o.exists("wgs_brd")){
    incompat_warning (o, "wgs_brd", "geom");
    incompat_warning (o, "wgs_brd", "wgs_geom");
    incompat_warning (o, "wgs_brd", "trk_brd");
    incompat_warning (o, "wgs_brd", "nom");
    incompat_warning (o, "wgs_brd", "google");\

    ret.border = o.get<dLine>("wgs_brd");
    if (ret.border.size()<3){
     cerr << "error: bad border line\n";
      exit(1);
    }
    ret.border.push_back(ret.border[0]);

    proj = o.get<Proj>("proj", Proj("tmerc"));
    proj_opts.put<double>("lon0",
      o.get("lon0", convs::lon2lon0(ret.border.range().CNT().x)));

    if (verbose) cerr << "mk_ref: brd = " << ret.border << "\n";
    refs = generalize(ret.border, -1, 5); // 5pt
    refs.resize(4);
  }
  else if (o.exists("trk_brd")){
    incompat_warning (o, "wgs_brd", "geom");
    incompat_warning (o, "wgs_brd", "wgs_geom");
    incompat_warning (o, "wgs_brd", "wgs_brd");
    incompat_warning (o, "wgs_brd", "nom");
    incompat_warning (o, "wgs_brd", "google");\

    geo_data W;
    io::in(o.get<string>("trk_brd"), W);
    if (W.trks.size()<1) {
      cerr << "error: file with a track is expected in trk_brd option\n";
      exit(1);
    }
    ret.border = W.trks[0];
    if (ret.border.size()<3){
     cerr << "error: bad border line\n";
      exit(1);
    }
    ret.border.push_back(ret.border[0]);

    proj = o.get<Proj>("proj", Proj("tmerc"));
    proj_opts.put<double>("lon0",
      o.get("lon0", convs::lon2lon0(ret.border.range().CNT().x)));

    if (verbose) cerr << "mk_ref: brd = " << ret.border << "\n";
    refs = generalize(ret.border, -1, 5); // 5pt
    refs.resize(4);
  }
  // nom map
  else if (o.exists("nom")){
    incompat_warning (o, "nom", "geom");
    incompat_warning (o, "nom", "wgs_geom");
    incompat_warning (o, "nom", "wgs_brd");
    incompat_warning (o, "nom", "trk_brd");
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
    refs = rect2line(geom, true, sw);
    ret.border = cnv.line_bck(refs, 1e-6);
    refs.resize(4);
    cnv.line_bck_p2p(refs);
  }
  // google tile
  else if (o.exists("google")){
    incompat_warning (o, "google", "geom");
    incompat_warning (o, "google", "wgs_geom");
    incompat_warning (o, "google", "wgs_brd");
    incompat_warning (o, "google", "trk_brd");
    incompat_warning (o, "google", "nom");

    datum=Datum("wgs84");
    proj=Proj("google");

    vector<int> crd = read_int_vec(o.get<string>("google"));
    if (crd.size()!=3){
      cerr << "error: wrong --google coordinates\n";
      exit(1);
    }
    int x=crd[0];
    int y=crd[1];
    int z=crd[2];
    //

    Tiles tcalc;
    dRect geom = tcalc.gtile_to_range(x,y,z);

    if (verbose) cerr << "mk_ref: geom = " << geom << "\n";
    refs = rect2line(geom, true,sw);
    ret.border = refs;
    refs.resize(4);

    rscale=o.get<double>("rscale", rscale);

    double lat=refs.range().CNT().y;
    rs_factor = 1/cos(M_PI*lat/180.0);

    // m -> tile pixel
    double k = 1/tcalc.px2m(z);
    dpi = k * 2.54/100.0*rscale*rs_factor;
  }
  else {
    cerr << "error: can't make map reference without\n"
         << "--geom or --nom or --google setting\n";
    exit(1);
  }
  ret.map_proj=proj;
  ret.map_datum=datum;
  ret.proj_opts=proj_opts;

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
  convs::pt2wgs cnv(datum, proj, proj_opts);
  dLine refs_r(refs);
  cnv.line_bck_p2p(refs_r);

  refs_r *= k; // to out units
  refs_r -= refs_r.range().TLC();
  double h = refs_r.range().h;

  // swap y if needed
  if (sw)
    for (int i=0;i<refs_r.size();i++)
      refs_r[i].y = h - refs_r[i].y;

  // add refpoints to our map
  for (int i=0;i<refs.size();i++)
    ret.push_back(g_refpoint(refs[i], refs_r[i]));

  // step 3:  converting border
  convs::map2wgs brd_cnv(ret);
  ret.border = brd_cnv.line_bck(ret.border);
  ret.border = generalize(ret.border, 1, -1); // 1 unit accuracy
  ret.border.resize(ret.border.size()-1);
  return ret;
}

