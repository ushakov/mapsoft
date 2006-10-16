#ifndef VIEWER_H
#define VIEWER_H

//#include <cmath>
//#include <algorithm>
//#include <boost/operators.hpp>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include <queue> 

#include "workplane.h"
#include <point.h>
#include <cache.h>
#include <rect.h>
#include <image.h>
#include <image_gdk.h>

// 
class Viewer : public Gtk::DrawingArea {

private:
    Workplane & workplane;
    Point<int> window_origin;
    Point<int> drag_pos;
    Rect<int> tiles_in_screen;
    bool in_drag;

    // плитки, полученные из workplane лежат в кэше.
    // если draw_tile хочет положить новую плитку в кэш -
    // он добавляет ее координаты в очередь tile_queue
    // Эту очередь просматривает крутящийся отдельно
    // cache_updater_thread.
    // после добавления в кэш новой картинки, он
    // испускает сигнал update_tile_signal,
    // который ловится функцией update_tile();
    // Она убирает задание из очереди и перерисовывает плитку.


    Cache<Point<int>, Image<int> > tile_cache;
    std::queue<Point<int> >        tile_queue;
    Glib::Thread        *cache_updater_thread;
    Glib::Dispatcher       update_tile_signal;

    // cache_updater_thread крутится, пока we_need_cache_updater == true
    bool we_need_cache_updater;
    // cache_updater_thread не работает, пока cache_updater_stopped == true
    bool cache_updater_stopped;

public:

    Viewer (Workplane & _workplane, int _cache_size)
	: workplane (_workplane),
	  tile_cache(_cache_size),
	  drag_pos (0,0),
          window_origin(0,0),
	  in_drag (false),
	  we_need_cache_updater(true),
	  cache_updater_stopped(false)
    {
        Glib::thread_init();
        update_tile_signal.connect(sigc::mem_fun(*this, &Viewer::update_tile));
        // сделаем отдельный thread из функции cache_updater
        // joinable = true, чтобы подождать его завершения в деструкторе...
        cache_updater_thread = Glib::Thread::create(sigc::mem_fun(*this, &Viewer::cache_updater), true);

	add_events (Gdk::BUTTON_PRESS_MASK | 
		    Gdk::BUTTON_RELEASE_MASK | 
		    Gdk::POINTER_MOTION_MASK | 
		    Gdk::BUTTON1_MOTION_MASK | 
		    Gdk::POINTER_MOTION_HINT_MASK | 
		    Gdk::KEY_PRESS_MASK | 
		    Gdk::KEY_RELEASE_MASK
		   );
    }
    virtual ~Viewer (){
	we_need_cache_updater = false;
	// подождем, пока cache_updater_thread завершиться
	cache_updater_thread->join();
    }

/**************************************/

  void cache_updater(){
    while (we_need_cache_updater){
      if (tile_queue.empty() || cache_updater_stopped) {
	Glib::usleep(100);
	continue;
      } 
      Point<int> key = tile_queue.front();

      // если нам подсунули плитку, которой нет на экране - выкинем ее
      if (!point_in_rect(key, tiles_in_screen)){
	tile_cache.erase(key);
	tile_queue.pop();
	continue;
      }

      tile_cache.add(key, workplane.get_image(key));
      // Чтобы не обрабатывать одну плитку много раз. 
      // Пусть tile_update() удалит ее из tile_queue, нарисует и
      // установит cache_updater_stopped=false;
      cache_updater_stopped = true;
      update_tile_signal.emit();
    }
  }
/**************************************/

    // функция вызывается по сигналу от cache_updater'а
    void update_tile(){
      if (tile_queue.empty()) return;
      Point<int> key = tile_queue.front();
      tile_queue.pop();
#ifdef DEBUG_VIEWER
      std::cerr << "update_tile: " << key << "\n";
#endif
      draw_tile(key);
      cache_updater_stopped = false;
    }

/**************************************/

    void draw_tile(const Point<int> & tile_key){

      int tile_size = workplane.get_tile_size();
      Rect<int> screen(window_origin.x, 
                       window_origin.y, 
                       get_width(), get_height());
      Rect<int> tile_rect(tile_key.x*tile_size, 
                          tile_key.y*tile_size,
                          tile_size, tile_size);
      Rect<int> tile_in_screen = tile_rect;
      clip_rect_to_rect(tile_in_screen,screen);

      if (tile_in_screen.empty()) return; 

      if (!tile_cache.contains(tile_key)){
        // Если такой плитки еще нет - добавим временную картинку
        // и поместим запрос на изготовление нормальной картинки в очередь
	tile_cache.add(tile_key, Image<int>(tile_size,tile_size, 0xFF000000));
        tile_queue.push(tile_key);
      }

      Image<int> tile = tile_cache.get(tile_key);
      Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(tile);
      Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
      Glib::RefPtr<Gdk::Window> widget = get_window();

     widget->draw_pixbuf(gc, pixbuf, 
	        tile_in_screen.x-tile_rect.x, tile_in_screen.y-tile_rect.y, // on pixbuf
                tile_in_screen.x-window_origin.x, tile_in_screen.y-window_origin.y,
                tile_in_screen.w, tile_in_screen.h,
                Gdk::RGB_DITHER_NORMAL, 0, 0);
    }
/**************************************/

    void fill (int sx, int sy, int w, int h) // in window coordinates, should be inside the window
    {
#ifdef DEBUG_VIEWER
	std::cerr << "fill: " << sx << "," << sy << " " << w << "x" << h << std::endl;
	std::cerr << "window_origin: " << window_origin.x << "," << window_origin.y << std::endl;
#endif

      int tile_size = workplane.get_tile_size();

      int x1 = window_origin.x + sx;
      int x2 = window_origin.x + sx + w - 1;
      int y1 = window_origin.y + sy;
      int y2 = window_origin.y + sy + h - 1;


      x1 = (x1>0) ? x1/tile_size : x1/tile_size - 1;
      x2 = (x2>0) ? x2/tile_size : x2/tile_size - 1;
      y1 = (y1>0) ? y1/tile_size : y1/tile_size - 1;
      y2 = (y2>0) ? y2/tile_size : y2/tile_size - 1;

      tiles_in_screen.x = x1;
      tiles_in_screen.y = y1;
      tiles_in_screen.w = x2-x1+1;
      tiles_in_screen.h = y2-y1+1;

      for (int tj = y1; tj<=y2; tj++){
	for (int ti = x1; ti<=x2; ti++){
	  draw_tile(Point<int>(ti,tj));
        }
      }
    }


/**************************************/
    
    virtual bool
    on_expose_event (GdkEventExpose * event)
    {
#ifdef DEBUG_VIEWER
	std::cerr << "expose: " << event->area.x << "," << event->area.y << " " << event->area.width << "x" << event->area.height << std::endl;
#endif
	fill (event->area.x, event->area.y, event->area.width, event->area.height);
	return true;
    }


    virtual bool
    on_button_press_event (GdkEventButton * event) {
#ifdef DEBUG_VIEWER
	std::cerr << "press: " << event->x << "," << event->y << " " << event->button << std::endl;
#endif
	if (event->button == 1) {
	    drag_pos = Point<int> ((int)event->x, (int)event->y);
	    return true;
	} else {
	    return Gtk::DrawingArea::on_button_press_event (event);
	}

	int dummy;
	Gdk::ModifierType dummy2;
	get_window()->get_pointer(dummy, dummy, dummy2);
    }


    virtual bool
    on_motion_notify_event (GdkEventMotion * event) {
	Point<int> pos ((int) event->x, (int) event->y);

#ifdef DEBUG_VIEWER
	    std::cerr << "drag: " << pos << std::endl;
#endif

	if (!(event->state & Gdk::BUTTON1_MASK)) {
	    return false;
	}
	if (!in_drag && (pos - drag_pos).manhattan_length() > 4) {
	    in_drag = true;
	}
	if (in_drag) {
	    window_origin += drag_pos - pos;
	    fill (0, 0, get_width(), get_height());
	    drag_pos = pos;
	}
	if (event->is_hint) {
#ifdef DEBUG_VIEWER
	    std::cerr << "move-hint: " << event->x << "," << event->y << std::endl;
#endif
	    int dummy;
	    Gdk::ModifierType dummy2;
	    get_window()->get_pointer(dummy, dummy, dummy2);
	}
	return true;
    }


    virtual bool
    on_button_release_event (GdkEventButton * event) {
	Point<int> pos ((int) event->x, (int) event->y);
#ifdef DEBUG_VIEWER
	std::cerr << "release: " << (int)event->x << "," << (int)event->y << std::endl;
#endif
	if (event->button == 1) {
	    window_origin += drag_pos - pos;
	    fill (0, 0, get_width(), get_height());
	    in_drag = false;
	    return true;
	} else {
	    return Gtk::DrawingArea::on_button_release_event (event);
	}
    }

    virtual bool
    on_keypress ( GdkEventKey * event ) {
#ifdef DEBUG_VIEWER
	std::cerr << "key: " << event->keyval << std::endl;
#endif
	if (event->keyval == 1) {
	    window_origin += Point<int>(get_width(), get_height());
	    fill (0, 0, get_width(), get_height());
	    return true;
	}

	return false;
    }



	

    void set_window_origin(Point<int> p){ 
	window_origin = p;
#ifdef DEBUG_VIEWER
	std::cerr << "new window_origin " << p.x << "," << p.y << std::endl;
#endif
	if (is_mapped()){
	    fill (0, 0, get_width(), get_height());
	}
    }

    void set_window_origin(int x, int y){
	set_window_origin(Point<int>(x,y));
    }

    Point<int> get_window_origin ()
    {
	return window_origin;
    }


// Работы с масштабами

    void set_scale(int scale){
      double old_scale = workplane.get_scale();
      Cache<Point<int>, Image<int> > ocache(tile_cache.size());
      workplane.set_scale(scale);



      tile_cache.swap(ocache);
      


      if (is_mapped()){
        fill (0, 0, get_width(), get_height());
      }
    }

    int get_scale(){
      return workplane.get_scale();
    }
    


};

#endif /* VIEWER_H */
