#!/bin/bash

glslangValidator -V font.vert
glslangValidator -V font.frag

mv frag.spv font_frag.spv
mv vert.spv font_vert.spv

exit
