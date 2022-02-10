#pragma once
#include <vector>
#include "engineMath.h"

class BoneTransform;
class AssetManager;
class Skeleton;

class Animation {
public:
	Animation();
	uint32_t GetNumBones() const;
	uint32_t GetNumFrames() const;
	float GetLength() const;
	static Animation* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool Load(const WCHAR* fileName);
	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const;
private:
	uint32_t numBones;
	uint32_t numFrames;
	float lengthofAnimation;
	std::vector<std::vector<BoneTransform>> animationData;
};