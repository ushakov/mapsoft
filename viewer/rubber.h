#ifndef RUBBER_H
#define RUBBER_H

#include <gtkmm.h>
#include <lib2d/point.h>


class RubberPoint {
public:
    bool active;
    Point<int> p;

    RubberPoint(Point<int> p_, int active_): active(active_), p(p_) { }

    Point<int> get(Point<int> pointer, Point<int> offset) {
      return active? (p+pointer) : (p-offset);
    }
};

class DrawnPair {
public:
  Point<int> p1,p2;
  bool active;

  DrawnPair(Point<int> p1_, Point<int> p2_, bool active_):
    p1(p1_), p2(p2_), active(active_) {};
};

class Rubber {
public:
    std::vector<std::pair<RubberPoint, RubberPoint> > lines;
    std::vector<DrawnPair> drawn;

    Rubber(){}

    void add_line (Point<int> p1, bool a1, Point<int> p2, bool a2) {
	lines.push_back(std::make_pair(RubberPoint(p1,a1),RubberPoint(p2,a2)));
        refresh();
    }
    void clear() {
	std::vector<std::pair<RubberPoint, RubberPoint> > dummy1;
	lines.swap(dummy1);
        refresh();
    }
    void refresh(){
      signal_refresh.emit();
    }
    sigc::signal<void> signal_refresh;

    int size(){return lines.size();}
};

#endif /* RUBBER_H */
