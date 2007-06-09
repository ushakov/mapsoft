#ifndef LAYER_GEO_H
#define LAYER_GEO_H

#include "layer.h"
#include "../geo_io/geo_data.h"

// Abstract class LayerGeo

// Layer -- �����, ��� ������ ��������� �������� � �������������
// ������������� �����������. ��������� �� ������������� ��������� ��
// ���������� ���������. 
// GeoLayer -- �������� ���������� � ������������� ��������

class LayerGeo : public Layer {
public:
    virtual g_map get_ref() const = 0;
    virtual void set_ref(const g_map & map) = 0;
    virtual void set_ref() = 0;

    LayerGeo() {}
    LayerGeo (LayerGeo const & other) { }
    virtual void swap (LayerGeo & other) { }
};



#endif /* LAYER_GEO_H */
