#!/bin/bash

glslangValidator -V animation_parallax_mapped.vert
glslangValidator -V animation_parallax_mapped.frag

mv frag.spv animation_parallax_mapped_frag.spv
mv vert.spv animation_parallax_mapped_vert.spv

exit
