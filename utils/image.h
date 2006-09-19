#ifndef IMAGE_H
#define IMAGE_H

#include "rect.h"

// �������� � ����� -- ��������� ������ ��������� ������������� ����. 

// ��� ���������� � ������������� �� ������ �������� ������ ������ �� ����������!
// (������� ������� ������ �� ������, ����� ������ �� �������� - ������ ���������)

// ����������� �������� � ������: image1 = image.copy()

// ������ � ������ ��������: image.get(x,y),  image.set(x,y,c)
// ������ � ������ ����:     image.wget(x,y), image.wset(x,y,c)
// � �������� ������� �� ����������� ����� �� ������� ��������!
// ��������� � ������ -O1 (�������� ��������� ���� � ���)

// ������ ��������: image.w0, image.h0
// ������ ����:     image.w,  image.h

// ��������, ���������� ������� ����:
//   Rect<int> r = image.window_get();
//   image.window_set(r);  // ���� ������������� �������� �� �������� - �� ��������� ����������.
// ���������� ���� �� ��� ��������:
//   image.window_expand();


template <typename T>
struct Image{

    Image(int _w, int _h){
      w0=w=_w; 
      h0=h=_h; 
      data0 = data = new T[w0*h0];
      refcounter   = new int;
      std::cerr << "[create data array]\n";

      *refcounter  = 1;
      std::cerr << "Image create:" 
                << " (" << w0 << "x" << h0 << ", "
                << data0 << " - " << *refcounter << ")\n";
    }

    Image(int _w, int _h, const T & fill){
      w0=w=_w; 
      h0=h=_h; 
      data0 = data = new T[w0*h0];
      refcounter   = new int;
      std::cerr << "[create data array]\n";
      *refcounter  = 1;
      std::cerr << "Image create:" 
                << " (" << w0 << "x" << h0 << ", "
                << data0 << " - " << *refcounter << ") "
                << "filled with " << fill << "\n";
      for (int i = 0; i<w0*h0;i++) data0[i]=fill; 
    }

    Image(const Image & im){
      w0=im.w0; h0=im.h0; 
      w=im.w;   h=im.h;
      data0 = im.data0;
      data  = im.data;
      refcounter = im.refcounter;
      (*refcounter)++; 
      if (*refcounter<=0) {
	std::cerr << "Image: refcounter overflow ("<< *refcounter << ")!\n"; 
	exit(0);
      }
      std::cerr << "Image init from other:" 
                << " (" << w0 << "x" << h0 << ", "
                << data0 << " - " << *refcounter << ")\n";
    }

    ~Image(){
      std::cerr << "Image destructor:" 
                << " (" << w0 << "x" << h0 << ", "
                << data0 << " - " << *refcounter << ")\n";
      (*refcounter)--; 
      if (*refcounter<=0){
	delete[] data0; 
	delete refcounter;
        std::cerr << "[delete data array]\n";
      }
    }

    Image & operator= (const Image & im){
      std::cerr << "Image assign. Old:" 
                << " (" << w0 << "x" << h0 << ", "
                << data0 << " - " << *refcounter << ")\n";

      (*refcounter)--; 
      if (*refcounter<=0){
	delete[] data0; 
	delete refcounter;
        std::cerr << "Image: [delete data array]\n";
      }

      w0=im.w0;  h0=im.h0;
      w=im.w;    h=im.h;
      data0 = im.data0;
      data  = im.data;
      refcounter = im.refcounter;
      (*refcounter)++; 
      if (*refcounter<=0) {
	std::cerr << "Image: refcounter overflow ("<< *refcounter << ")!\n"; 
	exit(0);
      }
      std::cerr << "              New:" 
                << " (" << w0 << "x" << h0 << ", "
                << data0 << " - " << *refcounter << ")\n";
      return *this;
    }

    Image copy(){
      Image ret(0,0);
      delete ret.data0;
      delete ret.refcounter;
      std::cerr << "[delete data array]\n";

      ret.w0=w0;  ret.h0=h0;
      ret.w=w;    ret.h=h;
      ret.data0       = new T[w0*h0];
      ret.refcounter  = new int;
      std::cerr << "[create data array]\n";
      *ret.refcounter = 1;
      ret.data = ret.data0+(data-data0);
      memcpy(data0,ret.data0,w0*h0);
      std::cerr << "Image copy:" 
                << " (" << w0 << "x" << h0 << ", "
                << ret.data0 << " - " << *ret.refcounter << ")\n";
      return ret;
    }

    inline T get(int x, int y){return data0[y*w0+x];}
    inline T wget(int x, int y){return data[y*w0+x];}

    inline void set(int x, int y, T c){data0[y*w0+x]=c;}
    inline void wset(int x, int y, T c){data[y*w0+x]=c;}



    Rect<int> window_get(){
      int x = (data-data0)%w0;
      int y = (data-data0)/w0;
      return Rect<int>(x,y,x+w,y+h);
    }

    Rect<int> window_set(Rect<int> r){
      clip_rect_to_rect(r, Rect<int>(0, 0, w0, h0));
      data = data0 + r.TLC.y*w0 + r.TLC.x;
      w = r.BRC.x-r.TLC.x;
      h = r.BRC.y-r.TLC.y;
      return r;
    }

    Rect<int> window_expand(){
      data=data0;
      w=w0; h=h0;
      return Rect<int>(0,0,w0,h0);
    }

    T *data;
    int w,h;

    T *data0;
    int w0,h0;

    int *refcounter;
};

// ���� �� �� ����� ������ �� ����� ����� �������� � ��������� ��
// ��� ������, ��� ����� ������� ����� �������:

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
