#!/bin/bash

glslangValidator -V parallax_mapped.vert
glslangValidator -V parallax_mapped.frag

mv frag.spv parallax_mapped_frag.spv
mv vert.spv parallax_mapped_vert.spv

exit
