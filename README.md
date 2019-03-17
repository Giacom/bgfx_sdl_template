NOTE:

* This project uses mesonbuild and builds bgfx, bimg and bx through git submodules. SDL2 is a static library that has been prebuilt.
* shaadercDebug.exe was compiled and taken from the BGFX project.
* The shaders need shader headers from the BGFX project. My shaders point to it relative to where the directory is, which goes out of the project directory.
* There's some vscode settings which you should remove if you don't need them.