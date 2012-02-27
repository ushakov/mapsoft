#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cstring>
#include <dirent.h>
#include <vector>
#include <list>

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
#include "io_zip.h"
#include "geofig.h"

namespace io {
using namespace std;

bool
testext(const string & nstr, const char *ext){
  int pos = nstr.rfind(ext);
  return ((pos>0)&&(pos == nstr.length()-strlen(ext)));
}

int
check_file(const string & name){
  struct stat st_buf;
  if (stat(name.c_str(), &st_buf) != 0) return -1;
  if (S_ISREG(st_buf.st_mode)) return 0;
  if (S_ISCHR(st_buf.st_mode)) return 1;
}

std::string
gps_detect(){
  DIR *D = opendir("/sys/module/garmin_gps/drivers/usb-serial:garmin_gps");
  if (!D) return "";
  dirent * de;
  while (de = readdir(D)){
    string fname = string("/dev/") + string(de->d_name);
    if (check_file(fname) == 1) return fname;
  }
  return "";
}

bool
in(const string & in_name, geo_data & world, const Options & opt){
  string name(in_name);
  bool v = opt.exists("verbose");

  if (name == "gps:"){
    name = gps_detect();
    if (name == ""){
      cerr << "Can't detect gps device\n";
      return false;
    }
  }

  int c = check_file(name);
  if (c < 0){
    cerr << "Can't access " << name << endl;
    return false;
  }

  if (c == 1){
    if (v) cerr << "Reading data from GPS via serial port "
                << name << endl;
    if (!gps::get_all(name.c_str(), world, opt)){
      cerr << "Can't get data from " << name << endl;
      return false;
    }
    return true;
  }

  if (testext(name, ".xml")){
    if (v) cerr << "Reading data from XML file " << name << endl;
    return xml::read_file (name.c_str(), world, opt);
  }
  if (testext(name, ".gpx")){
    if (v) cerr << "Reading data from GPX file " << name << endl;
    return gpx::read_file (name.c_str(), world, opt);
  }
  if (testext(name, ".gu")){
    if (v) cerr << "Reading data from Garmin-utils file " << name << endl;
    return gu::read_file (name.c_str(), world, opt);
  }
  if ((testext(name, ".plt")) || (testext(name, ".wpt")) || (testext(name, ".map"))){
    if (v) cerr << "Reading data from Ozi file " << name << endl;
    return oe::read_file (name.c_str(), world, opt);
  }
  if (testext(name, ".fig")){
    if (v) cerr << "Reading data from Fig file " << name << endl;
    fig::fig_world F;
    fig::read(name.c_str(), F);
    g_map m=fig::get_ref(F);
    fig::get_wpts(F, m, world);
    fig::get_trks(F, m, world);
    fig::get_maps(F, m, world);
    return true;
  }
  if (testext(name, ".zip")) {
    if (v) cerr << "Reading data from zip archive " << name << endl;
    return io_zip::read_file (name.c_str(), world, opt);
  }
  cerr << "Unknown extension: " << name << endl;
  return false;
}

void out(const string & out_name, geo_data const & world, const Options & opt){
  string name(out_name);
  bool v = opt.exists("verbose");

  if (name == "gps:"){
    name = gps_detect();
    if (name == ""){
      cerr << "Can't detect gps device" << endl;
      return;
    }
  }

  if (check_file(name) == 1){
    if (v) cerr << "Sending data to GPS via serial port " << name << endl;
    if (!gps::put_all (name.c_str(), world, opt))
      cerr << "Can't send data to " << name << endl;
    return;
  }

  // mapsoft XML format
  if (testext(name, ".xml")){
    if (v) cerr << "Writing to XML file " << name << endl;
    if (!xml::write_file (name.c_str(), world, opt))
      cerr << "Can't write " << name << endl;
    return;
  }

  // GPX format
  if (testext(name, ".gpx")){
    if (v) cerr << "Writing to GPX file " << name << endl;
    if (!gpx::write_file (name.c_str(), world, opt))
      cerr << "Can't write " << name << endl;
    return;
  }

  // KML and KMZ formats
  if (testext(name, ".kml") || testext(name, ".kmz")){
    string base(name.begin(), name.begin()+name.rfind('.'));
    string kml=base+".kml";
    if (v) cerr << "Writing to Google KML file " << kml << endl;
    if (!kml::write_file (kml.c_str(), world, opt))
      cerr << "Can't write " << name << endl;

    if (testext (name, ".kmz")){
      vector<string> files;
      files.push_back(kml);
      if (v) cerr << "Zipping to " << name << endl;
      if (!io_zip::write_file(name.c_str(), files))
        cerr << "Can't make zip-file " << name << endl;
    }
    return;
  }

  // Garmin-Utils format
  if (testext(name, ".gu")){
    if (v) cerr << "Writing to Garmin-utils file " << name << endl;
    if (!gu::write_file (name.c_str(), world, opt))
      cerr << "Can't write to " << name << endl;
    return;
  }

  // OziExplorer format
  if ((testext(name, ".wpt"))||
      (testext(name, ".plt"))||
      (testext(name, ".map"))||
      (testext(name, ".zip"))||
      (testext(name, ".oe"))){
    string base(name.begin(), name.begin()+name.rfind('.'));
    if (v) cerr << "Writing to OziExplorer files: \n";
    vector<string> files;

    // number of files with tracks, waypoints and maps
    int wn = world.wpts.size();
    int tn = world.trks.size();
    int mn = world.maps.size();

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

      ofstream f(oef.str().c_str());
      if (!oe::write_wpt_file (f, world.wpts[n], opt))
        cerr << "Can't write " << oef.str() << endl;
      else{
        if (v) cerr << "  " << oef.str() << " -- "
                    << world.wpts[n].size() << " waypoints\n";
        files.push_back(oef.str());
      }
    }

    for (size_t n = 0; n < tn; ++n){
      ostringstream oef;
      if (tw > 0)
        oef << base << setw(tw) << setfill('0') << n+1 << ".plt";
      else
        oef << base << ".plt";

      ofstream f(oef.str().c_str());
      if (!oe::write_plt_file (f, world.trks[n], opt))
        cerr << "Can't write " << oef.str() << endl;
      else{
        if (v) cerr << "  " << oef.str() << " -- "
                    << world.trks[n].size() << " points\n";
        files.push_back(oef.str());
      }
    }

    for (size_t nn = 0; nn < mn; ++nn){
      int mmn = world.maps[nn].size(); // map count in this maplist
      int mmw=0;
      if (mmn > 1) mmw = (int) floor (log (1.0 * mmn) / log (10.0)) + 1;

      for (size_t n = 0; n != world.maps[nn].size(); ++n) {
        ostringstream oef;
        oef << base;
        if (mmw > 0)  oef << setw(mmw) << setfill('0') << nn+1;
        if ((mmw > 0) && (mw > 0))  oef << "_";
        if (mw > 0)  oef << setw(mw) << setfill('0') << n+1;
        oef << ".map";

        ofstream f(oef.str().c_str());
        if (!oe::write_map_file (f, world.maps[nn][n], opt))
          cerr << "Can't write " << oef.str() << endl;

        else {
          if (v) cerr << "  " << oef.str() << " -- "
                      << world.maps[n].size() << " reference points" << endl;
          files.push_back(oef.str());
        }
      }
    }

    if (testext (name, ".zip")){
      if (v) cerr << "Zipping files to " << name << endl;
      io_zip::write_file(name.c_str(), files);
    }

    return;
  }

  cerr << "Can't write geodata to file " << name << endl
       << "Use .xml, .gu, .wpt, .plt, .oe or .zip extension" << endl
       << "or name of serial port device (for example /dev/ttyS0)" << endl
       << " or \"gps:\" for garmin gps." << endl;
}
}  // namespace io


