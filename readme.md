# Compile
- ``g++ src/main.cpp  -o bin/app -lfreeglut -Isrc/ -Idemos/ -Ivendor/ -Ivendor/glad/include/ -Ivendor/stb/include/``
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
- ``space,q`` to go up (couldn't resolve the crlt glut problem) 
-  ``e`` to go down