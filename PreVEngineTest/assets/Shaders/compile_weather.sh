#!/bin/bash

glslangValidator -V weather.comp

mv comp.spv weather_comp.spv

glslangValidator -V perlin_worley_noise_3d.comp

mv comp.spv perlin_worley_noise_3d_comp.spv

glslangValidator -V worley_noise_3d.comp

mv comp.spv worley_noise_3d_comp.spv

exit
