#ifndef RUBBER_VIEWER_H
#define RUBBER_VIEWER_H

#include <list>
#include "viewer.h"

///\addtogroup gred
///@{
/**\defgroup rubber
"Резиновые линии", которые можно прицепить ко вьюеру.
Резиновые линии рисуются поверх всех объектов. Они могут быть
привязаны к объекту или к мыши.

Несколько объектов типа Rubber хорошо уживаются на одном Viewer'е.
*/
///@{

typedef unsigned int rubbfl_t;
#define RUBBFL_PLANE     0 ///< обе точки привязаны к объекту
#define RUBBFL_MOUSE_P1X 1 ///< координата x первой точки привязана к мыши
#define RUBBFL_MOUSE_P1Y 2 ///< координата y первой точки привязана к мыши
#define RUBBFL_MOUSE_P1  3 ///< первая точка привязана к мыши
#define RUBBFL_MOUSE_P2X 4
#define RUBBFL_MOUSE_P2Y 8
#define RUBBFL_MOUSE_P2  0xC  ///< вторая точка привязана к мыши
#define RUBBFL_MOUSE     0xF  ///< обе точки привязаны к мыши

#define RUBBFL_TYPEMASK 0xF0
#define RUBBFL_LINE  0x00    ///< сегмент -- отрезок
#define RUBBFL_ELL   0x10    ///< сегмент -- эллипс, точки задают описывающий его прямоугольник
#define RUBBFL_ELLC  0x20    ///< сегмент -- эллипс, первая точка -- центр
#define RUBBFL_CIRC  0x30    ///< сегмент -- окружность, точки задают диаметр
#define RUBBFL_CIRCC 0x40    ///< сегмент -- окружность, точки задают радиус

#define RUBBFL_DRAWN 0x100   ///< флаг, показывающий, что сегмент в данный момент нарисован

/**
 Class for the rubber segment -- two points with some flags
 flags bits (see definitions above):
 0 - p1.x connected to mouse?
 1 - p1.y connected to mouse?
 2 - p2.x connected to mouse?
 3 - p2.y connected to mouse?
 4-7 - type of segment:
   0x00 -- line
   0x10 -- ellipse in a box with p1-p2 diagonal
   0x20 -- ellipse with center in p1
   0x30 -- circle with p1-p2 diameter
   0x40 -- circle with p1-p2 radius
 8 - was segment drawn?

Сегмент резиновой линии - это пара точек и флаги
RUBBFL_PLANE     0x0 -- обе точки привязаны к плоскости
RUBBFL_MOUSE_P1X 0x1 -- координата X точки 1 привязана к мыши
RUBBFL_MOUSE_P1Y 0x2 -- координата Y точки 1 привязана к мыши
RUBBFL_MOUSE_P2X 0x4 -- координата X точки 2 привязана к мыши
RUBBFL_MOUSE_P2Y 0x8 -- координата Y точки 2 привязана к мыши
RUBBFL_MOUSE_P1  0x3 -- точка 1 привязана к мыши
RUBBFL_MOUSE_P2  0xC -- точка 2 привязана к мыши
RUBBFL_MOUSE     0xF -- обе точки привязаны к мыши

RUBBFL_TYPEMASK  0xF0 -- маска типа сегмента
RUBBFL_LINE      0x00 -- сегмент является отрезком
RUBBFL_ELL       0x10 -- эллипс, заданный диагональю описанного прямоугольника
RUBBFL_ELLC      0x20 -- эллипс, заданный полудиагональю оп. прямоугольника
RUBBFL_CIRC      0x30 -- окружность, заданная диаметром
RUBBFL_CIRCC     0x40 -- окружность, заданная радиусом

RUBBFL_DRAWN    0x100 -- сегмент нарисован
*/
struct RubberSegment{
  rubbfl_t flags;
  iPoint p1, p2;
  iPoint pf1, pf2; // absolute coordinates of drawn segment

  /// create RubberSegment from two points and flags
  RubberSegment(const iPoint & p1_, const iPoint & p2_, const rubbfl_t flags_);

  /// save current absolute coordinates for drawing
  void fix(Point<int> mouse, Point<int> origin);
};

/// Rubber -- class for drawing rubber lines on a viewer
class Rubber{
private:
  std::list<RubberSegment> rubber;
  iPoint mouse_pos;
  Glib::RefPtr<Gdk::GC> gc;
  Viewer * viewer;

public:

  Rubber(Viewer * v);

private:

  /// helpers to be connected with viewer signals
  void before_motion_notify (GdkEventMotion * event);
  void after_motion_notify (GdkEventMotion * event);
  void init_gc();

  /// Function for drawing single rubber segment.
  /// Used by both draw() and erase()
  void draw_segment(const RubberSegment &s);

  void draw(const bool all=true); ///<draw rubber
  void erase(const bool all=true);///<erase rubber

public:

  /// add segment to a rubber
  void add(const RubberSegment & s);
  void add(const iPoint & p1, const iPoint & p2,
           const rubbfl_t flags = RUBBFL_PLANE);
  void add(const int x1, const int y1,
           const int x2, const int y2,
           const rubbfl_t flags = RUBBFL_PLANE);

  /// remove the last segment from the rubber and get it
  RubberSegment pop(void);
  /// get the last segment from the rubber
  RubberSegment get(void);
  /// fix mouse points at point p
  void fix(const iPoint & p);
  /// cleanup rubber
  void clear();
  /// count segments
  int size();
  /// dump rubber to stderr
  void dump(void) const;

  /// modify coordinates connected to plane
  void rescale(double k);

  /// High-level functions for adding some types of segments
  void add_src_sq(const iPoint & p, int size=5);    ///< square mark on the plane
  void add_src_mark(const iPoint & p, int size=5);  ///< mark with cross and circle on the plane
  void add_dst_sq(int size=5);                      ///< square mark around the mouse
  void add_line(const iPoint & p);                     ///< line from p to mouse
  void add_line(const iPoint & p1, const iPoint & p2); ///< line from p1 to p2
  void add_rect(const iPoint & p);                     ///< rectangle from p to mouse
  void add_rect(const iPoint & p1, const iPoint & p2); ///< rectangle from p1 to p2
  void add_ell(const iPoint & p);                      ///< ellipse
  void add_ellc(const iPoint & p);
  void add_circ(const iPoint & p);
  void add_circc(const iPoint & p);

};

#endif
