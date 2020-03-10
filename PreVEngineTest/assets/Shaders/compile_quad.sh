#!/bin/bash

glslangValidator -V quad.vert
glslangValidator -V quad.frag

mv frag.spv quad_frag.spv
mv vert.spv quad_vert.spv

exit
