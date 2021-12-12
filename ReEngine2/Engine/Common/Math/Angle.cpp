#include "Angle.h"
#include "Math.h"

RTTI_DEFINE_CLASS(Angle,
	{
		RTTI_PROPERTY(degree);
		//RTTI_PROPERTY_GETTER(degree, AsDegree);
		//RTTI_PROPERTY_SETTER(degree, AsDegree);
		RTTI_DRAWER(editor::AngleDrawer);
		//RTTI_SERIALIZER(rtti::TTypeSerializerCast<Angle>);
		//RTTI_DESCRIPTOR_NAME.SetTypeDrawer(RTTI_DESCRIPTOR_NAME.FindPropertyByNativeName(RE_STRINGIFY(degree)));
	});

const Angle Angle::full = Degree(360);
const Angle Angle::zero = Degree(0);

Angle::Angle()
	: degree(0)
{
}
Angle::Angle(float32 initialAngle)
	: degree(initialAngle)
{
}

Angle Radian(float32 angle)
{
	Angle a;
	a.degree = angle * 180.f / (float32)M_PI;
	return a;
}

Angle Degree(float32 angle)
{
	Angle a;
	a.degree = angle;
	return a;
}

Angle Angle::MinimalDiffirence(const Angle& other) const
{

	float32 diffirence = degree - other.degree;

	// remainder after division
	diffirence += -((int)(diffirence / 360)) * 360;

	if (diffirence > 180) diffirence -= 360;

	else if (diffirence < -180) diffirence += 360;

	/*debug*
	cout << "myAngle " << myAngle << "\n";
	cout << "forceAngle " << forceAngle << "\n";
	cout << "diffirence " << diffirence << "\n";
	/**/

	return Degree(diffirence);
}

Angle RandRange(Angle min, Angle max)
{
	return Degree(RandRange(min.AsDegree(), max.AsDegree()));
}

Angle Clamp(Angle value, Angle min, Angle max)
{
	return Degree(Clamp(value.AsDegree(), min.AsDegree(), max.AsDegree()));
}