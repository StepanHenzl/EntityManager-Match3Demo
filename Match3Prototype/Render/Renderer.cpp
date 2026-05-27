#include "Renderer.h"

#include <SDL3/SDL.h>

#include "WindowManager.h"
#include "EntityManager.h"
#include "EntityBatchData.h"

#include "ErrorHandler.h"

Renderer::Renderer(SDL_Renderer* RendererRefIn, EntityManager* EntityManagerRefIn)
{
    SDLRendererRef = RendererRefIn;

    EntityManagerRef = EntityManagerRefIn;

    SDL_SetRenderLogicalPresentation
    (
        SDLRendererRef,
        1920,
        1080,
        SDL_LOGICAL_PRESENTATION_STRETCH
    );
}

void Renderer::DrawRectanglesLayer(const EntityDataPerLayerZ& LayerDataRef)
{
    SDL_FRect RectangleSDL;
    for (const auto& Rectangle : LayerDataRef.RectanglesData)
    {
        RectangleSDL.x = Rectangle.X;
        RectangleSDL.y = Rectangle.Y;
        RectangleSDL.w = Rectangle.Width;
        RectangleSDL.h = Rectangle.Height;
        SDL_SetRenderDrawColor(SDLRendererRef, (Rectangle.Color >> 0), (Rectangle.Color >> 8), (Rectangle.Color >> 16), (Rectangle.Color >> 24));
        SDL_RenderFillRect(SDLRendererRef, &RectangleSDL);
    }
}

void Renderer::Tick()
{ 
    SDL_SetRenderDrawColor(SDLRendererRef, 0, 80, 0, 255);
    SDL_RenderClear(SDLRendererRef);

    if(!EntityManagerRef)
        Error_Abort("Render have invalid EntityManager pointer!\n");

    auto& EntityBatchDataRef = EntityManagerRef->ReadEntityBatchData();

    for (int LayerZ = 0; LayerZ < 256; LayerZ++)
    {
        DrawRectanglesLayer(EntityBatchDataRef.DataPerLayerZ[LayerZ]);
    }

    SDL_RenderPresent(SDLRendererRef);
}
