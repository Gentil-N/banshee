echo "------------------------------Shaders"

cd samples/data/shaders
glslc main.vert -o main.vert.spv
glslc main.frag -o main.frag.spv
glslc particle.vert -o particle.vert.spv
glslc particle.frag -o particle.frag.spv
cd ../../..

echo "------------------------------Building"

if [ "$1" = "debug" ]
then
       echo "------------------------------Debug"
       mkdir -p build/debug
       cd build/debug
       cmake ../../. -DCMAKE_BUILD_TYPE="Debug" -DORION_BUILD_SHARED=OFF -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF
elif [ "$1" = "release" ]
then
       echo "------------------------------Release"
       mkdir -p build/release
       cd build/release
       cmake ../../. -DCMAKE_BUILD_TYPE="Release" -DORION_BUILD_SHARED=OFF -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF
else
       echo "build type is not valid"
       exit 1
fi

make -j4

echo "------------------------------Launching"
./samples