#include "AppManager.h"

#include <SDL3/SDL.h>

#include "WindowManager.h"
#include "Renderer.h"
#include "EntityManager.h"

#include "ScopeTimer.h"
#include "BlackBoxEntityManager.h"

#include "Board.h"

AppManager::AppManager()
{
}

AppManager::~AppManager() = default;

int AppManager::InitApplication()
{
    WindowManagerRef = std::make_unique<WindowManager>();

    if (WindowManagerRef->CreateWindow() != 0)
    {
        return 1;
    }

    EntityManagerRef = std::make_unique<EntityManager>();

#ifdef EnableTests

    BlackBoxEntityManager BlackBoxTestSet(*EntityManagerRef);
    BlackBoxTestSet.RunAllTests();

#endif

    RendererRef = std::make_unique<Renderer>(WindowManagerRef->GetRendererRef(), EntityManagerRef.get());

    BoardRef = std::make_unique<Board>(EntityManagerRef.get());
    BoardRef->InitializeBoard();

    return 0;
}

void AppManager::Iterate()
{
    ScopeTimer AppTimerMS("AppLoop", [this](double DeltaIn) {SaveDeltaTime(DeltaIn); });

    RendererRef->Tick();
    BoardRef->Tick(LastDelta);
}

void AppManager::SaveDeltaTime(double Delta)
{
    LastDelta = Delta;
}

int AppManager::HandleInput(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return 1;
    }

    return 0;
}
