#include "Animations.h"

/// WIP

RTTI_DEFINE_CLASS(FrameRange,
	{
		RTTI_PROPERTY(begin);
		RTTI_PROPERTY(end);
	});

RTTI_DEFINE_CLASS(BoneStat,
	{
		RTTI_PROPERTY(angle);
		RTTI_PROPERTY(length);
	});

RTTI_DEFINE_CLASS(Bone,
	{
		RTTI_PROPERTY(boneStat);
		//RTTI_PROPERTY(position);
		//RTTI_PROPERTY(resultingAngle);
		RTTI_PROPERTY(parentId);
	});

RTTI_DEFINE_CLASS(SkeletalMeshBones,
	{
		RTTI_PROPERTY(bones);
	});
