# Compile
- ``mkdir bin && g++ src/main.cpp  -o bin/app -lfreeglut -Isrc/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/``
- try ``-lglut`` instead of ``-lfreeglut`` if prefer/problems

# Run
- ``bin/app``
- Windows ``bin\app.exe``

# Need FreeGlut

### Linux
- ``sudo apt-get install freeglut3-dev``

### Window
- Use ``msys2/mingw`` terminal to install freeglut with the command: ``pacman -S mingw-w64-x86_64-freeglut``.

# Commands:
- ``w,s,d,a``  forward, backward, sideways movement
- ``click`` and ``drag`` to control the view direction
- ``space,q`` to go up 
-  ``e`` to go down (couldn't resolve the crlt glut problem, so cant use crtl to go down) 
- ``wheel`` to change fov