# Compile
- Debian-Based ``g++ src/main.cpp  -o bin/app -lglut -ldl -Isrc/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/ -Ivendor/noise/ -O3 -Ofast -Wno-write-strings -fomit-frame-pointer -flto``
- try ``-lfreeglut`` instead of ``-lglut`` if prefer/problems

- Windows ``g++ src/main.cpp  -o bin/app -lfreeglut -Isrc/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/ -Ivendor/noise/ -O3 -Ofast -Wno-write-strings -fomit-frame-pointer -flto``

# Run
- ``bin/app`` [scene_input_file] 
- Window:  ``bin\app.exe`` [scene_input_file]
- If no ``scene_input_file`` is provided, a default one will be used

-------
# Need: 
## FreeGlut

#### Linux (Debian based)
- ``sudo apt-get install freeglut3-dev``

#### Windows
- Use ``msys2/mingw`` terminal/shell to install freeglut with the command: ``pacman -S mingw-w64-x86_64-freeglut``.
- If mingw libraries are not on path, go to the project directory using ``mingw shell`` called ``mingw64.exe`` and then compile it.
## C++ 20 compiler
- Might work with c++17 compiler, but it was only tested with ``g++ 12.2.0``

-----

# Commands:
- ``w, s, d, a``  forward, backward, sideways movement
- ``click`` and ``drag`` to control the view direction
- ``space, q`` to go up 
-  ``crtl+space, e`` to go down (couldn't resolve the crlt glut problem, so the use of crtl is finnicky) 
- ``wheel`` to change fov
