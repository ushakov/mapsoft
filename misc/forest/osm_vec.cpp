#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <cmath>

#include <yaml.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

#include "mapsoft.h"
#include <libshp/shapefil.h>

// OSM data
// downloaded from https://download.bbbike.org/
// as shape files

using namespace std;

void usage(){
    cerr << "usage: \n"
     " osm_vec <osm_dir> <cfg file> <out file>\n";
    exit(0);
}

/************************************************/
// read configuration

class osm_file_cfg {
  public:
  string name;
  map<string, int> types;
};

vector<osm_file_cfg>
read_osm_conf(const string & conf_file){

  // States for our YAML parser
  typedef enum {
    N,
    FILE_KEY,
    FILE_VAL,
    TYPE_KEY,
    TYPE_VAL
  } reading_state_t;

  reading_state_t state=N;
  bool done = false;

  yaml_parser_t parser;
  yaml_event_t event;

  vector<int> history;
  string key, val;

  vector<osm_file_cfg> ret;
  osm_file_cfg osmf;

  // read config file
  FILE *file = fopen(conf_file.c_str(), "r");

  if (!file)
    throw MapsoftErr("") << "Error while reading " << conf_file << ": "
                         << "can't open the file";

  if (!yaml_parser_initialize(&parser)){
    fclose(file);
    throw MapsoftErr("") << "Error while reading " << conf_file << ": "
                         << "can't initialize YAML parser";
  }

  yaml_parser_set_input_file(&parser, file);

  while(!done) {
    if (!yaml_parser_parse(&parser, &event)){
      yaml_parser_delete(&parser);
      fclose(file);
      throw MapsoftErr("") << "Error while reading " << conf_file << ": "
                           << parser.problem << " at line "
                           << parser.problem_mark.line+1;
    }

    try {  // on all errors in this block we want to delete event and parser, close file

      if (event.type == YAML_NO_EVENT)
        throw MapsoftErr("") << "Error while reading " << conf_file << ": "
                             "no_event at line " << parser.problem_mark.line+1;

      if ((event.type == YAML_STREAM_START_EVENT) ||
          (event.type == YAML_DOCUMENT_START_EVENT) ||
          (event.type == YAML_SEQUENCE_START_EVENT) ||
          (event.type == YAML_MAPPING_START_EVENT) ){
        history.push_back(event.type);
      }

      if ((event.type == YAML_STREAM_END_EVENT) ||
          (event.type == YAML_DOCUMENT_END_EVENT) ||
          (event.type == YAML_SEQUENCE_END_EVENT) ||
          (event.type == YAML_MAPPING_END_EVENT) ){
        if ((history.size()<=0) || (history[history.size()-1] != event.type-1))
          throw MapsoftErr("") << "Error while reading " << conf_file << ": "
                               << "unmatched stop event at line " << event.start_mark.line+1;
        history.pop_back();
      }

      /*
      #define pr(t) if (event.type == t) cerr << #t << "\n";
      pr(YAML_STREAM_START_EVENT);
      pr(YAML_NO_EVENT);
      pr(YAML_STREAM_START_EVENT);
      pr(YAML_STREAM_END_EVENT);
      pr(YAML_DOCUMENT_START_EVENT);
      pr(YAML_DOCUMENT_END_EVENT);
      pr(YAML_ALIAS_EVENT);
      pr(YAML_SCALAR_EVENT);
      pr(YAML_SEQUENCE_START_EVENT);
      pr(YAML_SEQUENCE_END_EVENT);
      pr(YAML_MAPPING_START_EVENT);
      pr(YAML_MAPPING_END_EVENT);
      */

      if (state == N && event.type == YAML_SCALAR_EVENT) {
        osmf = osm_file_cfg();
        osmf.name.insert(0, (const char *)event.data.scalar.value, event.data.scalar.length);
        state = FILE_VAL;
      }

      else if (state == FILE_VAL){
        if (event.type != YAML_MAPPING_START_EVENT)
          throw MapsoftErr("") << "Error while reading " << conf_file << ": "
                               << "mapping expected at line " << event.start_mark.line+1;
        state = TYPE_KEY;
      }

      else if (state == TYPE_KEY){
        if (event.type == YAML_MAPPING_END_EVENT) {
          ret.push_back(osmf);
          state = N;
        }
        else if (event.type == YAML_SCALAR_EVENT) {
          key.clear();
          key.insert(0, (const char *)event.data.scalar.value, event.data.scalar.length);
          state = TYPE_VAL;
        }
        else
          throw MapsoftErr("") << "Error while reading " << conf_file << ": "
                               << "scalar expected at line " << event.start_mark.line+1;
      }

      else if (state == TYPE_VAL){
        if (event.type != YAML_SCALAR_EVENT)
          throw MapsoftErr("") << "Error while reading " << conf_file << ": "
                               << "scalar expected at line " << event.start_mark.line+1;
        val.clear();
        val.insert(0, (const char *)event.data.scalar.value, event.data.scalar.length);
        istringstream str(val);
        int i;
        str >> hex >> i;
        if (str.good() || str.eof()) osmf.types[key] = i;

        state = TYPE_KEY;
      }

    }
    catch (MapsoftErr e){
       yaml_event_delete(&event);
       yaml_parser_delete(&parser);
       fclose(file);
       throw e;
    }

    if (event.type == YAML_STREAM_END_EVENT) done=true;
    yaml_event_delete(&event);
  }

  yaml_parser_delete(&parser);
  fclose(file);
  return ret;
}





int
main(int argc, char** argv){
  if (argc < 4) usage();

  std::string fdir = argv[1];
  std::string fcfg = argv[2];
  std::string fout = argv[3];

  vmap::world MAP;

  try{
    vector<osm_file_cfg> cfg = read_osm_conf(fcfg);

    // iterate through all files in configuration
    vector<osm_file_cfg>::const_iterator ff;
    for (ff=cfg.begin(); ff!=cfg.end(); ff++){

      // open shape-file
      string dbname = fdir+"/"+ff->name;
      SHPHandle sh  = SHPOpen( dbname.c_str(), "rb");
      if (sh==NULL) throw MapsoftErr() << "can't open shape file: " << (dbname+".shp");
      DBFHandle dbf = DBFOpen( (dbname+".dbf").c_str(), "rb");
      if (dbf==NULL) throw MapsoftErr() << "can't open dbf file: " << (dbname+".dbf");

      // find numbers for type and name fields
      int n_type = 0;
      int n_name = 0;
      for (int i=0; i<DBFGetFieldCount(dbf); i++){
        char fn[12];
        int w,d;
        DBFFieldType t = DBFGetFieldInfo(dbf, i, fn, &w, &d);
        if (strcmp("type", fn)==0) n_type = i;
        if (strcmp("name", fn)==0) n_name = i;
      }

      // get number of objects and object type
      int num, type;
      double mmin[4],mmax[4];
      SHPGetInfo(sh, &num, &type, mmin, mmax);

      // iterate through objects in the file
      for (int i=0; i<num; i++){
        SHPObject *o = SHPReadObject(sh, i);

        // create a vmap object
        vmap::object vobj;
        if (o->nParts == 0){
          dLine l;
            for (int j=0; j<o->nVertices; j++)
              l.push_back(dPoint(o->padfX[j],o->padfY[j]));
            vobj.push_back(l);
        }
        else {
          for (int p = 0; p<o->nParts; p++){
            dLine l;
            int j1 = o->panPartStart[p];
            int j2 = (p==o->nParts-1 ? o->nVertices : o->panPartStart[p+1]);
            for (int j=j1; j<j2; j++) l.push_back(dPoint(o->padfX[j],o->padfY[j]));
            vobj.push_back(l);
          }
        }
        join_polygons1(vobj);
        generalize(vobj, 1e-4);

        // get type and name
        string type = DBFReadStringAttribute(dbf, i, n_type);
        string name = DBFReadStringAttribute(dbf, i, n_name);

        if (vobj.size() && ff->types.count(type)){
          vobj.type = ff->types.find(type)->second;
          vobj.text = name;
          MAP.push_back(vobj);
        }
        else {
          cerr << "unknown type in " << ff->name << ": " << type << "\n";
        }
        SHPDestroyObject(o);
      }
    }

    // save vmap into a file
    vmap::write(fout.c_str(), MAP);
  }
  catch (MapsoftErr e){
    std::cerr << "Error: " << e.str() << "\n";
  }
}
