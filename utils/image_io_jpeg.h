#ifndef IMAGE_IO_TIF_H
#define IMAGE_IO_TIF_H

#include <string>
#include <cassert>

#include "image.h"
#include "rect.h"
#include "point.h"
#include "color.h"

#include <jpeg.h>

namespace jpeg_image{

// getting file dimensions
Point<int> size(std::string file){
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE * infile;

    if ((infile = fopen(file.c_str(), "rb")) == NULL) {
        std::cerr << "can't open " << filename << "\n";
        return Point<int>(0,0);
    }
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    Point<int> p(cinfo.image_width, cinfo.image_height);
    jpeg_destroy_decompress(&cinfo);

    return p;
}

// load part of image
Image<int> load(const std::string & file, Rect<int> R, int scale = 1){

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE * infile;

    if ((infile = fopen(file.c_str(), "rb")) == NULL) {
        std::cerr << "can't open " << filename << "\n";
        return Image<int>(0,0);
    }

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    int w = cinfo.image_width;
    int h = cinfo.image_height;

    cinfo.out_color_space = JCS_RGB; // Даже если grayscale
    // поддерживается уменьшение в 1,2,4,8 раз
    if (scale <1) scale=1;
    if (scale <2) cinfo.scale_denom = 1;
    else if (scale <4) cinfo.scale_denom = 2;
    else if (scale <8) cinfo.scale_denom = 4;
    else cinfo.scale_denom = 8;   

    jpeg_start_decompress(&cinfo);

    int w1 = cinfo.image_width;
    int h1 = cinfo.image_height;
    int s1 = cinfo.scale_denom;
    // здесь размеры уже с учетом scale_denom!

    clip_rect_to_rect(R, Rect<int>(0,0,w,h));
    Image<int> ret((R.width()-1)/scale+1, (R.height()-1)/scale+1);

    char *buf  = new char[cinfo.image_width]; 

    for (int j = 0; j<h1; j++){
      jpeg_read_scanlines(&cinfo, &buf, 1); // **char !
      if (j%(h/scale) ///!!!
      for (int i = 0; i<cinfo.image_width; i++){
      }
    }
    delete [] buf;
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

/////////////


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

// load the whole image
Image<int> load(const std::string & file, int scale=1){
  Point<int> s = size(file);
  return load(file, Rect<int>(0,0,s.x,s.y), scale);
}

// save window of image
int wsave(const std::string & file, const Image<int> & im, int quality=75){

	if ((quality<0)||(quality>100)){
            std::cerr << "jpeg quality not in range 0..100 (" << quality << ")\n";
            return 0;
        }

        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

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

        JSAMPROW row_pointer[1];        /* pointer to a single row */

        while (cinfo.next_scanline < cinfo.image_height) {
            row_pointer[0] = & image_buffer[cinfo.next_scanline * im.w * 3];
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);


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

// save the whole image
int save(const std::string & file, const Image<int> & im, int quality=75){
  Image<int> im1 = im;
  im1.window_expand();
  return wsave(file, im1, quality);
}

} // namespace
#endif
