#!/bin/bash

glslangValidator -V terrain_parallax_mapped.vert
glslangValidator -V terrain_parallax_mapped.frag

mv frag.spv terrain_parallax_mapped_frag.spv
mv vert.spv terrain_parallax_mapped_vert.spv

exit
