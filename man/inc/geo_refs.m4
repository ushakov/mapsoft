
B<<  --geom <WxH+X+Y> >>, B<<  --datum <arg> >>, B<<  --proj <arg> >>,
B<<  --lon0 <arg> >> -- set rectangular geometry in a given datum/proj

B<<  --wgs_geom <WxH+X+Y> >>, B<<  --proj <arg> >> -- set rectalgular
geometry in WGS lon-lat coordinates and convert it to the given
projection

B<<  --wgs_brd <X1,Y1,X2,Y2...> >> -- set border of area in WGS lon-lat
coordinates and convert it to the given projection

B<<  --nom <arg> >>

B<<  --google <X,Y,Z> >> -- set area as google tile

geom, wgs_geom, wgs_brd, nom and google options are incompatible with
each other.

B<<  --rscale <arg> >> -- reversed scale (10000 for 1:10000 map)

B<<  -d, --dpi <arg> >> -- resolution, dots per inch

B<<  --mag <arg> >> -- additional magnification

B<<  --swap_y >> -- swap vertical axis
