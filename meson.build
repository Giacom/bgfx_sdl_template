project('sdl_test', 'cpp',
    meson_version : '>=0.49.2',
    default_options : ['b_vscrt=mtd', 'cpp_std=c++17', 'default_library=static'])

sdl_deps = []
sdl_includes = []


if meson.get_compiler('cpp').get_id() == 'msvc'
    thirdparty_path = join_paths(meson.current_source_dir(), 'thirdparty')
    SDL2_path = join_paths(thirdparty_path, 'SDL2')

    sdl_deps += meson.get_compiler('cpp').find_library('SDL2-staticd', dirs : SDL2_path)
    sdl_deps += meson.get_compiler('cpp').find_library('SDL2maind', dirs : SDL2_path)

    foreach dep : ['version', 'imm32', 'winmm']
        sdl_deps += meson.get_compiler('cpp').find_library(dep)
    endforeach

    sdl_includes = include_directories(join_paths(SDL2_path, 'include'))


endif

#
# Builds bgfx, bimg and bx
# Build instructions were taken from here and converted to mesonbuild: https://github.com/jpcy/bgfx-minimal-example
#

# STDC_FORMAT_MACROS is required for the compatibilty files (e.g: bx/include/compat/msvc)
add_project_arguments('-DBX_CONFIG_DEBUG=1', '-DBGFX_CONFIG_DEBUG=1', '-D__STDC_CONSTANT_MACROS', '-D__STDC_LIMIT_MACROS', '-D__STDC_FORMAT_MACROS',
language : 'cpp')

bxlib_include = include_directories('bx/include/compat/msvc', 'bx/include', 'bx/3rdparty') # replace the 'msvc' if you're using another compiler
bxlib = static_library('bx', 'bx/src/amalgamated.cpp', include_directories: bxlib_include)

astc_codec_files = [
	'bimg/3rdparty/astc-codec/src/decoder/astc_file.cc',
	'bimg/3rdparty/astc-codec/src/decoder/codec.cc',
	'bimg/3rdparty/astc-codec/src/decoder/endpoint_codec.cc',
	'bimg/3rdparty/astc-codec/src/decoder/footprint.cc',
	'bimg/3rdparty/astc-codec/src/decoder/integer_sequence_codec.cc',
	'bimg/3rdparty/astc-codec/src/decoder/intermediate_astc_block.cc',
	'bimg/3rdparty/astc-codec/src/decoder/logical_astc_block.cc',
	'bimg/3rdparty/astc-codec/src/decoder/partition.cc',
	'bimg/3rdparty/astc-codec/src/decoder/physical_astc_block.cc',
	'bimg/3rdparty/astc-codec/src/decoder/quantization.cc',
	'bimg/3rdparty/astc-codec/src/decoder/weight_infill.cc',
]

bimglib_include = include_directories('bimg/include', 'bimg/3rdparty/astc-codec', 'bimg/3rdparty/astc-codec/include')
bimglib = static_library('bimg', 'bimg/src/image.cpp', 'bimg/src/bimg_p.h', 'bimg/include/bimg/bimg.h', 'bimg/include/bimg/decode.h',
						 'bimg/include/bimg/encode.h', 'bimg/src/image_gnf.cpp', astc_codec_files, include_directories: [bxlib_include, bimglib_include], link_with: [bxlib])

bgfx_include = include_directories('bgfx/include', 'bgfx/3rdparty', 'bgfx/3rdparty/dxsdk/include', 'bgfx/3rdparty/khronos')
bgfxlib = static_library('bgfx', 'bgfx/src/amalgamated.cpp', include_directories: [bxlib_include, bgfx_include, bimglib_include], link_with: [bxlib, bimglib])

executable('sdl_test', 'main.cpp', dependencies : [sdl_deps], include_directories : [sdl_includes, bgfx_include, bxlib_include], link_with: [bgfxlib, bimglib, bxlib])