#include "ErrorHandler.h"

#include <SDL3/SDL.h>

void Error_Abort(const char* ErrorMessage)
{
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", ErrorMessage);
	std::abort();
}
