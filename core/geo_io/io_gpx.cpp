#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "io_gpx.h"
#include <libxml/xmlreader.h>
#include <cstring>
#include "utils/err.h"

namespace gpx {

using namespace std;

// GPX time: 2011-09-27T18:50:03Z

Time
time_gpx2s(const char * str){
  return boost::lexical_cast<Time>(str);
}
string
time_s2gpx(const Time & t){
  ostringstream s;
  struct tm * ts = localtime(&t.value);
  if (ts == NULL) { time_t t = 0;  ts = localtime(&t);}
  s << std::setfill('0')
    << std::setw(4) << ts->tm_year+1900 << '-'
    << std::setw(2) << ts->tm_mon+1 << '-'
    << std::setw(2) << ts->tm_mday  << 'T'
    << std::setw(2) << ts->tm_hour  << ':'
    << std::setw(2) << ts->tm_min   << ':'
    << std::setw(2) << ts->tm_sec << 'Z';
  return s.str();
}

void
write_file (const char* filename, const geo_data & world, const Options & opt){
  ofstream f(filename);

  if (opt.exists("verbose")) cerr <<
    "Writing data to GPX file " << filename << endl;

  if (!f.good()) throw MapsoftErr("GEO_IO_GPX_OPENW") <<
    "Can't open file " << filename << " for writing";

  f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  f << "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\">" << endl;

  for (int i = 0; i < world.wpts.size(); i++) {
    for (g_waypoint_list::const_iterator wp = world.wpts[i].begin();
                                         wp != world.wpts[i].end(); ++wp) {
       f << fixed << setprecision(6)
         << "<wpt lat=\"" << wp->y
         << "\" lon=\""  << wp->x << "\">" << endl
         << setprecision(1)
         << "   <ele>"  << wp->z << "</ele>" << endl
         << "   <name>" << wp->name << "</name>" << endl
         << "   <cmt>"  << wp->comm << "</cmt>" << endl
         << "   <time>" << time_s2gpx(wp->t) << "</time>" << endl
         << "</wpt>" << endl;
    }
  }

  for (int i = 0; i < world.trks.size(); ++i) {
    f << "  <trk>" << endl;
    f << "    <name>" << world.trks[i].comm << "</name>" << endl;
    for (g_track::const_iterator tp = world.trks[i].begin(); tp != world.trks[i].end(); ++tp) {
      if (tp->start || tp == world.trks[i].begin()) {
        if (tp != world.trks[i].begin()) {
          f << "    </trkseg>" << endl;
        }
        f << "    <trkseg>" << endl;
      }
      f << fixed << setprecision(6)
        << "      <trkpt lat=\"" << tp->y << "\" lon=\"" << tp->x << "\">" << endl
        << setprecision(1)
        << "        <ele>" << tp->z << "</ele>" << endl
        << "        <time>" << time_s2gpx(tp->t) << "</time>" << endl
        << "      </trkpt>" << endl;
    }
    f << "    </trkseg>" << endl;
    f << "  </trk>" << endl;
  }
  f << "</gpx>" << endl;

  if (!f.good()) throw MapsoftErr("GEO_IO_GPX_WRITE") <<
    "Can't write to file " << filename;
}


#define TYPE_ELEM      1
#define TYPE_ELEM_END 15
#define TYPE_TEXT      3
#define TYPE_SWS      14

#define NAMECMP(x) (xmlStrcasecmp(name,(const xmlChar *)x)==0)
#define GETATTR(x) (const char *)xmlTextReaderGetAttribute(reader, (const xmlChar *)x)
#define GETVAL     (const char *)xmlTextReaderConstValue(reader)
int
read_wpt_node(xmlTextReaderPtr reader, geo_data & world){
  g_waypoint wpt;
  bool is_ele = false, is_name=false, is_comm=false, is_time=false;

  wpt.y = atof(GETATTR("lat"));
  wpt.x = atof(GETATTR("lon"));

  while(1){
    int ret =xmlTextReaderRead(reader);
    if (ret != 1) return ret;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("ele")){
      if (type == TYPE_ELEM) is_ele = true;
      if (type == TYPE_ELEM_END) is_ele = false;
    }
    else if (NAMECMP("name")){
      if (type == TYPE_ELEM) is_name = true;
      if (type == TYPE_ELEM_END) is_name = false;
    }
    else if (NAMECMP("comm")){
      if (type == TYPE_ELEM) is_comm = true;
      if (type == TYPE_ELEM_END) is_comm = false;
    }
    else if (NAMECMP("time")){
      if (type == TYPE_ELEM) is_time = true;
      if (type == TYPE_ELEM_END) is_time = false;
    }

    else if (type == TYPE_TEXT){
      if (is_ele)
        wpt.z = atof(GETVAL);
      if (is_name)
        wpt.name = GETVAL;
      if (is_comm)
        wpt.comm = GETVAL;
      if (is_time)
        wpt.t = time_gpx2s(GETVAL);
    }

    else if (NAMECMP("wpt") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in wpt (type: " << type << ")\n";
    }
  }
  if (world.wpts.size()<1) world.wpts.push_back(g_waypoint_list());
  world.wpts.back().push_back(wpt);
  return 1;
}

int
read_trkpt_node(xmlTextReaderPtr reader, g_track & trk, bool start){
  g_trackpoint pt;
  bool is_ele = false, is_time=false;

  pt.y = atof(GETATTR("lat"));
  pt.x = atof(GETATTR("lon"));

  while(1){
    int ret =xmlTextReaderRead(reader);
    if (ret != 1) return ret;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("ele")){
      if (type == TYPE_ELEM) is_ele = true;
      if (type == TYPE_ELEM_END) is_ele = false;
    }
    else if (NAMECMP("time")){
      if (type == TYPE_ELEM) is_time = true;
      if (type == TYPE_ELEM_END) is_time = false;
    }
    else if (type == TYPE_TEXT){
      if (is_ele)
        pt.z = atof(GETVAL);
      if (is_time)
        pt.t = time_gpx2s(GETVAL);
    }

    else if (NAMECMP("trkpt") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in trkpt (type: " << type << ")\n";
    }
  }
  pt.start = start;
  trk.push_back(pt);
  return 1;
}


int
read_trkseg_node(xmlTextReaderPtr reader, g_track & trk){
  bool start=true;
  while(1){
    int ret =xmlTextReaderRead(reader);
    if (ret != 1) return ret;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("trkpt") && (type == TYPE_ELEM)){
      ret = read_trkpt_node(reader, trk, start);
      if (ret != 1) return ret;
      start=false;
    }
    else if (NAMECMP("trkseg") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in trkseg (type: " << type << ")\n";
    }
  }
  return 1;
}

int
read_trk_node(xmlTextReaderPtr reader, geo_data & world){
  g_track trk;
  bool is_name=false;

  while(1){
    int ret =xmlTextReaderRead(reader);
    if (ret != 1) return ret;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("trkseg") && (type == TYPE_ELEM)){
      ret=read_trkseg_node(reader, trk);
      if (ret != 1) return ret;
    }
    else if (NAMECMP("name")){
     if (type == TYPE_ELEM) is_name = true;
     if (type == TYPE_ELEM_END) is_name = false;
    }
    else if (type == TYPE_TEXT){
      if (is_name) trk.comm = GETVAL;
    }
    else if (NAMECMP("trk") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in trk (type: " << type << ")\n";
    }
  }
  world.trks.push_back(trk);
  return 1;
}

int
read_gpx_node(xmlTextReaderPtr reader, geo_data & world){
  bool is_meta=false;
  while(1){
    int ret =xmlTextReaderRead(reader);
    if (ret != 1) return ret;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("metadata")){
      if (type == TYPE_ELEM) is_meta=true;
      if (type == TYPE_ELEM_END) is_meta=false;
    }
    else if (is_meta) continue;
    else if (NAMECMP("wpt") && (type == TYPE_ELEM)){
      ret=read_wpt_node(reader, world);
      if (ret != 1) return ret;
    }
    else if (NAMECMP("trk") && (type == TYPE_ELEM)){
      ret=read_trk_node(reader, world);
      if (ret != 1) return ret;
    }
    else if (NAMECMP("gpx") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in gpx (type: " << type << ")\n";
    }
  }
  return 1;
}

void
read_file(const char* filename, geo_data & world, const Options & opt) {

  LIBXML_TEST_VERSION

  if (opt.exists("verbose")) cerr <<
    "Reading data from GPX file " << filename << endl;

  xmlTextReaderPtr reader;
  int ret;

  reader = xmlReaderForFile(filename, NULL, 0);
  if (reader == NULL) throw MapsoftErr("GEO_IO_GPX_OPENR") <<
    "Can't open file " << filename << " for reading";

  // parse file
  while (1){
    ret = xmlTextReaderRead(reader);
    if (ret!=1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);
    if (NAMECMP("gpx") && (type == TYPE_ELEM))
      ret = read_gpx_node(reader, world);
    if (ret!=1) break;
  }

  // free resources
  xmlFreeTextReader(reader);

  if (ret != 0) throw MapsoftErr("GEO_IO_GPX_READ") <<
    "Can't parse GPX file " << filename;

  xmlCleanupParser();
  xmlMemoryDump();
}

} // namespace
