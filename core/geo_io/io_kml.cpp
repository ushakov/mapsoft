#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <vector>
#include <string>
#include <map>
#include <libxml/xmlreader.h>

#include "io_kml.h"

#include <math.h>

#include "utils/err.h"


namespace kml {
using namespace std;

// Записывает в KML-файл треки и точки
// Не записывает карты! (хм, а может, надо?)
void write_file (const char* filename, const geo_data & world, const Options & opt){

  if (opt.exists("verbose")) cerr <<
    "Writing data to KML file " << filename << endl;

  ofstream f(filename);
  if (!f.good()) throw MapsoftErr("GEO_IO_KML_OPENW")
                << "Can't open KML file " << filename << " for writing";

  f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  f << "<kml xmlns=\"http://earth.google.com/kml/2.1\">" << endl;
  f << "  <Document>" << endl;

  for (int i = 0; i < world.wpts.size(); i++) {
    f << "  <Folder>" << endl;
    f << "    <name>WPTS_" << i << "</name>" << endl;
    g_waypoint_list::const_iterator wp;
    for (wp = world.wpts[i].begin(); wp != world.wpts[i].end(); ++wp) {
      f << "    <Placemark>" << endl;
      f << "      <name><![CDATA[" << wp->name << "]]></name>" << endl;
      f << "      <description><![CDATA[" << wp->comm << "]]></description>" << endl;
      f << "      <Point>" << endl;
      f << fixed << setprecision(6)
        << "        <coordinates>" << wp->x << "," << wp->y << ","
        <<  setprecision(1) << wp->z << "</coordinates>" << endl;
      f << "      </Point>" << endl;
      f << "    </Placemark>" << endl;
    }
    f << "  </Folder>" << endl;
  }

  for (int i = 0; i < world.trks.size(); ++i) {
    f << "  <Placemark>" << endl;
    f << "    <description><![CDATA[" << world.trks[i].comm << "]]></description>" << endl;
    f << "    <MultiGeometry>" << endl;
    g_track::const_iterator tp;
    for (tp = world.trks[i].begin(); tp != world.trks[i].end(); ++tp) {
      if (tp->start || tp == world.trks[i].begin()) {
        if (tp != world.trks[i].begin()) {
          f << "        </coordinates>" << endl;
          f << "      </LineString>" << endl;
        }
        f << "      <LineString>" << endl;
        f << "        <tessellate>1</tessellate>" << endl;
        f << "        <coordinates>" << endl;
      }
      f << fixed << setprecision(6)
        << "          " << tp->x << "," << tp->y << ","
        <<  setprecision(1) << tp->z << endl;
    }
    f << "        </coordinates>" << endl;
    f << "      </LineString>" << endl;
    f << "    </MultiGeometry>" << endl;
    f << "  </Placemark>" << endl;
  }

  f << "  </Document>" << endl;
  f << "</kml>" << endl;

  if (!f.good()) throw MapsoftErr("GEO_IO_KML_WRITE")
                  << "Can't write data to KML file " << filename;
}

#define TYPE_ELEM      1
#define TYPE_ELEM_END 15
#define TYPE_TEXT      3
#define TYPE_CDATA     4
#define TYPE_SWS      14

#define NAMECMP(x) (xmlStrcasecmp(name,(const xmlChar *)x)==0)
#define GETATTR(x) (const char *)xmlTextReaderGetAttribute(reader, (const xmlChar *)x)
#define GETVAL     (const char *)xmlTextReaderConstValue(reader)

int
read_text_node(xmlTextReaderPtr reader, const char * nn, string & str){
  int ret=1;
  str.clear();
  while(1){
    ret =xmlTextReaderRead(reader);
    if (ret != 1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (type == TYPE_TEXT || type == TYPE_CDATA) str += GETVAL;
    else if (NAMECMP(nn) && (type == TYPE_ELEM_END)) break;
    else cerr << "Warning: Unknown node \"" << name << "\" in text node (type: " << type << ")\n";
  }
  return ret;
}

int
read_point_node(xmlTextReaderPtr reader, g_waypoint & ww){
  int ret=1;
  while(1){
    ret =xmlTextReaderRead(reader);
    if (ret != 1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("coordinates") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "coordinates", str);
      if (ret != 1) break;
      char s1,s2;
      istringstream s(str);
      s >> std::ws >> ww.x >> std::ws >> s1 >>
           std::ws >> ww.y >> std::ws >> s2 >>
           std::ws >> ww.z >> std::ws;
      if (s1!=',' || s2!=','){
        cerr << "Warning: Coord error\n";
        ret=0;
        break;
      }
    }
    else if (NAMECMP("Point") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in Point (type: " << type << ")\n";
    }
  }
  return ret;
}

int
read_linestring_node(xmlTextReaderPtr reader, g_track & T){
  int ret=1;
  while(1){
    ret =xmlTextReaderRead(reader);
    if (ret != 1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("tessellate") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "tessellate", str);
      if (ret != 1) break;
    }
    else if (NAMECMP("coordinates") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "coordinates", str);
      if (ret != 1) break;
      char s1,s2;
      istringstream s(str);
      g_trackpoint tp;
      tp.start=true;
      while (!s.eof()){
        s >> std::ws >> tp.x >> std::ws >> s1 >>
             std::ws >> tp.y >> std::ws >> s2 >>
             std::ws >> tp.z >> std::ws;
        if (s1!=',' || s2!=','){
          cerr << "Warning: Coord error\n";
          break;
        }
        T.push_back(tp);
        tp.start=false;
      }
    }
    else if (NAMECMP("LineString") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in LineString (type: " << type << ")\n";
    }
  }
  return ret;
}

int
read_gx_track_node(xmlTextReaderPtr reader, g_track & T){
  int ret=1;
  T = g_track();
  while(1){
    ret =xmlTextReaderRead(reader);
    if (ret != 1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("when") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "when", str);
      if (ret != 1) break;
    }
    else if (NAMECMP("gx:coord") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "gx:coord", str);
      if (ret != 1) break;
      istringstream s(str);
      g_trackpoint tp;
      s >> std::ws >> tp.x >> std::ws
        >> std::ws >> tp.y >> std::ws
        >> std::ws >> tp.z >> std::ws;
      T.push_back(tp);
    }
    else if (NAMECMP("gx:Track") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in gx:Track (type: " << type << ")\n";
    }
  }
  if (T.size()) T[0].start=true;
  return ret;
}

int
read_multigeometry_node(xmlTextReaderPtr reader, g_track & T){
  int ret=1;
  while(1){
    ret =xmlTextReaderRead(reader);
    if (ret != 1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("LineString") && (type == TYPE_ELEM)){
      ret=read_linestring_node(reader, T);
      if (ret != 1) break;
    }
    else if (NAMECMP("MultiGeometry") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in MultiGeometry (type: " << type << ")\n";
    }
  }
  return ret;
}

int
read_placemark_node(xmlTextReaderPtr reader,
     g_waypoint_list & W, g_track & T, g_map_list & M){

  g_waypoint ww;
  g_map mm;
  T = g_track();

  int ot=-1;
  bool skip=false;
  int ret=1;

  while(1){
    ret =xmlTextReaderRead(reader);
    if (ret != 1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    // Skip some blocks
    else if (NAMECMP("Camera") || NAMECMP("LookAt") ||
             NAMECMP("styleUrl") || NAMECMP("visibility")){
      if (type == TYPE_ELEM) skip=true;
      if (type == TYPE_ELEM_END) skip=false;
    }
    else if (skip) continue;
    else if (NAMECMP("name") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "name", str);
      if (ret != 1) break;
      ww.name += str;
      T.comm += str;
      mm.comm += str;
    }
    else if (NAMECMP("description") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "description", str);
      if (ret != 1) break;
      ww.comm += str;
      T.comm += str;
      mm.comm += str;
    }
//    else if (NAMECMP("TimeStamp") && (type == TYPE_ELEM)){
//    }
    else if (NAMECMP("Point") && (type == TYPE_ELEM)){
      ot=0;
      ret=read_point_node(reader, ww);
      if (ret != 1) break;
    }
    else if (NAMECMP("LineString") && (type == TYPE_ELEM)){
      ot=1;
      ret=read_linestring_node(reader, T);
      if (ret != 1) break;
    }
    else if (NAMECMP("MultiGeometry") && (type == TYPE_ELEM)){
      ot=1;
      ret=read_multigeometry_node(reader, T);
      if (ret != 1) break;
    }
    else if (NAMECMP("gx:Track") && (type == TYPE_ELEM)){
      ot=1;
      ret=read_gx_track_node(reader, T);
      if (ret != 1) break;
    }
    else if (NAMECMP("Placemark") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in Placemark (type: " << type << ")\n";
    }
  }
  if (ot==0) W.push_back(ww);
  return ret;
}



int
read_folder_node(xmlTextReaderPtr reader, geo_data & world){ // similar to document node
  g_waypoint_list W;
  g_track         T;
  g_map_list      M;
  bool skip=false;
  int ret=1;
  while(1){
    ret =xmlTextReaderRead(reader);
    if (ret != 1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("visibility") || NAMECMP("open")){
      if (type == TYPE_ELEM) skip=true;
      if (type == TYPE_ELEM_END) skip=false;
    }
    else if (skip) continue;
    else if (NAMECMP("name") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "name", str);
      if (ret != 1) break;
      W.comm = T.comm = M.comm = str;
    }
    else if (NAMECMP("Placemark") && (type == TYPE_ELEM)){
      ret=read_placemark_node(reader, W, T, M);
      if (T.size()) world.trks.push_back(T);
      if (ret != 1) break;
    }
    else if (NAMECMP("Folder") && (type == TYPE_ELEM)){
      ret=read_folder_node(reader, world);
      if (ret != 1) break;
    }
    else if (NAMECMP("Folder") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in Folder (type: " << type << ")\n";
    }
  }
  if (W.size()) world.wpts.push_back(W);
  if (M.size()) world.maps.push_back(M);
  return ret;
}

int
read_document_node(xmlTextReaderPtr reader, geo_data & world){
  bool skip=false;
  g_waypoint_list W;
  g_track         T;
  g_map_list      M;
  int ret=1;
  while(1){
    ret =xmlTextReaderRead(reader);
    if (ret != 1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    // Skip some blocks
    else if (NAMECMP("Style") || NAMECMP("StyleMap") ||
             NAMECMP("open") || NAMECMP("visibility") ||
             NAMECMP("name") || NAMECMP("Snippet") ||
             NAMECMP("LookAt")){
      if (type == TYPE_ELEM) skip=true;
      if (type == TYPE_ELEM_END) skip=false;
    }
    else if (skip) continue;
    else if (NAMECMP("name") && (type == TYPE_ELEM)){
      string str;
      ret=read_text_node(reader, "name", str);
      if (ret != 1) break;
      W.comm = T.comm = M.comm = str;
    }
    else if (NAMECMP("Placemark") && (type == TYPE_ELEM)){
      ret=read_placemark_node(reader, W, T, M);
      if (ret != 1) break;
    }
    else if (NAMECMP("Folder") && (type == TYPE_ELEM)){
      ret=read_folder_node(reader, world);
      if (ret != 1) break;
    }
    else if (NAMECMP("Document") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in Document (type: " << type << ")\n";
    }
  }
  if (W.size()) world.wpts.push_back(W);
  if (T.size()) world.trks.push_back(T);
  if (M.size()) world.maps.push_back(M);
  return ret;
}

int
read_kml_node(xmlTextReaderPtr reader, geo_data & world){
  while(1){
    int ret =xmlTextReaderRead(reader);
    if (ret != 1) return ret;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);

    if (type == TYPE_SWS) continue;
    else if (NAMECMP("Document") && (type == TYPE_ELEM)){
      ret=read_document_node(reader, world);
      if (ret != 1) return ret;
    }
    else if (NAMECMP("kml") && (type == TYPE_ELEM_END)){
      break;
    }
    else {
      cerr << "Warning: Unknown node \"" << name << "\" in kml (type: " << type << ")\n";
    }
  }
  return 1;
}


void
read_file(const char* filename, geo_data & world, const Options & opt) {

  LIBXML_TEST_VERSION

  if (opt.exists("verbose")) cerr <<
    "Reading data from KML file " << filename << endl;

  xmlTextReaderPtr reader;
  int ret;

  reader = xmlReaderForFile(filename, NULL, 0);
  if (reader == NULL) throw MapsoftErr("GEO_IO_KML_OPENR") <<
    "Can't open file " << filename << " for reading";

  // parse file
  while (1){
    ret = xmlTextReaderRead(reader);
    if (ret!=1) break;

    const xmlChar *name = xmlTextReaderConstName(reader);
    int type = xmlTextReaderNodeType(reader);
    if (NAMECMP("kml") && (type == TYPE_ELEM))
      ret = read_kml_node(reader, world);
    if (ret!=1) break;
  }

  // free resources
  xmlFreeTextReader(reader);
  if (ret != 0) throw MapsoftErr("GEO_IO_KML_READ") <<
    "Can't parse KML file " << filename;

  xmlCleanupParser();
  xmlMemoryDump();
}
} // namespace
