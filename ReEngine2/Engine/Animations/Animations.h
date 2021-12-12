#pragma once
#include "../Common/Math/Math.h"
#include "../Reflection/Reflection.h"

/// WIP

using BoneId = uint16;
using Frame = float32;

struct FrameRange
{
	RTTI_DECLARE_CLASS(FrameRange);

	Frame begin;
	Frame end;

	bool InRange(Frame f) const
	{
		return begin > f && f < end;
	}
};

struct BoneStat
{
	RTTI_DECLARE_CLASS(BoneStat);
	Angle angle = Degree(0);
	float length = 1;
};

struct Bone
{
	RTTI_DECLARE_CLASS(Bone);

	BoneStat boneStat;
	sf::Vector2f position = {0,0};
	Angle resultingAngle = Degree(0);
	BoneId parentId = 0;
};

class SkeletalMeshBones
{
	RTTI_DECLARE_CLASS(SkeletalMeshBones);
public:
	std::vector<Bone> bones;
	Bone bone;

	bool HasValidBoneOrder()
	{
		for (int i = 1; i < bones.size(); ++i)
		{
			auto& it = bones[i];
			if (it.parentId >= i)
			{
				return false;
			}
		}
		return true;
	}

	void UpdatePositions()
	{
		for (int i = 1; i < bones.size(); ++i)
		{
			auto& it = bones[i];
			auto& parent = bones[it.parentId];

			it.resultingAngle = it.boneStat.angle + parent.resultingAngle;
			auto v = it.resultingAngle.GetVersor() * it.boneStat.length;
			it.position.x = parent.position.x + v.x;
			it.position.y = parent.position.y + v.y;

			//it.resultingAngle.Normalize();
		}
	}
};



class SkeletalMeshSpriteDrawer
{
public:
	void Draw(sf::RenderTarget& renderTarget, const SkeletalMeshBones& mesh)
	{
		for (int i = 0; i < mesh.bones.size(); ++i)
		{
			auto pos = mesh.bones[i].position;
			auto angle = mesh.bones[i].resultingAngle.AsDegree();
			_sprites[i].setPosition({pos.x, pos.y});
			_sprites[i].setRotation(angle);

			renderTarget.draw(_sprites[i]);
		}
	}

	void BindTexture(size_t index, const sf::Texture& ts)
	{
		_sprites[index].setTexture(ts, true);
	}

	void BindTexture(size_t index, const sf::Texture& ts, const sf::Rect<int>& rect)
	{
		_sprites[index].setTexture(ts, false);
		_sprites[index].setTextureRect(rect);
	}


	void SetColor(size_t index, sf::Color cl)
	{
		_sprites[index].setColor(cl);
	}
	void SetScale(size_t index, sf::Vector2f scale)
	{
		_sprites[index].setScale(scale);
	}

private:
	std::vector<sf::Sprite> _sprites;
};

///////////////////////////////////////

/**
class KeyframeBoneList
{
public:
	KeyframeBoneList(BoneId boneId)
		: _boneId(boneId) {}

public:

	BoneStat Sample(Frame frame) const
	{
		_lastRange = GetRangeId(frame);

		auto& first = _boneStats[_lastRange.first];
		auto& second = _boneStats[_lastRange.second];

		float percentage = InverseLerp(
			_frames[_lastRange.first],
			_frames[_lastRange.second],
			frame
		);

		return {
			Lerp(first.angle,  second.angle,  percentage),
			Lerp(first.length, second.length, percentage)
		};
	}


public:

	void AddKeyframe(Frame frame, BoneStat boneStat)
	{
		int id = -1;
		Frame lastFrame = -999999;
		for (size_t i = 0; i < _frames.size(); ++i)
		{
			if (_frames[i] > lastFrame)
			{
				id = i;
				lastFrame = _frames[i];
			}
			else
				break;
		}

		_frames.insert(_frames.begin() + id + 1, frame);
		_boneStats.insert(_boneStats.begin() + id + 1, boneStat);
	}

	BoneId GetBoneId() const { return _boneId; }
	void SetBoneId(BoneId boneId) { _boneId = boneId; }


	Frame GetLastFrame() const
	{
		return _frames.back();
	}
	Frame GetFirstFrame() const
	{
		return _frames.front();
	}

	// TODO add batch initialization

private:
	std::pair<size_t, size_t> GetRangeId(Frame frame) const
	{
		Frame previous = _frames[_lastRange.first];
		if (frame > previous)
		{
			for (size_t i = _lastRange.first + 1; i < _frames.size(); ++i)
			{
				Frame current = _frames[i];
				if (frame < current)
				{
					return std::make_pair(i - 1, i);
				}
			}

			return std::make_pair(_frames.size() - 1, _frames.size() - 1);
		}
		else
		{
			for (size_t i = _lastRange.first - 1; i >= 0; --i)
			{
				Frame current = _frames[i];
				if (current > frame)
				{
					return std::make_pair(i, i + 1);
				}
			}

			return std::make_pair(0, 0);
		}
	}

	std::vector<BoneStat> _boneStats;
	std::vector<Frame> _frames;

	BoneId _boneId;

	// optimalization only, no visible state outside
	mutable std::pair<size_t, size_t> _lastRange;
};

class AnimationClip
{
public:

	void Sample(Frame frame, SkeletalMesh mesh) const
	{
		for (auto& it : _modificatedBones)
		{
			auto& bone = mesh.bones[it.GetBoneId()];
			bone.boneStat = it.Sample(frame);
		}
		mesh.UpdatePositions();
	}

	Frame GetLastFrame() const
	{
		Frame maxFrame = _modificatedBones[0].GetLastFrame();
		for (int i = 0; i < _modificatedBones.size(); ++i)
		{
			maxFrame = std::max(maxFrame, _modificatedBones[i].GetLastFrame());
		}

		return maxFrame;
	}
	Frame GetFirstFrame() const
	{
		Frame minFrame = _modificatedBones[0].GetLastFrame();
		for (int i = 0; i < _modificatedBones.size(); ++i)
		{
			minFrame = std::min(minFrame, _modificatedBones[i].GetFirstFrame());
		}

		return minFrame;
	}

	FrameRange GetFrameRange() const
	{
		return { GetFirstFrame(), GetLastFrame() };
	}

	// TODO loading/saving data
	// TODO serialization library?

private:
	std::vector<KeyframeBoneList> _modificatedBones;
};

class AnimationPlayerBase
{
public:
	virtual void ResetPlay(FrameRange frameRange) { }

	virtual bool Finished() = 0;

	virtual Frame AdvanceFrame() = 0;

};

class AnimationPlayerForward : AnimationPlayerBase
{
public:
	AnimationPlayerForward(Frame speed)
		: _speed(speed)
	{
	}

	virtual void ResetPlay(FrameRange frameRange) override
	{
		_frameRange = frameRange;
		_currentFrame = frameRange.begin;
	}

	virtual bool Finished() override
	{
		return _frameRange.InRange(_currentFrame);
	}

	virtual Frame AdvanceFrame() override
	{
		_currentFrame += _speed;
		return _currentFrame;
	}

private:
	FrameRange _frameRange;
	Frame _currentFrame;
	Frame _speed;

};

/**/

////// TODO
// Serialization, initialization
// Test it out
// animation blending

// Debug Mesh Drawing
// Actual Mesh Drawing
// SkeletalMeshActor (btw, change name "Actor")

// Mesh Attachements (as Actors)
// Animation controller - how states change under certain conditions

// Editors
//		Mesh (creating skeleton, attaching sprites and actors)
//		Animation
//		Animation Controller

// Animation Clip Asset
// Mesh Asset
// Animation Controller Asset

// Interaction with physics (when physics will be implemeneted )

// Callbacks on specific frame or Frame Range

// FrameRange class instead of std::pair

// think about ea stl

// Texture manager - storage, loading removing
// Resource manager in general?

// pass delta time to update (possibly also draw? maybe some class with such settings)