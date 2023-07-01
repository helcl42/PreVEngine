#!/bin/bash

for dir in */; do 
	echo "==========================================="
	echo "= Compiling shaders in: $dir";
	echo "==========================================="
	cd $dir
	for f in *.sh; do
		./$f;
	done
	cd ..
done