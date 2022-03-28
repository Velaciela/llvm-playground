# clone
git clone git@github.com:llvm/llvm-project.git

# cmake
cd llvm-project
cmake -S llvm -B build -G Ninja -DLLVM_ENABLE_PROJECTS=clang

# build & check
cd build
ninja
bin/llc -version

# install
# On a Unix-like system, the install directory is /usr/local
sudo ninja install
