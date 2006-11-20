#ifndef IMAGE_H
#define IMAGE_H

#include "rect.h"

// �������� -- ��������� ������ ��������� ������������� ����. 

// ��� ���������� � ������������� �� ������ �������� ������ ������ �� ����������!
// (������� ������� ������ �� ������, ����� ������ �� �������� - ������ ���������)

// ����������� �������� � ������: image1 = image.copy()

// ������ � ������ ��������: image.get(x,y),  image.set(x,y,c)

// ����� ���� ������ set_na, set_a
// (�� ������������� ���� ������������, ��� ��������� ��������� ������������ (�� ��������?))

// � �������� ������� �� ����������� ����� �� ������� ��������!
// ��������� � ������ -O1 (�������� ��������� ���� � ���)

// ������ ��������: image.w,  image.h

// �������� ������� ��������:
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

    inline void set_na(int x, int y, T c){data[y*w+x]=c|0xFF000000;}

    inline void set_a(int x, int y, T c){
      int a = c>>24;
      data[y*w+x] = (a==0xFF) ? c :
	((data[y*w+x]>>16) & 0xFF * (255-a) + (c>>16) & 0xFF * a) & 0xFF0000 + 
	(((data[y*w+x]>>8) & 0xFF * (255-a) + (c>>8) & 0xFF * a) >> 8) & 0xFF00 + 
	((data[y*w+x] & 0xFF * (255-a) + (c>>8) & 0xFF * a) >> 16) & 0xFF;
    }

    inline void safe_set(int x, int y, T c){
	if ((x>=0)&&(y>=0)&&(x<w)&&(y<h)) data[y*w+x]=c;
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
