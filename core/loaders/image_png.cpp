#include "image_png.h"
#include <png.h>

namespace image_png{

iPoint
size(const char *file){

    FILE * infile;
    if ((infile = fopen(file, "rb")) == NULL) {
        std::cerr << "Can't open " << file << "\n";
        return iPoint(0,0);
    }

    png_byte sign[8];
    const char sign_size = 8;
    if ((fread(sign, 1,sign_size,infile)!=sign_size)||
        (png_sig_cmp(sign, 0, sign_size)!=0)){
        std::cerr << "Not a PNG file: " << file << "\n";
        fclose(infile);
        return iPoint(0,0);
    }

    png_structp png_ptr = png_create_read_struct
       (PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
    if (!png_ptr){
        std::cerr << "Can't make png_read_struct\n";
        fclose(infile);
        return iPoint(0,0);
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_infop end_info = png_create_info_struct(png_ptr);
    if ((!info_ptr) || (!end_info))
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        std::cerr << "Can't make png_info_struct\n";
        fclose(infile);
        return iPoint(0,0);
    }

    if (setjmp(png_jmpbuf(png_ptr))){
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        std::cerr << "Can't read PNG file\n";
        fclose(infile);
        return iPoint(0,0);
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, sign_size);
    png_read_info(png_ptr, info_ptr);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(infile);
    return iPoint(width, height);
}

int
load(const char *file, iRect src_rect, 
         iImage & image, iRect dst_rect){


    FILE * infile;
    if ((infile = fopen(file, "rb")) == NULL)
        return 2;

    png_byte sign[8];
    const char sign_size = 8;
    if ((fread(sign, 1,sign_size,infile)!=sign_size)||
        (png_sig_cmp(sign, 0, sign_size)!=0)){
        fclose(infile);
        return 2;
    }

    png_structp png_ptr = png_create_read_struct
       (PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
    if (!png_ptr){
        fclose(infile);
        return 2;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_infop end_info = png_create_info_struct(png_ptr);
    if ((!info_ptr) || (!end_info)){
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(infile);
        return 2;
    }

    if (setjmp(png_jmpbuf(png_ptr))){
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(infile);
        return 2;
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, sign_size);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height,
       &bit_depth, &color_type, &interlace_type ,NULL,NULL);

    // зададим преобразования
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    if (png_get_valid(png_ptr, info_ptr,
        PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

    if (!(color_type & PNG_COLOR_MASK_ALPHA))
      png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    //

    png_read_update_info(png_ptr, info_ptr);

    png_bytep row_buf = (png_bytep)png_malloc(png_ptr,
      png_get_rowbytes(png_ptr, info_ptr));


    // подрежем прямоугольники
    clip_rects_for_image_loader(
      iRect(0,0,width,height), src_rect,
      iRect(0,0,image.w,image.h), dst_rect);
    if (src_rect.empty() || dst_rect.empty()) return 1;
    if (interlace_type ==PNG_INTERLACE_NONE){
      int src_y = 0;
      for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
        // откуда мы хотим взять строчку
        int src_y1 = src_rect.y + ((dst_y-dst_rect.y)*src_rect.h)/dst_rect.h;
        // при таком делении может выйти  src_y1 = src_rect.BRC.y, что плохо!
        if (src_y1 == src_rect.BRC().y) src_y1--;
        // пропустим нужное число строк:
        while (src_y<=src_y1){
          png_read_row(png_ptr, row_buf, NULL);
          src_y++;
        }
        // теперь мы находимся на нужной строке
        for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
          int src_x = src_rect.x + ((dst_x-dst_rect.x)*src_rect.w)/dst_rect.w;
          if (src_x == src_rect.BRC().x) src_x--;
          int r = row_buf[4*src_x];
          int g = row_buf[4*src_x+1];
          int b = row_buf[4*src_x+2];
          int a = row_buf[4*src_x+3];
          if (a<255){
            r = (r*a)/255;
            g = (g*a)/255;
            b = (b*a)/255;
          }
          image.set(dst_x, dst_y, r + (g<<8) + (b<<16) + (a<<24));
        }
      }
      png_free(png_ptr, row_buf);
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      fclose(infile);
      return 0;
    }

    if (interlace_type == PNG_INTERLACE_ADAM7){
      png_free(png_ptr, row_buf);
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      std::cerr << "PNG_INTERLACE_ADAM7 not supported yet! Fixme!\n";
      fclose(infile);
      return 2;

      // Очень заманчиво сделать честную поддержку interlaced картинок!
      // 1й проход - масштаб 1/8 
      // 1-3й проход - масштаб 1/4
      // 1-5й проход - масштаб 1/2

      // PNG_INTERLACE_ADAM7:
      // Xooooooo ooooXooo oooooooo ooXoooXo oooooooo oXoXoXoX oooooooo
      // oooooooo oooooooo oooooooo oooooooo oooooooo oooooooo XXXXXXXX
      // oooooooo oooooooo oooooooo oooooooo XoXoXoXo oXoXoXoX oooooooo
      // oooooooo oooooooo oooooooo oooooooo oooooooo oooooooo XXXXXXXX
      // oooooooo oooooooo XoooXooo ooXoooXo oooooooo oXoXoXoX oooooooo
      // oooooooo oooooooo oooooooo oooooooo oooooooo oooooooo XXXXXXXX
      // oooooooo oooooooo oooooooo oooooooo XoXoXoXo oXoXoXoX oooooooo
      // oooooooo oooooooo oooooooo oooooooo oooooooo oooooooo XXXXXXXX

      // TODO
      // посмотрим, можно ли загружать сразу уменьшенную картинку
      // (поддерживается уменьшение в 1,2,4,8 раз)
      int xscale = src_rect.w  / dst_rect.w;
      int yscale = src_rect.h / dst_rect.h;
      int scale = std::min(xscale, yscale);
      if (scale <2) scale = 1;
      else if (scale <4) scale = 2;
      else if (scale <8) scale = 4;
      else scale = 8;   
      // TODO

    }

    // других типов PNG_INTERLACE вообще-то не должно быть...
    png_free(png_ptr, row_buf);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(infile);
    return 2;
}

int
save(const iImage & im, const iRect & src_rect, const char *file){

    FILE *outfile = fopen(file, "wb");
    if (!outfile) return 2;

    png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) return 2;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
       png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
       return 2;
    }

    if (setjmp(png_jmpbuf(png_ptr))){
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(outfile);
        return 2;
    }

    png_init_io(png_ptr, outfile);

    png_set_IHDR(png_ptr, info_ptr, src_rect.w, src_rect.h,
       8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    png_bytep buf = (png_bytep)png_malloc(png_ptr, src_rect.w*3);
    if (!info_ptr) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return 2;
    }

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
      png_write_row(png_ptr, buf);
    }
    png_free(png_ptr, buf);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
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
save(const iImage & im, const char * file){
  return save(im, im.range(), file);
}
} // namespace
