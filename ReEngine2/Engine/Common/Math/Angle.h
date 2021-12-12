#pragma once
#include "../Settings.h"
#include "../../Reflection/Reflection.h"
#include "glm/vec2.hpp"

class Angle;
Angle Radian(float32 angle);
Angle Degree(float32 angle);

class Angle
{
	// TODO non polimorfic class body
	RTTI_DECLARE_CLASS(Angle);
	Angle(float32 initialDegree);
public:
	Angle();

	static const Angle full;
	static const Angle zero;

	friend Angle Radian(float32 angle);
	friend Angle Degree(float32 angle);


	inline float32 AsRadian() const
	{
		return (float32)(degree / 180.f * M_PI);
	}
	inline float32 AsDegree() const
	{
		return degree;
	}

	inline float Sin() const
	{
		return ::sin(AsRadian());
	}
	inline float Cos() const
	{
		return ::cos(AsRadian());
	}
	inline glm::vec2 GetVersor() const
	{
		return { -Sin(), Cos() };
	}

	Angle MinimalDiffirence(const Angle& other) const;
	///
	Angle GetNormalized() const
	{
		float32 diff = degree - (int)(degree / 360);
		return Degree((float32)((int)degree % 360) + diff);
	}
	void Normalize()
	{
		float32 diff = degree - (int)(degree / 360);
		degree = (float32)((int)degree % 360) + diff;
	}

	inline bool operator>(const Angle& other) const
	{
		return degree > other.degree;
	}
	inline bool operator>=(const Angle& other) const
	{
		return degree >= other.degree;
	}
	inline bool operator<(const Angle& other) const
	{
		return degree < other.degree;
	}
	inline bool operator<=(const Angle& other) const
	{
		return degree <= other.degree;
	}

	inline Angle operator+(const Angle& other) const
	{
		return Degree(AsDegree() + other.AsDegree());
	}
	inline Angle operator-(const Angle& other) const
	{
		return Degree(AsDegree() - other.AsDegree());
	}
	inline Angle operator*(const Angle& other) const
	{
		return Degree(AsDegree() * other.AsDegree());
	}
	inline Angle operator/(const Angle& other) const
	{
		assert(other.degree != 0);
		return Degree(AsDegree() / other.AsDegree());
	}


	inline Angle operator*(float32 other) const
	{
		return Degree(AsDegree() * other);
	}
	inline Angle operator/(float32 other) const
	{
		assert(other != 0);
		return Degree(AsDegree() / other);
	}

	inline void operator+=(const Angle& other)
	{
		degree += other.AsDegree();
	}
	inline void operator-=(const Angle& other)
	{
		degree -= other.AsDegree();
	}
	inline void operator*=(const Angle& other)
	{
		degree *= other.AsDegree();
	}
	inline void operator/=(const Angle& other)
	{
		assert(other.degree != 0);
		degree /= other.AsDegree();
	}

	inline void operator*=(float32 other)
	{
		degree *= other;
	}
	inline void operator/=(float32 other)
	{
		assert(other != 0);
		degree /= other;
	}

	inline Angle operator-() const
	{
		return Degree(-degree);
	}

	inline bool operator==(Angle other) const
	{
		return degree == other.degree;
	}
	inline bool operator!=(Angle other) const
	{
		return degree != other.degree;
	}

private:
	// angle in degrees
	float32 degree;
};

Angle RandRange(Angle min, Angle max);
Angle Clamp(Angle value, Angle min, Angle max);

namespace editor
{
	class AngleDrawer : public FloatDrawer<float>
	{
	public:
		AngleDrawer(float dragSpeed = 0.1f)
			:dragSpeed(dragSpeed)
		{

		}

		float dragSpeed;

		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			Angle angle = *((Angle*)context.objectStart);
			float varriable = angle.AsDegree();
			if (context.property != nullptr && context.property->GetOverridePropertyGetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.result = &varriable;
				context.property->GetOverridePropertyGetter()->Invoke(functionContext);
			}

			bool changed = DrawFloat<float>(context.GetDisplayName(), context.imGuiId, &varriable, (float)context.offset, dragSpeed, afterLabelCall);
			
			//if (changed)
			{
				if (context.property != nullptr && context.property->GetOverridePropertySetter())
				{
					rtti::FunctionCallingContext functionContext;
					functionContext.object = context.property->GetOwnerAddres(context.objectStart);
					functionContext.argumentPointers.push_back(&varriable);
					context.property->GetOverridePropertySetter()->Invoke(functionContext);
				}

				*((Angle*)context.objectStart) = Degree(varriable);
				
			}
			return changed;
		}
	};
}
