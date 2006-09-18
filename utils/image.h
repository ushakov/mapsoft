#ifndef IMAGE_H
#define IMAGE_H

#include "rect.h"

// Картинка с окном. 
// Можно пользоваться или набором data0, w0, h0 (если хочется работать
// с целыми картинками),
// или набором data, w, h, w0 - если хочется работать с окном

// доступ к окну и ко всей картинке
#define IMWIN(i,x,y) i.data[(y)*i.w0+(x)]
#define IMAGE(i,x,y) i.data0[(y)*i.w0+(x)]

template <typename T>
struct Image{

    Image(int _w, int _h){
      w0=w=_w; h0=h=_h; 
      data0 = new T[w0*h0];
      data = data0;
      std::cerr << "Image constructor " << (int)data0 << "  ";
      std::cerr << w0 << "x" << h0 << "\n";
    }

    Image(int _w, int _h, const T & fill){
      w0=w=_w; h0=h=_h; 
      data0 = new T[w0*h0];
      data = data0;
      for (int i = 0; i<w0*h0;i++) data0[i]=fill; 
      std::cerr << "Image constructor " << (int)data0 << "  ";
      std::cerr << w0 << "x" << h0 << " fill with " << fill << "\n";
    }

    Image(const Image & i){
      w0=i.w0; h0=i.h0; 
      w=i.w; h=i.h;
      data0 = new T[w0*h0];
      data = data0 + (i.data-i.data0);
      std::cerr << "Init from another image " << (int)data0 << "  ";
      std::cerr << w0 << "x" << h0 << "\n";
      memcpy(i.data0,data0,w0*h0);
    }

    ~Image(){
      std::cerr << "Image destructor " << (int)data0 << "  ";
      std::cerr << w0 << "x" << h0 << "\n";
      delete[] data0;
    }

    Image & operator= (const Image & im){
      delete[] data0;
      std::cerr << "Image copy " << w0 << "x" << h0 << " <- " << im.w0 << "x" << im.h0 << "\n";
      w0=im.w0;
      h0=im.h0;
      w=im.w;
      h=im.h;
      data0 = new T[w0*h0];
      data = data0+(im.data-im.data0);
      memcpy(im.data0,data0,w0*h0);
      return *this;
    }

    Rect<int> clip_window(Rect<int> r){
      clip_rect_to_rect(r, Rect<int>(0, 0, w0, h0));
      data = data0 + r.TLC.y*w0 + r.TLC.x;
      w = r.BRC.x-r.TLC.x;
      h = r.BRC.y-r.TLC.y;
      return r;
    }

    Rect<int> expand_window(){
      data=data0;
      w=w0; h=h0;
      return Rect<int>(0,0,w0,h0);
    }

    T *data;
    int w,h;

    T *data0;
    int w0,h0;
};

// пока мы не умеем читать из файла часть картинки и уменьшать ее
// при чтении, нам будут полезна такая функция:

template<typename T>
Image<T> fast_resize(const Image<T> & im, int scale){
  if (scale<=1) {
    return Image<T>(im);
  }
  Image<T> ret(im.w0/scale, im.h0/scale);
  for (int j = 0; j<im.h0; j+=scale){  
    for (int i = 0; i<im.w0; i+=scale){  
      ret.data0[i/scale+(j/scale)*ret.h0] = im.data0[i+j*im.h0];
    }
  }
  ret.w = im.w/scale;
  ret.h = im.h/scale;
  int x0 = (im.data-im.data0)%im.w0 / scale;
  int y0 = (im.data-im.data0)/im.w0 / scale;
  ret.data = ret.data0 + ret.w0*y0+x0;
  return ret;
}


template <typename T>
std::ostream & operator<< (std::ostream & s, const Image<T> & i)
{
  s << "Image(" << i.w0 << "x" << i.h0 << " with window "
    << i.w << "x" << i.h << "+" << (i.data-i.data0)%i.w0 << "+" << (i.data-i.data0)/i.w0
    << ")";
  return s;
}


#endif
