#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cstring>
#include <dirent.h>
#include <vector>
#include <list>
#include <map>

#include <sys/stat.h>
#include <math.h>
#include <zip.h>

#include "io.h"
#include "io_gps.h"
#include "io_gu.h"
#include "io_xml.h"
#include "io_kml.h"
#include "io_gpx.h"
#include "io_oe.h"
#include "io_js.h"
#include "io_zip.h"
#include "geofig.h"
#include "err/err.h"
#include "filetype/filetype.h"

namespace io {
using namespace std;

int
check_file(const string & name){
  struct stat st_buf;
  if (stat(name.c_str(), &st_buf) != 0) return -1;
  if (S_ISREG(st_buf.st_mode)) return 0;
  if (S_ISCHR(st_buf.st_mode)) return 1;
  return -1;
}

std::string
gps_detect(){
  DIR *D = opendir("/sys/bus/usb-serial/drivers/garmin_gps");
  if (!D) return "";
  dirent * de;
  while (de = readdir(D)){
    if ((strlen(de->d_name) < 1) || (de->d_name[0] == '.')) continue;
    string fname = string("/dev/") + string(de->d_name);
    if (check_file(fname) == 1) return fname;
  }
  return "";
}

void
in(const string & in_name, geo_data & world, const Options & opt){
  string name(in_name);
  bool v = opt.exists("verbose");

  if (name == "gps:"){
    name = gps_detect();
    if (name == "")
      throw Err() << "Can't detect gps device";
  }

  int c = check_file(name);
  if (c < 0)
    throw Err() << "Can't access file " << name;

  if (c == 1) return gps::get_all(name.c_str(), world, opt);
  if (testext(name, ".xml")) return xml::read_file (name.c_str(), world, opt);
  if (testext(name, ".gpx")) return gpx::read_file (name.c_str(), world, opt);
  if (testext(name, ".kml")) return kml::read_file (name.c_str(), world, opt);
  if (testext(name, ".gu"))  return gu::read_file (name.c_str(), world, opt);
  if (testext(name, ".wpt") ||
      testext(name, ".plt") ||
      testext(name, ".map")) return oe::read_file (name.c_str(), world, opt);
  if (testext(name, ".zip") ||
      testext(name, ".kmz")) return io_zip::read_file (name.c_str(), world, opt);
  if (testext(name, ".fig")){
    if (v) cerr << "Reading data from Fig file " << name << endl;
    fig::fig_world F;
    fig::read(name.c_str(), F);
    g_map m=fig::get_ref(F);
    fig::get_wpts(F, m, world);
    fig::get_trks(F, m, world);
    fig::get_maps(F, m, world);
    return;
  }
  throw Err() << "Can't determine input format for file " << name;
  return;
}

void
out(const string & out_name, geo_data const & world, const Options & opt){
  string name(out_name);
  bool v = opt.exists("verbose");

  if (name == "gps:"){
    name = gps_detect();
    if (name == "") throw Err() <<
        "Can't detect gps device";
  }

  // GPS device
  if (check_file(name) == 1) return gps::put_all (name.c_str(), world, opt);
  if (testext(name, ".xml")) return xml::write_file (name.c_str(), world, opt);
  if (testext(name, ".gpx")) return gpx::write_file (name.c_str(), world, opt);
  if (testext(name, ".js"))  return js::write_file (name.c_str(), world, opt);

  // KML and KMZ formats
  if (testext(name, ".kml") ||
      testext(name, ".kmz")){
    string base(name.begin(), name.begin()+name.rfind('.'));
    string kml=base+"kml";
    kml::write_file (kml.c_str(), world, opt);

    if (testext(name, ".kmz")){
      vector<string> files;
      files.push_back(kml);
      io_zip::write_file(name.c_str(), files);
    }
    return;
  }
  // Garmin-Utils format
  if (testext(name, ".gu")) return gu::write_file (name.c_str(), world, opt);

  // OziExplorer format
  if ((testext(name, "wpt"))||
      (testext(name, "plt"))||
      (testext(name, "map"))||
      (testext(name, "zip"))||
      (testext(name, "oe"))){
    string base(name.begin(), name.begin()+name.rfind('.'));
    vector<string> files;

    // number of files with tracks, waypoints and maps
    size_t wn = world.wpts.size();
    size_t tn = world.trks.size();
    size_t mn = world.maps.size();

    int ww=0, tw=0, mw=0; 
    if (wn > 1) ww = (int) floor (log (1.0 * wn) / log (10.0)) + 1;
    if (tn > 1) tw = (int) floor (log (1.0 * tn) / log (10.0)) + 1;
    if (mn > 1) mw = (int) floor (log (1.0 * mn) / log (10.0)) + 1;

    for (size_t n = 0; n < wn; ++n){
      ostringstream oef;
      if (ww>0)
        oef << base << setw(ww) << setfill('0') << n+1 << ".wpt";
      else
        oef << base << ".wpt";

      oe::write_wpt_file (oef.str().c_str(), world.wpts[n], opt);
      if (v) cerr << "  " << oef.str() << " -- "
                  << world.wpts[n].size() << " waypoints\n";
      files.push_back(oef.str());
    }

    for (size_t n = 0; n < tn; ++n){
      ostringstream oef;
      if (tw > 0)
        oef << base << setw(tw) << setfill('0') << n+1 << ".plt";
      else
        oef << base << ".plt";

      oe::write_plt_file (oef.str().c_str(), world.trks[n], opt);
      if (v) cerr << "  " << oef.str() << " -- "
                  << world.trks[n].size() << " points\n";
      files.push_back(oef.str());
    }

    for (size_t n = 0; n < mn; ++n){
      size_t mmn = world.maps[n].size(); // map count in this maplist
      size_t mmw=0;
      if (mmn > 1) mmw = (size_t) floor (log (1.0 * mmn) / log (10.0)) + 1;

      for (size_t nn = 0; nn != mmn; ++nn) {
        ostringstream oef;
        oef << base;
        if (mw > 0)  oef << setw(mw) << setfill('0') << n+1;
        if ((mmw > 0) && (mw > 0))  oef << "_";
        if (mmw > 0)  oef << setw(mmw) << setfill('0') << nn+1;
        oef << ".map";

        oe::write_map_file (oef.str().c_str(), world.maps[n][nn], opt);
        if (v) cerr << "  " << oef.str() << " -- "
                    << world.maps[n][nn].size() << " reference points" << endl;
        files.push_back(oef.str());
      }
    }

    if (testext (name, "zip"))
      io_zip::write_file(name.c_str(), files);

    return;
  }
  throw Err() << "Can't determine output format for file " << name;
}
}  // namespace io


