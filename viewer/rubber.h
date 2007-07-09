#ifndef RUBBER_H
#define RUBBER_H

#include <gtkmm.h>
#include <utils/point.h>

class RubberPoint {
public:
    enum {Pointer, Workplane};
    int base;
    Point<int> p;

    RubberPoint(Point<int> p_, int base_)
	: base(base_), p(p_) { }

    Point<int> get(Point<int> pointer, Point<int> offset) {
	if (base == Pointer) {
	    return p + pointer;
	} else if (base == Workplane) {
	    return p - offset;
	} else {
	    return Point<int>(0,0);
	}
    }
};

class Rubber {
public:
    std::vector<std::pair<RubberPoint, RubberPoint> > lines;
    std::vector<std::pair<Point<int>, Point<int> > > drawn;

    Rubber(){}

    void add_line (RubberPoint a, RubberPoint b) {
	lines.push_back(std::make_pair(a, b));
	drawn.push_back(std::make_pair(Point<int>(0,0), Point<int>(0,0)));
        refresh();
    }
    void clear() {
	std::vector<std::pair<RubberPoint, RubberPoint> > dummy;
	lines.swap(dummy);
        refresh();
    }
    void refresh(){
      signal_refresh.emit();
    }
    sigc::signal<void> signal_refresh;

    int size(){return lines.size();}
};

#endif /* RUBBER_H */
