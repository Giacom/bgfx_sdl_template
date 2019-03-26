
@set builddir=builddir
@set buildtype=Debug

@if "%~1"=="release" (
	@set buildtype=Release
	@set builddir=releasedir
	@set buildflags=-Dbuildtype=release -Db_vscrt=mt
)

@echo off
REM call ".\gen_icon.bat"
@echo on


@SETLOCAL
@echo off
REM Pipe the output to nul so it doesn't screw up the integrated terminal of VSCode
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" 1> nul
color
@echo on

@echo Building %buildtype% build..

@if not exist %builddir%\build.ninja (
	meson %builddir% %buildflags%
)
ninja -C %builddir% -v

@ENDLOCAL

@set result=%errorlevel%

@SetLocal EnableDelayedExpansion

@set target=%~1%
@echo Target is %target%

@if %result% EQU 0 (

	if !target!==dx11 call compile_dx11_shaders.bat
	if !target!==opengl call compile_opengl_shaders.bat

	if !errorlevel! NEQ 0 (
		echo BUILD.BAT: Could not compile shaders
		exit /B 1
	) else (
		echo BUILD.BAT: Shaders compiled successfully
	)

	@if "%~2"=="run" (
		%builddir%\sdl_test.exe --!target!
		echo BUILD.BAT: Program finished with exit code %errorlevel%
	) else (
		echo BUILD.BAT: Build finished
	)

) else (
	echo BUILD.BAT: Could not build
)