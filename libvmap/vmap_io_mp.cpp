#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

#include "vmap.h"
#include "../libmp/mp.h"

#include "../libzn/zn_key.h" // for backward compat. read old-style keys
#include <boost/lexical_cast.hpp>


using namespace std;
namespace vmap {

bool write_mp(const std::string & file, const world & w){
  mp::mp_world MP;
  MP.ID   = w.opts.get("ID", 0);
  MP.Name = w.opts.get("Name", string());
  MP.Comment=w.comm;

  string style=w.opts.get("Style", string("mmb"));
  MP.Opts.put("Style", style);

  legend leg(style);


  // TODO: rmap section!

  for (map<id_t, object>::const_iterator o=w.objects.begin(); o!=w.objects.end(); o++){

    mp::mp_object mpobj=leg.default_mp;
    string type=o->second.opts.get("type", string());

    legend::const_iterator t=leg.find(type);
    if (t!=leg.end()) mpobj=t->second.mp;

    mpobj.Opts.put("ID", o->first);
    mpobj.Comment=o->second.comm;

    for (MultiLine<double>::const_iterator s=o->second.data.begin(); s!=o->second.data.end(); s++)
      mpobj.push_back(*s);

    MP.push_back(mpobj);
  }
  ofstream of(file.c_str());
  if (!of){
    std::cerr << "map::write_mp: can't open file" << file << "\n";
    return false;
  }
  return mp::write(of, MP);
}

bool read_mp(const std::string & file, world & w){
  mp::mp_world MP;
  if (!mp::read(file.c_str(), MP)) return false;
  w.comm=MP.Comment;
  w.opts.put("ID", MP.ID);
  w.opts.put("Name", MP.Name);

  string style=MP.Opts.get("Style", string("mmb"));
  w.opts.put("Style", style);

  legend leg(style);

  // TODO: rmap section!

  for (mp::mp_world::iterator o=MP.begin(); o!=MP.end(); o++){

    // BC!! old-style keys
    zn::zn_key k=zn::get_key(*o);
    zn::clear_key(*o);
    id_t id=boost::lexical_cast<id_t>(k.id) + "@" + k.map;  // BC!!

    // new-style ID
    id=o->Opts.get("ID", id);
    if ((id=="")||(id=="0")) id=make_id();

    while (w.objects.find(id)!=w.objects.end()){
      std::cerr << "map::read_mp: duplicated ID.\n";
      id=make_id();
    }

    vmap::object mapobj;
    mapobj.comm=o->Comment;

    string type=leg.get_type(*o);
    if (type!="") mapobj.opts.put("type", type);
    else std::cerr << "map::read_mp: Unknown type for MP " 
                   << o->Class << " " << hex << o->Type << "\n";

    if (o->Label !="") mapobj.opts.put("label", o->Label);

    int dir=mapobj.opts.get("DirIndicator",0);

    for (mp::mp_object::const_iterator s=o->begin(); s!=o->end(); s++)
      mapobj.data.push_back(dir!=2 ? *s:s->inv());

    w.objects[id]=mapobj;
  }
  return true;
}

} // namespace