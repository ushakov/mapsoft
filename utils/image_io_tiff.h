#ifndef IMAGE_IO_TIF_H
#define IMAGE_IO_TIF_H

#include <string>
#include <cassert>

#include "image.h"
#include "rect.h"
#include "point.h"
#include "color.h"

#include <tiffio.h>

namespace tiff_image{

Point<int> size(std::string file){
    TIFF* tif = TIFFOpen(file.c_str(), "r");

    if (!tif) return Point<int>(0,0);

    uint32 w=0, h=0;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFClose(tif);
    return Point<int>(w,h);
}

Image<int> load(const std::string & file, Rect<int> R, int scale = 1){
    TIFF* tif = TIFFOpen(file.c_str(), "r");
    
    if (!tif) return Image<int>(0,0);

    int w,h;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

    clip_rect_to_rect(R, Rect<int>(0,0,w,h));
    Image<int> ret((R.width()-1)/scale+1, (R.height()-1)/scale+1);

    int scan = TIFFScanlineSize(tif);
    int bpp = scan/w;

    char *cbuf = (char *)_TIFFmalloc(scan);

    // Мы можем устроить произвольный доступ к строчкам,
    // если tiff без сжатия или если каждая строчка запакована отдельно.
    int compression_type, rows_per_strip;
    TIFFGetField(tif, TIFFTAG_COMPRESSION,  &compression_type);
    TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip);
    int step=1;
    if ((compression_type==1)||(rows_per_strip==1)) step=scale;
	
    for (int row = R.TLC.y; row < R.BRC.y; row+=step){
        TIFFReadScanline(tif, cbuf, row);
	if ((row-R.TLC.y)%scale!=0) continue;
	for (int col = R.TLC.x; col < R.BRC.x; col+=scale){
	    if (bpp==3) // RGB
 		  ret.set((col-R.TLC.x)/scale, (row-R.TLC.y)/scale, 
			  (int)RGB(cbuf[3*col], cbuf[3*col+1], cbuf[3*col+2]));
	    else if (bpp==4) // RGBA
 	          ret.set((col-R.TLC.x)/scale, (row-R.TLC.y)/scale, 
			  (int)RGBA(cbuf[4*col], cbuf[4*col+1], cbuf[4*col+2], cbuf[4*col+3]));
	    else if (bpp==1) // G
 	          ret.set((col-R.TLC.x)/scale, (row-R.TLC.y)/scale,
			  (int)RGB(cbuf[col], cbuf[col], cbuf[col]));
    	}
    }
    _TIFFfree(cbuf);
    TIFFClose(tif);
    return ret;
}

Image<int> load(const std::string & file, int scale=1){
  Point<int> s = size(file);
  return load(file, Rect<int>(0,0,s.x,s.y), scale);
}

int save(const std::string & file, const Image<int> & im, bool usealpha = false){
    TIFF* tif = TIFFOpen(file.c_str(), "w");

    if (!tif) return 1;
    int bpp = usealpha?4:3;
    int scan = bpp*im.w;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, im.w0);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, im.h0);
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
}

} // namespace
#endif
