## Compilation

This project requires `c++20` standard and it is meant to be compiled
with `g++ 11` or newer. Other C++ compilers such as `clang` or `msvc`
have **not** been tested. The compilation process requires `cmake`.

Clone this repository:
```
git clone https://github.com/fernando-lahoz/informatica-grafica.git
cd informatica-grafica
```
Set up the CMake project:
```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++ -S . -B ./build -G Ninja
```
Compile:
```
cmake --build build --config Release --target all
```

## Executables

As a result, the following executables should appear inside `./bin` directory:
 - **path_tracer:** ...
 - **tone_mapper:** ...

Run with `--help` for additional information of usage. 
