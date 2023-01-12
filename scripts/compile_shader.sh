#!/bin/sh

[ -n "$GLSLC" ] || {
  GLSLC=$(which glslc) || {
    echo "[ERROR] glslc executable not found. "
    exit
  }
}

(
  cd assets/shaders ;
  touch *.spv && rm *.spv ;
  [ "$1" = "clean" ] || {
    for s in $(ls) 
      do (exec $GLSLC $s -o "$s.spv")
    done
  }
)