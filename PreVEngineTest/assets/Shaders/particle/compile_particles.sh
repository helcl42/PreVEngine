#!/bin/bash

glslangValidator -V particles.vert
glslangValidator -V particles.frag

mv frag.spv particles_frag.spv
mv vert.spv particles_vert.spv

exit
