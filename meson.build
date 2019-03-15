project('sdl_test', 'cpp',
    meson_version : '>=0.49.2',
    default_options : ['b_vscrt=mtd', 'cpp_std=c++17', 'default_library=static'])

sdl_deps = []
sdl_includes = []

bgfx_deps = []
bgfx_includes = []

if meson.get_compiler('cpp').get_id() == 'msvc'
    thirdparty_path = join_paths(meson.current_source_dir(), 'thirdparty')
    SDL2_path = join_paths(thirdparty_path, 'SDL2')

    sdl_deps += meson.get_compiler('cpp').find_library('SDL2-staticd', dirs : SDL2_path)
    sdl_deps += meson.get_compiler('cpp').find_library('SDL2maind', dirs : SDL2_path)

    foreach dep : ['version', 'imm32', 'winmm']
        sdl_deps += meson.get_compiler('cpp').find_library(dep)
    endforeach

    sdl_includes = include_directories(join_paths(SDL2_path, 'include'))


    foreach dep : ['bgfx', 'bimg', 'bimg_decode', 'bx']
        dep_path = join_paths(thirdparty_path, dep)

        dep = dep + 'Debug'
        bgfx_deps += meson.get_compiler('cpp').find_library(dep, dirs : dep_path)

        bgfx_includes += include_directories(join_paths(dep_path, 'include'))
    endforeach


    add_project_arguments('-D_HAS_ITERATOR_DEBUGGING=0', '-D_ITERATOR_DEBUG_LEVEL=0', language : 'cpp')

endif

executable('sdl_test', 'main.cpp', dependencies : [sdl_deps, bgfx_deps], include_directories : [sdl_includes, bgfx_includes])