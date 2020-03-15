#!/bin/bash

glslangValidator -V water.vert
glslangValidator -V water.frag

mv frag.spv water_frag.spv
mv vert.spv water_vert.spv

exit
