#ifndef VIEWER_H
#define VIEWER_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include <queue>

#include <lib2d/image.h>
#include <utils/cache.h>

#include "rubber.h"
#include "workplane.h"

//
class Viewer : public Gtk::DrawingArea {

public:

    Viewer (boost::shared_ptr<Workplane> _workplane,
            boost::shared_ptr<Rubber>    _rubber);

    virtual ~Viewer ();

    void set_window_origin (Point<int> new_origin);
    void set_window_origin(int x, int y);
    Point<int> get_window_origin () const;
    Point<int> get_window_size () const;

    void refresh();

private:

    boost::shared_ptr<Workplane> workplane;
    boost::shared_ptr<Rubber>    rubber;

    Point<int> window_origin;
    Rect<int>  visible_tiles;

    std::map<Point<int>, Image<int> > tile_cache;
    std::set<Point<int> >             tiles_todo;
    std::set<Point<int> >             tiles_todo2;
    std::queue<Point<int> >           tile_done_queue;

    // ������, ���������� �� workplane, ����� � tile_cache.
    // ���� �� ����� ���������� �����-�� ������,
    // �� ������ �� ���������� � tiles_todo
    // ���������� �������� cache_updater_thread
    // ������������� tiles_todo, ������ ������,
    // � ��������� ������ update_tile_signal,
    // ������� ������� �������� update_tile();

    // � tiles_todo2 �������� ������� �� �������� ������,
    // ������� ��� �� ���� ������ ����������.
    // cache_updater ������ �� � ��������� ����� � ��
    // ������������� � ���...

    // cache_updater_cond ������������ ��� ����������
    // cache_updater_thread. �������� ���� �������� ������ ���� ��������
    // ���������� ����� cache_updater_thread ������ �����.
    // � ����������� �� ��������� ������� ������� ���� ����������
    // ��� ���������� ������ �� �������

    Glib::Thread        *cache_updater_thread;
    Glib::Cond          *cache_updater_cond;
    Glib::Dispatcher     update_tile_signal;

    // ��� ���� ��������� ��������, ����� viewer ������� �� �����
    // ��������, � cache_updater � ��� ����� ��������� �� ������� �
    // �������� � ��� ����������.
    Glib::Mutex         *mutex;

    // cache_updater_thread ��������, ���� we_need_cache_updater == true
    bool we_need_cache_updater;

    Glib::RefPtr<Gdk::GC> rubber_gc;

/**************************************/

    void cache_updater();

    // ������� ���������� �� ������� �� cache_updater'�
    void update_tile();

    void draw_tile(const Point<int> & tile_key);

/**************************************/

    // ����������� ������� ������, ������� ���� � �������
    void fill (int sx, int sy, int w, int h); // in window coordinates, should be inside the window

    void change_viewport ();

    virtual bool on_expose_event (GdkEventExpose * event);

    virtual void on_realize();

    virtual bool on_motion_notify_event(GdkEventMotion * event);

    void rubber_take_off(bool all=true);
    void rubber_render(bool all=true);
    void rubber_redraw();


/**************************************/
};

#endif /* VIEWER_H */
