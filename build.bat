
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


@if %errorlevel% LEQ 0 (

	compile_shaders.bat
	@REM %builddir%\tools\welder.exe resources %builddir%\baller.exe

	@if "%~1"=="run" (
		%builddir%\sdl_test.exe
		@echo BUILD.BAT: Program finished with exit code %errorlevel%
	) else (
		@echo BUILD.BAT: Build finished
	)

)