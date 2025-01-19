
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
  cmake ..
  #cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
else
  cd build
fi

make


