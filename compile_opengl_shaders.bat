@setlocal

@echo Compiling shaders
@REM Frag shader
shadercDebug.exe -f basic.frag --platform windows -o basic-frag.bin --type f -i bgfx/src --disasm
@if %errorlevel% NEQ 0 (
	exit /B 1
)

@REM Vertex shader
shadercDebug.exe -f basic.vert --platform windows -o basic-vert.bin --type v -i bgfx/src --disasm
@exit /B %errorlevel%