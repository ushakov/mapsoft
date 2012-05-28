#ifndef LAYER_H
#define LAYER_H

#include <boost/operators.hpp>
#include "2d/image.h"
#include "options/options.h"

/// Растровый слой -- абстрактный класс.
/// Знает, как отдать некоторую растровую картинку в прямоугольных
/// целочисленных координатах. Рассчитан на неоднократное обращение за
/// маленькими кусочками

class Layer:
  public boost::multiplicative<Layer,double>,
  public boost::additive<Layer, dPoint>
 {
public:
    virtual void draw(const iPoint origin, iImage & image) = 0;

    virtual iImage get_image (iRect src){
	iImage ret(src.w, src.h, 0);
	draw(src.TLC(), ret);
	return ret;
    }

    virtual iRect range () const = 0;
    virtual void refresh() { };

    virtual Layer & operator/= (double k) = 0;
    virtual Layer & operator*= (double k) = 0;
    virtual Layer & operator-= (dPoint k) = 0;
    virtual Layer & operator+= (dPoint k) = 0;

    Layer() {}
    Layer (Layer const & other) { }
    virtual void swap (Layer & other) { }
};


#endif /* LAYER_H */
