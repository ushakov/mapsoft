#ifndef LAYER_R_H
#define LAYER_R_H

#include "layer.h"
#include "../loaders/image_r.h"
#include "../loaders/image_i.h"
#include <math.h>
#include <string>

// ���� ��� ������ ��������� ��������.

// �������� �����������:
//  - �� ������� ��������� ����� ���� ����, ���� ���� �������.
//  - ��� ������������������� ������� ������������ ������ ������
//  - ��� ����, �� ������ �������, ���� �� ��������� ��������� ���-�� � ����� -
//    ����� ����� ����� ������ �����.
//    (����� ��������� ��� ��������, ��� ��������� 1/4, � 3/4 ����� �� ����� �������� ��������!)
// 
// ������, ���� ������ ���: � ��� ���� "�����" - ��������, ��������
// 2048*2048. ���� � ��� ������� �������� �� ������ - �� ������ ��������
// �� ������. ����� - �� ��������� ����� ����� � ������ ��������, ���,
// ����� ����������� �������� ���� � ��� ������.


const int layer_r_bufw = 2048;
const int layer_r_bufh = 2048;
 // ����� layer_r_bufw x layer_r_bufh
 // ������ ���� �������
 // ����������� - ������, ��� ��� ������� � layer'�
 // ����� ���������� - ������, ��� ������ ������, �� ������� ������������ ��������

class LayerR : public Layer {

private:
  std::string        file;
  Rect<int>          file_rect;
  Image<int>         buf;
  Rect<int>          buf_rect;  //  ��������� ������ �� (������������������) ��������
  double             buf_scale; //  ������� ������� ������

public:
    LayerR (const char *_file) : file(_file), buf(layer_r_bufw, layer_r_bufh), buf_rect(0,0,0,0), buf_scale(0) { 
        file_rect = Rect<int>(Point<int>(0,0), image_r::size(_file)); 
    }
    
    virtual void draw (Image<int> & img, Rect<int> src_rect, Rect<int> dst_rect){
        // �������� ��������������:
        clip_rects_for_image_loader(file_rect, src_rect, img.range(), dst_rect);
        if (src_rect.empty() || dst_rect.empty()) return;

	// ����� ������� ��� ����� - �� ������� ��� ����� ��������� ��������:
        double xscale = (double)src_rect.w / dst_rect.w;
        double yscale = (double)src_rect.h / dst_rect.h;
        double scale = std::min(xscale, yscale);

        // �������� �� ��� �����? ���� ��� - ������������ ���!
        if ((scale < buf_scale) && (buf_scale > 1) ||
             !point_in_rect(src_rect.TLC(), buf_rect) ||
             !point_in_rect(src_rect.BRC(), buf_rect)){
          // ��������� ����� ��������� ������:
          int w0 = int(layer_r_bufw * scale);
          int h0 = int(layer_r_bufh * scale);
      // �� ����������� - ����� ����������� ����� ��� � ������,
          // �� ����������� - ����� ����� �� ������ �� ������� �����
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

        // ������� ������ �������� �� ������:
        // ����� ��� ����� ��� �����:

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
