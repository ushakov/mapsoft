#include "image_r.h"
#include "filetype/filetype.h"
#include <cstring>

namespace image_r{

iPoint size(const char *file){
  if (testext(file, ".jpeg") ||
      testext(file, ".jpg")) return image_jpeg::size(file);
  if (testext(file, ".tiff") ||
      testext(file, ".tif")) return image_tiff::size(file);
  if (testext(file, ".png")) return image_png::size(file);
  if (testext(file, ".gif")) return image_gif::size(file);
  return iPoint(0,0);
}

// loading from Rect in file to Rect in image
//int load(const char *file, iRect src_rect, 
//         iImage & image, iRect dst_rect){
//  if (testext(file, ".jpeg") ||
//      testext(file, ".jpg")) return image_jpeg::load(file, src_rect, image, dst_rect);
//  if (testext(file, ".tiff") ||
//      testext(file, ".tif")) return image_tiff::load(file, src_rect, image, dst_rect);
//  if (testext(file, ".png")) return image_png::load(file, src_rect, image, dst_rect);
//  if (testext(file, ".gif")) return image_gif::load(file, src_rect, image, dst_rect);
//  return 2;
//}

// load the whole image 
iImage load(const char *file, const int scale){
  iPoint s = size(file);
  if (s.x*s.y==0) return iImage();
  if (testext(file, ".jpeg") ||
      testext(file, ".jpg")) return image_jpeg::load(file, scale);
  if (testext(file, ".tiff") ||
      testext(file, ".tif")) return image_tiff::load(file, scale);
  if (testext(file, ".png")) return image_png::load(file, scale);
  if (testext(file, ".gif")) return image_gif::load(file, scale);
  return iImage();
}

// save the whole image
// options:
//   jpeg_quality  0..100, default 75

//int save(const iImage & im, const iRect & src_rect,
//         const char *file, const Options & opts){
//
//  if (testext(file, ".jpeg") ||
//      testext(file, ".jpg")) return image_jpeg::save(im, src_rect, file, opts.get("jpeg_quality",75));
//  if (testext(file, ".tiff") ||
//      testext(file, ".tif")) return image_tiff::save(im, src_rect, file);
//  if (testext(file, ".png")) return image_png::save(im, src_rect, file);
//  if (testext(file, ".gif")) return image_gif::save(im, src_rect, file);
//  return 2;
//}

// save the whole image
int save(const iImage & im, const char * file, const Options & opts){
  if (testext(file, ".jpeg") ||
      testext(file, ".jpg")) return image_jpeg::save(im, file, opts.get("jpeg_quality",75));
  if (testext(file, ".tiff") ||
      testext(file, ".tif")) return image_tiff::save(im, file);
  if (testext(file, ".png")) return image_png::save(im, file);
  if (testext(file, ".gif")) return image_gif::save(im, file);
  return 1;
}

} // namespace
