#if 0

this_file="src/main.cpp"
this_exec="bin/app"

exec_line="g++ $this_file -o $this_exec  -ldl -lglut -Isrc/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/"
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
//#include "pointer_app.cpp"
//#include "project.cpp"
//#include "refactor.cpp"
//#include "simple.cpp"
//#include "fixed_pipeline.cpp"
//#include "learn-opengl.cpp"
#include "island.cpp"

// Compile: bash src/main.cpp 