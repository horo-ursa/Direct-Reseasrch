#include "stdafx.h"
#include "CollisionBox.h"
#include "assetManager.h"
#include "RenderObj.h"
#include "Physics.h"

CollisionBox::CollisionBox(RenderObj* obj, AssetManager* assetManager, Physics* physics) : 
	Component(obj), mAssetManager(assetManager), mPhysics(physics)
{
	
}

CollisionBox::~CollisionBox()
{
	mPhysics->RemoveObj(this);
}

void CollisionBox::LoadProperties(const rapidjson::Value& properties)
{
	Vector3 min;
	Vector3 max;
	GetVectorFromJSON(properties, "min", min);
	GetVectorFromJSON(properties, "max", max);
	box.min = min;
	box.max = max;
	mPhysics->AddObj(this);
}

Physics::AABB CollisionBox::GetAABB() const
{
	auto scale = obj->match.c_modelToWorld.GetScale();
	auto translation = obj->match.c_modelToWorld.GetTranslation();
	auto minVec = box.min * scale.x;
	minVec += translation;
	auto maxVec = box.max * scale.x;
	maxVec += translation;
	return Physics::AABB(minVec, maxVec);
}