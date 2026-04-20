#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <memory>

#include "AppManager.h"

static std::unique_ptr<AppManager> AppManagerRef = nullptr;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{

#ifdef _DEBUG
#include <crtdbg.h>
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG); // break into debugger instead of showing dialog
#endif

    AppManagerRef = std::make_unique<AppManager>();

    if (AppManagerRef->InitApplication() != 0)
    {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if(AppManagerRef->HandleInput(appstate, event) == 1)
        return SDL_APP_SUCCESS;

    if(AppManagerRef->HandleInput(appstate, event) == 0)
        return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    AppManagerRef->Iterate();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}