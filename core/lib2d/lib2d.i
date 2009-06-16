%module lib2d

%include "std_string.i"
%{
#include <sstream>
%}

%{
#include "point.h"
%}
%include "point.h"

%{
#include "rect.h"
%}
%include "rect.h"

