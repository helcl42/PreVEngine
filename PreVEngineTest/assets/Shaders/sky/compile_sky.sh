#!/bin/bash

glslangValidator -V sky.comp

mv comp.spv sky_comp.spv

glslangValidator -V sky_post_process.comp

mv comp.spv sky_post_process_comp.spv

glslangValidator -V sky_composite.vert
glslangValidator -V sky_composite.frag

mv frag.spv sky_composite_frag.spv
mv vert.spv sky_composite_vert.spv

exit
