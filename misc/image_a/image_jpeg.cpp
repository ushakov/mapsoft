#include <iostream>
#include <iomanip>
#include "2d/image.h"
#include <jpeglib.h>
#include <stdio.h>

void my_error_exit (j_common_ptr cinfo) {
  (*cinfo->err->output_message) (cinfo);
  throw 2;
}

int main(){

  const char ifile[]="i1.jpg";
  const char ofile[]="i2.jpg";

  iImage img;

  char *buf;
  int bw;
  {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * IN;

  // open file, get image size
  cinfo.err = jpeg_std_error(&jerr);
  jerr.error_exit = my_error_exit;
  // load always in RGB mode
  jpeg_create_decompress(&cinfo);

  if ((IN = fopen(ifile, "rb")) == NULL) {
      std::cerr << "can't open " << ifile << "\n";
      throw 3;
  }

  jpeg_stdio_src(&cinfo, IN);
  jpeg_read_header(&cinfo, TRUE);

  cinfo.out_color_space = JCS_RGB;

  cinfo.scale_denom = 1;

  jpeg_start_decompress(&cinfo);
  bw=cinfo.image_width * cinfo.output_components;
  buf  = new char[bw * cinfo.image_height];

  img = iImage(cinfo.image_width, cinfo.image_height);

  for (int i=0; i<cinfo.image_height; i++){
    jpeg_read_scanlines(&cinfo, (JSAMPLE**)&buf, 1);
    for (int j=0; j<cinfo.image_width; j++){
      img.set(j,i, buf[3*j] + (buf[3*j+1]<<8) + (buf[3*j+2]<<16) + (0xFF<<24));
      if (i==100){
        std::cerr << std::hex << (unsigned int)(buf[3*j]) << " " << (unsigned int)buf[3*j+1] << " "
                  << (unsigned int)(buf[3*j+2]) << " "  << img.get(j,i) << "\n";
      }
    }
    
  }

  jpeg_abort_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(IN);
  }

  {

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE * OUT;
    if ((OUT = fopen(ofile, "wb")) == NULL) {
        std::cerr << "Can't open " << ofile << "\n";
        return 1;
    }

    jpeg_stdio_dest(&cinfo, OUT);
    cinfo.image_width = img.w;
    cinfo.image_height = img.h;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, 80, true);
    jpeg_start_compress(&cinfo, TRUE);


    for (int y = 0; y < img.h; y++){
      for (int x = 0; x < img.w; x++){
        int c = img.get(x, y);
        buf[3*x] = c & 0xFF;
        buf[3*x+1] = (c >> 8) & 0xFF;
        buf[3*x+2]   = (c >> 16) & 0xFF;
      }
      jpeg_write_scanlines(&cinfo, (JSAMPLE**)&buf, 1);
    }
    delete [] buf;

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(OUT);
  }
};


