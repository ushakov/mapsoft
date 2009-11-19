#ifndef VIEWER_H
#define VIEWER_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include <queue>

#include "../core/lib2d/image.h"
#include "../core/utils/cache.h"

#include "rubber.h"
#include "workplane.h"

//
class Viewer : public Gtk::DrawingArea {

public:

    Viewer (int tile_size=256);
    virtual ~Viewer ();

    void set_window_origin (iPoint new_origin);
    void set_window_origin(int x, int y);
    iPoint get_window_origin () const;
    iPoint get_window_size () const;

    void refresh();
    void zoom_out(int i);
    void zoom_in(int i);

    Workplane workplane;
    Rubber    rubber;

private:

    iPoint window_origin;
    iPoint drag_pos;
    iRect  visible_tiles;

    std::map<iPoint, iImage> tile_cache;
    std::set<iPoint>             tiles_todo;
    std::set<iPoint>             tiles_todo2;
    std::queue<iPoint>           tile_done_queue;

    // плитки, полученные из workplane, лежат в tile_cache.
    // если мы хотим нарисовать какую-то плитку,
    // мы кладем ее координаты в tiles_todo
    // крутящийся отдельно cache_updater_thread
    // просматривает tiles_todo, делает плитку,
    // и испускает сигнал update_tile_signal,
    // который ловится функцией update_tile();

    // в tiles_todo2 кладутся запросы на соседние плитки,
    // которые нам не надо срочно показывать.
    // cache_updater делает их в свободное время и не
    // сигнализирует о том...

    // cache_updater_cond используется для блокировки
    // cache_updater_thread. основная нить посылает сигнал этой условной
    // переменной когда cache_updater_thread должна ожить.
    // в зависимости от состояния свойств объекта нить завершится
    // или обработает плитки из очереди

    Glib::Thread        *cache_updater_thread;
    Glib::Cond          *cache_updater_cond;
    Glib::Dispatcher     update_tile_signal;

    // Нам надо исключить ситуации, когда viewer удаляет из кэшей
    // элементы, а cache_updater в это время проверяет их наличие и
    // пытается к ним обращаться.
    Glib::Mutex         *mutex;

    // cache_updater_thread крутится, пока we_need_cache_updater == true
    bool we_need_cache_updater;

    Glib::RefPtr<Gdk::GC> rubber_gc;

/**************************************/

    void cache_updater();

    // функция вызывается по сигналу от cache_updater'а
    void update_tile();

    void draw_tile(const iPoint & tile_key);

/**************************************/

    // перерисовка области экрана, очистка кэша и заданий
    void fill (int sx, int sy, int w, int h); // in window coordinates, should be inside the window

    void change_viewport ();

    virtual void on_realize();

    virtual void on_hide();

    void rubber_take_off(bool all=true);
    void rubber_render(bool all=true);
    void rubber_redraw();

    virtual bool on_expose_event (GdkEventExpose * event);
    virtual bool on_button_press_event (GdkEventButton * event);
    virtual bool on_motion_notify_event (GdkEventMotion * event);

/**************************************/
};

#endif /* VIEWER_H */
