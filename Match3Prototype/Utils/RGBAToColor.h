#pragma once

#include <cstdint>

inline uint32_t RGBAToColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
{
	return (R << 0) | (G << 8) | (B << 16) | (A << 24);
}