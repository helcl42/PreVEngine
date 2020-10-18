#!/bin/bash

glslangValidator -V animation.vert
glslangValidator -V animation.frag

mv frag.spv animation_frag.spv
mv vert.spv animation_vert.spv

exit
