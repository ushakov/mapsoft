#include "image_tiff.h"

namespace image_tiff{

// getting file dimensions
iPoint size(const char *file){
    TIFF* tif = TIFFOpen(file, "rb");

    if (!tif){
//      std::cerr << "Can't read TIFF file!\n";
      return iPoint(0,0);
    }

    uint32 w=0, h=0;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFClose(tif);
    return iPoint(w,h);
}

// loading from Rect in jpeg-file to Rect in image
int load(const char *file, iRect src_rect, iImage & image, Rect<int> dst_rect){

    TIFF* tif = TIFFOpen(file, "rb");
    if (!tif) return 2;

    int tiff_w, tiff_h;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &tiff_w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &tiff_h);

    // подрежем прямоугольники
    clip_rects_for_image_loader(
      iRect(0,0,tiff_w,tiff_h), src_rect,
      iRect(0,0,image.w,image.h), dst_rect);
    if (src_rect.empty() || dst_rect.empty()) return 1;

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

#ifdef DEBUG_TIFF
      std::cerr << "tiff: can we skip lines: " << can_skip_lines << "\n";
#endif


    int src_y = 0;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
      // откуда мы хотим взять строчку
      int src_y1 = src_rect.y + ((dst_y-dst_rect.y)*src_rect.h)/dst_rect.h;
      // при таком делении может выйти  src_y1 = src_rect.BRC.y, что плохо!
      if (src_y1 == src_rect.BRC().y) src_y1--;
      // пропустим нужное число строк:
      if (!can_skip_lines){
        while (src_y<src_y1){
	  TIFFReadScanline(tif, cbuf, src_y);
          src_y++;
        }
      } else {src_y=src_y1;}
      TIFFReadScanline(tif, cbuf, src_y);
      src_y++;

      // теперь мы находимся на нужной строке
      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
        int src_x = src_rect.x + ((dst_x-dst_rect.x)*src_rect.w)/dst_rect.w;
        if (src_x == src_rect.BRC().x) src_x--;
	if (bpp==3) // RGB
 	      image.set(dst_x, dst_y, 
		    cbuf[3*src_x] + (cbuf[3*src_x+1]<<8) + (cbuf[3*src_x+2]<<16) + (0xFF<<24));
	else if (bpp==4) // RGBA
 	      image.set(dst_x, dst_y, 
		    cbuf[4*src_x] + (cbuf[4*src_x+1]<<8) + (cbuf[4*src_x+2]<<16) + (cbuf[4*src_x+3]<<24));
	else if (bpp==1) // G
 	      image.set(dst_x, dst_y, cbuf[src_x] + (cbuf[src_x]<<8) + (cbuf[src_x]<<16) + (0xFF<<24));
      }
    }

    _TIFFfree(cbuf);
    TIFFClose(tif);
    return 0;
}


// save part of image
int save(const iImage & im, const iRect & src_rect,
         const char *file, bool usealpha){

    TIFF* tif = TIFFOpen(file, "wb");

    if (!tif){
      std::cerr << "Can't write TIFF-file!\n";
      return 1;
    }
    int bpp = usealpha?4:3;
    int scan = bpp*src_rect.w;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, src_rect.w);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, src_rect.h);
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

    for (int row = 0; row < src_rect.h; row++){
      if ((row+src_rect.y <0)||(row+src_rect.y>=im.h)){
	for (int col = 0; col < src_rect.w*bpp; col++) cbuf[col] = 0;
      } else {
        for (int col = 0; col < src_rect.h; col++){
	  int c = 0;
          if ((col+src_rect.x >=0)&&(col+src_rect.x<im.w))
             c = im.get(src_rect.x+col,src_rect.y+row);
	  if (bpp==3){ // RGB
    	      cbuf[3*col]   = c & 0xFF;
    	      cbuf[3*col+1] = (c >> 8)  & 0xFF;
    	      cbuf[3*col+2] = (c >> 16) & 0xFF;
          }
	  if (bpp==4){ // RGBA
    	      cbuf[4*col]   = c & 0xFF;
    	      cbuf[4*col+1] = (c >> 8)  & 0xFF;
    	      cbuf[4*col+2] = (c >> 16) & 0xFF;
    	      cbuf[4*col+3] = (c >> 24) & 0xFF;
          }
        }
      }
      TIFFWriteScanline(tif, buf, row);
    }
    _TIFFfree(buf);
    TIFFClose(tif);
    return 0;
}

iImage load(const char *file, const int scale){
  iPoint s = size(file);
  iImage ret(s.x/scale,s.y/scale);
  if (s.x*s.y==0) return ret;
  load(file, iRect(0,0,s.x,s.y), ret, Rect<int>(0,0,s.x/scale,s.y/scale));
  return ret;
}

// save the whole image
int save(const iImage & im, const char * file, bool usealpha){
  return save(im, im.range(), file, usealpha);
}

} // namespace
