// Universal graphical editor

#include "../../core/lib2d/line.h"

/*  Objects are drawn on a plane in abstract coordinates GCoord,
    restricted by GCoord_min and GCoord_max, and addressed by
    SuperCells. Viewer renders this plane on the screen in some scale.

    This abstract has geographic reference which the objects can use
    if they need.

    Objects are encouraged to cache information using the supercell
    division.
*/

#include "gcoord.h"

struct GAbstractPlane {
  g_map ref;
  int nominator;
  int denominator;
};

class GObject {
 public:
  /** Render this object to an image.

      Renders part of this object within the cell addressed by cell to
      an image img. The rendering is done according to scale and ref
      of GPlane plane. The top-left corner of the cell goes to (0,0)
      in img, if img has wrong size, cropping/black parts happen.
  */
  virtual void draw(Image<int>* img, SuperCell cell, const GPlane& plane) { }

  /** Notify the object that the GPlane's ref has changed.

      Please note that the scale may change freely, and we do not need to
      notify objects!
  */
  virtual void ref_changed(const GPlane& plane) { }
};

// Convenience class
<typename Base>
class GTileCache : Cache<SuperCell, std::set<Base> > {
public:
  typedef std::set<Base> CacheElement;
  typedef Cache<SuperCell, CacheElement> CacheType;

  GTileCache(int n): CacheType(n) { }
  GTileCache(GTileCache const& other) : CacheType(other) { }

  std::pair<CacheElement::iterator, CacheElement::iterator> iterate(SuperCell c) {
    CacheElement el = this->get(c);
    return std::make_pair(el.begin(), el.end());
  }

  void add_at(SuperCell c, Base const& item) {
    if (!contains(c)) {
      CacheElement el;
      el.insert(item);
      put(c, el);
    } else {
      CacheElement& el = get(c);
      el.insert(item);
    }
  }

  void remove_at(SuperCell c, Base const& item) {
    if (!contains(c)) {
      // nothing to do
      return;
    }
    CacheElement& el = get(c);
    el.erase(item);
  }
};

/**  Reference GObject implementation: line.

     This is a MultiLine<double> that renders to abstract coordinates in
     1-to-1 fashion, then renders to the screen using only scale. It caches
     its contents in each SuperCell requested.

     This object ignores the geo-reference and thus does not implement
     ref_changed method.
*/
class GLine : public GObject {
private:
  MultiLine<double> data;
  typedef GTileCache<int> TileCache;
  TileCache cache;
public:
  GLine(MultiLine<double> data) : cache(100) {
    this->data = data;
  }

  virtual void draw(Image<int>* img, SuperCell cell, const GPlane& plane) {
    if (!cache.has_key(cell)) {
      Rect<GCoord> r = cell.range();
      for (int line = 0; line < data.size(); ++line) {
	int last = -1;
	Line<double>
	for (int i = 0; i < 
      }
    }
  }
};
