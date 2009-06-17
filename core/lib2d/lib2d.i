%module lib2d

%include "std_vector.i"
%include "../../core/utils/swig_utils.i"

%{
#include "point.h"
%}
%include "point.h"
%template(point_i) Point<int>;
%template(point_d) Point<double>;

%{
#include "rect.h"
%}
%include "rect.h"
%template(rect_i) Rect<int>;
%template(rect_d) Rect<double>;

%{
#include "line.h"
%}
%include "line.h"
%template(vector_point_i) std::vector<Point<int> >;
%template(vector_point_d) std::vector<Point<double> >;
%template(line_i) Line<int>;
%template(line_d) Line<double>;

%template(vector_line_i) std::vector<Line<int> >;
%template(vector_line_d) std::vector<Line<double> >;
%template(multiline_i) MultiLine<int>;
%template(multiline_d) MultiLine<double>;



