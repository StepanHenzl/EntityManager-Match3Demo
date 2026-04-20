#pragma once

#include <memory>

class WindowManager;

struct SDL_Renderer;

class EntityManager;

struct EntityDataPerLayerZ;

class Renderer
{
public:
	Renderer(SDL_Renderer* RendererRefIn, EntityManager* EntityManagerRefIn);

	void DrawRectanglesLayer(const EntityDataPerLayerZ& LayerDataRef);

	void Tick();

private:

	SDL_Renderer* SDLRendererRef;

	EntityManager* EntityManagerRef;
};