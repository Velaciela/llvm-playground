rm -rf build
rm -rf output

cmake -S ../calc/ \
      -B build \
      -G Ninja  -DCMAKE_CXX_STANDARD=14 \
                -DCMAKE_BUILD_TYPE=Release \
                -DLLVM_DIR=/usr/local/lib/cmake/llvm \
                -DCMAKE_INSTALL_PREFIX=./output

cd build
ninja
ninja install
cd ..
