## shell functions for coord conversions

MAPSOFT_CRD=1

# get lon0 for a tmerc geometry
geom2lon0(){
  echo "$1" |
  sed -n 's/^[0-9\.]\+x[0-9\.]\+[\+-]\([0-9]\+\)[0-9]\{6\}[\+\.-].*/\1*6-3/p' |
  bc
}

# remove prefix from tmerc geometry
geom_prefixrem(){
  printf "%s\n" "$1" |
  sed -n '
    s/^\([0-9\.]\+x[0-9\.]\+[\+-]\)[0-9]*\([0-9]\{6\}[\+\.-].*\)/\1\2/p
    /^[0-9\.]\+x[0-9\.]\+[\+-]\([0-9]\{0,5\}[\+\.-].*\)/p'
}

# get wgs lonlat bbox for a pulkovo tmerc geometry
geom2ll(){
  echo "bb_frw $(geom_prefixrem "$1") 1" |
    convs_pt2pt "pulk" "tmerc" "lon0=$(geom2lon0 "$1")" "wgs84" "lonlat" ""
}

# get wgs lonlat bbox for a nom name
nom2ll(){
  pulk="$(convs_nom -n "$1")"
  echo "bb_frw $pulk" |
    convs_pt2pt "pulk" "lonlat" "" "wgs84" "lonlat" ""
}

# nomenclature names
geom2nom(){
  local geom="$1"
  local rscale="${2:-50000}"
  convs_nom -r "$(geom2ll "$geom")" "$rscale"
}
