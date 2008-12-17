#ifndef VIEWER_H
#define VIEWER_H

#include <gtkmm.h>
#include <sigc++/sigc++.h>
#include <boost/shared_ptr.hpp>

#include <queue> 

#include "viewer/workplane.h"
#include <lib2d/image.h>
#include <utils/image_gdk.h>
#include <utils/image_brez.h>
#include <utils/cache.h>
#include <viewer/rubber.h>

// 
class Viewer : public Gtk::DrawingArea {


public:

    Viewer (boost::shared_ptr<Workplane> _workplane, 
            boost::shared_ptr<Rubber>    _rubber, 
            Point<int> _window_origin = Point<int>(0,0)/*, 
	    int _scale_nom = 1,
            int _scale_denom = 1*/)
	: workplane (_workplane),
          rubber    (_rubber),
          window_origin(_window_origin),
	  we_need_cache_updater(true)
    {
//        workplane->set_scale(1, 1);
        Glib::thread_init();
	mutex = new(Glib::Mutex);
        cache_updater_cond = new(Glib::Cond);

        update_tile_signal.connect(sigc::mem_fun(*this, &Viewer::update_tile));

        workplane->signal_refresh.connect(sigc::mem_fun(*this, &Viewer::refresh));
        rubber->signal_refresh.connect(sigc::mem_fun(*this, &Viewer::rubber_redraw));


        // ������� ��������� thread �� ������� cache_updater
        // joinable = true, ����� ��������� ��� ���������� � �����������...
        cache_updater_thread = Glib::Thread::create(sigc::mem_fun(*this, &Viewer::cache_updater), true);

 	add_events (Gdk::BUTTON_PRESS_MASK | 
		    Gdk::BUTTON_RELEASE_MASK | 
 		    Gdk::SCROLL_MASK | 
 		    Gdk::POINTER_MOTION_MASK | 
 		    Gdk::POINTER_MOTION_HINT_MASK );



    }


    virtual ~Viewer (){
        mutex->lock();
	we_need_cache_updater = false;
        cache_updater_cond->signal();
        mutex->unlock();
	// ��������, ���� cache_updater_thread ����������
	cache_updater_thread->join();
	delete(mutex);
        delete(cache_updater_cond);
    }

    void set_window_origin (Point<int> new_origin) {
	Point<int> shift = window_origin - new_origin;
//	rubber_take_off();
	window_origin -= shift;
	change_viewport();
	get_window()->scroll(shift.x, shift.y);
        //  rubber render ���������� ��� ��������� �����!
//	rubber_render();
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

    void refresh(){
      std::cerr << "Viewer::refresh()\n";

      // extra � �.�. ������ ���� ����� ��, ��� � change_viewport
      Rect<int> tiles = tiles_on_rect(
        Rect<int>(window_origin.x, window_origin.y,
        get_width(), get_height()), workplane->get_tile_size());

      const int extra = std::max(tiles.w, tiles.h);

      Rect<int> tiles_in_cache = Rect<int>
       (tiles.x-extra, tiles.y-extra, tiles.w+2*extra, tiles.h+2*extra);

      for (int x = tiles_in_cache.x; x < tiles_in_cache.x+tiles_in_cache.w; ++x) {
	for (int y = tiles_in_cache.y; y < tiles_in_cache.y+tiles_in_cache.h; ++y) {
	  mutex->lock();
	  if (point_in_rect(Point<int>(x,y), tiles)) {
	      tiles_todo.insert(Point<int>(x,y));
	  } else {
	      tiles_todo2.insert(Point<int>(x,y));
	  }
	  cache_updater_cond->signal();
	  mutex->unlock();
	}
      }
    }

private:

    boost::shared_ptr<Workplane> workplane;
    boost::shared_ptr<Rubber>    rubber;
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

    Glib::RefPtr<Gdk::GC> rubber_gc;


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
	     tile_cache.erase(key);
             tile_cache.insert(std::make_pair(key, tile));
             mutex->unlock();

             // � ������ �� ������...
	     // upd. ������! ����� refresh �� ��������!
	     // upd. ���� �� � ����
//             tile_done_queue.push(key);
//             update_tile_signal.emit();
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
	  
      widget->draw_pixbuf(gc, pixbuf, 
			  tile_in_screen.x-tile_rect.x, tile_in_screen.y-tile_rect.y, // on pixbuf
			  tile_in_screen.x-window_origin.x, tile_in_screen.y-window_origin.y,
			  tile_in_screen.w, tile_in_screen.h,
			  Gdk::RGB_DITHER_NORMAL, 0, 0);
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
      rubber_take_off();
      for (int tj = tiles.y; tj<tiles.y+tiles.h; tj++){
	for (int ti = tiles.x; ti<tiles.x+tiles.w; ti++){
	  draw_tile(Point<int>(ti,tj));
        }
      }
      rubber_render();
    }


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

    virtual bool on_expose_event (GdkEventExpose * event)
    {
#ifdef DEBUG_VIEWER
	std::cerr << "expose: " << event->area.x << "," << event->area.y << " " << event->area.width << "x" << event->area.height << std::endl;
#endif
	fill (event->area.x, event->area.y, event->area.width, event->area.height);
	return true;
    }



// ����������� � ��������� ������!
    virtual void on_realize() {
        Gtk::DrawingArea::on_realize();
        rubber_gc = Gdk::GC::create(get_window());
        rubber_gc->set_rgb_fg_color(Gdk::Color("white"));
        rubber_gc->set_function(Gdk::XOR);
    }

    virtual bool on_motion_notify_event(GdkEventMotion * event){
        if (!(event->state & Gdk::BUTTON1_MASK)) { 
          rubber_take_off(false); 
          rubber_render(false); 
        }
        return false;
    }

    void rubber_take_off(bool all=true) {
        std::vector<DrawnPair> dummy;
        // erase old lines from drawn vector
        for (int i = 0; i < rubber->drawn.size(); ++i){
          Point<int> p1=rubber->drawn[i].p1 - window_origin;
          Point<int> p2=rubber->drawn[i].p2 - window_origin;
          if (!all && !rubber->drawn[i].active) {
             dummy.push_back(DrawnPair(rubber->drawn[i]));
          } else {
            get_window()->draw_line(rubber_gc, p1.x, p1.y, p2.x, p2.y);
          }
        }
        // update drawn vector
        rubber->drawn.swap(dummy);
    }

    void rubber_render(bool all=true) {
        Point<int> pointer;
        Gdk::ModifierType dummy1;
        get_window()->get_pointer(pointer.x, pointer.y, dummy1);

        // draw new lines, push then into drawn vector
        for (int i = 0; i < rubber->lines.size(); ++i) {
          bool active = rubber->lines[i].first.active || rubber->lines[i].second.active;
          if (!all && !active) continue;
          Point<int> p1=rubber->lines[i].first.get(pointer, window_origin);
          Point<int> p2=rubber->lines[i].second.get(pointer, window_origin);
          get_window()->draw_line(rubber_gc, p1.x, p1.y, p2.x, p2.y);
          rubber->drawn.push_back(DrawnPair(p1+window_origin, p2+window_origin, active));
        }
    }
    void rubber_redraw() {
       rubber_take_off();
       rubber_render();
    }


/**************************************/
};

#endif /* VIEWER_H */
