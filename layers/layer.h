#ifndef LAYER_H
#define LAYER_H

#include "../utils/point.h"
#include "../utils/rect.h"
#include "../utils/image.h"

// Abstract class Layer

// Layer -- �����, ��� ������ ��������� �������� � �������������
// ������������� �����������. ��������� �� ������������� ��������� ��
// ���������� ���������


class Layer {
public:
    virtual void draw (Image<int> & img, Rect<int> src, Rect<int> dst) = 0;
    virtual Rect<int> range () = 0;

    Layer() {}

    Layer (Layer const & other) { }

    virtual void swap (Layer & other) { }
};



#endif /* LAYER_H */
