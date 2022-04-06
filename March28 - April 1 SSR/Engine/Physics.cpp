#include "stdafx.h"
#include "Physics.h"
#include "CollisionBox.h"

struct TestAABB
{
	Physics::AABB a;
	Physics::AABB b;
	Physics::AABB overlap;
};

const TestAABB testAABB[] =
{
	{
		Physics::AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-110.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3(-100.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(110.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(100.0f, 10.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -110.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f)),
		Physics::AABB(Vector3(-10.0f, -100.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 110.0f, 10.0f)),
		Physics::AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 100.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, -110.0f), Vector3(10.0f, 10.0f, -90.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, -100.0f), Vector3(10.0f, 10.0f, -90.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 110.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 100.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-120.0f, -10.0f, -10.0f), Vector3(-110.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(110.0f, -10.0f, -10.0f), Vector3(120.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -120.0f, -10.0f), Vector3(10.0f, -110.0f, 10.0f)),
		Physics::AABB(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, 110.0f, -10.0f), Vector3(10.0f, 120.0f, 10.0f)),
		Physics::AABB(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, -120.0f), Vector3(10.0f, 10.0f, -110.0f)),
		Physics::AABB(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, 110.0f), Vector3(10.0f, 10.0f, 120.0f)),
		Physics::AABB(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f))
	},
};

struct TestSegment
{
	Physics::AABB box;
	Physics::LineSegment segment;
	bool hit;
	Vector3 point;
};
const TestSegment testSegment[] =
{
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(-110.0f, 0.0f, 0.0f), Vector3(-90.0f, 0.0f, 0.0f)),
		true, Vector3(-100.0f, 0.0f, 0.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, -110.0f, 0.0f), Vector3(0.0f, -90.0f, 0.0f)),
		true, Vector3(0.0f, -100.0f, 0.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 0.0f, -110.0f), Vector3(0.0f, 0.0f, -90.0f)),
		true, Vector3(0.0f, 0.0f, -100.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(110.0f, 0.0f, 0.0f), Vector3(90.0f, 0.0f, 0.0f)),
		true, Vector3(100.0f, 0.0f, 0.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 110.0f, 0.0f), Vector3(0.0f, 90.0f, 0.0f)),
		true, Vector3(0.0f, 100.0f, 0.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 0.0f, 110.0f), Vector3(0.0f, 0.0f, 90.0f)),
		true, Vector3(0.0f, 0.0f, 100.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(-120.0f, 0.0f, 0.0f), Vector3(-110.0f, 0.0f, 0.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, -120.0f, 0.0f), Vector3(0.0f, -110.0f, 0.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 0.0f, -120.0f), Vector3(0.0f, 0.0f, -110.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(120.0f, 0.0f, 0.0f), Vector3(110.0f, 0.0f, 0.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 120.0f, 0.0f), Vector3(0.0f, 110.0f, 0.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 0.0f, 120.0f), Vector3(0.0f, 0.0f, 110.0f)),
		false, Vector3::Zero
	},
};

bool Physics::UnitTest() {
	bool isOk = true;
	//AABB intersect AABB
	for (int i = 0; i < ARRAY_SIZE(testAABB); i++) {
		auto temp = AABB();
		auto overlap = testAABB[i].overlap;
		auto a = testAABB[i].a;
		auto b = testAABB[i].b;
		bool interCondition = Intersect(a, b, &temp);
		if (!interCondition) {
			isOk &= (overlap.min.x > overlap.max.x);
		}
		else {
			bool minEqual = overlap.min.isCloseEnuf(temp.min);
			bool maxEqual = overlap.max.isCloseEnuf(temp.max);
			isOk &= (minEqual && maxEqual);
		}
	}
	//LineSegment intersect AABB
	for (int i = 0; i < ARRAY_SIZE(testSegment); i++) {
		auto hitPoint = Vector3(0.0f, 0.0f, 0.0f);
		bool hit = Intersect(testSegment[i].segment, testSegment[i].box, &hitPoint);
		if (!hit) {
			isOk &= true ? testSegment[i].hit == false : true;
			isOk &= hitPoint.isCloseEnuf(Vector3::Zero);
		}
		else {
			isOk &= (testSegment[i].hit && hitPoint.isCloseEnuf(testSegment[i].point));
		}
		hitPoint = Vector3(0.0f, 0.0f, 0.0f);
	}
	return isOk;
}

bool Physics::Intersect(const AABB& a, const AABB& b, AABB* pOverlap)
{
	if ((a.min.x > b.max.x) || (a.max.x < b.min.x) ||
		(a.min.y > b.max.y) || (a.max.y < b.min.y) ||
		(a.min.z > b.max.z) || (a.max.z < b.min.z))
	{
		return false;
	}
	else {
		pOverlap->min = Vector3(max(a.min.x, b.min.x), max(a.min.y, b.min.y), max(a.min.z, b.min.z));
		pOverlap->max = Vector3(min(a.max.x, b.max.x), min(a.max.y, b.max.y), min(a.max.z, b.max.z));
		return true;
	}
}


bool Physics::Intersect(const LineSegment& segment, const AABB& box, Vector3* pHitPoint)
{
	Vector3 dir = segment.endPoint - segment.startPoint;
	auto start = segment.startPoint;
	auto end = segment.endPoint;
	float tmin = 0.0f;
	float tmax = 1.0f;
	//if ray is parallel to slab, reture false
	float* m_dir = reinterpret_cast<float*>(&dir.x);
	float* m_start = reinterpret_cast<float*>(&start.x);
	float* m_min = reinterpret_cast<float*>(const_cast<float*>(&(box.min.x)));
	float* m_max = reinterpret_cast<float*>(const_cast<float*>(&(box.max.x)));

	for (int i = 0; i < 3; i++) {
		if (Math::IsCloseEnuf(m_dir[i], 0)) {
			if (m_start[i] < m_min[i] || m_start[i] > m_max[i]) { return false; }
		}
		else {
			float ood = 1.0f / m_dir[i];
			float t1 = (m_min[i] - m_start[i]) * ood;
			float t2 = (m_max[i] - m_start[i]) * ood;

			if (t1 > t2) { std::swap(t1, t2); }
			tmin = max(tmin, t1);
			tmax = min(tmax, t2);
			if (tmin > tmax) { return false; }
		}
	}

	Vector3 hitp = start + dir * tmin;
	pHitPoint->x = hitp.x; pHitPoint->y = hitp.y; pHitPoint->z = hitp.z;
	return true;
}

Physics::Physics() {

}

void Physics::AddObj(const CollisionBox* pObj)
{
	mCollisionBox.push_back(pObj);
}
void Physics::RemoveObj(const CollisionBox* pObj) {
	//if (std::find(mCollisionBox.begin(), mCollisionBox.end(), pObj) != mCollisionBox.end()) {}
	for (int i = 0; i < mCollisionBox.size(); i++) {
		if (mCollisionBox[i] == pObj) {
			mCollisionBox[i] = NULL;
		}
	}
}

bool Physics::RayCast(const LineSegment& segment, Vector3* pHitPoint)
{
	std::vector<Vector3> hitPoints;
	for (int i = 0; i < mCollisionBox.size(); i++) {
		if (mCollisionBox[i] != NULL) {
			auto aabb = mCollisionBox[i]->GetAABB();
			auto hitp = Vector3();
			if (Intersect(segment, aabb, &hitp)) {
				hitPoints.push_back(hitp);
			}
		}
	}
	if (hitPoints.size() == 0) {
		return false;
	}
	float length = (std::numeric_limits<float>::max)();
	Vector3 returnVal;
	auto begin = segment.startPoint;
	for (auto& i : hitPoints) {
		if ((i - begin).Length() < length) {
			length = (i - begin).Length();
			returnVal = i;
		}
	}
	pHitPoint->x = returnVal.x; pHitPoint->y = returnVal.y; pHitPoint->z = returnVal.z;
}