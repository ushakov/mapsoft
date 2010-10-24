#ifndef OCAD_SYMBOL_H
#define OCAD_SYMBOL_H

#include <string>
#include "ocad_types.h"

namespace ocad{

/// This is common structure for all ocad symbols.
/// Now symbols are kept as blobs which are not
/// compatable for different versions.

struct ocad_symbol{

  ocad_long sym;    // symbol number
  ocad_byte type;   // symbol type
  ocad_long extent; // extent size

  std::string desc; // symbol description

  ocad_long blob_version;
  std::string blob;

  /// Empty constructor.
  ocad_symbol();

  /// Dump data (excluding blob) to stdout.
  void dump(int verb) const;
};

} // namespace
#endif
