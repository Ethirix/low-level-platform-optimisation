#pragma once
#include <list>

#include "ColliderObject.h"
#include "Vector3.h"

#define THREAD_OCTREE_SPLIT 0

class Octree
{
public:
	Octree(Vector3 position, float halfSize, Octree* parent)
		: Parent(parent), _position(position), _halfSize(halfSize)
	{
	}

	~Octree();

	void SplitNode(unsigned depth);
	void AddCollider(ColliderObject* collider);
	void RunPhysics(float deltaTime);
	void ClearColliders();

	Octree* Parent;
	Octree* Children[8];

	std::list<ColliderObject*> Colliders{};

	Vector3 GetPosition() { return _position; }
	float GetHalfSize() { return _halfSize; }

	inline void SetHalfSize(float halfSize);

private:
	Vector3 _position;
	float _halfSize;
};
