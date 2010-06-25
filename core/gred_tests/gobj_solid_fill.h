#ifndef GOBJ_SOLID_FILL_H
#define GOBJ_SOLID_FILL_H

#include "iface/gobj.h"

class GObjSolidFill: public GObj{
  int color;

public:
  GObjSolidFill(const int c=0xFF000000);
  virtual int draw(iImage &img, const iPoint &origin);
};

#endif
