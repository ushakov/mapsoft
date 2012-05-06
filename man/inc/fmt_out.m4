=head1 OUTPUT FILE FORMATS

Format is determined by file extension:

B<<  *.xml >> -- mapsoft native XML-like format

B<<  *.wpt, *.plt, *.map, *.oe  >> -- OziExplorer format (Each track,
waypoint set or map reference will be written in a separate .plt, .wpt
or .map file)

B<<  *.zip >> -- zipped OziExplorer files

B<<  *.gpx >> -- GPX format /partial support, tracks and waypoints/

B<<  *.kml >> -- Google KML format /partial support, tracks and waypoints/

B<<  *.kmz >> -- zipped kml

B<<  *.gu  >> -- old garmin-utils format (tracks and waypoints only)

B<<  *.tif, .tiff, .jpg, .jpeg >> -- raster image (see below)

B<<  gps: >> -- send data to Garmin GPS via autodetected serial device

B<<  <character device> >> -- send data to Garmin GPS via serial device
