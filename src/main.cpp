#if 0

this_file="src/main.cpp"
this_exec="bin/app"

exec_line="g++ src/main.cpp  -o bin/app -lglut -ldl -Isrc/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/ -Ivendor/noise/ -O3 -Ofast -Wno-write-strings -fomit-frame-pointer -flto"
echo "exec: $exec_line"

eval $exec_line

if [ $? -eq 0 ]; then
    echo "Compile succeeded: $this_file --> $this_exec"
    exit $?
else
    echo "Compile error"
    exit 1
fi
#endif

// You can call this file as a bash file to compile the project
// bash src/main.cpp from the root of this project
// The start and main function of the project is on "island.cppc"
#include "island.cpp"
