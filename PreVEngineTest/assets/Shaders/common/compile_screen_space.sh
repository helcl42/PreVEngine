#!/bin/bash

glslangValidator -V screen_space.vert
glslangValidator -V screen_space.frag

mv frag.spv screen_space_frag.spv
mv vert.spv screen_space_vert.spv

exit
