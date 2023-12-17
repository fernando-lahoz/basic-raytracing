## Compilation

This project requires `c++20` standard and it is meant to be compiled
with `g++ 11` or newer. Other C++ compilers such as `clang` or `msvc`
have **not** been tested. The compilation process requires `cmake`.

Clone this repository:
```shell
git clone https://github.com/fernando-lahoz/informatica-grafica.git
cd informatica-grafica
```
Set up the CMake project:
```shell
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++ -S . -B ./build -G Ninja
```
Compile:
```shell
cmake --build build --config Release --target all
```

## Executables

As a result, the following executables should appear inside `./bin` directory:
 - **renderer:** renders the compiled scene using one of the available algorithms.
 - **tone_mapper:** applies a tone mapping function to the luminance of every pixel.

Run with `--help` for additional information of usage.

## Usage examples

### Photon Mapping

```shell
# Linux shell
./renderer --algorithm=photon-mapping \
           --paths-per-pixel=10 \
           --photon-mapping-use-next-event-estimation \
           --photon-mapping-evaluation-radius=0.4 \
           --photon-mapping-evaluation-photons=1000 \
           --photon-mapping-total-saved-photons=100000 \
           \
           --dimensions=256:256 \
           --color-resolution=32bit \
           --output-format=bmp \
           output_image.bmp

./tone_mapper.exe --strategy=gm:2.2 output_image.bmp output_image_mapped.bmp
```

```powershell
# Windows powershell
chcp 65001; ./renderer --algorithm=photon-mapping --paths-per-pixel=10 --photon-mapping-use-next-event-estimation --photon-mapping-evaluation-radius=0.4 --photon-mapping-evaluation-photons=1000 --photon-mapping-total-saved-photons=100000 --dimensions=256:256 --color-resolution=32bit --output-format=bmp output_image.bmp

./tone_mapper.exe --strategy=gm:2.2 output_image.bmp output_image_mapped.bmp
```

```shell
# Linux shell
./renderer --algorithm=path-tracing \
           --paths-per-pixel=100 \
           \
           --dimensions=256:256 \
           --color-resolution=32bit \
           --output-format=bmp \
           output_image.bmp

./tone_mapper.exe --strategy=gm:2.2 output_image.bmp output_image_mapped.bmp
```

```powershell
# Windows powershell
chcp 65001; ./renderer --algorithm=path-tracing --paths-per-pixel=100 --dimensions=256:256 --color-resolution=32bit --output-format=bmp output_image.bmp

./tone_mapper.exe --strategy=gm:2.2 output_image.bmp output_image_mapped.bmp
```
