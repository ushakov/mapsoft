## Mapsoft

Mapsoft is a set of libraries and tools for working with geodata, raster
and vector maps etc.  It was is not too accurate, but it was used for
many our projects including making vector maps for Moscow region (
https://nakarte.me/#m=7/55.61249/37.65564&l=O/Z ) and some mountain
regions ( https://nakarte.me/#m=2/20.13847/57.48047&l=O/Q ).

Git repositary: https://github.com/ushakov/mapsoft

At the moment the project is not developing. I'm trying to rewrite it
fixing some fundamental problems and improving the code (
https://github.com/slazav/mapsoft2 ), but this work is far from its end.

Main mapsoft programs (man-pages are available):

- mapsoft_convert: Geodata and raster map converter.
  Supports a few formats: OziExplorer, GPX, GeoJSON, GarminUtils, KML, custom
  XML-like format. Supports SRTM data. Supports geo-referenced extension for
  xfig file format.

- mapsoft_mapview: viewer/editor for geodata, raster maps, SRTM data.

- mapsoft_vmap: vector map converter. Supports MP vector map format,
  geo-referenced extension for xfig file format, custom VMAP format.

- vmap_render: vector map rendering.
