#ifndef LINE_DIST_H
#define LINE_DIST_H

#include "line.h"

///\addtogroup lib2d
///@{
///\defgroup line_dist
///@{

/**
 Класс, позволяющий перемещаться вдоль линии, получая различные параметры.
 Сделано для рисования условных обозначений - всяких сложных пунктиров и т.п.
 Линия параметризуется длиной, по ней можно ходить вперед и назад, получая
 точки, расположенные через нужные интервалы, направление касательной и нормали
 в этих точках и т.п.
*/
class LineDist{

  dLine   line; // копия линии

  std::vector<double> ls; // lengths from the beginning to every node
  size_t  current_n;      // current node
  double  current_l;      // current distance from the current_n

public:

  LineDist(const dLine & _line); ///< Constructor: create LineDist object from a line.

  double length() const; ///< Get line length.

  dPoint pt() const;    ///< Get current point.
  double dist() const;  ///< Get current distance from the line beginning.
  dPoint tang() const;  ///< Get unit tangent vector at current point.
  dPoint norm() const;  ///< Get unit normal vector at current point.

  /// Get part of line with dl length, starting from current point;
  /// move current point by dl
  dLine get_points(double dl);

  void move_begin();        ///< Move current point to the first node.
  void move_end();          ///< Move current point to the last node.
  void move_frw(double dl); ///< Move current point forward by dl distance.
  void move_bck(double dl); ///< Move current point backward by dl distance.
  void move_frw_to_node();  ///< Move current point forward to the nearest node.
  void move_bck_to_node();  ///< Move current point backward to the nearest node.

  bool is_begin() const;    ///< Is current point at the first node?
  bool is_end() const;      ///< Is current point at the last node?
};

#endif
