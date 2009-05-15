#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

#include "vmap.h"

using namespace std;
namespace vmap {

bool write_dir(const string & dir, const world & w){

  // create new directories or fail
  if (mkdir(dir.c_str(), 0755)  ||
      mkdir((dir + "/objects").c_str(), 0755) ||
      mkdir((dir + "/rmaps").c_str(), 0755)){
    cerr << "map::write_dir: can't create map directory in " << dir << "\n";
    return false;
  }

  ofstream fi((dir + "/info").c_str());
  if (!fi){
    cerr << "map::write_dir: error while writing map info file\n";
    return false;
  }
  else fi<<w;
  fi.close();

  for (map<id_t, object>::const_iterator i=w.objects.begin(); i!=w.objects.end(); i++){
    ofstream f((dir + "/objects/" + i->first).c_str());
    if (!f){
      cerr << "map::write_dir: error while writing map object" << i->first << "\n";
    }
    else f << i->second;
    f.close();
  }

  for (map<id_t, rmap>::const_iterator i=w.rmaps.begin(); i!=w.rmaps.end(); i++){

    ofstream f((dir + "/rmaps/" + i->first).c_str());
    if (!f){
      cerr << "map::write_dir: error while writing rmap " << i->first << "\n";
    }
    else f << i->second;
    f.close();
  }
  return true;
}

bool read_dir(const string & dir, world & w){

  ifstream fi((dir+"/info").c_str());
  if (!fi){
    cerr << "map::read_dir: error while reading map info file\n";
    return false;
  }
  fi>>w;

  DIR *d;
  struct dirent *de;

  d=opendir((dir+"/objects").c_str());
  while ((de=readdir(d))!=NULL){
    if ((de->d_name==NULL) || (de->d_name[0]=='.')) continue;
    ifstream f((dir+"/objects/"+de->d_name).c_str());
    object o;
    if (!f){
      cerr << "map::read_dir: error while reading map object " << de->d_name << "\n";
    }
    else {
      f>>o;
      w.objects[de->d_name]=o;
    }
  }
  closedir(d);

  d=opendir((dir+"/rmaps").c_str());
  while ((de=readdir(d))!=NULL){
    if ((de->d_name==NULL) || (de->d_name[0]=='.')) continue;
    ifstream f((dir+"/rmaps/"+de->d_name).c_str());
    rmap m;
    if (!f){
      cerr << "map::read_dir: error while reading rmap " << de->d_name << "\n";
    }
    else {
      f>>m;
      w.rmaps[de->d_name]=m;
    }
  }
  closedir(d);
  return true;
}

} // namespace
