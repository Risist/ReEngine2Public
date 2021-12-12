#pragma once
#include "../EditorWindow.h"

namespace editor
{

	class AssetInspectorWindow : public EditorWindow
	{
		RTTI_POLYMORPHIC_CLASS_BODY(AssetInspectorWindow)
	public:
		virtual void Draw() override;
	};
}