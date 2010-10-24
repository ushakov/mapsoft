#ifndef OCAD_OBJECT_H
#define OCAD_OBJECT_H

#include "ocad_types.h"
#include <cstdio>
#include <vector>
#include <string>

#include "2d/point.h"
#include "2d/rect.h"
#include "2d/line.h"

namespace ocad{

// common structure for OCAD object
struct ocad_object{

  ocad_long  sym;
  ocad_byte  type;
  ocad_byte  status;
  ocad_byte  viewtype;
  ocad_small idx_col;
  ocad_small implayer;
  ocad_small ang;
  ocad_small col;
  ocad_small width;
  ocad_small flags;

  ocad_small extent; // TODO - not here?

  std::vector<ocad_coord> coords;
  std::string text;

  /// Empty constructor.
  ocad_object();

  /// Convert to iLine.
  /// TODO -- correct holes processing!
  iLine line() const;

  /// Set points.
  /// For creating objects use ocad_file::add_object()!
  void set_coords(const iLine & l);

  /// Get range (with extent added).
  iRect range() const;

  /// dump all information.
  void dump(int verb) const;


  // Functions for internal use:

  /// Lower-left corner (with extent added).
  ocad_coord LLC() const;

  /// Upper-right corner (with extent added).
  ocad_coord URC() const;

  /// Get number of 8-byte blocks needed for 0x0000-terminates string txt.
  int txt_blocks(const std::string & txt) const;

  /// Write text.
  void write_text(const std::string & txt, FILE *F, int limit=-1) const;

  /// Write coordinates.
  void write_coords(FILE *F, int limit=-1) const;

  /// Read coordinates.
  void read_coords(FILE *F, int n);

  /// Read text.
  void read_text(FILE *F, int n);
};

} // namespace
#endif
