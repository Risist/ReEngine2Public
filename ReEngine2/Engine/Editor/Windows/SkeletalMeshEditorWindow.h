#pragma once
#include "../EditorWindow.h"

namespace editor
{

	class SkeletalMesheditorWindow : public EditorWindow
	{
		RTTI_POLYMORPHIC_CLASS_BODY(SkeletalMesheditorWindow)
	public:
		SkeletalMesheditorWindow();

		virtual void Draw() override;
	};
}