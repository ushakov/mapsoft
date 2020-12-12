#include "image_jpeg.h"

#include "2d/image_source.h"

#include <jpeglib.h>
#include <stdio.h>

namespace image_jpeg{

void
my_error_exit (j_common_ptr cinfo) {
  (*cinfo->err->output_message) (cinfo);
  throw 2;
}

iPoint
size(const char *file){
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    try {
      cinfo.err = jpeg_std_error(&jerr);
      jerr.error_exit = my_error_exit;
      jpeg_create_decompress(&cinfo);

      FILE * infile;

      if ((infile = fopen(file, "rb")) == NULL) {
          std::cerr << "Can't open " << file << "\n";
          throw 3;
      }

      jpeg_stdio_src(&cinfo, infile);
      jpeg_read_header(&cinfo, TRUE);
      iPoint p(cinfo.image_width, cinfo.image_height);
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
      return p;
    }
    catch(int x){ return iPoint(0,0);}
}


struct ImageSourceJPEG : iImageSource {

  size_t row,col;
  unsigned char *buf;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * infile;

  ImageSourceJPEG(const char *file, int denom): row(0),col(0) {
    // open file, get image size
    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = my_error_exit;
    jpeg_create_decompress(&cinfo);

    if ((infile = fopen(file, "rb")) == NULL) {
        std::cerr << "can't open " << file << "\n";
        throw 3;
    }

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    // load always in RGB mode
    cinfo.out_color_space = JCS_RGB;

    assert((denom==1) || (denom==2) || (denom==4) || (denom==8));
    cinfo.scale_denom = denom;

    jpeg_start_decompress(&cinfo);
    buf  = new unsigned char[(cinfo.image_width+1) * 3];
  }

  ~ImageSourceJPEG(){
    delete[] buf;
    jpeg_abort_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
  }


  iRect range() const override{
    return iRect(0,0,cinfo.image_width,cinfo.image_height);
  }

  size_t get_row() const override{
    return row;
  }

  bool skip(const size_t n) override{
    for (size_t i=row; i<row+n; i++){
      if (row>cinfo.image_height) break;
      jpeg_read_scanlines(&cinfo, (JSAMPLE**)&buf, 1);
    }
    row+=n;
    return (row <= cinfo.image_height) && (buf);
  }

  bool read_data(size_t x, int len) override{
    col=x;
    if (len+x > cinfo.image_width) return false;
    return skip(1); // go to the next line
  };

  int get_value(size_t x) const override{
    return 0xFF000000 + buf[3*(x+col)] + (buf[3*(x+col)+1]<<8) +
           (buf[3*(x+col)+2]<<16);
  }

};

int
load(const char *file, iRect src_rect, iImage & image, iRect dst_rect){

    // посмотрим, можно ли загружать сразу уменьшенный jpeg
    // (поддерживается уменьшение в 1,2,4,8 раз)
    int xscale = src_rect.w  / dst_rect.w;
    int yscale = src_rect.h / dst_rect.h;
    int scale = std::min(xscale, yscale);

    int denom=1;
    if (scale <2) denom = 1;
    else if (scale <4) denom = 2;
    else if (scale <8) denom = 4;
    else denom = 8;

    src_rect /= denom;

#ifdef DEBUG_JPEG
      std::cerr << "jpeg: loading at scale 1/" << denom << "\n";
#endif

    try {
      ImageSourceJPEG source(file, denom);
      return source.render_to_image(image, src_rect, dst_rect)? 0:1;
    } catch(int x) {return x;}
}

int
save(const iImage & im, const iRect & src_rect, const char *file, int quality){

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
    cinfo.image_width = src_rect.w;
    cinfo.image_height = src_rect.h;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, quality, true);
    jpeg_start_compress(&cinfo, TRUE);

    unsigned char *buf  = new unsigned char[src_rect.w * 3];

    for (int y = src_rect.y; y < src_rect.y+src_rect.h; y++){
      if ((y<0)||(y>=im.h)){
        for (int x = 0; x < src_rect.w*3; x++) buf[x] = 0;
      } else {
        for (int x = 0; x < src_rect.w; x++){
          int c = 0;
          if ((x+src_rect.x >= 0) && (x+src_rect.x<im.w))
            c = im.get(x+src_rect.x, y);
          buf[3*x] = c & 0xFF;
          buf[3*x+1] = (c >> 8) & 0xFF;
          buf[3*x+2]   = (c >> 16) & 0xFF;
        }
      }
      jpeg_write_scanlines(&cinfo, (JSAMPLE**)&buf, 1);
    }
    delete [] buf;

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
    return 0;
}

iImage
load(const char *file, const int scale){
  iPoint s = size(file);
  iImage ret(s.x/scale,s.y/scale);
  if (s.x*s.y==0) return ret;
  load(file, iRect(0,0,s.x,s.y), ret, iRect(0,0,s.x/scale,s.y/scale));
  return ret;
}

int
save(const iImage & im, const char * file, int quality){
  return save(im, im.range(), file, quality);
}

}
