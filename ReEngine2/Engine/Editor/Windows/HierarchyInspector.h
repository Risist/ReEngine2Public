#pragma once
#include <memory>
#include <functional>
#include "../EditorWindow.h"

namespace entity
{
	class Entity;
}

namespace editor
{
	class HierarchyInspector : public EditorWindow
	{
		RTTI_POLYMORPHIC_CLASS_BODY(HierarchyInspector)
	public:

		virtual void Init() override;

		virtual void Draw() override;

		using OnActorClickedCallback = std::function<void(std::weak_ptr<entity::Entity>)>;

		OnActorClickedCallback onEntityClicked = [](std::weak_ptr<entity::Entity> actor) {};

	protected:
		void DrawEntity(std::weak_ptr<entity::Entity> actor, bool recursive = true);
		
		std::shared_ptr<class entity::Entity> hoveredEntity;
		std::shared_ptr<class entity::Entity> renamedEntity;
	};
}