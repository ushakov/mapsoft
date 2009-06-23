#include "image_r.h"

namespace image_r{

iPoint size(const char *file){
    // Поглядим на расширение:
    int l = strlen(file);
    if ((l>=4 && (!strncmp(file + (l-4), ".jpg", 4)||
                  !strncmp(file + (l-4), ".JPG", 4)))||
        (l>=5 && (!strncmp(file + (l-5), ".jpeg", 5)||
                  !strncmp(file + (l-5), ".JPEG", 5)))){
      return image_jpeg::size(file);
    }
    if ((l>=4 && (!strncmp(file + (l-4), ".tif", 4)||
                  !strncmp(file + (l-4), ".TIF", 4)))||
        (l>=5 && (!strncmp(file + (l-5), ".tiff", 5)||
                  !strncmp(file + (l-5), ".TIFF", 5)))){
      return image_tiff::size(file);
    }
    if (l>=4 && (!strncmp(file + (l-4), ".png", 4)||
                 !strncmp(file + (l-4), ".PNG", 4))){
      return image_png::size(file);
    }
    std::cerr << "Can't determine file format by extension in " << file << "\n"
              << "supported formats: jpeg, tiff, png\n";
    return iPoint(0,0);
}

// loading from Rect in file to Rect in image
int load(const char *file, iRect src_rect, 
         iImage & image, iRect dst_rect){

    // Поглядим на расширение:
    int l = strlen(file);
    if (!strncmp(file + (l-4), ".jpg", 4)||
        !strncmp(file + (l-4), ".JPG", 4)||
        !strncmp(file + (l-5), ".jpeg", 5)||
        !strncmp(file + (l-5), ".JPEG", 5)){
      return image_jpeg::load(file, src_rect, image, dst_rect);
    }
    if (!strncmp(file + (l-4), ".tif", 4)||
        !strncmp(file + (l-4), ".TIF", 4)||
        !strncmp(file + (l-5), ".tiff", 5)||
        !strncmp(file + (l-5), ".TIFF", 5)){
      return image_tiff::load(file, src_rect, image, dst_rect);
    }
    if (!strncmp(file + (l-4), ".png", 4)||
        !strncmp(file + (l-4), ".PNG", 4)){
      return image_png::load(file, src_rect, image, dst_rect);
    }
    return 2;
}

// load the whole image 
iImage load(const char *file, const int scale){
  iPoint s = size(file);
  iImage ret(s.x/scale,s.y/scale);
  if (s.x*s.y==0) return ret;
  load(file, iRect(0,0,s.x,s.y), ret, Rect<int>(0,0,s.x/scale,s.y/scale));
  return ret;
}

// save the whole image
// options:
//   jpeg_quality  0..100, default 75
//   tiff_usealpha 0..1,   default 0

int save(const iImage & im, const iRect & src_rect,
         const char *file, const Options & opts){

    // Поглядим на расширение:
    int l = strlen(file);
    if (!strncmp(file + (l-4), ".jpg", 4)||
        !strncmp(file + (l-4), ".JPG", 4)||
        !strncmp(file + (l-5), ".jpeg", 5)||
        !strncmp(file + (l-5), ".JPEG", 5)){
      return image_jpeg::save(im, src_rect, file, opts.get("jpeg_quality",75));
    }
    if (!strncmp(file + (l-4), ".tif", 4)||
        !strncmp(file + (l-4), ".TIF", 4)||
        !strncmp(file + (l-5), ".tiff", 5)||
        !strncmp(file + (l-5), ".TIFF", 5)){
      return image_tiff::save(im, src_rect, file, opts.exists("tiff_usealpha"));
    }
    if (!strncmp(file + (l-4), ".png", 4)||
        !strncmp(file + (l-4), ".PNG", 4)){
      std::cerr << "Save to PNG not supported! Fixme!\n";
//      return image_png::save(im, src_rect, fale);
    }
    return 2;
}

// save the whole image
int save(const iImage & im, const char * file, const Options & opts){
    return save(im, im.range(), file, opts);
}

} // namespace
