# Compilation
This is what you'll need:

## For the C++ EXE:
- NVidia GPU
- CUDA 11+ (may work with 10?)
- vcpkg exe on path
- `VCPKG_ROOT` environment variable defined to where the vcpkg is installed
- cmake
- Visual Studio Code
    - CMake Tools extension
    - C++ extension
- C++ 20 compiler (support std::future, std::async required). I used visual studio 2022 with clang x64 as the cmake backend.

vcpkg will do the rest (hopefully)

### Usage
`dashcam --in=videoin --out=videoout`

Currenlty video out is huge, around 9 gb/5min 4k. Will need to fix that.

# Python: Calibration
I used Conda, create the environment with 
```
conda env create -f=environment.yml
conda activate dashcam
```
