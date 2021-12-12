#pragma once
#include "../EditorWindow.h"
#include "../../Common/Settings.h"

namespace editor
{
	class TextureManagerWindow : public EditorWindow
	{
		RTTI_POLYMORPHIC_CLASS_BODY(TextureManagerWindow)
	public:
		TextureManagerWindow();

		virtual void Draw() override;

	};
}