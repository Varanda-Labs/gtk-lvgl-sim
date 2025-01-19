
#####################################
#
#  GTK LVGL Simulator
#
#  Copyright (c) 2025 Varanda Labs
#  MIT License
#
#####################################

set -e

# checkout submodules if needed
if [ ! -d "lvgl/src" ]; then
  echo checking out submodules
  git submodule update --init --recursive --progress
fi

if [ ! -d "build" ]; then
  echo "creating build directory"
  mkdir build
  cd build
  cmake -DDEBUG=ON -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
else
  cd build
fi

make


