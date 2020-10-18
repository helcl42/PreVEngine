#!/bin/bash

glslangValidator -V normal_mapped.vert
glslangValidator -V normal_mapped.frag

mv frag.spv normal_mapped_frag.spv
mv vert.spv normal_mapped_vert.spv

exit
