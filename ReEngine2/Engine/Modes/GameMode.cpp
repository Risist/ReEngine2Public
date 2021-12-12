#include "GameMode.h"
#include <Engine/AssetManagament/AssetManager.h>


RTTI_DEFINE_ASSET(Creature,
	{
		RTTI_BASE(entity::Entity);

		RTTI_PROPERTY(x);
		RTTI_PROPERTY(sprite);
	});