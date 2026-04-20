#pragma once

struct SDL_Window;
struct SDL_Renderer;

class WindowManager
{
public:
	WindowManager();

	int CreateWindow();

	SDL_Renderer* GetRendererRef();

private:

	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;

};