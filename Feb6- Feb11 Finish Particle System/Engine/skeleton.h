#pragma once
#include "engineMath.h"
#include  <string>
#include <vector>


class BoneTransform;
class AssetManager;

class Skeleton {
	
	struct Bone {
		BoneTransform* localBindPose;
		std::string name;
		int parentIndex;
	};

public:
	size_t GetNumBones() const;
	const Bone& GetBone(size_t idx) const;
	const std::vector<Bone>& GetBones() const;
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const;
	static Skeleton* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool Load(const WCHAR* fileName);
private:
	std::vector<Bone> mBones;
	std::vector<Matrix4> globalInverseBindPoseMatrix;
	void ComputeGlobalInvBindPose();
};