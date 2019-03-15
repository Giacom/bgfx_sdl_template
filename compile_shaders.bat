
@REM Frag shader
shadercDebug.exe -f basic.frag --platform windows -o basic-frag.bin --type f -debug -O 0 -i ../bgfx/bgfx/src --disasm -p ps_5_0

@REM Vertex shader
shadercDebug.exe -f basic.vert --platform windows -o basic-vert.bin --type v -debug -O 0 -i ../bgfx/bgfx/src --disasm -p vs_5_0