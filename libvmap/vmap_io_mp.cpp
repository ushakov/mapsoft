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

bool write_mp(const std::string & file, const legend & leg, const world & w){
  mp::mp_world MP;
  return true;
}

bool read_mp(const std::string & file, const legend & leg, world & w){
  mp::mp_world MP;
  if (!mp::read(file.c_str(), MP)) return false;
//  w.opts=MP.Opts;
  w.comm=MP.Comment;
  w.opts.put("ID", MP.ID);
  w.opts.put("Name", MP.Name);

  for (mp::mp_world::const_iterator o=MP.begin(); o!=MP.end(); o++){

    // BC!! old-style keys
    mp::mp_object mo=*o; // copy for make clear_key
    zn::zn_key k=zn::get_key(mo);
    zn::clear_key(mo);
    id_t id=boost::lexical_cast<id_t>(k.id);  // BC!!
    id=o->Opts.get("ID", id);
    if ((id=="")||(id=="0")) id=make_id();

    vmap::object mapobj;
    mapobj.opts=o->Opts;
    mapobj.comm=mo.Comment;

    int type=leg.get_type(*o);
    if (type!=0) mapobj.opts.put("type", o->Label);

    if (o->Label !="") mapobj.opts.put("label", o->Label);

    int dir=mapobj.opts.get("DirIndicator",0);

    for (mp::mp_object::const_iterator s=o->begin(); s!=o->end(); s++)
      mapobj.data.push_back(dir!=2 ? *s:s->inv());

    while (w.objects.find(id)!=w.objects.end()){
      std::cerr << "map::read_mp: duplicated ID.\n";
      id=make_id();
    }

    w.objects[id]=mapobj;
  }
  return true;
}

} // namespace
