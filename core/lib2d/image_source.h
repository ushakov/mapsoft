#ifndef IMAGE_SOURCE_H
#define IMAGE_SOURCE_H

#include "point.h"
#include "rect.h"
#include <cassert>
#include "image.h"


/// ImageSource<T> interface -- source for image loader

template <typename T>
struct ImageSource{

    int row;

    /// Image range
    virtual iRect range() const = 0;

    /// Skip n rows of input
    virtual bool skip(const int n) = 0;

    /// Get row number
    virtual int get_row() const = 0;

    /// Get data from current row in [x..x+len) range
    virtual T *get_data(int x, int len) const = 0;

    /// Render src_rect to dst_rect on Image
    bool render_to_image(iImage & dst_img, iRect src_rect, iRect dst_rect){
      assert(row!=0);
      // подрежем прямоугольники
      clip_rects_for_image_loader(
        range(), src_rect, dst_img.range(), dst_rect);
      if (src_rect.empty() || dst_rect.empty()) return false;

      for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
        // source row
        int src_y = src_rect.y + ((dst_y-dst_rect.y)*src_rect.h)/dst_rect.h;
        // we can get src_y1 = src_rect.BRC.y after conversion!
        if (src_y == src_rect.BRC().y) src_y--;
        if (!skip(src_y-row)) break; // skip input up to src_y line
        T *D=get_data(src_rect.x, src_rect.w);
        for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
          int src_x = ((dst_x-dst_rect.x)*src_rect.w)/dst_rect.w;
          if (src_x == src_rect.w) src_x--;
          dst_img.set(dst_x, dst_y, D[src_x]);
        }
      }
      return true;
    }

};

typedef ImageSource<double> dImageSource;
typedef ImageSource<int>    iImageSource;


/// ImageSourceImage -- source from Image

template <typename T>
struct ImageSourceImage: ImageSource<T>{
  const Image<T> & I;

  ImageSourceImage(const Image<T> & _I): I(_I){
    ImageSource<T>::row=0;
  }

  iRect range() const{
    return I.range();
  }

  bool skip(const int n){
    ImageSource<T>::row+=n;
    return ImageSource<T>::row < I.h;
  }

  int get_row() const{
    return ImageSource<T>::row;
  }

  T *get_data(int x, int len) const{
    return I.data + (ImageSource<T>::row * I.w + x);
  };
};

typedef ImageSourceImage<double> dImageSourceImage;
typedef ImageSourceImage<int>    iImageSourceImage;

#endif
