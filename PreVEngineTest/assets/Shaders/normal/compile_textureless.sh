#!/bin/bash

glslangValidator -V textureless.vert
glslangValidator -V textureless.frag

mv frag.spv textureless_frag.spv
mv vert.spv textureless_vert.spv

exit
