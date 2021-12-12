#pragma once
#include <memory>
#include "../EditorWindow.h"

namespace editor
{
	class ImGuiDemoWindow : public EditorWindow
	{
		RTTI_POLYMORPHIC_CLASS_BODY(ImGuiDemoWindow)
	public:
		virtual void Draw() override;

	};
}
