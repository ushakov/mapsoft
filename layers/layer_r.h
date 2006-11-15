#ifndef LAYER_R_H
#define LAYER_R_H

#include "layer.h"
#include "../loaders/image_r.h"
#include "../loaders/image_i.h"
#include <math.h>
#include <string>

// Слой для показа растровых картинок.

// Политика кэширования:
//  - не хочется загружать сразу весь файл, если файл большой.
//  - при перемасштабировании хочется использовать старые данные
//  - при этом, во многих случаях, если уж требуется прочитать что-то с диска -
//    может иметь смысл читать много.
//    (лучше прочитать всю картинку, чем прочитать 1/4, а 3/4 взять из более крупного масштаба!)
// 
// Потому, надо делать так: У нас есть "буфер" - картинка, например
// 2048*2048. если у нас требуют картинку из буфера - мы отдаем картинку
// из буфера. Иначе - мы загружаем новый буфер в нужном масштабе, так,
// чтобы запрошенная картинка была в его центре.


const int layer_r_bufw = 2048;
const int layer_r_bufh = 2048;
 // буфер layer_r_bufw x layer_r_bufh
 // должен быть большим
 // обязательно - больше, чем все запросы к layer'у
 // очень желательно - больше, чем размер экрана, на котором показывается картинка

class LayerR : public Layer {

private:
  std::string        file;
  Rect<int>          file_rect;
  Image<int>         buf;
  Rect<int>          buf_rect;  //  положение буфера на (немасштабированной) картинке
  double             buf_scale; //  текущий масштаб буфера

public:
    LayerR (const char *_file) : file(_file), buf(layer_r_bufw, layer_r_bufh), buf_rect(0,0,0,0), buf_scale(0) { 
        file_rect = Rect<int>(Point<int>(0,0), image_r::size(_file)); 
    }
    
    virtual void draw (Image<int> & img, Rect<int> src_rect, Rect<int> dst_rect){
        // Подрежем прямоугольники:
        clip_rects_for_image_loader(file_rect, src_rect, img.range(), dst_rect);
        if (src_rect.empty() || dst_rect.empty()) return;

	// Какой масштаб нам нужен - во сколько раз будем уменьшать картинку:
        double xscale = (double)src_rect.w / dst_rect.w;
        double yscale = (double)src_rect.h / dst_rect.h;
        double scale = std::min(xscale, yscale);

        // подойдет ли нам буфер? Если нет - перезагрузим его!
        if ((scale < buf_scale) && (buf_scale > 1) ||
             !point_in_rect(src_rect.TLC(), buf_rect) ||
             !point_in_rect(src_rect.BRC(), buf_rect)){
          // определим новое положение буфера:
          int w0 = int(layer_r_bufw * scale);
          int h0 = int(layer_r_bufh * scale);
      // по возможности - чтобы запрошенный кусок был в центре,
          // по возможности - чтобы буфер не торчал за границы файла
          int x0 = (src_rect.x + src_rect.w/2) - w0/2;
          int y0 = (src_rect.y + src_rect.h/2) - h0/2;
          if (x0+w0>file_rect.w) x0=file_rect.w-w0;
          if (y0+h0>file_rect.h) y0=file_rect.h-h0;
          if (x0<0) x0=0;
          if (y0<0) y0=0;
          buf_rect = Rect<int>(x0,y0,w0,h0);
          buf_scale = scale;
#ifdef DEBUG_LAYER_R
      std::cerr << "layer_r loading to buf. Scale: " << buf_scale << ", rect: " << buf_rect << "\n";
#endif
          image_r::load(file.c_str(), buf_rect, buf, buf.range());
        }

        // получим нужную картинку из буфера:
        // какая его часть нам нужна:

        Rect<int> src = src_rect - buf_rect.TLC();
        src.x/=buf_scale;
        src.y/=buf_scale;
        src.w/=buf_scale;
        src.h/=buf_scale;
        image_i::load(buf, src, img, dst_rect);
        
    }

    virtual Rect<int> range (){
	return file_rect;
    }
};


#endif 
