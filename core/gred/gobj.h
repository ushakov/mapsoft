#ifndef GRED_GOBJ_H
#define GRED_GOBJ_H

#include "2d/rect.h"
#include "2d/image.h"
#include <sigc++/sigc++.h>

///\addtogroup gred
///@{
///\defgroup gobj
///@{

/**
Объект, умеющий нарисоваться на растровой картинке.

Важно: draw() может вызываться из соседнего потока. В некоторых
случаях в объекте нужна правильная блокировка (пример - workplane,
рисование не должно происходить при добавлении, удалении новых объектов)
*/
class GObj{
public:
  const static int TILE_SIZE = 256; //< size of tiles
  const static int FILL_NONE = 0; // object draws nothing
  const static int FILL_PART = 1; // object draws some points
  const static int FILL_ALL  = 2; // object fills in the whole image with opaque colors
  const static iRect MAX_RANGE;

  GObj() {}

  /** Рисование на картинке img со смещением origin.
   \return одно из следующих значений:
   - GObj::FILL_NONE  -- ничего не было нариовано
   - GObj::FILL_PART  -- что-то было нарисовано
   - GObj::FILL_ALL   -- все изображение было зарисовано непрозрачным цветом
   NOTE:
    - range() returns range in viewer coords
  */
  virtual int draw(iImage &img, const iPoint &origin) = 0;

  virtual iImage get_image (iRect src){
    if (rect_intersect(range(), src).empty()) return iImage(0,0);
    iImage ret(src.w, src.h, 0);
    if (draw(ret, src.TLC()) == GObj::FILL_NONE) return iImage(0,0);
    return ret;
  }

  virtual iRect range(void) const {return GObj::MAX_RANGE;}
  virtual void rescale(double k) {} ///< change scale (refresh must be inside)

  virtual bool get_xloop() const {return false;};
  virtual bool get_yloop() const {return false;}

  sigc::signal<void, iRect> & signal_redraw_me()  {return signal_redraw_me_;}

private:
  sigc::signal<void, iRect> signal_redraw_me_;
};

#endif
