#ifndef IMAGE_BREZ_H
#define IMAGE_BREZ_H

// рисование линий и кругов!
// сюда б еще многоугольники...

#include "image.h"

namespace image_brez{

// Линия толщины w, алгоритм Брезенхема
    template <typename T>
    void line(Image<T> & im, 
	      const int x1, const int y1, 
              const int x2, const int y2, 
              const int w, const T & c){
        int Dx=x2-x1, Dy=y2-y1;
        int sx=1, sy=1;
        if (Dx<0){sx=-1; Dx=-Dx;}
        if (Dy<0){sy=-1; Dy=-Dy;}

        int sw=0;
        if (Dy>Dx) {sw=1; int tmp=Dx; Dx=Dy; Dy=tmp;}

        int  e = (Dy << 1) - Dx;
        int x=x1, y=y1;

        int w1=w>>1;
        int w2=w>>1;
        if (w1+w2<w) w2++;

        for(int i=0; i<Dx; i++){
            for (int j=-w1;j<w2;j++){
                if (sw==0) im.safe_set(x, y+j, c);
                else       im.safe_set(x+j, y, c);
            }
            while(e >= 0){
                if (sw==0) y+=sy;
                else x+=sx;

                e -= Dx<<1;
            }
            if (sw==0) x+=sx;
            else y+= sy;
            e += Dy<<1;
        }
        for (int j=-w1;j<w2;j++){
            if (sw==0) im.safe_set(x2, y2+j, c);
            else       im.safe_set(x2+j, y2, c);
        }
    }

    template <typename T>
    void set8p(Image<T> & im, 
		const int xc, const int x, 
		const int yc, const int y, const T & c){
        if ((x==0)&&(y==0)){
          im.safe_set(xc, yc, c);
          return;
        }
        im.safe_set(xc + x, yc + y, c);
        im.safe_set(xc - y, yc + x, c);
        im.safe_set(xc + y, yc - x, c);
        im.safe_set(xc - x, yc - y, c);
        if ((x>0)&&(x<y)){
            im.safe_set(xc + x, yc - y, c);
            im.safe_set(xc + y, yc + x, c);
            im.safe_set(xc - x, yc + y, c);
            im.safe_set(xc - y, yc - x, c);
        }
    }


// Окружность
    template <typename T>
    void circ(Image<T> & im, 
	     const int xc, const int yc, 
	     const int r, int w, 
	     const T & fgc, const T & bgc=0xffffffff, 
	     const bool fill=false){
        int x = 0, y = r;
        int d = (1 - r) << 1;
        w = std::min(w, r+1);

        while(y >= x){
            if (fill) for (int i=x; i<=y-w; i++)               set8p(im, xc, x, yc, i, bgc);
            for (int i=std::max(y-w+1,x); i<=y; i++) set8p(im, xc, x, yc, i, fgc);
            if(d + y > 0){
                y--;
//      d -= (2 * y * e) - 1; //for ellipse
                d -= (y <<1) - 1;
            }
            if(x > d){
                x++;
                d += (x << 1) + 1;
            }
        }
    }
}

#endif
