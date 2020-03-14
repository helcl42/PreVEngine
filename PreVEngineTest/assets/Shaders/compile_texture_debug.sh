#!/bin/bash

glslangValidator -V texture_debug.vert
glslangValidator -V texture_debug.frag

mv frag.spv texture_debug_frag.spv
mv vert.spv texture_debug_vert.spv

exit
