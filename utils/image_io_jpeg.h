#ifndef IMAGE_IO_JPEG_H
#define IMAGE_IO_JPEG_H

#include <string>
#include <cassert>

#include "image.h"
#include "rect.h"
#include "point.h"
#include "color.h"

#include <jpeglib.h>

namespace jpeg_image{

// getting file dimensions
Point<int> size(std::string file){
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE * infile;

    if ((infile = fopen(file.c_str(), "rb")) == NULL) {
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
int load(char *file, Rect<int> src_rect, Image<int> & image, Rect<int> dst_rect){

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

    // тут еще нужна проверка на то, что файл не JPEG

    int jpeg_w = cinfo.image_width;
    int jpeg_h = cinfo.image_height;
    cinfo.out_color_space = JCS_RGB; // Даже если grayscale

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

/*
// load the whole image
Image<int> load(const std::string & file, int scale=1){
  Point<int> s = size(file);
  return load(file, Rect<int>(0,0,s.x,s.y), scale);
}
*/
/*
// save window of image
int wsave(const std::string & file, const Image<int> & im, int quality=75){

	if ((quality<0)||(quality>100)){
            std::cerr << "jpeg quality not in range 0..100 (" << quality << ")\n";
            return 0;
        }

        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);

        FILE * outfile;
        if ((outfile = fopen(file.c_str(), "wb")) == NULL) {
            std::cerr << "can't open " << filename << "\n";
            return 0;
        }

        jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = im.w;
	cinfo.image_height = im.h;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
        jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, quality, true);
        jpeg_start_compress(&cinfo, TRUE);

        JSAMPROW row_pointer[1];        // pointer to a single row

        while (cinfo.next_scanline < cinfo.image_height) {
            row_pointer[0] = & image_buffer[cinfo.next_scanline * im.w * 3];
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
    fclose(infile);
//////

    tdata_t buf = _TIFFmalloc(scan);
    char *cbuf = (char *)buf;

    for (int row = 0; row < im.h; row++){
      for (int col = 0; col < im.w; col++){
	int c = im.get(col,row);
	if (bpp==3){ // RGB
    	    cbuf[3*col]   = (c >> 24) & 0xFF;
    	    cbuf[3*col+1] = (c >> 16) & 0xFF;
    	    cbuf[3*col+2] = (c >> 8)  & 0xFF;
        }
	if (bpp==4){ // RGBA
    	    cbuf[4*col]   = (c >> 24) & 0xFF;
    	    cbuf[4*col+1] = (c >> 16) & 0xFF;
    	    cbuf[4*col+2] = (c >> 8)  & 0xFF;
    	    cbuf[4*col+3] = c & 0xFF;
        }
      }
      TIFFWriteScanline(tif, buf, row);
    }
    _TIFFfree(buf);
    TIFFClose(tif);
    fclose(infile);
}

// save the whole image
int save(const std::string & file, const Image<int> & im, int quality=75){
  Image<int> im1 = im;
  im1.window_expand();
  return wsave(file, im1, quality);
}*/

} // namespace
#endif
