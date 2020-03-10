#!/bin/bash

glslangValidator -V skybox.vert
glslangValidator -V skybox.frag

mv frag.spv skybox_frag.spv
mv vert.spv skybox_vert.spv

exit
