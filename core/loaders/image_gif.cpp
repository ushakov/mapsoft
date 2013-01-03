#include "image_gif.h"
#include <gif_lib.h>

namespace image_gif{

iPoint
size(const char *file){

    GifFileType *gif = DGifOpenFileName(file);
    if (!gif) {
        std::cerr << "Can't open " << file << ": ";
        PrintGifError();
        return iPoint(0,0);
    }
    iPoint ret(gif->SWidth, gif->SHeight);
    DGifCloseFile(gif);
    return ret;
}

int
load(const char *file, iRect src_rect,
         iImage & image, iRect dst_rect){

    // open file and read screen descriptor
    GifFileType *gif = DGifOpenFileName(file);
    if (!gif){ PrintGifError(); return 2; }


    /* Go to the first image, skip all extensions */
    GifRecordType RecordType;
    int ExtCode, GifLineLen;
    GifByteType *Extension, *GifLine;
    do {
      if (DGifGetRecordType(gif, &RecordType) == GIF_ERROR)
            {PrintGifError(); DGifCloseFile(gif); return 2;}
      if  (RecordType == TERMINATE_RECORD_TYPE)  return 2;
      if  (RecordType ==  EXTENSION_RECORD_TYPE){
        if (DGifGetExtension(gif, &ExtCode, &Extension) == GIF_ERROR)
          {PrintGifError(); DGifCloseFile(gif); return 2;}
        while (Extension != NULL){
          if (DGifGetExtensionNext(gif, &Extension) == GIF_ERROR)
            {PrintGifError(); DGifCloseFile(gif); return 2;}
        }
      }
    }
    while (RecordType != IMAGE_DESC_RECORD_TYPE);

    /* read image description */
    if (DGifGetImageDesc(gif) == GIF_ERROR)
      {PrintGifError(); DGifCloseFile(gif); return 2;}


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
      DGifCloseFile(gif); return 2;
    }

    // подрежем прямоугольники
    clip_rects_for_image_loader(
      iRect(0,0,width,height), src_rect,
      iRect(0,0,image.w,image.h), dst_rect);
    if (src_rect.empty() || dst_rect.empty()){
      DGifCloseFile(gif); free(GifLine); return 1;}

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
          {PrintGifError(); DGifCloseFile(gif); free(GifLine); return 2;}
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
    DGifCloseFile(gif);
    return 0;
}

int
save(const iImage & im, const iRect & src_rect, const char *file){
  std::cerr << "GIF writing is not supported\n";
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
