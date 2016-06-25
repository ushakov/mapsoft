#include <tiffio.h>
#include <cstring>
#include <cstdio>
#include <iostream>

// convert TIFF to hgt
using namespace std;

main(int argc, char **argv){

  if (argc!=3) {
    cerr << "Usage: tif2hgt <file.tif> <file.hgt>\n";
    return 1;
  }

  TIFF* tif = TIFFOpen(argv[1], "rb");
  if (!tif){
    cerr << "tif2hgt: can't open file: " << argv[1] << endl;
    return 1;
  }
  int tiff_w, tiff_h;
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &tiff_w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &tiff_h);

  int scan = TIFFScanlineSize(tif);
  int bpp = scan/tiff_w;
  uint8 *cbuf = (uint8 *)_TIFFmalloc(scan);

  int photometric=0;
  TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
  if (photometric != PHOTOMETRIC_MINISBLACK){
    cerr << "tif2hgt: unsupported photometric type: "
         << photometric << endl;
    return 1;
  }
  if (bpp != 2){
    cerr << "tif2hgt: not a 2 byte-per-pixel tiff\n";
    return 1;
  }

  FILE *hgt = fopen(argv[2], "w");
  if (!hgt){
    cerr << "tif2hgt: can't open file: " << argv[2] << "\n";
    return 1;
  }

  bool packed=false;
  if (tiff_h==tiff_w) {
    packed=false;
  }
  else if ((tiff_h-1) == (tiff_w-1)*2){
    packed=true;
  }
  else {
    cerr << "tif2hgt: bad aspect ratio\n";
    return 1;
  }

  for (int y = 0; y<tiff_h; y++){
    TIFFReadScanline(tif, cbuf, y);
    for (int x = 0; x<tiff_w; x++){
      if (packed){
        char c=0;
        fwrite(cbuf+2*x+1, 1, 1, hgt);
        fwrite(cbuf+2*x,   1, 1, hgt);
        if (x<tiff_w-1){
          fwrite(cbuf+2*x+1, 1, 1, hgt);
          fwrite(cbuf+2*x,   1, 1, hgt);
        }
      }
      else {
        fwrite(cbuf+2*x+1, 1, 1, hgt);
        fwrite(cbuf+2*x,   1, 1, hgt);
      }
    }
  }
  fclose(hgt);
  _TIFFfree(cbuf);
  TIFFClose(tif);
  return 0;
}

