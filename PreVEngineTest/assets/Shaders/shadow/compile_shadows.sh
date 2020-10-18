#!/bin/bash

glslangValidator -V default_shadows.vert

mv vert.spv default_shadows_vert.spv

glslangValidator -V animation_shadows.vert

mv vert.spv animation_shadows_vert.spv

glslangValidator -V terrain_shadows.vert

mv vert.spv terrain_shadows_vert.spv

glslangValidator -V bump_mapped_shadows.vert

mv vert.spv bump_mapped_shadows_vert.spv

glslangValidator -V animation_bump_mapped_shadows.vert

mv vert.spv animation_bump_mapped_shadows_vert.spv

glslangValidator -V terrain_bump_mapped_shadows.vert

move /y vert.spv terrain_bump_mapped_shadows_vert.spv

exit
