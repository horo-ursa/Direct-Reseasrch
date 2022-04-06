#pragma once

#include "Physics.h"
#include "Component.h"

class Game;
class RenderObj;
class AssetManager;
class Physics;

class CollisionBox : public Component {
public:
	CollisionBox(RenderObj* obj, AssetManager* assetManager, Physics* physics);
	~CollisionBox();
	void LoadProperties(const rapidjson::Value& properties) override;
	Physics::AABB GetAABB() const;
	Physics::AABB box;
private:
	AssetManager* mAssetManager;
	Physics* mPhysics;
};