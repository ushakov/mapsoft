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
private:
    std::vector<std::pair<RubberPoint, RubberPoint> > lines;
    std::vector<std::pair<Point<int>, Point<int> > > drawn;
    Glib::RefPtr<Gdk::Window> window;
    Glib::RefPtr<Gdk::GC> gc;

    void xor_line (std::pair<Point<int>, Point<int> > line) {
	window->draw_line(gc, line.first.x, line.first.y, line.second.x, line.second.y);
    }

    void take_off_line(int i) {
	if (drawn[i].first.x != 0 ||
	    drawn[i].first.y != 0 ||
	    drawn[i].second.x != 0 ||
	    drawn[i].second.y != 0) {
	    xor_line (drawn[i]);
	    drawn[i] = std::make_pair(Point<int>(0,0), Point<int>(0,0));
	}
    }

    void render_line(int i, Point<int> pointer, Point<int> origin) {
	take_off_line(i);
	std::pair<Point<int>, Point<int> > new_position;
	new_position.first = lines[i].first.get(pointer, origin);
	new_position.second = lines[i].second.get(pointer, origin);
	xor_line(new_position);
	drawn[i] = new_position;
    }
    
    void render(Point<int> pointer, Point<int> origin) {
	for (int i = 0; i < lines.size(); ++i) {
	    render_line(i, pointer, origin);
	}
	for (int i = 0; i < lines.size(); ++i) {
	    if (drawn[i].first.x == 0 &&
		drawn[i].first.y == 0 &&
		drawn[i].second.x == 0 &&
		drawn[i].second.y == 0) {
		std::cerr << "Unrealised line after render: " << drawn[i].first << "--" << drawn[i].second << std::endl;
	    }
	}
    }
    
    void take_off() {
	for (int i = 0; i < lines.size(); ++i) {
	    take_off_line(i);
	}
	for (int i = 0; i < lines.size(); ++i) {
	    if (drawn[i].first.x != 0 ||
		drawn[i].first.y != 0 ||
		drawn[i].second.x != 0 ||
		drawn[i].second.y != 0) {
		std::cerr << "Remaining line after take-off: " << drawn[i].first << "--" << drawn[i].second << std::endl;
	    }
	}
    }

    bool visible;

public:
    // construct
    Rubber (Glib::RefPtr<Gdk::Window> window_)  {
	window = window_;
	gc = Gdk::GC::create(window);
	gc->set_rgb_fg_color(Gdk::Color("white"));
	gc->set_function(Gdk::XOR);
	visible = false;
    }
    
    // render

    void update(Point<int> pointer, Point<int> origin) {
	render(pointer, origin);
	visible = true;
    }

    void hide () {
	if (visible) {
	    take_off();
	}
	visible = false;
    }

    bool is_visible() {
	return visible;
    }

    // modify
    void add_line (RubberPoint a, RubberPoint b) {
	lines.push_back(std::make_pair(a, b));
	drawn.push_back(std::make_pair(Point<int>(0,0), Point<int>(0,0)));
    }
    void clear() {
	hide();
	std::vector<std::pair<RubberPoint, RubberPoint> > dummy;
	lines.swap(dummy);
    }

};

#endif /* RUBBER_H */
