#pragma once
#include <map>
#include "Component.h"

class SkinnedObj;
class Skeleton;
class Animation;
class RenderObj;
class AssetManager;

class Character : public Component {
public:
	Character(RenderObj* obj, AssetManager* assetManager);
	void LoadProperties(const rapidjson::Value& properties) override;
	bool SetAnim(const std::string& animName);
	void UpdateAnim(float deltaTime);
	void Update(float deltaTime) override;
protected:
	SkinnedObj* mSkinnedObj;
	Skeleton* mSkeleton;
	std::unordered_map<std::string, const Animation*> mAnimationTable;
	const Animation* currAnimation;
	float currAnimationTime = 0.0f;
	AssetManager* mAssetManager;
};