#include "image_r.h"
#include <cstring>

namespace image_r{

bool testext(const char *file, const char *ext){
  int lf=strlen(file);
  int le=strlen(ext);
  return (lf>=le) && (strncmp(file + (lf-le), ext, le)==0);
}

iPoint size(const char *file){
    // Поглядим на расширение:
    if (testext(file, ".jpg") || testext(file, ".jpeg") ||
        testext(file, ".JPG") || testext(file, ".JPEG")){
      return image_jpeg::size(file);
    }
    if (testext(file, ".tif") || testext(file, ".tiff") ||
        testext(file, ".TIF") || testext(file, ".TIFF")){
      return image_tiff::size(file);
    }
    if (testext(file, ".png") || testext(file, ".PNG")){
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
    if (testext(file, ".jpg") || testext(file, ".jpeg") ||
        testext(file, ".JPG") || testext(file, ".JPEG")){
      return image_jpeg::load(file, src_rect, image, dst_rect);
    }
    if (testext(file, ".tif") || testext(file, ".tiff") ||
        testext(file, ".TIF") || testext(file, ".TIFF")){
      return image_tiff::load(file, src_rect, image, dst_rect);
    }
    if (testext(file, ".png") || testext(file, ".PNG")){
      return image_png::load(file, src_rect, image, dst_rect);
    }
    return 2;
}

// load the whole image 
iImage load(const char *file, const int scale){
  iPoint s = size(file);
  iImage ret(s.x/scale,s.y/scale);
  if (s.x*s.y==0) return ret;
  load(file, iRect(0,0,s.x,s.y), ret, iRect(0,0,s.x/scale,s.y/scale));
  return ret;
}

// save the whole image
// options:
//   jpeg_quality  0..100, default 75
//   tiff_usealpha 0..1,   default 0

int save(const iImage & im, const iRect & src_rect,
         const char *file, const Options & opts){

    // Поглядим на расширение:
    if (testext(file, ".jpg") || testext(file, ".jpeg") ||
        testext(file, ".JPG") || testext(file, ".JPEG")){
      return image_jpeg::save(
        im, src_rect, file, opts.get("jpeg_quality",75));
    }
    if (testext(file, ".tif") || testext(file, ".tiff") ||
        testext(file, ".TIF") || testext(file, ".TIFF")){
      return image_tiff::save(im, src_rect, file,
        opts.exists("tiff_usealpha"));
    }
    if (testext(file, ".png") || testext(file, ".PNG")){
      return image_png::save(im, src_rect, file);
    }
    return 2;
}

// save the whole image
int save(const iImage & im, const char * file, const Options & opts){
    return save(im, im.range(), file, opts);
}

} // namespace
