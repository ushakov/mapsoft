import sys
import core

w = core.geo_data()
core.geo_read(sys.argv[1], w)

bb = core.rect_d(37, 55, 0.01, 0.01)

layer = core.LayerGeoMap(w)
ref = layer.get_ref()

m2p = core.map2pt(ref, core.Datum("wgs84"), core.Proj("lonlat"))
r = m2p.bb_bck(bb)

im = layer.get_image(r)
core.save(im, "out.jpg", {})
