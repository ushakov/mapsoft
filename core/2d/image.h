#ifndef IMAGE_H
#define IMAGE_H

#include "point.h"
#include "line_tests.h"
#include "rect.h"
#include <cassert>

#ifdef DEBUG_IMAGE
#include <iostream>
#endif


///\addtogroup lib2d
///@{
///\defgroup image
///@{

/// 2-d array of T elements.
/// - Image data is not duplicated at copying.
/// - Compile with -O1 option to increase speed
template <typename T>
struct Image{

  public:
    int w,h;
    T *data;

  private:
    int *refcounter;

    /// create image
    void create(int _w, int _h){
      w=_w; h=_h;
      assert(w>=0);
      assert(h>=0);
      data = new T[w*h];
      assert(data);
      refcounter   = new int;
      *refcounter  = 1;
#ifdef DEBUG_IMAGE
      std::cerr << "[create data array]\n";
      std::cerr << "Image create:" 
                << " (" << w << "x" << h << ", "
                << data << " - " << *refcounter << ")\n";
#endif
    }

    /// copy image
    void copy(const Image & other){
      w=other.w; h=other.h;
      data = other.data;
      refcounter = other.refcounter;
      (*refcounter)++;
      assert(*refcounter>0);
#ifdef DEBUG_IMAGE
      std::cerr << "Copy image:"
                << " (" << w << "x" << h << ", "
                << data << " - " << *refcounter << ")\n";
#endif
    }

    /// destroy image
    void destroy(void){
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

  public:
    /// Create empty image
    Image(){
      create(0,0);
    }
    /// Create image with uninitialized data
    Image(int _w, int _h){
      create(_w, _h);
    }
    /// Create image with data set to value
    Image(int _w, int _h, const T & value){
      create(_w, _h);
      fill(value);
    }
    /// Copy constructor
    Image(const Image & other){
      copy(other);
    }
    /// Destuctor
    ~Image(){
      destroy();
    }
    /// Assign
    Image & operator= (const Image & other){
      if (&other == this) return *this;
      destroy();
      copy(other);
      return *this;
    }
    /// Create copy of image data
    Image dup() const{
      Image ret(w,h);
      for (int i=0;i<w*h;i++) ret.data[i]=data[i];
#ifdef DEBUG_IMAGE
      std::cerr << "Image copy:" 
                << " (" << w << "x" << h << ", "
                << ret.data << " - " << *ret.refcounter << ")\n";
#endif
      return ret;
    }

    /// Fill image with some value
    void fill(const T value){
      for (int i = 0; i<w*h;i++) data[i]=value;
    }
    /// with transparency
    void fill_a(const T value){
      for (int j=0; j<h; j++)
        for (int i=0; i<w; i++) set_a(i,j,value);
    }
    /// Is image empty
    bool empty() const{
      return (w<=0)||(h<=0);
    }
    /// Image range
    iRect range() const{
      return iRect(0,0,w,h);
    }


    /// get and set functions
    inline T get(int x, int y) const {
      return data[y*w+x];
    }
    inline void set(int x, int y, T c){
      data[y*w+x]=c;
    }
    inline T get(const iPoint & p) const {
      return data[p.y*w+p.x];
    }
    inline void set(const iPoint & p, T c){
      data[p.y*w+p.x]=c;
    }


    inline T safe_get(int x, int y, T def = 0) const{
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return def;
        return get(x,y);
    }
    inline void safe_set(int x, int y, T c){
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return;
	set(x,y,c);
    }
    inline T safe_get(const iPoint & p, T def = 0) const{
	return safe_get(p.x, p.y, def);
    }
    inline void safe_set(const iPoint & p, T c){
	safe_set(p.x, p.y, c);
    }


    // (Only useful for iImage)
    inline T get_na(int x, int y, T c) const {
      return data[y*w+x] | 0xFF000000;
    }
    inline void set_na(int x, int y, T c) {
	data[y*w+x] = (c | 0xff000000);
    }
    inline T get_na(const iPoint & p, T c) const {
      return data[p.y*w+p.x] | 0xFF000000;
    }
    inline void set_na(const iPoint & p, T c){
      set_na(p.x, p.y, c);
    }

    inline T safe_get_na(int x, int y, T def = 0) const{
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return def;
        return get_na(x,y);
    }
    inline void safe_set_na(int x, int y, T c){
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return;
	set_na(x,y,c);
    }
    inline T safe_get_na(const iPoint & p, T def = 0) const{
	return safe_get_na(p.x, p.y, def);
    }
    inline void safe_set_na(const iPoint & p, T c){
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
            int ao = data[y*w+x] >> 24;
	    int an = ao + (255-ao) * a / 255;

	    int r = (((color >> 16) & 0xff) * a +
                     ((data[y*w+x] >> 16) & 0xff) * (255-a)) / 255;
	    int g = (((color >> 8) & 0xff) * a +
                     ((data[y*w+x] >> 8) & 0xff) * (255-a)) / 255;
	    int b = ((color & 0xff) * a +
                     (data[y*w+x] & 0xff) * (255-a)) / 255;
	    data[y*w+x] =
              (an << 24) +
	      (r << 16) +
	      (g << 8) +
	      b;
	}
    }
    inline void set_a(const iPoint & p, T c){
      set_a(p.x, p.y, c);
    }

    inline void safe_set_a(int x, int y, T c){
	if ((x<0)||(y<0)||(x>=w)||(y>=h)) return;
	set_a(x,y,c);
    }
    inline void safe_set_a(const iPoint & p, T c){
	safe_set_a(p.x, p.y, c);
    }

    // Set full transparency outside border line, remove transparency inside it.
    // If border line is empty, remove transparancy on the whole image.
    // (Only useful for iImage)
    void set_border(const iLine & brd = iLine()){
      iLineTester  lt(brd);

      //process image rows
      for (int j = 0; j < h; j++){
        // remove transparency if border is empty
        if (brd.size() == 0){
          for (int i=0; i<w; i++) data[j*w+i] = data[j*w+i] | 0xFF000000;
          continue;
        }
        // get sorted border crossings for each row
        std::vector<int> cr = lt.get_cr(j);

        // set/remove tranparency
        size_t i=0;
        for (size_t k = 0; k < cr.size()+1; k++){
          int ic = (k<cr.size())?std::min(cr[k], w) : w;
          if (ic < 0) continue;
          for (size_t ii=i; ii<(size_t)ic; ii++){
            data[j*w+ii] = (k%2)?
               data[j*w+ii] | 0xFF000000 :
               data[j*w+ii] & 0x00FFFFFF ;
          }
          i=ic;
          if (i>=(size_t)w) break;
        }
      }
    }

    inline void render (iPoint offset, Image<T> const & other) {
      iRect r = rect_intersect(range(), other.range()+offset);
      for (int y = 0; y < r.h; ++y) {
        for (int x = 0; x < r.w; ++x) {
          set_a(x+offset.x, y+offset.y, other.get(x,y));
        }
      }
    }
    inline void render (int x, int y, Image<T> const & other) {
      render(iPoint(x,y), other);
    }
#ifdef SWIG
  %extend {
    swig_str();
  }
#endif  // SWIG
};

/// \relates Image
typedef Image<double> dImage;

/// \relates Image
typedef Image<int> iImage;

/// \relates Image
typedef Image<char> cImage;

/// \relates Image
typedef Image<short int> sImage;


/// \relates Image
template <typename T>
std::ostream & operator<< (std::ostream & s, const Image<T> & i)
{
  s << "Image(" << i.w << "x" << i.h << ")";
  return s;
}


#endif
