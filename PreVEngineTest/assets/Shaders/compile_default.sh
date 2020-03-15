#!/bin/bash

glslangValidator -V default.vert
glslangValidator -V default.frag

mv frag.spv default_frag.spv
mv vert.spv default_vert.spv

exit
