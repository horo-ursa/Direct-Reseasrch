#include "stdafx.h"
#include "character.h"
#include "skeleton.h"
#include "RenderObj.h"
#include "assetManager.h"
#include <codecvt>
#include <locale>
#include "Animation.h"
#include "SkinnedObj.h"

Character::Character(RenderObj* obj, AssetManager* assetManager) : Component(obj), mAssetManager(assetManager) {
	mSkinnedObj = dynamic_cast<SkinnedObj*>(obj);
}

void Character::LoadProperties(const rapidjson::Value& properties) {
	std::wstring skeletonFile;
	GetWStringFromJSON(properties, "skeleton", skeletonFile);
	mSkeleton = mAssetManager->LoadSkeleton(skeletonFile);
	const rapidjson::Value& AnimeData = properties["animations"];
	for (rapidjson::SizeType i = 0; i < AnimeData.Size(); i++) {
		std::string animeName = AnimeData[i][0].GetString();
		std::string animeFile = AnimeData[i][1].GetString();
		std::wstring fileName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(animeFile);
		Animation* anime = mAssetManager->LoadAnimation(fileName);
		mAnimationTable.insert(std::make_pair(animeName, anime));
	}
}

bool Character::SetAnim(const std::string& animName) {
	if (mAnimationTable.find(animName) != mAnimationTable.end()) {
		currAnimation = mAnimationTable.find(animName)->second;
		currAnimationTime = 0.0f;
		return true;
	}
	return false;
}

void Character::UpdateAnim(float deltaTime) {
	auto timeLimit = currAnimation->GetLength();
	currAnimationTime += deltaTime;
	if (timeLimit != 0) {
		if (currAnimationTime > timeLimit) {
			while (currAnimationTime > timeLimit) {
				currAnimationTime -= timeLimit; 
			}
		}
	}
	std::vector<Matrix4> outPose;
	std::vector<Matrix4> finalMatrix;
	currAnimation->GetGlobalPoseAtTime(outPose, mSkeleton, currAnimationTime);
	std::vector<Matrix4> invBindPos = mSkeleton->GetGlobalInvBindPoses();
	for (int i = 0; i < outPose.size(); i++) {
		//outPose[i] =  invBindPos[i];
		//finalMatrix.push_back(outPose[i] * invBindPos[i]);
		finalMatrix.push_back(invBindPos[i] * outPose[i]);
	}
	mSkinnedObj->BindtoConstant(finalMatrix);
}

void Character::Update(float deltaTime) {
	if (currAnimation == nullptr) {
		SetAnim("run");
	}
	UpdateAnim(deltaTime);
}