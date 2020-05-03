
#include <SDL2/SDL.h>
#include <SDL2/SDL_system.h>
#include <SDL2/SDL_syswm.h>
#include <cstdio>
#include <cstring>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600


struct DebugCallback : public bgfx::CallbackI {
	DebugCallback() {}

	void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) {
		static const char* codes[] = {
			"DebugCheck",
			"InvalidShader",
			"UnableToInitialize",
			"UnableToCreateTexture",
			"DeviceLost",
			"Count",
		};
		fprintf(stderr, "[%s:%d] %s %s\n", _filePath, _line, codes[(int)_code], _str);
	}

	void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) {}
	void profilerBegin(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) {}
	void profilerBeginLiteral(const char* _name, uint32_t _abgr, const char* _filePath, uint16_t _line) {}
	void profilerEnd() {}
	uint32_t cacheReadSize(uint64_t _id) { return 0; }
	bool cacheRead(uint64_t _id, void* _data, uint32_t _size) { return false; }
	void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) {}
	void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) {}
	void captureBegin(uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip) {}
	void captureEnd() {}
	void captureFrame(const void* _data, uint32_t _size) {}
};

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


int main(int argc, char** argv) {

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}

	bgfx::RendererType::Enum renderer =
#if _WIN32
		bgfx::RendererType::Enum::Direct3D11;
#else
		bgfx::RendererType::Enum::OpenGL;
#endif

	for (int i = 0; i < argc; i++) {
		const char* argument = argv[i];
		if (strcmp("--dx11", argument) == 0) {
			renderer = bgfx::RendererType::Enum::Direct3D11;
		}
		else if (strcmp("--dx9", argument) == 0) {
			// Doesn't currently work
			renderer = bgfx::RendererType::Enum::Direct3D9;
		}
		else if (strcmp("--opengl", argument) == 0) {
			renderer = bgfx::RendererType::Enum::OpenGL;
		}
		else if (strcmp("--opengles", argument) == 0) {
			renderer = bgfx::RendererType::Enum::OpenGLES;
		}
		else if (strcmp("--vulkan", argument) == 0) {
			// Doesn't currently work
			renderer = bgfx::RendererType::Enum::Vulkan;
		}
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

	DebugCallback debugCallback;

	bgfx::Init init;
	init.type = renderer;
	init.debug = true;
	init.callback = &debugCallback;
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

	if (bgfx::getRendererType() != renderer) {
		fprintf(stderr, "BGFX could not create a %s renderer\n", bgfx::getRendererName(renderer));
		return 1;
	}


	fprintf(stderr, "BGFX initialised with %s\n", bgfx::getRendererName(bgfx::getRendererType()));


	// Load texture
	int x, y, components;
	unsigned char* textureRaw = stbi_load("duck.tga", &x, &y, &components, 4);
	if (!textureRaw) {
		fprintf(stderr, "stbi_load could not load duck.tga");
		return 1;
	}


	auto texHandle = bgfx::createTexture2D(x, y, false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_UVW_CLAMP,
		bgfx::makeRef(textureRaw, x * y * components, [](void* ptr, void* userData) {
			stbi_image_free(ptr);
		}
	));


	// Setup vertices

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030FF, 1.0f, 0);

	float w_ratio = (float)x / (float)SCREEN_WIDTH;
	float h_ratio = (float)y / (float)SCREEN_HEIGHT;

	const float zoom = 0.5f;
	w_ratio *= zoom;
	h_ratio *= zoom;


	fprintf(stderr, "W Ratio: %f / R Ratio: %f\n", w_ratio, h_ratio);

	float square_vertex[] = {
		// Bottom right triangle
		/* Position */ -w_ratio, -h_ratio, 0.0f, /* Colour */ 1.0f, 1.0f, 1.0f, 1.0f, /* UV */  0.0f, 1.0f,
		/* Position */  w_ratio, -h_ratio, 0.0f, /* Colour */ 1.0f, 1.0f, 1.0f, 1.0f, /* UV */  1.0f, 1.0f,
		/* Position */  w_ratio,  h_ratio, 0.0f, /* Colour */ 1.0f, 1.0f, 1.0f, 1.0f, /* UV */  1.0f, 0.0f,

		// Top left triangle
		/* Position */  w_ratio,  h_ratio, 0.0f, /* Colour */ 1.0f, 1.0f, 1.0f, 1.0f, /* UV */  1.0f, 0.0f,
		/* Position */ -w_ratio,  h_ratio, 0.0f, /* Colour */ 1.0f, 1.0f, 1.0f, 1.0f, /* UV */  0.0f, 0.0f,
		/* Position */ -w_ratio, -h_ratio, 0.0f, /* Colour */ 1.0f, 1.0f, 1.0f, 1.0f, /* UV */  0.0f, 1.0f,
	};

	bgfx::VertexLayout vertexDecl;
	vertexDecl
		.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();


	auto vb = bgfx::createVertexBuffer(bgfx::makeRef(square_vertex, sizeof(square_vertex)), vertexDecl);

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

	auto texUniform = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

	SDL_Event event;
	bool running = true;
	int i = 0;
	bool debug = false;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}

			if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_V) {
				debug = !debug;
				bgfx::setDebug(debug ? BGFX_DEBUG_STATS : 0);
			}

		}

		bgfx::setViewRect(0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		bgfx::touch(0);

		bgfx::setVertexBuffer(0, vb);
		bgfx::setTexture(0, texUniform, texHandle);
		bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA);
		bgfx::submit(0, program);

		bgfx::frame();
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	fprintf(stderr, "Finished running\n");
	return 0;
}
