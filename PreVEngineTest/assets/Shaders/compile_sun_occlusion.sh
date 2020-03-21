#!/bin/bash

glslangValidator -V sun_occlusion.vert
glslangValidator -V sun_occlusion.frag

mv frag.spv sun_occlusion_frag.spv
mv vert.spv sun_occlusion_vert.spv

exit
