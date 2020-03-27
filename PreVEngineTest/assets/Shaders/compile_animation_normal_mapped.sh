#!/bin/bash

glslangValidator -V animation_normal_mapped.vert
glslangValidator -V animation_normal_mapped.frag

mv frag.spv animation_normal_mapped_frag.spv
mv vert.spv animation_normal_mapped_vert.spv

exit
