# Some data about maps are stored in maps.txt file:
#  <name> <geometry> <title>
# Geometry describes a rectangular area in Soviet map coordinates.
# Using map_data() func one can get map geometry and title by its name.
# I do not use this library at the moment

MAPSOFT_MAP=1

datafile="maps.txt"

get_line(){
  ## get line from datafile,
  ## remove duplicated spaces (for cut program)
  base="$1"
  file="$2"
  sed -n "/^[[:space:]]*$base[[:space:]]/{s/[[:space:]]\+/ /g; p}" "$2"
}

# Geometry or title information is read from maps.txt file:
# <base name> <geometry> <title>
map_data(){
  field="$1"  # geom | title
  base="$2"

  if [ ! -f "$datafile" ]; then
    echo "ERROR: can't find $datafile file" > /dev/stderr
    exit 1
  fi

  dat="$(get_line "$base" "maps.txt")"

  if [ -z "$dat" ]; then
    echo "ERROR: can't find map in $datafile: $base" > /dev/stderr
    exit 1
  fi

  if   [ "$field" = "geom" ]; then
    printf "%s\n" "$dat" | cut -d ' ' -f 2
  elif [ "$field" = "title" ]; then
    printf "%s\n" "$dat" | cut -d ' ' -f 3-
  else
    echo "ERROR: unknown field: $field" > /dev/stderr
    exit 1
  fi
}
