## shell functions for westra passes downloading

MAPSOFT_WP=1
[ -n "${MAPSOFT_CRD:-}" ] || . mapsoft_crd.sh

# get url for downloading data from westra passes catalog
# usage:  geom2wp <wgs lonlat geom> (mp|txt|wpt)
ll2wp(){
  local geom="$1"
  local fmt="${2:-mp}"

  echo "$geom" |
  sed 's/\(^[0-9\.]\+\)x\([0-9\.]\+\)\([\+-][0-9\.]\+\)\([\+-][0-9\.]\+\)/\1 \2 \3 \4/' |
  {
    local x1 x2 y1 y2 dx dy
    read dx dy x1 y1
    x2="$(printf -- "${x1#+} + $dx\n" | bc -l)"
    y2="$(printf -- "${y1#+} + $dy\n" | bc -l)"
    printf "http://www.westra.ru/cgi-bin/show_pass.pl?lat1=%f&lat2=%f&lon1=%f&lon2=%f&searchbtn=1&fmt=$fmt"\
     $y1 $y2 $x1 $x2
  }
}

# download mp, wpt, txt files for a given wgs latlon range
# remove old-style comments
download_ll_wp(){
  local geom="$1"
  local name="$2"

  date="$(date +"%F %T")"
  wget "$(ll2wp "$geom")" -O - |
    sed "/^;[[:space:]]*\([0-9]\+@westra_passes\)/d;
         s/&quot;/\"/g" > "${name}_wp.mp"
  wget "$(ll2wp "$geom" txt)" -O - |
    iconv -f utf-8 -c | sort -k1,1 -n > "${name}_wp.txt"
  wget "$(ll2wp "$geom" wpt)" -O - > "${name}_wp.wpt"
}


# download mp, wpt, txt files for a given pulkovo tmerc range
# fix old style comments, add Source=westra_passes
download_gk_wp(){
  local geom="$1"
  local name="$2"

  ll="$(geom2ll "$geom")"
  download_ll_wp "$ll" "$name"
}


