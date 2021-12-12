#include "CompoundEntity.h"
#include <Engine/AssetManagament/AssetManager.h>
#include "EntityLevel.h"

RTTI_DEFINE_ASSET(entity::CompoundEntity,
	{
		RTTI_BASE(entity::Entity);
	});

namespace entity
{
	bool CompoundEntity::CanBeAChild(const rtti::ClassType& potentialChildType) const
	{
		return true;
	}
	
	std::shared_ptr<Entity> CompoundEntity::Duplicate() const
	{
		std::shared_ptr<Entity> duplicated(GetType()->DuplicateNewShared<Entity>(this));
		duplicated->_this = duplicated;
		duplicated->SetName(duplicated->GetName() + " (Copy) ");

		//
		RE_ASSERT(dynamic_cast<CompoundEntity*>(duplicated.get()));
		auto compound = (CompoundEntity*)duplicated.get();
		for (const auto& it : _components)
		{
			compound->InsertComponent(it->Duplicate());
		}

		return duplicated;
	}
}