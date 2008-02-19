#ifndef LAYER_GEO_H
#define LAYER_GEO_H

#include "layer.h"
#include "../geo_io/geo_data.h"

/// Растровый слой, содержащий информацию о геодезической привязке - 
/// абстрактный класс.

class LayerGeo : public Layer {
public:
    virtual g_map get_ref() const = 0;
    virtual void set_ref(const g_map & map) = 0;
    virtual void set_ref() = 0;

    virtual LayerGeo & operator/= (double k){ set_ref(get_ref()/k); return *this; }
    virtual LayerGeo & operator*= (double k){ set_ref(get_ref()*k); return *this; }
    virtual LayerGeo & operator-= (g_point k){ set_ref(get_ref()-k); return *this; }
    virtual LayerGeo & operator+= (g_point k){ set_ref(get_ref()+k); return *this; }

    LayerGeo() {}
    LayerGeo (LayerGeo const & other) { }
    virtual void swap (LayerGeo & other) { }
};

#endif /* LAYER_GEO_H */
