
set -e

if [ -d "build" ]; then
  echo "deleting build directory"
  rm -rf build
fi

rm CMakeCache.txt | true
rm -rf CMakeFiles | true
rm build.ninja | true
rm cmake_install.cmake | true
rm lvgl-build/cmake_install.cmake | true



