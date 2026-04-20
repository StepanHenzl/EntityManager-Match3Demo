#pragma once

template<typename T, typename FloatType = double>
inline T Lerp(T A, T B, FloatType Time)
{
	return static_cast<T>(A + (B - A) * Time);
}

struct VectorF
{
	float X;
	float Y;

	bool operator==(const VectorF& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	VectorF operator+(const VectorF& Other) const
	{
		return { X + Other.X, Y + Other.Y };
	}
};