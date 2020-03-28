#!/bin/bash

glslangValidator -V terrain_normal_mapped.vert
glslangValidator -V terrain_normal_mapped.frag

mv frag.spv terrain_normal_mapped_frag.spv
mv vert.spv terrain_normal_mapped_vert.spv

exit
