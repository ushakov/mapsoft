// Universal graphical editor

/*  We want to draw objects on a large raster
    plane (GPlane). We want to create viewer, allowing us to view
    part of this plane, to rescale and drag it, to pass some events
    (mouse clicks) to editor functions, specific for each type of objects.

    Type GCoord is used for coordinates on this plane. They are
    restricted by values GCoord::min, GCoord::max; GCoord must be an
    unsigned type.
*/

typedef GCoord_t unsigned int;
const GCoord_min=0;
const GCoord_max=UINT_MAX;


/*  GPlane has some hints for talling objects how to draw on it.
    It is g_map for geo-referenced objects and scale for others.
    When you are change scale in viewer both parameters changes.
    Objects can use these parameters as they want.
*/

struct GPlane{
  unsigned int nom;
  unsigned int denom;
  double scale;
  g_map  ref;

  GPlane & operator/= (double k);
  GPlane & operator*= (double k);
  GPlane & operator-= (Point<GCoord_t> k);
  GPlane & operator+= (Point<GCoord_t> k);
};

/*  We want to redraw parts of our plane efficiently: not to iterate though all
    existing objects and through all points of large objects.

    We split plane into tiles. Each tile (GTile) knows about objects on it,
    and each object caches information how to draw itself on each tile
    (it can be raster picture, set of points with reference to GC etc.).

*/

struct GSplitting : std::map<GTileID, GTile>{
  virtual Image<GCoord> draw(Rect<GCoord>) = 0;
};

struct GTile{
  Rect<GCoord> range;
  std::set<GObject *> objects;
  Image<GCoord> redraw();
};

/*  Each object knows what to draw on given tile at given GPlane.
    It is strongly recomended to cache information needed for
    drawing.
*/

struct GObject{
  virtual int render(
    Image<GCoord> & img,
    const GPlane & plane,
    const Point<GCoord> & origin) = 0;
};

/*  On object change:
     - GSplitting: find tiles which cover object
     - Update references in tiles
     - ? (Recache object for all these tiles)
     - Rearrange tiles if needed (need weight of each object in tile!)

    On redrawing region:
     - GSplitting: find tiles which cover region
     - for each tile redraw each object

    On change workplane scale:
     - For each object: clear cache
*/
