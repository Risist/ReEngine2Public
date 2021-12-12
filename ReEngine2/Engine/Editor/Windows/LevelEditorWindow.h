#pragma once
#include "../EditorWindow.h"

namespace editor
{
	class LevelEditorWindow : public EditorWindow
	{
		RTTI_POLYMORPHIC_CLASS_BODY(LevelEditorWindow)
	public:
		LevelEditorWindow();

		virtual void Draw() override;


	};
}