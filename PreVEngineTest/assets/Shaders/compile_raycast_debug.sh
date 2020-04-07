#!/bin/bash

glslangValidator -V raycast_debug.vert
glslangValidator -V raycast_debug.geom
glslangValidator -V raycast_debug.frag

mv frag.spv raycast_debug_frag.spv
mv geom.spv raycast_debug_geom.spv
mv vert.spv raycast_debug_vert.spv

exit
