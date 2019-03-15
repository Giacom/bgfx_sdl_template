
#include <SDL2/SDL.h>
#include <SDL2/SDL_system.h>
#include <SDL2/SDL_syswm.h>
#include <cstdio>
#include <cstring>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

const bgfx::ShaderHandle loadShader(const char* filename) {
	auto* file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Could not find %s\n", filename);
		throw;
	}

	const bgfx::Memory* memory;
	if (fseek(file, 0, SEEK_END) == 0) {
		long buffer_size = ftell(file);
		if (buffer_size == -1) {
			fprintf(stderr, "Could not determine size of the file %s.\n", filename);
			throw;
		}

		memory = bgfx::alloc(sizeof(char) * buffer_size);

		if (fseek(file, 0, SEEK_SET) != 0) { throw; }

		size_t new_len = fread(memory->data, sizeof(char), buffer_size, file);
		if (ferror(file) != 0) {
			fprintf(stderr, "Error reading file\n");
		}
	}
	fclose(file);

	bgfx::ShaderHandle handle = bgfx::createShader(memory);
	bgfx::setName(handle, filename);
	return handle;
}

int main(int argc, char* args[]) {

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow(
					"hello_sdl2",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					SCREEN_WIDTH, SCREEN_HEIGHT,
					SDL_WINDOW_SHOWN
					);

	if (!window) {
		fprintf(stderr, "Could not create window. Reason: %s\n", SDL_GetError());
		return 1;
	}

	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window, &wmi)) {
		fprintf(stderr, "Could not retrieve WM info\n");
		return 1;
	}

	bgfx::Init init;
	init.type = bgfx::RendererType::Enum::Count;//bgfx::RendererType::Enum::Direct3D11;
	init.debug = false;
	init.resolution.width  = SCREEN_WIDTH;
	init.resolution.height = SCREEN_HEIGHT;
	init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X8;
	init.platformData.ndt = nullptr;
	init.platformData.nwh = wmi.info.win.window;

	bool result = bgfx::init(init);
	if (!result) {
		fprintf(stderr, "BGFX did not successfully initialise! HWND was %p\n", wmi.info.win.window);
		return 1;
	}

	fprintf(stderr, "BGFX initialised with %s\n", bgfx::getRendererName(bgfx::getRendererType()));




	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);

	static float triangle_vertex[] = {
		0.0f, 0.5f, 0.0f, /* */ 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, /* */ 0.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, /* */  1.0f, 0.0f, 0.0f, 1.0f,
	};

	bgfx::VertexDecl vertexDecl;
	vertexDecl
		.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float, true)
		.end();


	auto vb = bgfx::createVertexBuffer(bgfx::makeRef(triangle_vertex, sizeof(triangle_vertex)), vertexDecl);

	auto fragShader = loadShader("basic-frag.bin");
	auto vertShader = loadShader("basic-vert.bin");

	if (!bgfx::isValid(fragShader) || !bgfx::isValid(vertShader)) {
		fprintf(stderr, "Could not load shaders\n");
		return 1;
	}

	bgfx::ProgramHandle program = bgfx::createProgram(vertShader, fragShader, true);
	if (!bgfx::isValid(program)) {
		fprintf(stderr, "Could not load shader program\n");
		return 1;
	}


	SDL_Event event;
	bool running = true;
	int i = 0;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}

		}

		bgfx::setViewRect(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		bgfx::touch(0);

		bgfx::setVertexBuffer(0, vb);
		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW | BGFX_STATE_MSAA);
		bgfx::submit(0, program);

		bgfx::frame();
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	fprintf(stderr, "Finished running\n");
	return 0;
}
