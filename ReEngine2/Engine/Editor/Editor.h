#pragma once
#include <memory>
#include "EditorWindow.h"
#include "UndoRedo/UndoRedo.h"

class Asset;

namespace editor
{
	class EditorWindow;

	class EditorContext
	{
	public:

		bool HasAnyAssetChoosen() noexcept
		{
			return !referencedAsset.expired();
		}
		void Unselect() noexcept
		{
			referencedAsset = std::weak_ptr<Asset>();
		}

		void Select(std::weak_ptr<Asset> object) noexcept
		{
			referencedAsset = object;
		}

		std::weak_ptr<Asset> GetAsset() const noexcept
		{
			return referencedAsset;
		}

	private:
		std::weak_ptr<Asset> referencedAsset;
	};

	class Editor
	{
	public:
		Editor();

		void Init();


		void Draw();

		void AddNewWindow(std::unique_ptr<EditorWindow> newWindow);

		EditorContext editorContext;
		undoRedo::Manager redoUndoManager;

	private:
		std::vector<std::unique_ptr<EditorWindow>> _editorWindows;

		void DrawMenuBar();
		void DrawWindows();
	};

	extern std::unique_ptr<Editor> editor;
}

/////// TODO
// enabled flags should appear only for those values that have their _canEverXXXX flag enabled
// saving and restoring last open windows
// saving and restoring world state
// Default size for the windows