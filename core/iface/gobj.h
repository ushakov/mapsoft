#ifndef GOBJ_IFACE_H
#define GOBJ_IFACE_H

#include "lib2d/rect.h"
#include "lib2d/image.h"

// return codes for draw function
extern const int GOBJ_FILL_NONE; // object draws nothing
extern const int GOBJ_FILL_PART; // object draws some points
extern const int GOBJ_FILL_ALL; // object fills in the whole image with opaque colors

extern const iRect GOBJ_MAX_RANGE;

class GObj{
public:

  GObj();

  virtual int draw(iImage &img, const iPoint &origin) = 0;

  virtual iRect range(void) const;

  virtual void set_scale(const double k);
//  virtual void set_conv(const Conv & cnv);

  // object can be drawn in "gray mode":
  // gray colors, borders instead of images etc.
  bool is_gray();
  void gray_on();
  void gray_off();

private:
  bool gray_mode;
};

#endif