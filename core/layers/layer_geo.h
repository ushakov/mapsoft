#ifndef LAYER_GEO_H
#define LAYER_GEO_H

#include "layer.h"
#include "2d/conv.h"
#include "2d/conv_triv.h"
#include "geo/geo_data.h"

/// Растровый слой, содержащий информацию о геодезической привязке - 
/// абстрактный класс.

class LayerGeo : public Layer {
    static ConvTriv trivial_cnv;
public:

    virtual Conv * get_cnv() const{ return cnv; }

    virtual void set_cnv(Conv * c, int hint=-1) {cnv = c; cnv_hint=hint;}

    virtual g_map get_myref() const = 0;

    virtual LayerGeo & operator/= (double k){
      cnv->rescale_src(1/k); return *this; }
    virtual LayerGeo & operator*= (double k){
      cnv->rescale_src(k); return *this; }

    LayerGeo():cnv(&trivial_cnv),cnv_hint(-1) {}
    LayerGeo (LayerGeo const & other):cnv(&trivial_cnv),cnv_hint(-1) { }
    virtual void swap (LayerGeo & other) { }

    Conv * cnv;
    int cnv_hint;
};

#endif /* LAYER_GEO_H */
