#ifndef FIG_UTILS_H
#define FIG_UTILS_H

#include <list>
#include "fig_data.h"
#include "fig_mask.h"

/// связанное с fig-файлами
namespace fig {

  // размер fig-объектов
  iRect range(std::list<fig_object> & objects);

  // заключить fig-объекты в составной объект.
  void fig_make_comp(std::list<fig_object> & objects);

  // повернуть на угол a вокруг точки p0
  void fig_rotate(std::list<fig_object> & objects,
    const double a, const iPoint & p0 = iPoint(0,0));

  // преобразовать ломаную в сплайн
  void any2xspl(fig_object & o, const double x, const double y=0);

  /// обрезка fig-файла по прямоугольнику
  void rect_crop(const iRect & cutter, std::list<fig_object> & objects);

  /// Обрезка fig-объекта по прямоугольнику. Объект после обрезки 
  /// может не содержать точек, т.е. быть плохим с т.з. fig-а!
  void rect_crop(const iRect & cutter, fig_object & o);

}
#endif
