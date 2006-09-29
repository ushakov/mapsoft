#ifndef IMAGE_IO_JPEG_H
#define IMAGE_IO_JPEG_H

#include "image.h"
#include "rect.h"
#include "point.h"

#include <jpeglib.h>

namespace jpeg{

// getting file dimensions
Point<int> size(const char *file){
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE * infile;

    if ((infile = fopen(file, "rb")) == NULL) {
        std::cerr << "can't open " << file << "\n";
        return Point<int>(0,0);
    }
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    Point<int> p(cinfo.image_width, cinfo.image_height);
    jpeg_destroy_decompress(&cinfo);

    return p;
}

// loading from Rect in jpeg-file to Rect in image
int load_to_image(const char *file, Rect<int> src_rect, Image<int> & image, Rect<int> dst_rect){

    // откроем файл, получим размеры:
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE * infile;

    if ((infile = fopen(file, "rb")) == NULL) {
        std::cerr << "can't open " << file << "\n";
        return 1;
    }

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    // тут еще нужна проверка на то, что файл не JPEG!!!

    int jpeg_w = cinfo.image_width;
    int jpeg_h = cinfo.image_height;
    // ч/б и RGB -- все загружается как RGB
    cinfo.out_color_space = JCS_RGB;

    // если на входе пустой прямоугольник - растянем его до максимума
    if (src_rect.empty()) src_rect = Rect<int>(0,0,jpeg_w,jpeg_h);
    if (dst_rect.empty()) dst_rect = Rect<int>(0,0,image.w,image.h);

    // подрежем прямоугольники
    clip_rect_to_rect(src_rect, Rect<int>(0,0,jpeg_w,jpeg_h));
    clip_rect_to_rect(dst_rect, Rect<int>(0,0,image.w,image.h));
    
    // посмотрим, можно ли загружать сразу уменьшенный jpeg
    // (поддерживается уменьшение в 1,2,4,8 раз)
    int xscale = src_rect.width()  / dst_rect.width();
    int yscale = src_rect.height() / dst_rect.height();
    int scale = std::min(xscale, yscale);

    if (scale <2) cinfo.scale_denom = 1;
    else if (scale <4) cinfo.scale_denom = 2;
    else if (scale <8) cinfo.scale_denom = 4;
    else cinfo.scale_denom = 8;   

    src_rect /= cinfo.scale_denom;
    jpeg_w /= cinfo.scale_denom;
    jpeg_h /= cinfo.scale_denom;

    jpeg_start_decompress(&cinfo);

    char *buf1  = new char[jpeg_w * 3]; 

    int src_y = 0;
    for (int dst_y = dst_rect.TLC.y; dst_y<dst_rect.BRC.y; dst_y++){
      // откуда мы хотим взять строчку
      int src_y1 = src_rect.TLC.y + ((dst_y-dst_rect.TLC.y)*src_rect.height())/dst_rect.height();
      // при таком делении может выйти  src_y1 = src_rect.BRC.y, что плохо!
      if (src_y1 == src_rect.BRC.y) src_y1--;
      // пропустим нужное число строк:
      while (src_y<=src_y1){ 
	jpeg_read_scanlines(&cinfo, (JSAMPLE**)&buf1, 1);
	src_y++;
      }
      // теперь мы находимся на нужной строке
      for (int dst_x = dst_rect.TLC.x; dst_x<dst_rect.BRC.x; dst_x++){
        int src_x = src_rect.TLC.x + ((dst_x-dst_rect.TLC.x)*src_rect.width())/dst_rect.width();
        if (src_x == src_rect.BRC.x) src_x--;
	image.set(dst_x, dst_y, 
	    (buf1[3*src_x]<<24) + (buf1[3*src_x+1]<<16) + (buf1[3*src_x+2]<<8));
      }
    }

    delete [] buf1;
    jpeg_abort_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
}


// save window of image
int wsave(const char *file, const Image<int> & im, int quality=75){

    if ((quality<0)||(quality>100)){
        std::cerr << "JPEG quality not in range 0..100 (" << quality << ")\n";
        return 1;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE * outfile;
    if ((outfile = fopen(file, "wb")) == NULL) {
        std::cerr << "Can't open " << file << "\n";
        return 1;
    }

    jpeg_stdio_dest(&cinfo, outfile);
    cinfo.image_width = im.ww;
    cinfo.image_height = im.wh;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, quality, true);
    jpeg_start_compress(&cinfo, TRUE);

    char *buf  = new char[im.ww * 3];
    for (int y = 0; y < im.wh; y++){
      for (int x = 0; x < im.ww; x++){
        int c = im.wget(x,y);
        buf[3*x]   = (c >> 24) & 0xFF;
        buf[3*x+1] = (c >> 16) & 0xFF;
        buf[3*x+2] = (c >> 8)  & 0xFF;
      }
      jpeg_write_scanlines(&cinfo, (JSAMPLE**)&buf, 1);
    }
    delete [] buf;

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

// load the whole image -- не зависит от формата, вероятно, надо перенести в image_io.h
Image<int> load(const char *file, const int scale=1){
  Point<int> s = size(file);
  Image<int> ret(s.x/scale,s.y/scale);
  load_to_image(file, Rect<int>(0,0,s.x,s.y), ret, Rect<int>(0,0,s.x/scale,s.y/scale));
  return ret;
}

// save the whole image
int save(const char * file, const Image<int> & im, int quality=75){
  Image<int> im1 = im;
  im1.window_expand();
  return wsave(file, im1, quality);
}

} // namespace
#endif
