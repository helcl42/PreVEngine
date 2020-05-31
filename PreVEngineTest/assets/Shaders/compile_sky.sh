#!/bin/bash

glslangValidator -V sky.vert
glslangValidator -V sky.frag

mv frag.spv sky_frag.spv
mv vert.spv sky_vert.spv

exit
