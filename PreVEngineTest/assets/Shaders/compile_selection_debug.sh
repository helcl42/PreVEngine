#!/bin/bash

glslangValidator -V selection_debug.vert
glslangValidator -V selection_debug.frag

mv frag.spv selection_debug_frag.spv
mv vert.spv selection_debug_vert.spv

exit
