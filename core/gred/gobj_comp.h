#ifndef GRED_GOBJ_COMP_H
#define GRED_GOBJ_COMP_H

#include <vector>
#include "gred/gobj.h"

class GObjComp : public GObj, public std::vector<GObj *> {
public:

  int draw(iImage &img, const iPoint &origin){
    std::vector<GObj *>::const_iterator i;
    for (i=begin(); i!=end(); i++) (*i)->draw(img, origin);
  }

  void set_cnv(Conv * c, int hint) {
    cnv = c; cnv_hint=hint;
    std::vector<GObj *>::const_iterator i;
    for (i=begin(); i!=end(); i++) (*i)->set_cnv(cnv, cnv_hint);
    refresh();
  }

  void refresh() {
    std::vector<GObj *>::const_iterator i;
    for (i=begin(); i!=end(); i++) (*i)->refresh();
  }

};

#endif
