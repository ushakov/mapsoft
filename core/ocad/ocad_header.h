#ifndef OCAD_HEADER_H
#define OCAD_HEADER_H

#include <cstdio>
#include "ocad_types.h"

namespace ocad{

/*
OCAD header has a compatable structure for all versions 6..9.
It contains version information and addresses of data in the file.
There are defferent data blocks for different ocad versions.
Unused data addresses are set to 0.
*/

struct ocad_header {

  /// file version
  ocad_int version, subversion;

  /// file type
  ocad_int type; // 0 for normal map, 1 for course setting

  /// posinitons of data structures
  ocad_int sym_pos, obj8_pos, obj9_pos, str_pos;

  /// OCAD 6..8 only
  ocad_int setup_pos, setup_size, info_pos, info_size;

  /// OCAD 9 only
  ocad_int fname_pos, fname_size;


  /// Empty constructor.
  ocad_header();

  /// Rewind file and read header.
  /// set OCAD version 0 on errors.
  /// set data positions.
  void read(FILE *F);

  /// One more constructor.
  ocad_header(FILE *F);

  /// Rewind file and write header.
  void write(FILE *F) const;

  /// Just fseek to the end of header.
  void seek(FILE *F) const;

  /// Dump header information.
  void dump(int verb);
};

} // namespace
#endif
