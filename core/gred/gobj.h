#ifndef GOBJ_IFACE_H
#define GOBJ_IFACE_H

#include "2d/rect.h"
#include "2d/image.h"

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

Получает картинку, ее координаты на растровой плоскости,
и информацию о привязки и масштабе растровой плоскости.
Рисует что-то на картинке. Что-то в таком духе:
int GObject::draw(GImage & img, const GPoint & origin, const GObjInfo & info);

GObjInfo -- информация о привязке и масштабе. Объект может использовать или
не использовать ее по своему усмотрению.
(jpeg-картинка может эффективно грузиться с уменьшением,
иконка может вообще игнорировать масштаб/привязку...)

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
public:

  GObj();
  /** Рисование на картинке img со смещением origin.
   \return одно из следующих значений:
   - GOBJ_FILL_NONE  -- ничего не было нариовано
   - GOBJ_FILL_PART  -- что-то было нарисовано
   - GOBJ_FILL_ALL   -- все изображение было зарисовано непрозрачным цветом
  */
  virtual int draw(iImage &img, const iPoint &origin) = 0;

  virtual iRect range(void) const;

  virtual void set_scale(const double k);

  virtual bool get_xloop() const;
  virtual bool get_yloop() const;
};

#endif
