#include "../Common/Settings.h"
#include "EditorWindow.h"

RTTI_DEFINE_CLASS(editor::EditorWindow,)

namespace editor
{
	static int guidCounter = 0;
	EditorWindow::EditorWindow()
		: _isOpen(true)
	{
		_guid = guidCounter++;
	}

	EditorWindow::~EditorWindow()
	{
		--guidCounter;
	}
}
