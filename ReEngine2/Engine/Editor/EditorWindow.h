#pragma once
#include "ImGuiManager.h"
#include "../Reflection/Reflection.h"
namespace editor
{
	class EditorWindow
	{
		RTTI_POLYMORPHIC_CLASS_BODY(EditorWindow)
	public:
		EditorWindow();
		virtual ~EditorWindow();

		virtual void Init()
		{
		}

		virtual void Draw() = 0;

		bool IsOpen() const { return _isOpen; }

		int GetGuid() const { return _guid; }

	protected:
		bool _isOpen;
		int _guid;
	};
}
//RTTI_REGISTER_CLASS(EditorWindow)