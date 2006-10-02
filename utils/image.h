#ifndef IMAGE_H
#define IMAGE_H

#include "rect.h"

// Картинка с окном -- двумерный массив элементов произвольного типа. 

// При присвоении и инициализации из другой картинки массив данных не копируется!
// (устроен счетчик ссылок на массив, когда ссылок не остается - массив удаляется)

// Копирование картинки и данных: image1 = image.copy()

// Доступ к точкам картинки: image.get(x,y),  image.set(x,y,c)
// Доступ к точкам окна:     image.wget(x,y), image.wset(x,y,c)
// В функциях доступа не проверяется выход за границы картинки!
// Компилить с ключом -O1 (скорость возрастет раза в три)

// Размер картинки: image.w0, image.h0
// Размер окна:     image.w,  image.h

// Получить, установить размеры окна:
//   Rect<int> r = image.window_get();
//   image.window_set(r);  // если прямоугольник вылезает за картинку - он правильно обрезается.
// Установить окно во всю картинку:
//   image.window_expand();

template <typename T>
struct Image{

    Image(int _w, int _h){
      w=ww=_w; 
      h=wh=_h; 
      data = wdata = new T[w*h];
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
      w=ww=_w; 
      h=wh=_h; 
      data = wdata = new T[w*h];
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
      ww=im.ww; wh=im.wh;
      data  = im.data;
      wdata = im.wdata;
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
      ww=im.ww;  wh=im.wh;
      data   = im.data;
      wdata  = im.wdata;
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

    Image copy(){
      Image ret(w,h);
      ret.ww=ww;    
      ret.wh=wh;
      ret.wdata = ret.data+(wdata-data);
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
    inline T wget(int x, int y) const {return wdata[y*w+x];}

    inline void set(int x, int y, T c){data[y*w+x]=c;}
    inline void wset(int x, int y, T c){wdata[y*w+x]=c;}



    Rect<int> window_get(){
      int x = (wdata-data)%w;
      int y = (wdata-data)/w;
      return Rect<int>(x,y,ww,wh);
    }

    Rect<int> window_set(Rect<int> r){
      clip_rect_to_rect(r, Rect<int>(0, 0, w, h));
      wdata = data + r.y*w + r.x;
      ww = r.w;
      wh = r.h;
      return r;
    }

    Rect<int> window_expand(){
      wdata=data;
      ww=w; wh=h;
      return Rect<int>(0,0,w,h);
    }

    T *wdata;
    int ww,wh;

    T *data;
    int w,h;

    int *refcounter;
};

// пока мы не умеем читать из файла часть картинки и уменьшать ее
// при чтении, нам будут полезна такая функция:

template<typename T>
Image<T> fast_resize(const Image<T> & im, int scale){
  if (scale<=1) {
    return Image<T>(im);
  }
  Image<T> ret(im.w/scale, im.h/scale);
  for (int j = 0; j<im.h; j+=scale){  
    for (int i = 0; i<im.w; i+=scale){  
      ret.data[i/scale+(j/scale)*ret.h] = im.data[i+j*im.h];
    }
  }
  ret.ww = im.ww/scale;
  ret.wh = im.wh/scale;
  int wx = (im.wdata-im.data)%im.w / scale;
  int wy = (im.wdata-im.data)/im.w / scale;
  ret.wdata = ret.data + ret.w*wy + wx;
  return ret;
}


template <typename T>
std::ostream & operator<< (std::ostream & s, const Image<T> & i)
{
  s << "Image(" << i.w << "x" << i.h << " with window "
    << i.ww << "x" << i.wh << "+" << (i.wdata-i.data)%i.w << "+" << (i.wdata-i.data)/i.w
    << ")";
  return s;
}


#endif
