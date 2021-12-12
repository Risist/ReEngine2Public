#pragma once
#include "rttiArray.h"
#include "rttiTypeBuilder.h"
#include <functional>

namespace rtti
{
	template<typename ArrayType>
	struct GetTypeDescriptor<std::vector<ArrayType>>
	{
		TypeBase* operator()()
		{
			static TVectorArray<ArrayType> RTTI_DESCRIPTOR_NAME(RE_STRINGIFY(std::vector<ArrayType>), GetTypeDescriptor<ArrayType>());
			return &RTTI_DESCRIPTOR_NAME;
		}
		TypeBase* operator->()
		{
			return operator()();
		}
		operator TypeBase* ()
		{
			return operator()();
		}
	};
}

