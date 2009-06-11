#include <iostream>
#include "simple_rubb_viewer.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlaneTestGrid  pl;

    SimpleRubbViewer viewer(&pl);

    viewer.rubber.push_back(RubberSegment(Point<int>(5,5), 0, Point<int>(5,15),0));
    viewer.rubber.push_back(RubberSegment(Point<int>(5,15), 0, Point<int>(15,15),0));
    viewer.rubber.push_back(RubberSegment(Point<int>(15,15), 0, Point<int>(15,5),0));
    viewer.rubber.push_back(RubberSegment(Point<int>(15,5), 0, Point<int>(5,5),0));

    viewer.rubber.push_back(RubberSegment(Point<int>(10,10), 0, Point<int>(0,0),3));
    viewer.rubber.push_back(RubberSegment(Point<int>(0,10), 1, Point<int>(0,0),3));
    viewer.rubber.push_back(RubberSegment(Point<int>(10,0), 2, Point<int>(0,0),3));
    viewer.rubber.push_back(RubberSegment(Point<int>(0,10), 0, Point<int>(0,10),1));
    viewer.rubber.push_back(RubberSegment(Point<int>(10,0), 0, Point<int>(10,0),2));

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
