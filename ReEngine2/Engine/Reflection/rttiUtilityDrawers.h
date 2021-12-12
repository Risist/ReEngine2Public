#pragma once
#include "rttiTypeDrawer.h"


namespace rtti
{
	class TypeDrawerSpace : public TypeDrawerBase
	{
	public:
		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;
	};

	class TypeDrawerInfoBox : public TypeDrawerBase
	{
	public:
		TypeDrawerInfoBox(const char* message)
			: _message(message)
		{

		}

		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall = [](){} ) const override;

	private:
		const char* _message;
	};


#define RTTI_SPACE									\
	{												\
		static rtti::TypeDrawerSpace  drawer;		\
		RTTI_DESCRIPTOR_NAME.AddDrawer(&drawer);	\
	}

#define RTTI_INFOBOX(info)								\
	{													\
		static rtti::TypeDrawerInfoBox  drawer(info);	\
		RTTI_DESCRIPTOR_NAME.AddDrawer(&drawer);		\
	}

// TODO 
}