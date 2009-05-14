#include "image_jpeg.h"

namespace image_jpeg {

void my_error_exit (j_common_ptr cinfo) {
  my_error_mgr * myerr = (my_error_mgr *) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

// getting file dimensions
Point<int> size(const char *file){
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jpeg_create_decompress(&cinfo);

    FILE * infile;

    if ((infile = fopen(file, "rb")) == NULL) {
        std::cerr << "Can't open " << file << "\n";
        return Point<int>(0,0);
    }

    if (setjmp(jerr.setjmp_buffer)) {
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
      return Point<int>(0,0);
    }

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    Point<int> p(cinfo.image_width, cinfo.image_height);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return p;
}

// loading from Rect in jpeg-file to Rect in image
int load(const char *file, Rect<int> src_rect, 
         Image<int> & image, Rect<int> dst_rect){

    // откроем файл, получим размеры:
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jpeg_create_decompress(&cinfo);

    FILE * infile;

    if ((infile = fopen(file, "rb")) == NULL) {
        std::cerr << "can't open " << file << "\n";
        return 3;
    }
    if (setjmp(jerr.setjmp_buffer)) {
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
      return 2;
    }

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    int jpeg_w = cinfo.image_width;
    int jpeg_h = cinfo.image_height;
    // ч/б и RGB -- все загружается как RGB
    cinfo.out_color_space = JCS_RGB;

    // подрежем прямоугольники
    clip_rects_for_image_loader(
      Rect<int>(0,0,jpeg_w,jpeg_h), src_rect,
      Rect<int>(0,0,image.w,image.h), dst_rect);
    if (src_rect.empty() || dst_rect.empty()) return 1;
    
    // посмотрим, можно ли загружать сразу уменьшенный jpeg
    // (поддерживается уменьшение в 1,2,4,8 раз)
    int xscale = src_rect.w  / dst_rect.w;
    int yscale = src_rect.h / dst_rect.h;
    int scale = std::min(xscale, yscale);

    if (scale <2) cinfo.scale_denom = 1;
    else if (scale <4) cinfo.scale_denom = 2;
    else if (scale <8) cinfo.scale_denom = 4;
    else cinfo.scale_denom = 8;   

#ifdef DEBUG_JPEG
      std::cerr << "jpeg: loading at scale 1/" << cinfo.scale_denom << "\n";
#endif

    src_rect /= cinfo.scale_denom;
    jpeg_w /= cinfo.scale_denom;
    jpeg_h /= cinfo.scale_denom;

    jpeg_start_decompress(&cinfo);

    char *buf1  = new char[(jpeg_w+1) * 3]; 

    int src_y = 0;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
      // откуда мы хотим взять строчку
      int src_y1 = src_rect.y + ((dst_y-dst_rect.y)*src_rect.h)/dst_rect.h;
      // при таком делении может выйти  src_y1 = src_rect.BRC.y, что плохо!
      if (src_y1 >= src_rect.BRC().y) src_y1=src_rect.BRC().y-1;
      // пропустим нужное число строк:
      while (src_y<=src_y1){ 
	jpeg_read_scanlines(&cinfo, (JSAMPLE**)&buf1, 1);
	src_y++;
      }
      // теперь мы находимся на нужной строке
      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
        int src_x = src_rect.x + ((dst_x-dst_rect.x)*src_rect.w)/dst_rect.w;
        if (src_x >= src_rect.BRC().x) src_x=src_rect.BRC().x-1;
	image.set(dst_x, dst_y, 
	    buf1[3*src_x] + (buf1[3*src_x+1]<<8) + (buf1[3*src_x+2]<<16) + (0xFF<<24));
      }
    }

    delete [] buf1;
    jpeg_abort_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
}


// save part of image
int save(const Image<int> & im, const Rect<int> & src_rect, 
         const char *file, int quality){

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

    char *buf  = new char[src_rect.w * 3];

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
}

// load the whole image -- не зависит от формата, вероятно, надо перенести в image_io.h
Image<int> load(const char *file, const int scale){
  Point<int> s = size(file);
  Image<int> ret(s.x/scale,s.y/scale);
  if (s.x*s.y==0) return ret;
  load(file, Rect<int>(0,0,s.x,s.y), ret, Rect<int>(0,0,s.x/scale,s.y/scale));
  return ret;
}

// save the whole image
int save(const Image<int> & im, const char * file, int quality){
  return save(im, im.range(), file, quality);
}

}
