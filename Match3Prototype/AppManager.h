#pragma once

#include <memory>
#include <cstdint>

class WindowManager;
class Renderer;
class EntityManager;

class Board;

union SDL_Event;

class AppManager
{
public:
	AppManager();
	~AppManager();

	int InitApplication();
	void Iterate();

	int HandleInput(void* appstate, SDL_Event* event);

private:
	void SaveDeltaTime(double Delta);

	std::unique_ptr<WindowManager> WindowManagerRef;
	std::unique_ptr<Renderer> RendererRef;
	std::unique_ptr<EntityManager> EntityManagerRef;

	std::unique_ptr<Board> BoardRef;

	uint64_t LastFrameTime = 0;

	double LastDelta = 0;
};