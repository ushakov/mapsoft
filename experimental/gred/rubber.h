#ifndef RUBBER_VIEWER_H
#define RUBBER_VIEWER_H

#include <list>
#include "simple_viewer.h"

typedef unsigned int rubbfl_t;
#define RUBBFL_PLANE     0
#define RUBBFL_MOUSE_P1X 1
#define RUBBFL_MOUSE_P1Y 2
#define RUBBFL_MOUSE_P1  3
#define RUBBFL_MOUSE_P2X 4
#define RUBBFL_MOUSE_P2Y 8
#define RUBBFL_MOUSE_P2  0xC
#define RUBBFL_MOUSE     0xF

#define RUBBFL_TYPEMASK 0xF0
#define RUBBFL_LINE  0x00
#define RUBBFL_ELL   0x10
#define RUBBFL_ELLC  0x20
#define RUBBFL_CIRC  0x30
#define RUBBFL_CIRCC 0x40

struct RubberSegment{
  rubbfl_t flags;
  iPoint p1, p2;

  /// create RubberSegment from two points and flags
  RubberSegment(const iPoint & p1_, const iPoint & p2_, const rubbfl_t flags_);

  /// convert segment coordinates to absolute values
  RubberSegment absolute(Point<int> mouse, Point<int> origin) const;
};

class Rubber{
private:
  std::list<RubberSegment> rubber, drawn;
  iPoint mouse_pos;
  Glib::RefPtr<Gdk::GC> rubber_gc;
  SimpleViewer * viewer;

public:

  Rubber(SimpleViewer * v);

private:

  /// helpers to be connected with viewer signals
  void before_motion_notify (GdkEventMotion * event);
  void after_motion_notify (GdkEventMotion * event);
  void init_gc();

  /// Function for drawing single rubber segment.
  /// Used by both rubber_draw() and rubber_erase()
  void rubber_draw_segment(const RubberSegment &s);

  /// functions for drawing and erasing rubber
  void rubber_draw(const bool all=true);
  void rubber_erase(const bool all=true);

public:

  /// add segment to a rubber
  void rubber_add(const RubberSegment & s);
  void rubber_add(const iPoint & p1, const iPoint & p2,
                  const rubbfl_t flags);
  void rubber_add(const int x1, const int y1,
                  const int x2, const int y2,
                  const rubbfl_t flags);

  /// cleanup rubber
  void rubber_clear();

  /// High-level functions for adding some types of segments
  void rubber_add_src_sq(const iPoint & p, int size);
  void rubber_add_dst_sq(int size);
  void rubber_add_diag(const iPoint & p);
  void rubber_add_rect(const iPoint & p);
  void rubber_add_ell(const iPoint & p);
  void rubber_add_ellc(const iPoint & p);
  void rubber_add_circ(const iPoint & p);
  void rubber_add_circc(const iPoint & p);
};

#endif
