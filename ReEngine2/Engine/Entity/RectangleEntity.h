#pragma once
#include "TransformableEntity.h"
#include <Engine/Graphics/Rendering.h>

namespace entity
{
	// TODO assigning texture in editor should by default set _rectSize to the texture dimensions
	//		param to prerceive aspect ratio
	//		change texture to sprite?

	class RectangleEntity : public TransformableEntity
	{
		RTTI_DECLARE_ASSET(TransformableEntity)
	public:
		RectangleEntity();

		virtual void OnDraw(rendering::RenderViewport& viewport) override;

		virtual void SetFillColor(sf::Color color)
		{
			_fillColor = color;
			_shape.setFillColor(_fillColor);
		}
		sf::Color GetFillColor() const
		{
			return _fillColor;
		}

	protected:
		virtual void OnPositionChanged() override
		{
			_shape.setPosition(GetGlobalPosition());
		}
		virtual void OnRotationChanged() override
		{
			_shape.setRotation(GetGlobalRotation().AsDegree());
		}
		virtual void OnScaleChanged() override
		{
			_shape.setScale(GetGlobalScale());
		}

		virtual virtual void OnInit() override;

	private:
		sf::Color _fillColor;

		sf::RectangleShape _shape;

		std::weak_ptr<rendering::SpriteAsset> _sprite;
	};

}