#ifndef LAYER_H
#define LAYER_H

#include <boost/operators.hpp>
#include "2d/image.h"
#include "2d/conv.h"
#include "2d/conv_triv.h"

/// Растровый слой -- абстрактный класс.
/// Знает, как отдать некоторую растровую картинку в прямоугольных
/// целочисленных координатах. Рассчитан на неоднократное обращение за
/// маленькими кусочками

class Layer: public boost::multiplicative<Layer,double> {
    static ConvTriv trivial_cnv;
public:
    virtual void draw(const iPoint origin, iImage & image) = 0;
    virtual iImage get_image (iRect src);

    virtual Conv * get_cnv() const;
    virtual void set_cnv(Conv * c, int hint=-1);

    virtual iRect range () const = 0;
    virtual void refresh();

    virtual Layer & operator/= (double k);
    virtual Layer & operator*= (double k);

    Layer();
    Layer(Layer const & other);
    virtual void swap (Layer & other);

    Conv * cnv;
    int cnv_hint;
};

#endif /* LAYER_H */
