#!/bin/bash

glslangValidator -V shadow_map_debug.vert
glslangValidator -V shadow_map_debug.frag

mv frag.spv shadow_map_debug_frag.spv
mv vert.spv shadow_map_debug_vert.spv

exit
