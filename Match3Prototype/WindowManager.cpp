#include "WindowManager.h"

#include <SDL3/SDL.h>

WindowManager::WindowManager()
{

}

int WindowManager::CreateWindow()
{
    if (!SDL_CreateWindowAndRenderer("My Game", 800, 600, SDL_WINDOW_RESIZABLE , &Window, &Renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);

	return 0;
}

SDL_Renderer* WindowManager::GetRendererRef()
{
    return Renderer;
}






