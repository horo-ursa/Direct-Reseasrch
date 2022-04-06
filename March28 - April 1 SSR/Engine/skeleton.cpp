#include "stdafx.h"
#include "skeleton.h"
#include "bonetransform.h"
#include "assetManager.h"
#include <fstream>
#include <sstream>
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include "jsonUtil.h"

void Skeleton::ComputeGlobalInvBindPose()
{
	for (int i = 0; i < mBones.size(); i++) {
		if (mBones[i].parentIndex != -1) {
			auto bindPose = mBones[i].localBindPose->ToMatrix() * globalInverseBindPoseMatrix[mBones[i].parentIndex];
			globalInverseBindPoseMatrix.push_back(bindPose);
		}
		else {
			globalInverseBindPoseMatrix.push_back(mBones[i].localBindPose->ToMatrix());
		}
	}

	for (int i = 0; i < mBones.size(); i++) {
		globalInverseBindPoseMatrix[i].Invert();
	}
}

size_t Skeleton::GetNumBones() const
{
	return mBones.size();
}

const Skeleton::Bone& Skeleton::GetBone(size_t idx) const
{
	return mBones[idx];
}

const std::vector<Skeleton::Bone>& Skeleton::GetBones() const
{
	return mBones;
}

const std::vector<Matrix4>& Skeleton::GetGlobalInvBindPoses() const
{
	return globalInverseBindPoseMatrix;
}

bool Skeleton::Load(const WCHAR* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		DbgAssert(false, "Unable to open Skeleton file");
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();
	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itpskel" ||
		ver != 1)
	{
		DbgAssert(false, "Skeleton File Incorrect Version");
		return false;
	}
	
	//load in bone data
	const rapidjson::Value& boneData = doc["bones"];
	if (!boneData.IsArray() || boneData.Size() < 1) {
		DbgAssert(false, "bone data error");
		return false;
	}
	for (rapidjson::SizeType i = 0; i < boneData.Size(); i++) {
		if (!boneData[i].IsObject()) {
			DbgAssert(false, "Bone Invalid Data");
			return false;
		}
		Skeleton::Bone tempBone;
		tempBone.localBindPose = new BoneTransform();
		tempBone.name = boneData[i]["name"].GetString();
		tempBone.parentIndex = boneData[i]["parent"].GetInt();
		Quaternion quat;
		Vector3 vec;
		GetQuaternionFromJSON(boneData[i]["bindpose"], "rot", quat);
		GetVectorFromJSON(boneData[i]["bindpose"], "trans", vec);
		tempBone.localBindPose->mTranslation = vec;
		tempBone.localBindPose->mRotation = quat;
		mBones.push_back(tempBone);
	}

	ComputeGlobalInvBindPose();
	return true;
}

Skeleton* Skeleton::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Skeleton* skeleton = new Skeleton();
	if (false == skeleton->Load(fileName))
	{
		delete skeleton;
		return nullptr;
	}
	return skeleton;
}
