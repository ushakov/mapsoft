#include "image_gif.h"
#include <gif_lib.h>
#include "err/err.h"

namespace image_gif{

#if defined(GIFLIB_MAJOR) && defined(GIFLIB_MINOR)
#if GIFLIB_MAJOR == 4 && GIFLIB_MINOR >= 2
#define GIFV 420
#endif
#if GIFLIB_MAJOR >= 5
#define GIFV 500
#endif
#endif

#ifndef GIFV
#define GIFV 0
#endif


// GIFLIB_MAJOR and GIFLIB_MINOR defined in 4.1.6 and later
// GifErrorString(code)
#if GIFV == 500
  GifFileType* GifOpen(const char *file){
    int code;
    GifFileType *gif = DGifOpenFileName(file, &code);
    if (!gif) throw Err() <<  GifErrorString(code);
    return gif;
  }
  void GifClose(GifFileType *gif){
    int code;
    DGifCloseFile(gif, &code);
    if (code) throw Err() <<  GifErrorString(code);
  }
#endif
// 4.2 <= v < 5
// GifErrorString()
#if GIFV == 420
  GifFileType* GifOpen(const char *file){
    GifFileType *gif = DGifOpenFileName(file);
    if (!gif) throw Err() <<  GifErrorString();
    return gif;
  }
  void GifClose(GifFileType *gif){
    DGifCloseFile(gif);
  }
#endif

// old versions
// GifLastError()
#if GIFV == 0
  GifFileType* GifOpen(const char *file){
    GifFileType *gif = DGifOpenFileName(file);
    if (!gif) throw Err() <<  GifLastError();
    return gif;
  }
  void GifClose(GifFileType *gif){
    DGifCloseFile(gif);
  }
#endif



iPoint
size(const char *file){

    GifFileType *gif = GifOpen(file);
    iPoint ret(gif->SWidth, gif->SHeight);
    GifClose(gif);
    return ret;
}

int
load(const char *file, iRect src_rect,
         iImage & image, iRect dst_rect){

    // open file and read screen descriptor
    GifFileType *gif = GifOpen(file);

    /* Go to the first image, skip all extensions */
    GifRecordType RecordType;
    int ExtCode, GifLineLen;
    GifByteType *Extension, *GifLine;
    do {
      if (DGifGetRecordType(gif, &RecordType) == GIF_ERROR)
            {GifClose(gif); return 2;}
      if  (RecordType == TERMINATE_RECORD_TYPE)  return 2;
      if  (RecordType ==  EXTENSION_RECORD_TYPE){
        if (DGifGetExtension(gif, &ExtCode, &Extension) == GIF_ERROR)
          {GifClose(gif); return 2;}
        while (Extension != NULL){
          if (DGifGetExtensionNext(gif, &Extension) == GIF_ERROR)
            {GifClose(gif); return 2;}
        }
      }
    }
    while (RecordType != IMAGE_DESC_RECORD_TYPE);

    /* read image description */
    if (DGifGetImageDesc(gif) == GIF_ERROR)
      {GifClose(gif); return 2;}


    int width = gif->SWidth;
    int height = gif->SHeight;

    int w  = gif->Image.Width;
    int h  = gif->Image.Height;
    int dx = gif->Image.Left;
    int dy = gif->Image.Top;


    int colors[256];
    for (int i=0; i<gif->SColorMap->ColorCount; i++){
      colors[i] = gif->SColorMap->Colors[i].Red +
                  (gif->SColorMap->Colors[i].Green << 8) +
                  (gif->SColorMap->Colors[i].Blue << 16) + (0xFF<<24);
    }
    int bgcolor = colors[gif->SBackGroundColor];

    GifLine = (GifByteType *)malloc(width);
    if (!GifLine){
      std::cerr << "can't allocate memory\n";
      GifClose(gif); return 2;
    }

    // подрежем прямоугольники
    clip_rects_for_image_loader(
      iRect(0,0,width,height), src_rect,
      iRect(0,0,image.w,image.h), dst_rect);
    if (src_rect.empty() || dst_rect.empty()){
      GifClose(gif); free(GifLine); return 1;}

    int src_y = 0;
    for (int dst_y = dst_rect.y; dst_y<dst_rect.y+dst_rect.h; dst_y++){
      // откуда мы хотим взять строчку
      int src_y1 = src_rect.y + ((dst_y-dst_rect.y)*src_rect.h)/dst_rect.h;
      // при таком делении может выйти  src_y1 = src_rect.BRC.y, что плохо!
      if (src_y1 == src_rect.BRC().y) src_y1--;

      if (src_y1 < dy || src_y1>=dy+h){
        for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
          image.set(dst_x, dst_y, bgcolor);
        }
        src_y++;
        continue;
      }

      // пропустим нужное число строк:
      while (src_y<=src_y1){
        if (DGifGetLine(gif, GifLine, w) == GIF_ERROR)
          {GifClose(gif); free(GifLine); return 2;}
        src_y++;
      }
      // теперь мы находимся на нужной строке
      for (int dst_x = dst_rect.x; dst_x<dst_rect.x+dst_rect.w; dst_x++){
        int src_x = src_rect.x + ((dst_x-dst_rect.x)*src_rect.w)/dst_rect.w;
        if (src_x == src_rect.BRC().x) src_x--;
        if (src_x <dx || src_x>=dx+w) image.set(dst_x, dst_y, bgcolor);
        else image.set(dst_x, dst_y, colors[GifLine[src_x-dx]]);
      }
    }
    free(GifLine);
    GifClose(gif);
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
  std::cerr << "GIF writing is not supported\n";
  return 0;
}
} // namespace
