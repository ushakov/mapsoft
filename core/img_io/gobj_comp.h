#ifndef GOBJ_COMP_H
#define GOBJ_COMP_H

#include <vector>
#include "gobj_geo.h"

class GObjComp : public GObjGeo, public std::vector<GObjGeo *> {
public:

  void connect_signals(){
    // connect signal_redraw_me signals from all objects
    std::vector<GObjGeo *>::iterator i;
    sigc::signal<void, iRect> & sig = signal_redraw_me();
    for (i=begin(); i!=end(); i++)
      (*i)->signal_redraw_me().connect(
        sigc::mem_fun (&sig, &sigc::signal<void, iRect>::emit));
  }

  int draw(iImage &img, const iPoint &origin){
    std::vector<GObjGeo *>::const_iterator i;
    for (i=begin(); i!=end(); i++) (*i)->draw(img, origin);
  }

  void set_ref(const g_map & ref) {
    GObjGeo::set_ref(ref);
    std::vector<GObjGeo *>::const_iterator i;
    for (i=begin(); i!=end(); i++) (*i)->set_ref(ref);
  }

  void refresh() {
    std::vector<GObjGeo *>::const_iterator i;
    for (i=begin(); i!=end(); i++) (*i)->refresh();
  }

};

#endif
