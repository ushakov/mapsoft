#ifndef IMAGE_H
#define IMAGE_H

#include "rect.h"

// Картинка -- двумерный массив элементов произвольного типа. 

// При присвоении и инициализации из другой картинки массив данных не копируется!
// (устроен счетчик ссылок на массив, когда ссылок не остается - массив удаляется)

// Копирование картинки и данных: image1 = image.copy()

// Доступ к точкам картинки: image.get(x,y),  image.set(x,y,c)

// также есть версии set_na, set_a
// (не устанавливать байт прозрачности, или аккуратно применить прозрачность (не работает?))

// В функциях доступа не проверяется выход за границы картинки!
// Компилить с ключом -O1 (скорость возрастет раза в три)

// Размер картинки: image.w,  image.h

// Получить размеры картинки:
//   Rect<int> r = image.range();

template <typename T>
struct Image{

    Image(int _w, int _h){
      w=_w; 
      h=_h; 
      data = new T[w*h];
      refcounter   = new int;
      *refcounter  = 1;
#ifdef DEBUG_IMAGE
      std::cerr << "[create data array]\n";
      std::cerr << "Image create:" 
                << " (" << w << "x" << h << ", "
                << data << " - " << *refcounter << ")\n";
#endif
    }

    Image(int _w, int _h, const T & fill){
      w=_w; 
      h=_h; 
      data = new T[w*h];
      refcounter   = new int;
      *refcounter  = 1;
#ifdef DEBUG_IMAGE
      std::cerr << "[create data array]\n";
      std::cerr << "Image create:" 
                << " (" << w << "x" << h << ", "
                << data << " - " << *refcounter << ") "
                << "filled with " << fill << "\n";
#endif
      for (int i = 0; i<w*h;i++) data[i]=fill; 
    }

    Image(const Image & im){
      w=im.w;   h=im.h; 
      data  = im.data;
      refcounter = im.refcounter;
      (*refcounter)++; 
      if (*refcounter<=0) {
	std::cerr << "Image: refcounter overflow ("<< *refcounter << ")!\n"; 
	exit(0);
      }
#ifdef DEBUG_IMAGE
      std::cerr << "Image init from other:" 
                << " (" << w << "x" << h << ", "
                << data << " - " << *refcounter << ")\n";
#endif
    }

    ~Image(){
#ifdef DEBUG_IMAGE
      std::cerr << "Image destructor:" 
                << " (" << w << "x" << h << ", "
                << data << " - " << *refcounter << ")\n";
#endif
      (*refcounter)--; 
      if (*refcounter<=0){
	delete[] data; 
	delete refcounter;
#ifdef DEBUG_IMAGE
        std::cerr << "[delete data array]\n";
#endif
      }
    }

    Image & operator= (const Image & im){
      if (&im == this) return *this;

#ifdef DEBUG_IMAGE
      std::cerr << "Image assign. Old:" 
                << " (" << w << "x" << h << ", "
                << data << " - " << *refcounter << ")\n";
#endif
      (*refcounter)--; 
      if (*refcounter<=0){
	delete[] data; 
	delete refcounter;
#ifdef DEBUG_IMAGE
        std::cerr << "[delete data array]\n";
#endif
      }


      w=im.w;    h=im.h;
      data   = im.data;
      refcounter = im.refcounter;
      (*refcounter)++; 
      if (*refcounter<=0) {
	std::cerr << "Image: refcounter overflow ("<< *refcounter << ")!\n"; 
	exit(0);
      }
#ifdef DEBUG_IMAGE
      std::cerr << "              New:" 
                << " (" << w << "x" << h << ", "
                << data << " - " << *refcounter << ")\n";
#endif
      return *this;
    }

    Image copy() const{
      Image ret(w,h);
      for (int i=0;i<w*h;i++) ret.data[i]=data[i];
#ifdef DEBUG_IMAGE
      std::cerr << "Image copy:" 
                << " (" << w << "x" << h << ", "
                << ret.data << " - " << *ret.refcounter << ")\n";
#endif
      return ret;
    }

    bool empty() const{
      return (w<=0)||(h<=0);
    } 


    inline T get(int x, int y) const {return data[y*w+x];}
    inline void set(int x, int y, T c){data[y*w+x]=c;}
    inline T get(const Point<int> & p) const {return data[p.y*w+p.x];}
    inline void set(const Point<int> & p, T c){data[p.y*w+p.x]=c;}

    inline T safe_get(int x, int y) const{
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return 0;
        return get(x,y);
    }
    inline void safe_set(int x, int y, T c){
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return;
	set(x,y,c);
    }
    inline T safe_get(const Point<int> & p) const{
	return safe_get(p.x, p.y);
    }
    inline void safe_set(const Point<int> & p, T c){
	safe_set(p.x, p.y, c);
    }

    inline void set_na(int x, int y, T c) {
	data[y*w+x] = (c | 0xff000000);
    }
    inline void set_na(const Point<int> & p, T c){ set_na(p.x, p.y, c); }
    
    inline T get_na(int x, int y, T c) const { return data[y*w+x]|0xFF000000;}
    inline T get_na(const Point<int> & p, T c) const {return data[p.y*w+p.x]|0xFF000000;}

    inline T safe_get_na(int x, int y) const{
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return 0;
        return get_na(x,y);
    }
    inline void safe_set_na(int x, int y, T c){
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return;
	set_na(x,y,c);
    }
    inline T safe_get_na(const Point<int> & p) const{
	return safe_get_na(p.x, p.y);
    }
    inline void safe_set_na(const Point<int> & p, T c){
	safe_set_na(p.x, p.y, c);
    }


    inline void set_a(int x, int y, T c){
	unsigned int color = c;
	int a = color >> 24;
	if (a == 0xff) {
	    data[y*w+x] = color;
	} else if (a == 0) {
	    // do nothing
	} else {
	    int r = (((color >> 16) & 0xff) * a + ((data[y*w+x] >> 16) & 0xff) * (255-a)) / 255;
	    int g = (((color >> 8) & 0xff) * a + ((data[y*w+x] >> 8) & 0xff) * (255-a)) / 255;
	    int b = ((color & 0xff) * a + (data[y*w+x] & 0xff) * (255-a)) / 255;
	    data[y*w+x] = (data[y*w+x] & 0xff000000) +
		(r << 16) +
		(g << 8) +
		b;
	}
    }
    inline void set_a(const Point<int> & p, T c){ set_a(p.x, p.y, c); }

    inline void safe_set_a(int x, int y, T c){
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return;
	set_a(x,y,c);
    }
    inline void safe_set_a(const Point<int> & p, T c){
	safe_set_a(p.x, p.y, c);
    }

    inline void render (Point<int> offset, Image<T> const & other) {
	Rect<int> r = rect_intersect(range(), other.range()+offset);
	for (int y = 0; y < r.h; ++y) {
	    for (int x = 0; x < r.w; ++x) {
		set_a(x+offset.x, y+offset.y, other.get(x,y));
	    }
	}
    }
    inline void render (int x, int y, Image<T> const & other) {
	render(Point<int>(x,y), other);
    }



    Rect<int> range() const{
      return Rect<int>(0,0,w,h);
    }

    T *data;
    int w,h;

    int *refcounter;
};

template <typename T>
std::ostream & operator<< (std::ostream & s, const Image<T> & i)
{
  s << "Image(" << i.w << "x" << i.h << ")";
  return s;
}


#endif
