@setlocal

@echo Compiling shaders
@REM Frag shader
shadercDebug.exe -f basic.frag --platform linux -o basic-frag.bin --type f -debug -O 0 -i bgfx/src --disasm -p spirv
@if %errorlevel% NEQ 0 (
	exit /B 1
)

@REM Vertex shader
shadercDebug.exe -f basic.vert --platform linux -o basic-vert.bin --type v -debug -O 0 -i bgfx/src --disasm -p spirv
@exit /B %errorlevel%