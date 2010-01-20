. shell-config
. shell-error
. shell-quote

get_map_par(){
  local cnf="./.maps"
  local sep="[[:space:]]*=[[:space:]]*"
  local name="$1"
  local par="$2"
  local def="$3"

  local ret="$(shell_config_get "$cnf" "$par:$name" "$sep")"
  [ -n "$ret" ] ||
    ret="$(shell_config_get "$cnf" "$par" "$sep")"
  [ -n "$ret" ] ||
    ret="$def"
  printf "%s" "$ret"
}

# get lon0 for a tmerc geometry
geom2lon0(){
  echo "$1" |
  sed -n 's/^[0-9\.]\+x[0-9\.]\+[\+-]\([0-9]\+\)[0-9]\{6\}[\+\.-].*/\1*6-3/p' |
  bc
}

# get wgs lonlat bbox for a pulkovo tmerc geometry
geom2ll(){
  echo "bb_frw $1 1" |
  convs_pt2pt "pulk" "tmerc" "lon0=$(geom2lon0 "$geom")" "wgs84" "lonlat" ""
}

# get url for downloading data from westra passes catalog
geom2wp(){
  local geom="$1"
  local fmt="${2:-mp}"
  geom2ll "$geom" |
  sed 's/\(^[0-9\.]\+\)x\([0-9\.]\+\)\([\+-][0-9\.]\+\)\([\+-][0-9\.]\+\)/\1 \2 \3 \4/' |
  {
    local x1 x2 y1 y2 dx dy
    read dx dy x1 y1
    x2="$(printf "${x1#+} + $dx\n" | bc -l)"
    y2="$(printf "${y1#+} + $dy\n" | bc -l)"
    printf "http://www.westra.ru/cgi-bin/show_pass.pl?lat1=%f&lat2=%f&lon1=%f&lon2=%f&searchbtn=1&fmt=$fmt"\
     $y1 $y2 $x1 $x2
  }
}

download_wp(){
  local geom="$1"
  date="$(date +"%F %T")"
  wget "$(geom2wp "$geom")" -O - |
    sed "s/^;[[:space:]]*\([0-9]\+@westra_passes\)/;0x0 $date \1/" > "${name}_wp.mp"
  wget "$(geom2wp "$geom" txt)" -O - |
    iconv -f utf-8 -c | sort -k1,1 -n > "${name}_wp.txt"
}