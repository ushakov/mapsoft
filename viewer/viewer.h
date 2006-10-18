#ifndef VIEWER_H
#define VIEWER_H

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
    Rect<int>  visible_tiles;
    Point<int> drag_pos;

    std::map<Point<int>, Image<int> > tile_cache;
    std::set<Point<int> >             tiles_todo;
    Point<int>                        tile_done;
    
    // плитки, полученные из workplane лежат в tile_cache.
    // если мы хотим нарисовать какую-то плитку,
    // мы кладем ее координаты в tiles_todo
    // крутящийся отдельно cache_updater_thread
    // просматривает tiles_todo, делает плитку, 
    // и испускает сигнал update_tile_signal,
    // который ловится функцией update_tile();

    Glib::Thread        *cache_updater_thread;
    Glib::Dispatcher       update_tile_signal;

    // cache_updater_thread крутится, пока we_need_cache_updater == true
    bool we_need_cache_updater;
    // нам нужно останавливать cache_updater, пока мы не прорисовали готовую плитку
    bool cache_updater_stopped;

public:

    Viewer (Workplane & _workplane, 
            Point<int> _window_origin = Point<int>(0,0), 
	    int _scale_nom = 1,
            int _scale_denom = 1)
	: workplane (_workplane),
          window_origin(_window_origin),
	  we_need_cache_updater(true),
	  cache_updater_stopped(false)
    {
        workplane.set_scale(_scale_nom, _scale_denom);
        Glib::thread_init();
        update_tile_signal.connect(sigc::mem_fun(*this, &Viewer::update_tile));
        // сделаем отдельный thread из функции cache_updater
        // joinable = true, чтобы подождать его завершения в деструкторе...
        cache_updater_thread = Glib::Thread::create(sigc::mem_fun(*this, &Viewer::cache_updater), true);

	add_events (Gdk::BUTTON_PRESS_MASK | 
		    Gdk::BUTTON_RELEASE_MASK | 
		    Gdk::POINTER_MOTION_MASK | 
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

      Glib::usleep(100);

      if (cache_updater_stopped) continue;

      // какие плитки видны на экране:
//      Rect<int> tiles = pointrect_to_tilerect(
//        Rect<int>(window_origin.x, window_origin.y, get_width(), get_height()) );

      Rect<int> tiles = rect_intdiv(
        Rect<int>(window_origin.x, window_origin.y, 
                  get_width(), get_height()), workplane.get_tile_size());

      // Плитки, которые были запрошены, но уже уехали
      // с экрана -- нам неинтересны. Пропалываем tiles_todo

      std::set<Point<int> >::const_iterator it = tiles_todo.begin(), it1;

      while (it != tiles_todo.end()){
	it1 = it; it1++;
        if (!point_in_rect(*it, tiles)){
          tile_cache.erase(*it);
	  tiles_todo.erase(*it);  
        }
        it = it1;
      }

      // Пропалываем tile_cache
      // Здесь у нас такая политика:
      // если на экране w x h плиток, то мы хотим хранить 3w x 3h плиток и
      // постепенно их заполнять.
      // а все более далекие плитки будем убирать из кэша

      Rect<int> tiles_in_cache = Rect<int>
       (tiles.x-tiles.w, tiles.y-tiles.h, tiles.x+3*tiles.w, tiles.y+3*tiles.h);

      std::map<Point<int>, Image<int> >::iterator map_it=tile_cache.begin(), map_it1;

      while (map_it!=tile_cache.end()){
        map_it1 = map_it; map_it1++;
        if (!point_in_rect(map_it->first, tiles_in_cache)){
	    tile_cache.erase(map_it);
	}
	map_it=map_it1;
      }

      if (!tiles_todo.empty()){
    
        // сделаем плитку, которую просили
 
        Point<int> key = *tiles_todo.begin();
        tile_cache.erase(key);
        tiles_todo.erase(key);
        tile_cache.insert(std::pair<Point<int>,Image<int> >(key, workplane.get_image(key)));
        tile_done = key;
        cache_updater_stopped=true;
        update_tile_signal.emit();
      }
      else {
	// в свободное время сделаем одну из соседних плиток.
        // todo
      }
    }
  }
/**************************************/

    // функция вызывается по сигналу от cache_updater'а
    void update_tile(){
#ifdef DEBUG_VIEWER
      std::cerr << "update_tile: " << tile_done << "\n";
#endif
      draw_tile(tile_done);
      cache_updater_stopped=false;
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

      if (tile_cache.count(tile_key)==0){
        // Если такой плитки еще нет - добавим временную картинку
        // и поместим запрос на изготовление нормальной картинки в очередь
	tile_cache.insert(std::pair<Point<int>,Image<int> >(tile_key, Image<int>(tile_size,tile_size, 0xFF000000)));
        tiles_todo.insert(tile_key);
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
/*
    Rect<int> pointrect_to_tilerect(Rect<int> pr){
      int tile_size = workplane.get_tile_size();
      int x1 = pr.x;
      int x2 = pr.x + pr.w - 1;
      int y1 = pr.y;
      int y2 = pr.y + pr.h - 1;

      x1 = (x1>=0) ? x1/tile_size : x1/tile_size - 1;
      x2 = (x2>0)  ? x2/tile_size : x2/tile_size - 1;
      y1 = (y1>=0) ? y1/tile_size : y1/tile_size - 1;
      y2 = (y2>0)  ? y2/tile_size : y2/tile_size - 1;

      return Rect<int>( x1,y1,x2-x1+1,y2-y1+1);
    }*/
/**************************************/

    void fill (int sx, int sy, int w, int h) // in window coordinates, should be inside the window
    {
//      Rect<int> tiles = pointrect_to_tilerect(
//        Rect<int>(window_origin.x + sx, window_origin.y + sy, w, h)
//      );
      Rect<int> tiles = rect_intdiv(
        Rect<int>(window_origin.x + sx, window_origin.y + sy, 
        w, h), workplane.get_tile_size());


#ifdef DEBUG_VIEWER
	std::cerr << "fill: " << sx << "," << sy << " " << w << "x" << h << std::endl;
	std::cerr << "window_origin: " << window_origin << std::endl;
	std::cerr << "tiles: " << tiles << std::endl;
#endif

      for (int tj = tiles.y; tj<tiles.y+tiles.h; tj++){
	for (int ti = tiles.x; ti<tiles.x+tiles.w; ti++){
	  draw_tile(Point<int>(ti,tj));
        }
      }
    }
/**************************************/


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

    }

    virtual bool
    on_motion_notify_event (GdkEventMotion * event) {
	Point<int> pos ((int) event->x, (int) event->y);

#ifdef DEBUG_VIEWER
	std::cerr << "motion: " << pos << std::endl;
#endif

	if (!(event->state & Gdk::BUTTON1_MASK) || !event->is_hint) 
	    return false;

#ifdef DEBUG_VIEWER
	std::cerr << "move-hint: " << event->x << "," << event->y << std::endl;
#endif
	Gdk::ModifierType dummy2;
	get_window()->get_pointer(pos.x, pos.y, dummy2);
	window_origin += drag_pos - pos;
	fill (0, 0, get_width(), get_height());
	drag_pos = pos;
	return true;
    }

/*
    virtual bool
    on_button_release_event (GdkEventButton * event) {
	Point<int> pos ((int) event->x, (int) event->y);
#ifdef DEBUG_VIEWER
	std::cerr << "release: " << (int)event->x << "," << (int)event->y << std::endl;
#endif
	if (event->button == 1) {
	    in_drag = false;
	    return true;
	} else {
	    return Gtk::DrawingArea::on_button_release_event (event);
	}
    }
*/

/*    virtual bool
    on_keypress_event ( GdkEventKey * event ) {
#ifdef DEBUG_VIEWER
	std::cerr << "key: " << event->keyval << std::endl;
#endif
      switch (event->keyval) {
        case 43:
        case 65451: // +
        {
	  scale_inc();
          return true;
        }
        case 45:
        case 65453: // -
        {
	  scale_dec();
          return true;
        }
      }

      return false;
    }
*/
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

    Point<int> get_window_origin (){
	return window_origin;
    }

    Point<int> get_window_size (){
	return Point<int>(get_width(), get_height());
    }

    void clear_cache(){
	tile_cache.clear();
	tiles_todo.clear();
	fill(0, 0, get_width(), get_height());
    }

// Работы с масштабами

    void scale_inc(){
	if     (scale_denom()/scale_nom() > 1) set_scale(1, scale_denom()/scale_nom()-1);
        else set_scale(scale_nom()/scale_denom() + 1,1);
    }
    void scale_dec(){
	if     (scale_nom()/scale_denom() > 1) set_scale(scale_nom()/scale_denom()-1,1);
        else set_scale(1, scale_denom()/scale_nom() + 1);
    }

    void set_scale(int scale_nom, int scale_denom){
#ifdef DEBUG_VIEWER
	std::cerr << "set_scale: " << scale_nom << ":" << scale_denom << std::endl;
#endif
      int n  = workplane.get_scale_denom()*scale_nom;
      int dn = workplane.get_scale_nom()*scale_denom;
      // todo -- перемасштабировать КЭШ
      
      Point<int> wcenter = get_window_origin() + get_window_size()/2;

      set_window_origin((wcenter*n)/dn-get_window_size()/2);
      workplane.set_scale_nom(scale_nom);
      workplane.set_scale_denom(scale_denom);
      clear_cache();
    }

    int scale_nom(){
      return workplane.get_scale_nom();
    }
    int scale_denom(){
      return workplane.get_scale_denom();
    }
};

#endif /* VIEWER_H */
