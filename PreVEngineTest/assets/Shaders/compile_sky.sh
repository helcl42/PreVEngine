#!/bin/bash

glslangValidator -V sky.comp

mv comp.spv sky_comp.spv

glslangValidator -V sky_post_process.comp

mv comp.spv sky_post_process_comp.spv

exit
