#pragma once
#include "rttiClassType.h"
#include "rttiClassBuilder.h"

namespace rtti
{
	template<typename Ty>
	inline bool ClassType::IsA() const
	{
		return IsA(GetClassDescriptor<Ty>());
	}
}
