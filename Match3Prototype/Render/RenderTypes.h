#pragma once

#include <cstdint>
#include <variant>

enum class ERenderType : uint8_t
{
	Rectangle,
	Image
};

struct Rectangle
{
	float X;
	float Y;
	float Width;
	float Height;
	uint32_t Color;

	Rectangle(float InX, float InY, float InWidth = 0, float InHeight = 0, uint32_t InColor = 0) : X(InX), Y(InY), Width(InWidth), Height(InHeight), Color(InColor)
	{
	}

	void SetRenderLocation(float XIn, float YIn)
	{
		X = XIn;
		Y = YIn;
	}
};

struct Image
{
	float X;
	float Y;
	//IMG DATA

	Image(float InX, float InY) : X(InX), Y(InY)
	{
	}

	void SetRenderLocation(float XIn, float YIn)
	{
		X = XIn;
		Y = YIn;
	}
};

using RenderData = std::variant<Rectangle, Image>;