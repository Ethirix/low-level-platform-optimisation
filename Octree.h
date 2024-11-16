#pragma once
#include <list>

#include "ColliderObject.h"
#include "Vector3.h"

class Octree
{
	//https://iq.opengenus.org/octree/
	//bounding box
	//array of subsections[octrees still] 8 of them
public:
	Octree(Vector3 pos, float halfSize, Octree* parent)
		: Parent(parent), _position(pos), _halfSize(halfSize)
	{

	}

	Octree* Parent;
	Octree* Children[8];

	std::list<ColliderObject*> Colliders;

	Vector3 GetPosition() { return _position; }
	float GetHalfSize() { return _halfSize; }
private:
	Vector3 _position;
	float _halfSize;
};
