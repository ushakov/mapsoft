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

TODO:

Объект может (и должен) кэшировать информацию, нужную для рисования
определенной области. Формат данных в кэше - на усмотрение объекта: это может
быть набор точек для рисования линии или преобразованная растровая картинка.

Объект может отдавать "сложность" кэширования и рисования.
Это хочется применять для умного разбиения плоскости на плитки в зависимости от
сложности рисования объектов на плитках. Полезность этого пока
непонятна.

Объект может предоставлять функции поиска (ткнули в такую-то точку - вернули
объект или точку объекта, или сегмент объекта или т.п.).
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
  virtual void refresh() {}

  virtual bool get_xloop() const {return false;};
  virtual bool get_yloop() const {return false;}

  virtual Conv * get_cnv() const{ return cnv; }
  virtual void set_cnv(Conv * c, int hint=-1){
    cnv = c; cnv_hint=hint; refresh();}

  Conv * cnv;
  int cnv_hint;
};

#endif
