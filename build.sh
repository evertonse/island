#!/bin/env bash
# pushd and popd doesn't exist in pure posix shell



file="src/island.cpp"
exe="bin/island"

# Check if running on WSL
# if 0; then
# Use MinGW for WSL and just open a .exe instead
# if grep -qEi "(Microsoft|WSL)" /proc/version; then
# else
# fi
build_mingw() {
    cxx="x86_64-w64-mingw32-g++"
    cc="x86_64-w64-mingw32-gcc"
    glfw_obj="bin/rglfw.obj"
    glfw_flags=""
    # libs="-lm -lgdi32 -luser32"
    libs="-lgdi32 -luser32"
}

build_linux() {
    cc="gcc"
    cxx="g++"
    glfw_obj="bin/rglfw.o"
    glfw_flags="-D_GLFW_X11"
    libs="-ldl"
}


build_msys2() {
    cxx="g++"
    cc="gcc"
    glfw_obj="bin/rglfw.obj"
    glfw_flags=""
    libs="-lgdi32 -luser32 -lws2_32"

    # On Msys2 it's necessary to tell clang where the header files are and this solves it
    if [[ "$cxx" == "clang++" ]]; then
        libs="--target=x86_64-w64-windows-gnu $libs"
    fi
}



# Check if running on Windows with MSYS2
if [[ "$OSTYPE" == "msys" ]]; then
    build_msys2
else
    # Check if running on WSL
    if grep -qEi "(Microsoft|WSL)" /proc/version; then
        if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
            build_mingw
        else
            echo "Error: x86_64-w64-mingw32-g++ not found. Please install mingw to build windows exe on WSL."
            exit 1
        fi
    else
        if command -v g++ &> /dev/null; then
            build_linux
        else
            echo "Error: g++ not found. Please install g++ to build on Linux."
            exit 1
        fi
    fi
fi


[ -d "bin/" ] || mkdir bin/

set -xe
pushd ./vendor/glfw/
[ -f $glfw_obj ] || $cc rglfw.c -c  $glfw_flags -lc -lm -o ../../$glfw_obj
popd

# -flto  -O3 makes it not compile
flags="-std=c++20 -O3 -fomit-frame-pointer -Wno-write-strings -ffast-math"
$cxx $flags $file $glfw_obj -o $exe $libs -Isrc/ -Ivendor/glfw/glfw/include/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/ -Ivendor/noise/  -Wno-stringop-overflow

# $cxx $file $glfw_obj -o $exe $libs -Isrc/ -Ivendor/glfw/glfw/include/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/ -Ivendor/noise/ -g

if [ $? -eq 0 ]; then
    echo "Compilation succeeded: $file --> $exe"
    exit $?
else
    echo "Compilation error"
    exit 1
fi
