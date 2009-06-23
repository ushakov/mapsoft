#include <iostream>
#include "mthread_rubb_viewer.h"


int main(int argc, char **argv){

    Gtk::Main     kit (argc, argv);
    Gtk::Window   win;
    GPlaneTestTile      pl1;
    GPlaneTestTileSlow  pl2;

    MThreadRubbViewer viewer(&pl1, &pl2);

    viewer.rubber.push_back(RubberSegment(Point<int>(5,5), 0, Point<int>(5,15),0));
    viewer.rubber.push_back(RubberSegment(Point<int>(5,15), 0, Point<int>(15,15),0));
    viewer.rubber.push_back(RubberSegment(Point<int>(15,15), 0, Point<int>(15,5),0));
    viewer.rubber.push_back(RubberSegment(Point<int>(15,5), 0, Point<int>(5,5),0));

    viewer.rubber.push_back(RubberSegment(Point<int>(10,10), 0, Point<int>(0,0),3));

    viewer.rubber.push_back(RubberSegment(Point<int>(0,10), 1, Point<int>(0,0),3));
    viewer.rubber.push_back(RubberSegment(Point<int>(10,0), 2, Point<int>(0,0),3));
    viewer.rubber.push_back(RubberSegment(Point<int>(10,10), 0, Point<int>(0,10),1));
    viewer.rubber.push_back(RubberSegment(Point<int>(10,10), 0, Point<int>(10,0),2));

    viewer.rubber.push_back(RubberSegment(Point<int>(-5,-5), 3, Point<int>(-5,5),3));
    viewer.rubber.push_back(RubberSegment(Point<int>(-5,5), 3, Point<int>(5,5),3));
    viewer.rubber.push_back(RubberSegment(Point<int>(5,5), 3, Point<int>(5,-5),3));
    viewer.rubber.push_back(RubberSegment(Point<int>(5,-5), 3, Point<int>(-5,-5),3));

    win.add(viewer);
    win.set_default_size(640,480);
    win.show_all();

    kit.run(win);
}
