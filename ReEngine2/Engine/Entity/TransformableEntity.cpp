#include "TransformableEntity.h"
#include <Engine/AssetManagament/AssetManager.h>

RTTI_DEFINE_ASSET(entity::TransformableEntity,
	{
		RTTI_BASE(entity::CompoundEntity);
		RTTI_CATEGORY("Transform");
		RTTI_TOOLTIP(
			"Base class for all object that supports positioning, \n"
			"Propagates position changes to children \n"
			"Only another Transformable can be a child"
		);

		RTTI_PROPERTY(_localPosition)
			.SetOverrideTypeDrawer<editor::Vector2FDrawer<sf::Vector2f>>(1)
			.SetDisplayName("Position");

		RTTI_PROPERTY(_localRotation)
			.SetOverrideTypeDrawer<editor::AngleDrawer>(0.15f)
			.SetDisplayName("Rotation");

		RTTI_PROPERTY(_localScale)
			.SetOverrideTypeDrawer<editor::Vector2FDrawer<sf::Vector2f>>(0.01)
			.SetDisplayName("Scale");

		RTTI_PROPERTY_GETTER(_localPosition, GetLocalPosition);
		RTTI_PROPERTY_SETTER(_localPosition, SetLocalPosition);

		RTTI_PROPERTY_GETTER(_localScale, GetLocalScale);
		RTTI_PROPERTY_SETTER(_localScale, SetLocalScale);

		RTTI_PROPERTY_GETTER(_localRotation, GetLocalRotation);
		RTTI_PROPERTY_SETTER(_localRotation, SetLocalRotation);
	});

namespace entity
{

	TransformableEntity::TransformableEntity()
	{
		// trigger initial callbacks
		SetGlobalPosition({});
		SetGlobalScale({ 1,1 });
		SetGlobalRotation(Degree(0));
	}

	bool TransformableEntity::CanBeAChild(const rtti::ClassType& potentialChildType) const
	{
		return potentialChildType.IsA<TransformableEntity>();
	}

	sf::Vector2f TransformableEntity::GetOrigin() const
	{
		if (GetParent().expired() == false)
		{
			auto owner = GetParent().lock();
			auto ownerTransform = dynamic_cast<TransformableEntity*>(owner.get());

			if (ownerTransform)
			{
				return ownerTransform->GetGlobalPosition();
			}
		}

		return {};
	}

	const sf::Transform& TransformableEntity::GetTransform() const
	{
		return sf::Transformable::getTransform();
	}

	const sf::Transform& TransformableEntity::GetInverseTransform() const
	{
		return sf::Transformable::getInverseTransform();
	}

	void TransformableEntity::UpdateGlobal()
	{
		if (GetParent().expired() == false)
		{
			auto owner = GetParent().lock();
			auto ownerTransform = dynamic_cast<TransformableEntity*>(owner.get());

			if (ownerTransform)
			{
				sf::Transformable::setOrigin(ownerTransform->GetGlobalPosition());

				sf::Transformable::setRotation(ownerTransform->GetGlobalRotation().AsDegree() + _localRotation.AsDegree());
				sf::Transformable::setPosition(ownerTransform->GetTransform().transformPoint(_localPosition));
				sf::Transformable::setScale(
					ownerTransform->GetLocalScale().x * _localScale.x,
					ownerTransform->GetLocalScale().y * _localScale.y
				);

				RefreshChildTransforms();
				OnPositionChanged();
				OnRotationChanged();
				OnScaleChanged();
				return;
			}
		}

		sf::Transformable::setOrigin({});
		sf::Transformable::setPosition(_localPosition);
		sf::Transformable::setRotation(_localRotation.AsDegree());
		sf::Transformable::setScale(_localScale.x, _localScale.y);

		RefreshChildTransforms();
		OnPositionChanged();
		OnRotationChanged();
	}

	void TransformableEntity::RefreshChildTransforms()
	{
		for(auto it : *this)
		{
			if (it)
			{
				auto chldTransformable = dynamic_cast<TransformableEntity*>(it.get());
				if (chldTransformable)
				{
					chldTransformable->UpdateGlobal();
				}
			}
		}
	}

	void TransformableEntity::SetLocalScale(sf::Vector2f scale)
	{
		_localScale = scale;

		UpdateGlobal();
		OnScaleChanged();
	}

	void TransformableEntity::SetGlobalScale(sf::Vector2f scale)
	{
		setScale(scale);

		if (GetParent().expired())
		{
			_localScale = scale;
		}
		else
		{
			auto owner = GetParent().lock();
			auto ownerTransform = dynamic_cast<TransformableEntity*>(owner.get());

			_localScale = scale - ownerTransform->GetGlobalScale();
		}

		RefreshChildTransforms();
		OnScaleChanged();
	}


	void TransformableEntity::SetLocalPosition(sf::Vector2f position)
	{
		_localPosition = position;

		UpdateGlobal();
		OnPositionChanged();
	}

	void TransformableEntity::SetGlobalPosition(sf::Vector2f position)
	{
		setPosition(position);
		setOrigin(GetOrigin());
		_localPosition = GetTransform().transformPoint(_localPosition);

		RefreshChildTransforms();
		OnPositionChanged();
	}

	void TransformableEntity::SetLocalRotation(Angle rotation)
	{
		_localRotation = rotation;

		UpdateGlobal();
		OnRotationChanged();
	}

	void TransformableEntity::SetGlobalRotation(Angle rotation)
	{
		setRotation(rotation.AsDegree());

		if (GetParent().expired())
		{
			_localRotation = rotation;
		}
		else
		{
			auto owner = GetParent().lock();
			auto ownerTransform = dynamic_cast<TransformableEntity*>(owner.get());

			_localRotation = rotation - ownerTransform->GetGlobalRotation();
		}

		RefreshChildTransforms();
		OnRotationChanged();
	}
}