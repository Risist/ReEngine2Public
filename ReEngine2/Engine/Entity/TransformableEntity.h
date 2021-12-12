#include "CompoundEntity.h"
#include "../Common/Math/Math.h"

namespace entity
{
	class TransformableEntity : public CompoundEntity, private sf::Transformable
	{
		RTTI_DECLARE_ASSET(TransformableEntity)
	public:
		TransformableEntity();

		void SetLocalRotation(Angle rotation);
		Angle GetLocalRotation() const
		{
			return _localRotation;
		}

		void SetGlobalRotation(Angle rotation);
		Angle GetGlobalRotation() const
		{
			return Degree(sf::Transformable::getRotation());
		}
		// TODO set global rotation

		void SetLocalPosition(sf::Vector2f position);
		sf::Vector2f GetLocalPosition() const
		{
			return _localPosition;
		}

		void SetGlobalPosition(sf::Vector2f position);
		sf::Vector2f GetGlobalPosition() const
		{
			return sf::Transformable::getPosition();
		}

		void SetLocalScale(sf::Vector2f scale);
		sf::Vector2f GetLocalScale() const
		{
			return _localScale;
		}

		void SetGlobalScale(sf::Vector2f scale);
		sf::Vector2f GetGlobalScale() const
		{
			return sf::Transformable::getScale();
		}

		const sf::Transform& GetTransform() const;
		const sf::Transform& GetInverseTransform() const;


	protected:
		// callbacks for reacting on parameters change
		virtual void OnPositionChanged() {}
		virtual void OnRotationChanged() {}
		virtual void OnScaleChanged() {}

	public:
		virtual bool CanBeAChild(const rtti::ClassType& potentialChildType) const;

	private:

		sf::Vector2f _localScale;
		sf::Vector2f _localPosition;
		Angle _localRotation;

		void UpdateGlobal();
		void RefreshChildTransforms();

		sf::Vector2f GetOrigin() const;
	};


}