#!/bin/bash

glslangValidator -V bounding_volume_debug.vert
glslangValidator -V bounding_volume_debug.frag

mv frag.spv bounding_volume_debug_frag.spv
mv vert.spv bounding_volume_debug_vert.spv

exit
