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

// load part of image
Image<int> load(const std::string & file, Rect<int> R, int scale = 1){

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE * infile;

    if ((infile = fopen(file.c_str(), "rb")) == NULL) {
        std::cerr << "can't open " << file << "\n";
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


    int s1 = cinfo.scale_denom;
    int w1 = w/s1;
    int h1 = h/s1;

    jpeg_start_decompress(&cinfo);
    // здесь размеры уже с учетом scale_denom!

    clip_rect_to_rect(R, Rect<int>(0,0,w,h));
    Image<int> ret((R.width()-1)/scale+1, (R.height()-1)/scale+1);

    char *buf1  = new char[w1*3]; 
    int dstrow=0;
    for (int j = 0; j<h1; j++){
      jpeg_read_scanlines(&cinfo, (JSAMPLE**)&buf1, 1);

      int dy = dstrow*scale;
      int sy  = j*s1;
      int syo = (j-1)*s1;

      if (((sy==0)||(syo>dy)||(sy<dy)) &&
          (sy!=dy)&&(j!=h1-1)) continue;

      int dstcol=0;
      int c;

      for (int i = 0; i<w1; i++){

        c = (buf1[3*i]<<24) + (buf1[3*i+1]<<16) + (buf1[3*i+2]<<8);
        int dx = dstcol*scale;
        int sx  = i*s1;
        int sxo = (i-1)*s1;

        if (((sy==0)||(syo>dy)||(sy<dy)) &&
            (sy!=dy)&&(j!=w1-1)) continue;

	int x = dstcol - R.TLC.x/scale;
	int y = dstrow - R.TLC.y/scale;
        if ((x >= 0)&&(x < ret.w0 )&&
            (y >= 0)&&(y < ret.h0 ))
	  ret.set(x,y,c);
	dstcol++;
      }
      dstrow++;
    }

    delete [] buf1;
    jpeg_abort_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return ret;
}

// load the whole image
Image<int> load(const std::string & file, int scale=1){
  Point<int> s = size(file);
  return load(file, Rect<int>(0,0,s.x,s.y), scale);
}

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
