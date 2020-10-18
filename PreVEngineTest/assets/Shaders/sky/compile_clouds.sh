#!/bin/bash

glslangValidator -V clouds.comp

mv comp.spv clouds_comp.spv

glslangValidator -V clouds_perlin_worley_noise_3d.comp

mv comp.spv clouds_perlin_worley_noise_3d_comp.spv

exit
