#!/bin/bash

glslangValidator -V animation_cone_step_mapped.vert
glslangValidator -V animation_cone_step_mapped.frag

mv frag.spv animation_cone_step_mapped_frag.spv
mv vert.spv animation_cone_step_mapped_vert.spv

exit
