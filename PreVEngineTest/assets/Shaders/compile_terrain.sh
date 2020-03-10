#!/bin/bash

glslangValidator -V terrain.vert
glslangValidator -V terrain.frag

mv frag.spv terrain_frag.spv
mv vert.spv terrain_vert.spv

exit
