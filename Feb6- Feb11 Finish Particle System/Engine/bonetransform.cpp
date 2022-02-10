#include "stdafx.h"
#include "bonetransform.h"

Matrix4 BoneTransform::ToMatrix() const {
	return Matrix4::CreateFromQuaternion(mRotation)* Matrix4::CreateTranslation(mTranslation);
	//return Matrix4::CreateTranslation(mTranslation) * Matrix4::CreateFromQuaternion(mRotation);
};

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	Vector3 lerpVec = Lerp(a.mTranslation, b.mTranslation, f);
	Quaternion slerpQua = Slerp(a.mRotation, b.mRotation, f);
	BoneTransform bone;
	bone.mRotation = slerpQua;
	bone.mTranslation = lerpVec;
	return bone;
}