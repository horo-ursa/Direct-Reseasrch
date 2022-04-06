#pragma once
#include "engineMath.h"
#include <vector>

class CollisionBox;

class Physics {
public:
	class AABB {
	public:
		AABB() {}
		AABB(Vector3 min, Vector3 max) {
			this->min = min;
			this->max = max;
		}
		Vector3 min;
		Vector3 max;
	};

	class LineSegment {
	public:
		LineSegment(Vector3 start, Vector3 end) {
			startPoint = start;
			endPoint = end;
		}
		Vector3 startPoint;
		Vector3 endPoint;
	};
	static bool Intersect(const AABB& a, const AABB& b, AABB* pOverlap = nullptr); 
	static bool Intersect(const LineSegment& segment, const AABB& box, Vector3* pHitPoint = nullptr); 
	static bool UnitTest();

	Physics();
	void AddObj(const CollisionBox* pObj);
	void RemoveObj(const CollisionBox* pObj);

	bool RayCast(const LineSegment& segment, Vector3* pHitPoint = nullptr);
private:
	std::vector<const CollisionBox*> mCollisionBox;
};