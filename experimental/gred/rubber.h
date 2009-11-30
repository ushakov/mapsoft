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

/// Class for the rubber segment -- two points with some flags
/// flags bits (see definitions above):
/// 0 - p1.x connected to mouse?
/// 1 - p1.y connected to mouse?
/// 2 - p2.x connected to mouse?
/// 3 - p2.y connected to mouse?
/// 4-7 - type of segment:
///   0x00 -- line
///   0x10 -- ellipse in a box with p1-p2 diagonal
///   0x20 -- ellipse with the enter in p1
///   0x30 -- circle with p1-p2 diameter
///   0x40 -- circle with p1-p2 radius

struct RubberSegment{
  rubbfl_t flags;
  iPoint p1, p2;

  /// create RubberSegment from two points and flags
  RubberSegment(const iPoint & p1_, const iPoint & p2_, const rubbfl_t flags_);

  /// convert segment coordinates to absolute values
  RubberSegment absolute(Point<int> mouse, Point<int> origin) const;
};

/// Class for drawing rubber lines on a viewer
class Rubber{
private:
  std::list<RubberSegment> rubber, drawn;
  iPoint mouse_pos;
  Glib::RefPtr<Gdk::GC> gc;
  SimpleViewer * viewer;

public:

  Rubber(SimpleViewer * v);

private:

  /// helpers to be connected with viewer signals
  void before_motion_notify (GdkEventMotion * event);
  void after_motion_notify (GdkEventMotion * event);
  void init_gc();

  /// Function for drawing single rubber segment.
  /// Used by both draw() and erase()
  void draw_segment(const RubberSegment &s);

  /// functions for drawing and erasing rubber
  void draw(const bool all=true);
  void erase(const bool all=true);

public:

  /// add segment to a rubber
  void add(const RubberSegment & s);
  void add(const iPoint & p1, const iPoint & p2,
           const rubbfl_t flags);
  void add(const int x1, const int y1,
           const int x2, const int y2,
           const rubbfl_t flags);

  /// cleanup rubber
  void clear();

  /// High-level functions for adding some types of segments
  void add_src_sq(const iPoint & p, int size);
  void add_dst_sq(int size);
  void add_diag(const iPoint & p);
  void add_rect(const iPoint & p);
  void add_ell(const iPoint & p);
  void add_ellc(const iPoint & p);
  void add_circ(const iPoint & p);
  void add_circc(const iPoint & p);
};

#endif
