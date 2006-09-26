#ifndef IMAGE_IO_TIFF_H
#define IMAGE_IO_TIFF_H

#include "image.h"
#include "rect.h"
#include "point.h"

#include <tiffio.h>

namespace tiff_image{

// getting file dimensions
Point<int> size(const char *file){
    TIFF* tif = TIFFOpen(file, "rb");

    if (!tif) return Point<int>(0,0);

    uint32 w=0, h=0;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFClose(tif);
    return Point<int>(w,h);
}

// loading from Rect in jpeg-file to Rect in image
int load_to_image(const char *file, Rect<int> src_rect, Image<int> & image, Rect<int> dst_rect){

    TIFF* tif = TIFFOpen(file, "rb");
    if (!tif) return 1;

    int tiff_w, tiff_h;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &tiff_w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &tiff_h);

    // если на входе пустой прямоугольник - растянем его до максимума
    if (src_rect.empty()) src_rect = Rect<int>(0,0,tiff_w,tiff_h);
    if (dst_rect.empty()) dst_rect = Rect<int>(0,0,image.w,image.h);

    // подрежем прямоугольники
    clip_rect_to_rect(src_rect, Rect<int>(0,0,tiff_w,tiff_h));
    clip_rect_to_rect(dst_rect, Rect<int>(0,0,image.w,image.h));

    int scan = TIFFScanlineSize(tif);
    int bpp = scan/tiff_w;

    char *cbuf = (char *)_TIFFmalloc(scan);

    // Мы можем устроить произвольный доступ к строчкам,
    // если tiff без сжатия или если каждая строчка запакована отдельно.
    bool can_skip_lines = false;

    int compression_type, rows_per_strip;
    TIFFGetField(tif, TIFFTAG_COMPRESSION,  &compression_type);
    TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip);
    if ((compression_type==COMPRESSION_NONE)||(rows_per_strip==1)) can_skip_lines = true;


    int src_y = 0;
    for (int dst_y = dst_rect.TLC.y; dst_y<dst_rect.BRC.y; dst_y++){
      // откуда мы хотим взять строчку
      int src_y1 = src_rect.TLC.y + ((dst_y-dst_rect.TLC.y)*src_rect.height())/dst_rect.height();
      // при таком делении может выйти  src_y1 = src_rect.BRC.y, что плохо!
      if (src_y1 == src_rect.BRC.y) src_y1--;
      // пропустим нужное число строк:
      if (!can_skip_lines){
        while (src_y<src_y1){
	  TIFFReadScanline(tif, cbuf, src_y);
          src_y++;
        }
      } else {src_y=src_y1;}
      TIFFReadScanline(tif, cbuf, src_y);

      // теперь мы находимся на нужной строке
      for (int dst_x = dst_rect.TLC.x; dst_x<dst_rect.BRC.x; dst_x++){
        int src_x = src_rect.TLC.x + ((dst_x-dst_rect.TLC.x)*src_rect.width())/dst_rect.width();
        if (src_x == src_rect.BRC.x) src_x--;
	if (bpp==3) // RGB
 	      image.set(dst_x, dst_y, 
		    (cbuf[3*src_x]<<24) + (cbuf[3*src_x+1]<<16) + (cbuf[3*src_x+2]<<8));
	else if (bpp==4) // RGBA
 	      image.set(dst_x, dst_y, 
		    (cbuf[4*src_x]<<24) + (cbuf[4*src_x+1]<<16) + (cbuf[4*src_x+2]<<8) + cbuf[4*src_x+3]);
	else if (bpp==1) // G
 	      image.set(dst_x, dst_y, (cbuf[src_x]<<24) + (cbuf[src_x]<<16) + (cbuf[src_x]<<8));
      }
    }

    _TIFFfree(cbuf);
    TIFFClose(tif);
    return 0;
}


// save window of image
int wsave(const char *file, const Image<int> & im, bool usealpha = false){

    TIFF* tif = TIFFOpen(file, "wb");

    if (!tif){
      std::cerr << "Can't load TIFF-file\n";
      return 1;
    }
    int bpp = usealpha?4:3;
    int scan = bpp*im.ww;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, im.ww);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, im.wh);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, bpp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG,    1);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,    1);
    TIFFSetField(tif, TIFFTAG_COMPRESSION,     COMPRESSION_LZW);


    if (bpp==4){
      int type=EXTRASAMPLE_UNASSALPHA;
      TIFFSetField(tif, TIFFTAG_EXTRASAMPLES,  1, &type);
    }

    tdata_t buf = _TIFFmalloc(scan);
    char *cbuf = (char *)buf;

    for (int row = 0; row < im.wh; row++){
      for (int col = 0; col < im.ww; col++){
	int c = im.wget(col,row);
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
    return 0;
}

// load the whole image -- не зависит от формата, вероятно, надо перенести в image_io.h
Image<int> load(const char *file, const int scale=1){
  Point<int> s = size(file);
  Image<int> ret(s.x/scale,s.y/scale);
  load_to_image(file, Rect<int>(0,0,s.x,s.y), ret, Rect<int>(0,0,s.x/scale,s.y/scale));
  return ret;
}

// save the whole image
int save(const char * file, const Image<int> & im, bool usealpha = false){
  Image<int> im1 = im;
  im1.window_expand();
  return wsave(file, im1, usealpha);
}

} // namespace
#endif
