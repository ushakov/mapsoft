#ifndef LAYER_H
#define LAYER_H

#include <boost/operators.hpp>
#include <lib2d/image.h>
#include <utils/options.h>

/// Растровый слой -- абстрактный класс.
/// Знает, как отдать некоторую растровую картинку в прямоугольных
/// целочисленных координатах. Рассчитан на неоднократное обращение за
/// маленькими кусочками

class Layer:
  public boost::multiplicative<Layer,double>,
  public boost::additive<Layer, Point<double> >
 {
public:
    virtual void draw(const Point<int> origin, Image<int> & image) = 0;

    virtual Image<int> get_image (Rect<int> src){
	Image<int> ret(src.w, src.h, 0);
	draw(src.TLC(), ret);
	return ret;
    }

    virtual Rect<int> range () = 0;
    virtual void refresh() = 0;

    /// Gets current layer configuration as Options
    /// Default implementation returns empty Options
    virtual Options get_config() {
	return Options();
    }

    /// Gets layer configuration from Options
    /// Default implementation does nothing
    virtual void set_config(Options opt) {  }

    virtual Layer & operator/= (double k) = 0;
    virtual Layer & operator*= (double k) = 0;
    virtual Layer & operator-= (Point<double> k) = 0;
    virtual Layer & operator+= (Point<double> k) = 0;

    Layer() {}
    Layer (Layer const & other) { }
    virtual void swap (Layer & other) { }
};


#endif /* LAYER_H */
