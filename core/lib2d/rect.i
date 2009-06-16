%{
#include "rect.h"
%}

template <typename T>
class Rect
{
public:
    T x,y,w,h;
    Rect (Point<T> p1, Point<T> p2);
    Rect (T _x, T _y, T _w=0, T _h=0);
    Rect ();

    bool empty() const;
    Point<T> TLC() const;
    Point<T> TRC() const;
    Point<T> BRC() const;
    Point<T> BLC() const;

    Rect<T> & operator/= (T k);
    Rect<T> & operator*= (T k);
    Rect<T> & operator-= (Point<T> p);
    Rect<T> & operator+= (Point<T> p);
    bool operator< (const Rect<T> & r) const;
    bool operator== (const Rect<T> & r) const;
};

%template(rect_i) Rect<int>;
%template(rect_d) Rect<double>;

template <typename T>
Rect<T> rect_pump (Rect<T> const & R, T val);

template <typename T>
Rect<T> rect_pump (Rect<T> & R, Point<T> p);

template <typename T>
Rect<T> rect_pump (Rect<T> const & R, Rect<T> bounds);

template <typename T>
Rect<T> rect_intersect (Rect<T> const & R1, Rect<T> const & R2);

template <typename T>
Rect<T> rect_bounding_box (Rect<T> const & R1, Rect<T> const & R2);

// точка не всегда будет входит в прямоугольник!!! 
template <typename T>
void clip_point_to_rect (Point<T> & p, const Rect<T> & r);

template <typename T>
void clip_rect_to_rect (Rect<T> & r1, const Rect<T> & r2);

template <typename T>
bool point_in_rect (const Point<T> & p, const Rect<T> & r);

// диапазон плиток, накрывающих данный прямоугольник
Rect<int> tiles_on_rect(const Rect<int> & r, int tsize);
// диапазон плиток, лежащих внутри данного прямоугольника
Rect<int> tiles_in_rect(const Rect<int> & r, int tsize);

// два прямоугольника задают преобразование.
// функция соответствующим образом сдвигает и растягивает третий прямоугольник
void transform_rect(const Rect<int> & src, const Rect<int> & dst, Rect<int> & r);
// Функция, нужная для загрузчика картинок.
// Правильное подрезание краев, выходящих за пределы картинки
void clip_rects_for_image_loader(
    const Rect<int> & src_img, Rect<int> & src,
    const Rect<int> & dst_img, Rect<int> & dst);
