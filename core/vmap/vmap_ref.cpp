#include "vmap.h"
#include "geo/geo_data.h"
#include "geo_io/geo_refs.h"
#include "2d/line_utils.h"

using namespace std;
namespace vmap{

g_map mk_tmerc_ref(const world & W, double u_per_cm, bool yswap){

  // create refs from brd or from map range
  dLine refs=W.brd;
  if (refs.size()<3) refs=rect2line(W.range());

  g_map ref = mk_tmerc_ref(refs, 100.0/W.rscale * u_per_cm, yswap);
  ref.comm=W.name;
  return ref;
}

} // namespace

