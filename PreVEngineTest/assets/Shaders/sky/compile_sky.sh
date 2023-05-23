#!/bin/bash

glslangValidator -V clouds.comp

mv comp.spv clouds_comp.spv

glslangValidator -V clouds_perlin_worley_noise_3d.comp

mv comp.spv clouds_perlin_worley_noise_3d_comp.spv

glslangValidator -V sky.vert
glslangValidator -V sky.frag

mv frag.spv sky_frag.spv
mv vert.spv sky_vert.spv

glslangValidator -V sky_post_process.vert
glslangValidator -V sky_post_process.frag

mv frag.spv sky_post_process_frag.spv
mv vert.spv sky_post_process_vert.spv

glslangValidator -V sky_composite.vert
glslangValidator -V sky_composite.frag

mv frag.spv sky_composite_frag.spv
mv vert.spv sky_composite_vert.spv

exit
