#pragma once

#include <cstdint>
#include <string_view>
#include <print>
#include <functional>

#include "SDL3/SDL.h"

class ScopeTimer
{
public:
	ScopeTimer(std::string_view LabelIn, std::function<void(double)> CallbackIn = nullptr) : InitialTime(SDL_GetPerformanceCounter()), Label(LabelIn), Callback(CallbackIn)
	{
	};

	~ScopeTimer()
	{
		const uint64_t EndTime = SDL_GetPerformanceCounter();
		const uint64_t Frequency = SDL_GetPerformanceFrequency();

		const double MS = static_cast<double>(EndTime - InitialTime) * 1000.0 / static_cast<double>(Frequency);

		std::println("{} - {} ms", Label, MS);

		if(Callback)
			Callback(MS);
	};

	ScopeTimer(const ScopeTimer&) = delete;
	ScopeTimer& operator=(const ScopeTimer&) = delete;

	ScopeTimer(ScopeTimer&&) = delete;
	ScopeTimer& operator=(ScopeTimer&&) = delete;

private:

	const uint64_t InitialTime;
	std::string_view Label;
	std::function<void(double)> Callback;
};
