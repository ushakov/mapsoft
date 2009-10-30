#include "vmap.h"
#include "../../core/lib2d/line_rectcrop.h"

using namespace std;
namespace vmap{
void vmap::add(const vmap & M1){

  map<string,string> id_tr;
  for (map<string, mobj_t>::const_iterator o=M1.mobjs.begin(); o!=M1.mobjs.end(); o++){
    if (mobjs.count(o->first) < 1) mobjs[o->first]=o->second;
    else {
      // create new id for the object
      string new_id=make_id();
      id_tr[o->first]=new_id;
      mobjs[new_id]=o->second;
    }
  }

  for (map<string, rmap_t>::const_iterator o=M1.rmaps.begin(); o!=M1.rmaps.end(); o++){
    if (rmaps.count(o->first) == 0) rmaps[o->first]=o->second;
  }

  for (map<string, lpos_t>::const_iterator o=M1.lpos.begin(); o!=M1.lpos.end(); o++){
    for (lpos_t::const_iterator p=o->second.begin(); p!=o->second.end(); p++){
      string oid=p->first;
      if (id_tr.count(oid)>0) oid=id_tr[oid];
      lpos[o->first].insert(pair<string,lp_t>(oid, p->second));
    }
  }

}

void vmap::cut_inner(const dRect & cutter, bool erase_lpos){
  map<string, mobj_t>::iterator o=mobjs.begin();

  while (o!=mobjs.end()){

    bool rem=false;
    for (dMultiLine::iterator i  = o->second.begin();
                              i != o->second.end(); i++){
      if (rect_crop_test(cutter, *i, true)){
        rem=true;
        break;
      }
    }
    if (rem){
      string id=o->first;
      o++;
      if (erase_lpos){
        for (map<string,lpos_t>::iterator i  = lpos.begin();
                                          i != lpos.end(); i++)
          i->second.erase(id);
      }
      mobjs.erase(id);
    }
    else o++;
  }
}

}//namespace