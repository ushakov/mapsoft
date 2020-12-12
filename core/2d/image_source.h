#ifndef IMAGE_SOURCE_H
#define IMAGE_SOURCE_H

#include "point.h"
#include "rect.h"
#include <cassert>
#include "image.h"

///\addtogroup lib2d
///@{
///\defgroup image_source
///@{

/// ImageSource<T> interface -- source for image loader
template <typename T>
struct ImageSource{

    /// Image range
    virtual iRect range() const = 0;

    /// Get row number
    virtual size_t get_row() const = 0;

    /// Skip n rows of input
    virtual bool skip(const size_t n) = 0;

    /// Prepare data line in [x..x+len) range of current row
    virtual bool read_data(size_t x, int len) = 0;

    /// Get data from the prepared line
    virtual T get_value(size_t x) const = 0;


    /// Render src_rect to dst_rect on Image
    bool render_to_image(iImage & dst_img, iRect src_rect, iRect dst_rect){
      assert(get_row()==0);
      // подрежем прямоугольники
      clip_rects_for_image_loader(
        range(), src_rect, dst_img.range(), dst_rect);
      if (src_rect.empty() || dst_rect.empty()) return false;

      for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
        // source row
        int src_y = src_rect.y + ((dst_y-dst_rect.y)*src_rect.h)/dst_rect.h;
        // we can get src_y1 = src_rect.BRC.y after conversion!
        if (src_y == src_rect.BRC().y) src_y--;
        if ((!skip(src_y - get_row())) ||  // skip input up to src_y line
            (!read_data(src_rect.x, src_rect.w))) break;
        for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
          int src_x = ((dst_x-dst_rect.x)*src_rect.w)/dst_rect.w;
          if (src_x == src_rect.w) src_x--;
          dst_img.set(dst_x, dst_y, get_value(src_x));
        }
      }
      return true;
    }

};

typedef ImageSource<double> dImageSource;
typedef ImageSource<int>    iImageSource;


// ImageSourceImage -- source from Image

template <typename T>
struct ImageSourceImage: ImageSource<T>{
  const Image<T> & I;
  size_t row, col;

  ImageSourceImage(const Image<T> & _I): I(_I), row(0){
  }

  iRect range() const override{
    return I.range();
  }

  size_t get_row() const override{
    return row;
  }

  bool skip(const size_t n) override{
    row+=n;
    return row < I.h;
  }

  bool read_data(size_t x, int len) override{
    col=x;
    return skip(1); // go to the next line
  };

  T get_value(size_t x) const override{
    return I.data[row * I.w + col + x];
  }

};

typedef ImageSourceImage<double> dImageSourceImage;
typedef ImageSourceImage<int>    iImageSourceImage;

#endif
