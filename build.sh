rm -rf ./build/*
cmake -B build
cmake --build ./build
./build/vulkan-playground
