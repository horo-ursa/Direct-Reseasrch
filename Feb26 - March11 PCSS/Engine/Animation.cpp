#include "stdafx.h"
#include "Animation.h"
#include <fstream>
#include <sstream>
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include "jsonUtil.h"
#include "assetManager.h"
#include "bonetransform.h"

Animation::Animation() {

}

uint32_t Animation::GetNumBones() const
{
	return numBones;
}

uint32_t Animation::GetNumFrames() const 
{
	return numFrames;
}

float Animation::GetLength() const
{
	return lengthofAnimation;
}

bool Animation::Load(const WCHAR* fileName)
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
		DbgAssert(false, "Unable to open Animation file");
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();
	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itpanim" ||
		ver != 2)
	{
		DbgAssert(false, "Animation File Incorrect Version");
		return false;
	}

	const rapidjson::Value& sequenceData = doc["sequence"];
	if (!sequenceData.IsObject()) {
		DbgAssert(false, "Animation File Sequence Incorrect Version");
		return false;
	}
	numFrames = doc["sequence"]["frames"].GetInt();
	GetFloatFromJSON(doc["sequence"], "length", lengthofAnimation);
	numBones = doc["sequence"]["bonecount"].GetInt();
	//create animationData with zero length bonetransform
	for (int t = 0; t < numBones; t++) {
		std::vector<BoneTransform> temp;
		animationData.push_back(temp);
	}


	//load track data
	const rapidjson::Value& tracksData = doc["sequence"]["tracks"];
	if (!tracksData.IsArray() || tracksData.Size() < 1) {
		DbgAssert(false, "Animation Tracks Incorrect format");
		return false;
	}
	for (rapidjson::SizeType i = 0; i < tracksData.Size(); i++) {
		if (!tracksData[i].IsObject()) {
			DbgAssert(false, "tracks Invalid Data");
			return false;
		}
		//bone index
		int boneIndex = tracksData[i]["bone"].GetInt();
		//load transforms data
		std::vector<BoneTransform> bonetransVec;
		const rapidjson::Value& transformsData = doc["sequence"]["tracks"][i]["transforms"];

		if (!transformsData.IsArray() || transformsData.Size() < 1) {
			DbgAssert(false, "transformsData Invalid Data");
			return false;
		}

		for (rapidjson::SizeType j = 0; j < transformsData.Size(); j++) {
			BoneTransform temp;
			Quaternion quat;
			Vector3 vec;
			GetQuaternionFromJSON(transformsData[j], "rot", quat);
			GetVectorFromJSON(transformsData[j], "trans", vec);
			temp.mRotation = quat;
			temp.mTranslation = vec;
			bonetransVec.push_back(temp);
		}
		animationData[boneIndex] = bonetransVec;
	}
	return true;
}

Animation* Animation::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Animation* anime = new Animation();
	if (false == anime->Load(fileName)) {
		delete anime;
		return nullptr;
	}
	return anime;
}
void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const
{
	auto AllBone = inSkeleton->GetBones();
	auto timePerFrame = GetLength() / (GetNumFrames() - 1);
	int keyframe1 = std::floor(inTime / timePerFrame);
	int keyframe2 = std::ceil(inTime / timePerFrame);
	float f = (inTime / timePerFrame) - keyframe1;
	for (int i = 0; i < AllBone.size(); i++) {
		if (animationData[i].size() == 0) {//no animation
			outPoses.push_back(Matrix4::Identity);
		}
		else {
			//have parent
			int parentIndex = AllBone[i].parentIndex;
			//auto boneMatrix = animationData[i][keyframe].ToMatrix();
			BoneTransform bone = BoneTransform::Interpolate(animationData[i][keyframe1], animationData[i][keyframe2], f);
			if (parentIndex != -1) {
				//outPoses.push_back(animationData[i][keyframe].ToMatrix() * outPoses[parentIndex]);
				outPoses.push_back(bone.ToMatrix() * outPoses[parentIndex]);
			}
			else {
				//outPoses.push_back(animationData[i][keyframe].ToMatrix());
				outPoses.push_back(bone.ToMatrix());
			}
		}
	}
}
