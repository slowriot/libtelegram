#!/bin/bash
# Simple helper script to Make, since Make is too retarded to do this
# sort of thing by itself

projectfile="$1"
objectdir="$2"

if [ "$projectfile" = "" ] || [ "$objectdir" = "" ]; then
  echo "Usage: $0 projectfile objectdir"
  exit 1
fi

if [ "$objectdir" = "--source" ]; then
  objectlist=$(grep '<Unit filename=".*\.cpp" />' "$projectfile" | cut -d '"' -f 2)
  echo $objectlist
else
  objectdir_escaped=$(echo "$objectdir" | sed 's/$/\//;s/\//\\\//g')
  objectlist=$(grep '<Unit filename=".*\.cpp" />' "$projectfile" | cut -d '"' -f 2 | sed 's/^/'"$objectdir_escaped"'/;s/\.cpp$/.o/')
  echo $objectlist
fi
