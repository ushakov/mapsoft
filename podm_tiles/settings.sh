PATH="../vector/vmap3:$PATH"

# directory with source vmap files
vmap_dir=./map_podm
fig_dir=./fig
png_dir=./png
tile_dir=./tiles


nom_rscale=50000 # just to get correct map names
fig_rscale=89415 # scale in google proj units / m = 50000/cos(56d)

# scale and size of "big" google tile
sc=9
tile_size_px=4096 # 256 * 8

# tiles to create
x1=310
x2=310
y1=160
y2=161

k=20037508.3 # mapsoft google proj units (equator meters) per scale=1 tile


calc(){
  var=$1; shift
  eval "$var=\"\$(printf \"%s\n\" \"$*\" | bc -l)\""
}

# size of map, cm
calc map_size_cm "100.0/$fig_rscale * $k/2^($sc-1)"

# dpi to get tile_size_px
calc tile_dpi "($tile_size_px+1) / $map_size_cm * 2.54"
