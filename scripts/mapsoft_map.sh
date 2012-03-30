## shell functions for reading map data

MAPSOFT_MAP=1

# Geometry or title information is read from maps.txt file:
# <base name> <geometry> <title>
map_data(){
  field="$1"  # geom | title
  base="$2"

  if [ ! -f "maps.txt" ]; then
    echo "ERROR: can't find maps.txt file" > /dev/stderr
    exit 1
  fi

  dat="$(grep "^$base[[:space:]]" "maps.txt" ||:)"

  if [ -z "$dat" ]; then
    echo "ERROR: can't find map in maps.txt: $base" > /dev/stderr
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
