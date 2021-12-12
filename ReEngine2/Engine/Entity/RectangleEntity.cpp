#include "RectangleEntity.h"
#include "EntityLevel.h"

RTTI_DEFINE_ASSET(entity::RectangleEntity,
	{
		RTTI_BASE(entity::TransformableEntity);
		RTTI_TOOLTIP(
			"Test rect rendering, \n"
		);
		RTTI_DISPLAY_NAME("RectangleEntity");

		RTTI_CATEGORY("Rendering");
		RTTI_PROPERTY(_fillColor)
			.SetDisplayName("FillColor");

		RTTI_PROPERTY_GETTER(_fillColor, GetFillColor);
		RTTI_PROPERTY_SETTER(_fillColor, SetFillColor);

		RTTI_PROPERTY(_sprite)
			.SetDisplayName("Sprite");

	});

namespace entity
{
	RectangleEntity::RectangleEntity()
	{
		SetFillColor(sf::Color::White);
	}

	void RectangleEntity::OnDraw(rendering::RenderViewport& viewport)
	{
		_shape.setTexture(nullptr, true);

		if (_sprite.expired() == false)
		{
			auto lockedSprite = _sprite.lock();
			RE_ASSERT(lockedSprite->textureAssetSource.expired() == false);

			_shape.setTexture(&lockedSprite->textureAssetSource.lock()->texture, true);

			_shape.setSize((sf::Vector2f)lockedSprite->size);
			_shape.setTextureRect(sf::IntRect(lockedSprite->position, lockedSprite->size));
			_shape.setOrigin((sf::Vector2f)lockedSprite->size * 0.5f);
		}

		viewport._window.draw(_shape);
	}

	void RectangleEntity::OnInit()
	{
		entity::CompoundEntity::OnSpawn();
		
		auto locked = _currentLevel.lock();
		locked->RegisterForDisplay(_this, 0);
		//locked->RegisterForUpdate(_this);
		//locked->RegisterForFixedUpdate(_this);

	}

}