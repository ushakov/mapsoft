#ifndef GRED_GOBJ_H
#define GRED_GOBJ_H

#include "2d/rect.h"
#include "2d/image.h"
#include "2d/conv_triv.h"

///\addtogroup gred
///@{
///\defgroup gobj
///@{

// return codes for draw function
extern const int GOBJ_FILL_NONE; // object draws nothing
extern const int GOBJ_FILL_PART; // object draws some points
extern const int GOBJ_FILL_ALL; // object fills in the whole image with opaque colors

extern const iRect GOBJ_MAX_RANGE;

/**
Объект, умеющий нарисоваться на растровой картинке.

Важно: draw() может вызываться из соседнего потока. В некоторых
случаях в объекте нужна правильная блокировка (пример - workplane,
рисование не должно происходить при добавлении, удалении новых объектов)
*/
class GObj{
    static ConvTriv trivial_cnv;
public:

  GObj():cnv(&trivial_cnv),cnv_hint(-1) {}

  /** Рисование на картинке img со смещением origin.
   \return одно из следующих значений:
   - GOBJ_FILL_NONE  -- ничего не было нариовано
   - GOBJ_FILL_PART  -- что-то было нарисовано
   - GOBJ_FILL_ALL   -- все изображение было зарисовано непрозрачным цветом
  */
  virtual int draw(iImage &img, const iPoint &origin) = 0;

  virtual iImage get_image (iRect src){
    if (rect_intersect(range(), src).empty()) return iImage(0,0);
    iImage ret(src.w, src.h, 0);
    if (draw(ret, src.TLC()) == GOBJ_FILL_NONE) return iImage(0,0);
    return ret;
  }

  virtual iRect range(void) const {return GOBJ_MAX_RANGE;}
  virtual void refresh() {} ///< must be called after data or cnv change

  virtual bool get_xloop() const {return false;};
  virtual bool get_yloop() const {return false;}

  virtual Conv * get_cnv() const{ return cnv; }
  virtual void set_cnv(Conv * c, int hint=-1){
    cnv = c; cnv_hint=hint; refresh();}

  Conv * cnv;
  int cnv_hint;
};

#endif
