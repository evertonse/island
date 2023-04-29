# Compile
- ``g++ src/main.cpp  -o bin/app -lfreeglut -Isrc/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/ -Ivendor/noise/ -O3 -Ofast -Wno-write-strings -fomit-frame-pointer -flto``
- try ``-lglut`` instead of ``-lfreeglut`` if prefer/problems

# Run
- ``bin/app``
- Windows ``bin\app.exe``

# Need FreeGlut

### Linux
- ``sudo apt-get install freeglut3-dev``

### Window
- Use ``msys2/mingw`` terminal/shell to install freeglut with the command: ``pacman -S mingw-w64-x86_64-freeglut``.
- If mingw libraries are not on path, go to the project directory using ``mingw shell`` called ``mingw64.exe`` and then compile it.

# Commands:
- ``w, s, d, a``  forward, backward, sideways movement
- ``click`` and ``drag`` to control the view direction
- ``space, q`` to go up 
-  ``crtl+space, e`` to go down (couldn't resolve the crlt glut problem, so the use of crtl is finnicky) 
- ``wheel`` to change fov