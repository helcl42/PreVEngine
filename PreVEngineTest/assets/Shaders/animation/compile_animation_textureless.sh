#!/bin/bash

glslangValidator -V animation_textureless.vert
glslangValidator -V animation_textureless.frag

mv frag.spv animation_textureless_frag.spv
mv vert.spv animation_textureless_vert.spv

exit
