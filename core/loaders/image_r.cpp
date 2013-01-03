#include "image_r.h"
#include <cstring>

namespace image_r{

bool testext(const char *file, const char *ext){
  int lf=strlen(file);
  int le=strlen(ext);
  return (lf>=le) && (strncasecmp(file + (lf-le), ext, le)==0);
}

#define TYPE_JPG 1
#define TYPE_PNG 2
#define TYPE_TIF 3

int get_type(const char *file){
  if (testext(file, ".jpg") || testext(file, ".jpeg")) return TYPE_JPG;
  if (testext(file, ".tif") || testext(file, ".tiff")) return TYPE_TIF;
  if (testext(file, ".png")) return TYPE_PNG;
  std::cerr << "Unknown file type: " << file << "\n";
  return 0;
}

iPoint size(const char *file){
  switch (get_type(file)){
    case TYPE_JPG: return image_jpeg::size(file);
    case TYPE_TIF: return image_tiff::size(file);
    case TYPE_PNG: return image_png::size(file);
  }
  return iPoint(0,0);
}

// loading from Rect in file to Rect in image
int load(const char *file, iRect src_rect, 
         iImage & image, iRect dst_rect){
  switch (get_type(file)){
    case TYPE_JPG: return image_jpeg::load(file, src_rect, image, dst_rect);
    case TYPE_TIF: return image_tiff::load(file, src_rect, image, dst_rect);
    case TYPE_PNG: return image_png::load(file, src_rect, image, dst_rect);
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

  switch (get_type(file)){
    case TYPE_JPG: return image_jpeg::save(im, src_rect, file, opts.get("jpeg_quality",75));
    case TYPE_TIF: return image_tiff::save(im, src_rect, file, opts.exists("tiff_usealpha"));
    case TYPE_PNG: return image_png::save(im, src_rect, file);
  }
  return 2;
}

// save the whole image
int save(const iImage & im, const char * file, const Options & opts){
    return save(im, im.range(), file, opts);
}

} // namespace
