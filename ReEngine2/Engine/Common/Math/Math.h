#pragma once

#include "Angle.h"
#include <glm/glm.hpp>

template <typename Ty>
inline Ty Min(Ty x, Ty y)
{
	return x > y ? y : x;
}

template <typename Ty>
inline Ty Max(Ty x, Ty y)
{
	return x > y ? x : y;
}

template< class Type>
inline Type Clamp(Type value, Type min, Type max)
{
	if (value > max)
		return max;
	else if (value < min)
		return min;
	return value;
}

inline float32 RandRange(float32 min, float32 max)
{
	return (float)rand() / RAND_MAX * (max - min) + min;
}
inline sf::Time RandRange(sf::Time min, sf::Time max)
{
	return (float)rand() / RAND_MAX * (max - min) + min;
}

template<typename Ty, typename AlphaTy >
inline Ty Lerp(Ty x1, Ty x2, AlphaTy t)
{
	return x1 + (x2 - x1) * t;
}

template<typename Ty, typename AlphaTy = float>
AlphaTy InverseLerp(Ty xx, Ty yy, Ty value)
{
	return (value - xx) / (yy - xx);
}

template<typename Ty = Angle, typename AlphaTy >
inline Angle Lerp(Angle x1, Angle x2, AlphaTy t)
{
	return x1 + x2.MinimalDiffirence(x1) * t;
}
