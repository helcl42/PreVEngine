#!/bin/bash

glslangValidator -V lens_flare.vert
glslangValidator -V lens_flare.frag

mv frag.spv lens_flare_frag.spv
mv vert.spv lens_flare_vert.spv

exit
