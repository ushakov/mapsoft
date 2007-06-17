#ifndef VIEWER_H
#define VIEWER_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include <queue> 

#include "viewer/workplane.h"
#include <utils/point.h>
#include <utils/cache.h>
#include <utils/rect.h>
#include <utils/image.h>
#include <utils/image_gdk.h>
#include <utils/image_brez.h>
#include <viewer/rubber.h>

// 
class Viewer : public Gtk::DrawingArea {

private:
    boost::shared_ptr<Workplane> workplane;
    Point<int> window_origin;
    Rect<int>  visible_tiles;
    Point<int> drag_pos;

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
    Glib::Dispatcher       update_tile_signal;

    // ��� ���� ��������� ��������, ����� viewer ������� �� �����
    // ��������, � cache_updater � ��� ����� ��������� �� ������� �
    // �������� � ��� ����������. 
    Glib::Mutex         *mutex;

    // cache_updater_thread ��������, ���� we_need_cache_updater == true
    bool we_need_cache_updater;

    // Rubber things
    boost::shared_ptr<Rubber> rubber;

public:

    Viewer (boost::shared_ptr<Workplane> _workplane, 
            Point<int> _window_origin = Point<int>(0,0), 
	    int _scale_nom = 1,
            int _scale_denom = 1)
	: workplane (_workplane),
          window_origin(_window_origin),
	  we_need_cache_updater(true)
    {
        workplane->set_scale(_scale_nom, _scale_denom);
        Glib::thread_init();
	mutex = new(Glib::Mutex);
        cache_updater_cond = new(Glib::Cond);
        update_tile_signal.connect(sigc::mem_fun(*this, &Viewer::update_tile));
        // ������� ��������� thread �� ������� cache_updater
        // joinable = true, ����� ��������� ��� ���������� � �����������...
        cache_updater_thread = Glib::Thread::create(sigc::mem_fun(*this, &Viewer::cache_updater), true);

 	add_events (Gdk::BUTTON_PRESS_MASK | 
 		    Gdk::BUTTON_RELEASE_MASK | 
 		    Gdk::POINTER_MOTION_MASK | 
 		    Gdk::POINTER_MOTION_HINT_MASK
	    );
    }

//    virtual void on_realise() {
//	  rubber.reset(new Rubber(this->get_window()));
//    }
    
    virtual ~Viewer (){
        mutex->lock();
	we_need_cache_updater = false;
        cache_updater_cond->signal();
        mutex->unlock();
	// ��������, ���� cache_updater_thread �����������
	cache_updater_thread->join();
	delete(mutex);
        delete(cache_updater_cond);
    }

    void fill_temp_tile (Image<int> & tile, int type = 0) {
	image_brez::line(tile, 0, 0, tile.w, 0, 5, int(0xffff0000));
	image_brez::line(tile, 0, 0, 0, tile.h, 5, int(0xffff0000));
	image_brez::line(tile, tile.w, 0, tile.w, tile.h, 5, int(0xffff0000));
	image_brez::line(tile, 0, tile.h, tile.w, tile.h, 5, int(0xffff0000));

	if (type == 0) {
	    image_brez::line(tile, 0, 0, tile.w, tile.h, 5, int(0xff00ff00));
	    image_brez::line(tile, tile.w, 0, 0, tile.h, 5, int(0xff00ff00));
	} else if (type == 1) {
	    image_brez::line(tile, 0, 0, tile.w, tile.h, 5, int(0xff00ffff));
	    image_brez::line(tile, tile.w, 0, 0, tile.h, 5, int(0xff00ffff));
	}
    }

/**************************************/

    void cache_updater(){

       while (1) {

          mutex->lock();
          while (we_need_cache_updater &&
		 tiles_todo.empty() &&
		 tiles_todo2.empty())
             cache_updater_cond->wait(*mutex);

          if (!we_need_cache_updater) {
             mutex->unlock();
             break;
          }

          if (!tiles_todo.empty()){
             // ������� ������, ������� �������
             Point<int> key = *tiles_todo.begin();
             tiles_todo.erase(key);
             mutex->unlock();

//	     // �����������, ��� �� ������ ��������� ������
//	     fill_temp_tile (tile_cache.find(key)->second, 1);
//	     tile_done_queue.push(key);
//	     update_tile_signal.emit();

             Image<int> tile = workplane->get_image(key);

             mutex->lock();
             // ����� ��� ������������������� � ��������� ���� � ���� �� ������ ������ �������� 8|
             if (tile_cache.count(key)!=0){
                tile_cache.erase(key);
		tile_cache.insert(std::pair<Point<int>,Image<int> >(key, tile));
	     }
             mutex->unlock();

             tile_done_queue.push(key);
             update_tile_signal.emit();
             continue;
          }

          if (!tiles_todo2.empty()) {
             // ������� ������ ������ �������
             Point<int> key = *tiles_todo2.begin();
             tiles_todo2.erase(key);
             mutex->unlock();

             Image<int> tile = workplane->get_image(key);

             mutex->lock();
             tile_cache.insert(std::pair<Point<int>,Image<int> >(key, tile));
             mutex->unlock();

             // � ������ �� ������...
             continue;
          }

          mutex->unlock();
       }
       Glib::Thread::Exit();
    }
/**************************************/

    // ������� ���������� �� ������� �� cache_updater'�
    void update_tile(){
	Point<int> p = tile_done_queue.front();
	tile_done_queue.pop();
#ifdef DEBUG_VIEWER
	std::cerr << "update_tile: " << p << "\n";
#endif
	draw_tile(p);
	//cache_updater_stopped=0; // ��������� cache_updater
	cache_updater_cond->signal();
    }

/**************************************/

    void draw_tile(const Point<int> & tile_key){


      int tile_size = workplane->get_tile_size();
      Rect<int> screen(window_origin.x, 
                       window_origin.y, 
                       get_width(), get_height());
      Rect<int> tile_rect(tile_key.x*tile_size, 
                          tile_key.y*tile_size,
                          tile_size, tile_size);
      Rect<int> tile_in_screen = tile_rect;
      clip_rect_to_rect(tile_in_screen,screen);

      if (tile_in_screen.empty()) return; 

      if (tile_cache.count(tile_key)==0){
        // ���� ����� ������ ��� ��� - ������� ��������� ��������
        // � �������� ������ �� ������������ ���������� �������� � �������
        mutex->lock();
	Image<int> temp_tile(tile_size,tile_size, 0xFF000000);
//	fill_temp_tile (temp_tile);
	tile_cache.insert(std::pair<Point<int>,Image<int> >(tile_key, temp_tile));
        tiles_todo.insert(tile_key);
        cache_updater_cond->signal();
        mutex->unlock();
      }
      Image<int> tile = tile_cache.find(tile_key)->second;
      
      Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(tile);
      Glib::RefPtr<Gdk::GC> gc = get_style()->get_fg_gc (get_state());
      Glib::RefPtr<Gdk::Window> widget = get_window();
	  
      if (!rubber) {
	  rubber.reset(new Rubber(this->get_window()));
      }
      bool rubber_was_visible = false;
      if (rubber->is_visible()) {
	  rubber_was_visible = true;
	  rubber->hide();
      }
      widget->draw_pixbuf(gc, pixbuf, 
			  tile_in_screen.x-tile_rect.x, tile_in_screen.y-tile_rect.y, // on pixbuf
			  tile_in_screen.x-window_origin.x, tile_in_screen.y-window_origin.y,
			  tile_in_screen.w, tile_in_screen.h,
			  Gdk::RGB_DITHER_NORMAL, 0, 0);
      if (rubber_was_visible) {
	  Point<int> pos;
	  Gdk::ModifierType dummy2;
	  get_window()->get_pointer(pos.x, pos.y, dummy2);
	  rubber->update(pos, window_origin);
      }
    }


/**************************************/

    // ����������� ������� ������, ������� ���� � �������
    void fill (int sx, int sy, int w, int h) // in window coordinates, should be inside the window
    {
      // ����� ������ ����� �� ������:
      Rect<int> tiles = tiles_on_rect(
        Rect<int>(window_origin.x + sx, window_origin.y + sy, 
        w, h), workplane->get_tile_size());

#ifdef DEBUG_VIEWER
	std::cerr << "fill: " << sx << "," << sy << " " << w << "x" << h << std::endl;
	std::cerr << "window_origin: " << window_origin << std::endl;
	std::cerr << "tiles: " << tiles << std::endl;
#endif

      // �������� ������, �������� ������� ������ �������.
      for (int tj = tiles.y; tj<tiles.y+tiles.h; tj++){
	for (int ti = tiles.x; ti<tiles.x+tiles.w; ti++){
	  draw_tile(Point<int>(ti,tj));
        }
      }
    }


/**************************************/
private:

    void change_viewport () {
      // tiles -- ������������� ������, ����������� ��� ��������� ������
      Rect<int> tiles = tiles_on_rect(
        Rect<int>(window_origin.x, window_origin.y, 
        get_width(), get_height()), workplane->get_tile_size());

      // ������, ������� ���� ���������, �� �� �������, � ��� ������
      // � ������ -- ��� �����������. ����������� tiles_todo
      mutex->lock();
      std::set<Point<int> >::const_iterator it = tiles_todo.begin(), it1;
      while (it != tiles_todo.end()){
	it1 = it; it1++;
        if (!point_in_rect(*it, tiles)){
          tile_cache.erase(*it);
	  tiles_todo.erase(*it);  
        }
        it = it1;
      }
      mutex->unlock();

      // ����������� tile_cache
      // ����� � ��� ����� ��������:
      // ���� �� ������ w x h ������, �� �� ����� ������� (w+2extra) x (h+2extra) ������ �
      // ���������� �� ���������.
      // � ��� ����� ������� ������ ����� ������� �� ����

      const int extra = std::max(tiles.w, tiles.h);

      Rect<int> tiles_in_cache = Rect<int>
       (tiles.x-extra, tiles.y-extra, tiles.w+2*extra, tiles.h+2*extra);

      std::map<Point<int>, Image<int> >::iterator map_it=tile_cache.begin(), map_it1;

      while (map_it!=tile_cache.end()){
        map_it1 = map_it; map_it1++;
        if (!point_in_rect(map_it->first, tiles_in_cache)){
	    mutex->lock();
	    tile_cache.erase(map_it);
  	    mutex->unlock();
	}
	map_it=map_it1;
      }

      // ������ ������� �� ��������� ������:
      mutex->lock();
      tiles_todo2.clear();
      mutex->unlock();

      for (int x = tiles_in_cache.x; x < tiles_in_cache.x+tiles_in_cache.w; ++x) {
	for (int y = tiles_in_cache.y; y < tiles_in_cache.y+tiles_in_cache.h; ++y) {
	  if (!point_in_rect(Point<int>(x,y), tiles) &&
	      tile_cache.count(Point<int>(x,y))==0) {
	    mutex->lock();
	    tiles_todo2.insert(Point<int>(x,y));
	    cache_updater_cond->signal();
	    mutex->unlock();
	  }
	}
      }
    }
  

/**************************************/
public:

    virtual bool
    on_expose_event (GdkEventExpose * event)
    {
#ifdef DEBUG_VIEWER
	std::cerr << "expose: " << event->area.x << "," << event->area.y << " " << event->area.width << "x" << event->area.height << std::endl;
#endif
	fill (event->area.x, event->area.y, event->area.width, event->area.height);
	return true;
    }


//     virtual bool
//     on_button_press_event (GdkEventButton * event) {
// #ifdef DEBUG_VIEWER
// 	std::cerr << "press: " << event->x << "," << event->y << " " << event->button << std::endl;
// #endif
// 	if (event->button == 1) {
// 	    drag_pos = Point<int> ((int)event->x, (int)event->y);
// 	    if (!rubber) {
// 		rubber.reset(new Rubber(this->get_window()));
// 	    }
// 	    rubber->hide();
// 	    return true;
// 	} else {
// 	    return Gtk::DrawingArea::on_button_press_event (event);
// 	}

//     }

//     virtual bool
//     on_motion_notify_event (GdkEventMotion * event) {
// 	Point<int> pos ((int) event->x, (int) event->y);
// #ifdef DEBUG_VIEWER
// 	std::cerr << "motion: " << pos << std::endl;
// #endif

// 	if (!(event->state & Gdk::BUTTON1_MASK) || !event->is_hint) {
// 	    Gdk::ModifierType dummy2;
// 	    get_window()->get_pointer(pos.x, pos.y, dummy2);
// 	    rubber->update(pos, window_origin);
// 	    return false;
// 	}

// #ifdef DEBUG_VIEWER
// 	std::cerr << "move-hint: " << event->x << "," << event->y << std::endl;
// #endif
// 	Gdk::ModifierType dummy2;
// 	get_window()->get_pointer(pos.x, pos.y, dummy2);
// 	Point<int> shift = pos - drag_pos;
// 	window_origin -= shift;
// 	change_viewport();

// 	get_window()->scroll(shift.x, shift.y);
// 	drag_pos = pos;

// 	return true;
//     }

    void pointer_notify (Point<int> where) {
	if (rubber->is_visible()) {
	    rubber->update(where, window_origin);
	}
    }

    void set_window_origin (Point<int> new_origin) {
	Point<int> shift = window_origin - new_origin;
	if (!rubber) {
	    rubber.reset(new Rubber(this->get_window()));
	}
	bool rubber_was_visible = false;
	if (rubber->is_visible()) {
	    rubber->hide();
	    rubber_was_visible = true;
	}
	window_origin -= shift;
	change_viewport();
	get_window()->scroll(shift.x, shift.y);
	if (rubber_was_visible) {
	    Point<int> pos;
	    Gdk::ModifierType dummy2;
	    get_window()->get_pointer(pos.x, pos.y, dummy2);
	    rubber->update(pos, window_origin);
	}
    }

    void set_window_origin(int x, int y){
	set_window_origin(Point<int>(x,y));
    }

    Point<int> get_window_origin (){
	return window_origin;
    }

    Point<int> get_window_size (){
	return Point<int>(get_width(), get_height());
    }

    void clear_cache(){
      mutex->lock();
      tile_cache.clear();
      tiles_todo.clear();
      tiles_todo2.clear();
      mutex->unlock();

      if (is_mapped()){
	  fill (0, 0, get_width(), get_height());
      }
    }

// ������ � ����������

    void scale_inc(){
	if     (scale_denom()/scale_nom() > 1) set_scale(scale_nom(), scale_denom()/2);
        else set_scale(scale_nom()*2, scale_denom());
    }
    void scale_dec(){
	if     (scale_denom()/scale_nom() >= 1) set_scale(scale_nom(), scale_denom()*2);
        else set_scale(scale_nom()/2, scale_denom());
    }



    void set_scale(int scale_nom, int scale_denom){
#ifdef DEBUG_VIEWER
	std::cerr << "set_scale: " << scale_nom << ":" << scale_denom << std::endl;
#endif
      int n  = workplane->get_scale_denom()*scale_nom;
      int dn = workplane->get_scale_nom()*scale_denom;
      // todo -- ������������������ ���
      workplane->set_scale_nom(scale_nom);
      workplane->set_scale_denom(scale_denom);

      Point<int> wcenter = get_window_origin() + get_window_size()/2;
      window_origin = (wcenter*n)/dn-get_window_size()/2;

      clear_cache();
    }

    int scale_nom(){
      return workplane->get_scale_nom();
    }
    int scale_denom(){
      return workplane->get_scale_denom();
    }
};

#endif /* VIEWER_H */
