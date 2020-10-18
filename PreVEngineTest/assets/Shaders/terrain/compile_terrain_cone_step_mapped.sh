#!/bin/bash

glslangValidator -V terrain_cone_step_mapped.vert
glslangValidator -V terrain_cone_step_mapped.frag

mv frag.spv terrain_cone_step_mapped_frag.spv
mv vert.spv terrain_cone_step_mapped_vert.spv

exit
