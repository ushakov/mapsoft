#ifndef RECT_H
#define RECT_H

#include "point.h"

class Rect {
public:

    // исправить на две точки!!!
    int x, y, w, h;
    Rect (int _x, int _y, int _w, int _h)
	: x (_x), y (_y), w (_w), h (_h)
    { }

    Rect ()
	: x(0), y(0), w(0), h(0)
    { }

    Rect intersect (Rect const & other) const
    {
	int x1 = std::max (x, other.x);
	int y1 = std::max (y, other.y);
	int x2 = std::min (x + w, other.x + other.w);
	int y2 = std::min (y + h, other.y + other.h);
	return Rect (x1, y1, std::max (x2-x1, 0), std::max (y2-y1, 0));
    }
    
    Rect bounding_box (Rect const & other) const
    {
	if (empty()) {
	    return other;
	} else if (other.empty()) {
	    return *this;
	}
	
	int x1 = std::min (x, other.x);
	int y1 = std::min (y, other.y);
	int x2 = std::max (x + w, other.x + other.w);
	int y2 = std::max (y + h, other.y + other.h);
	return Rect (x1, y1, std::max (x2-x1, 0), std::max (y2-y1, 0));
    }

    PointRR clip_point (PointRR p) const 
    {
	p.x = std::max (x, p.x);
	p.x = std::min (x+w, p.x);
	p.y = std::max (y, p.y);
	p.y = std::min (y+h, p.y);
	return p;
    }

    PointRR const 
    TLC ()
    {
	return PointRR (x, y);
    }

    PointRR
    BRC () const
    {
	return PointRR (x+w, y+h);
    }

    bool
    empty() const
    {
	return w == 0 || h == 0;
    }

};


#endif /* RECT_H */
