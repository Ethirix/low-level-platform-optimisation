#include "Octree.h"

#include <thread>

Octree::~Octree()
{
	delete Children[0];
	delete Children[1];
	delete Children[2];
	delete Children[3];
	delete Children[4];
	delete Children[5];
	delete Children[6];
	delete Children[7];
}

void Octree::SplitNode(unsigned depth)
{
	//Once again, slower than 
#if THREAD_OCTREE_SPLIT
	std::thread t{[this](Octree* Children[8], Vector3 _position, float _halfSize, unsigned depth) -> void {
#endif

	// Initialise child nodes
	float quarterSize = _halfSize / 2.0f;
	Children[0] = new Octree({_position.X + quarterSize, _position.Y + quarterSize, _position.Z + quarterSize},
	                         quarterSize, this);
	Children[1] = new Octree({_position.X + quarterSize, _position.Y + quarterSize, _position.Z - quarterSize},
	                         quarterSize, this);
	Children[2] = new Octree({_position.X + quarterSize, _position.Y - quarterSize, _position.Z + quarterSize},
	                         quarterSize, this);
	Children[3] = new Octree({_position.X - quarterSize, _position.Y + quarterSize, _position.Z + quarterSize},
	                         quarterSize, this);
	Children[4] = new Octree({_position.X + quarterSize, _position.Y - quarterSize, _position.Z - quarterSize},
	                         quarterSize, this);
	Children[5] = new Octree({_position.X - quarterSize, _position.Y - quarterSize, _position.Z + quarterSize},
	                         quarterSize, this);
	Children[6] = new Octree({_position.X - quarterSize, _position.Y + quarterSize, _position.Z - quarterSize},
	                         quarterSize, this);
	Children[7] = new Octree({_position.X - quarterSize, _position.Y - quarterSize, _position.Z - quarterSize},
	                         quarterSize, this);

	if (depth > 1)
	{
		// Loop unwinding
		Children[0]->SplitNode(depth - 1);
		Children[1]->SplitNode(depth - 1);
		Children[2]->SplitNode(depth - 1);
		Children[3]->SplitNode(depth - 1);
		Children[4]->SplitNode(depth - 1);
		Children[5]->SplitNode(depth - 1);
		Children[6]->SplitNode(depth - 1);
		Children[7]->SplitNode(depth - 1);
	}
#if THREAD_OCTREE_SPLIT
		}, Children, _position, _halfSize, depth
	};

	t.join();
#endif
}

void Octree::AddCollider(ColliderObject* collider)
{
	if ((collider->Position.X - collider->Size.X / 2 <= _position.X + _halfSize && collider->Position.X + collider->Size
			.X / 2
			>= _position.X - _halfSize) &&
		(collider->Position.Y - collider->Size.Y / 2 <= _position.Y + _halfSize && collider->Position.Y + collider->Size
			.Y / 2
			>= _position.Y - _halfSize) &&
		(collider->Position.Z - collider->Size.Z / 2 <= _position.Z + _halfSize && collider->Position.Z + collider->Size
			.Z / 2
			>= _position.Z - _halfSize))
	{
		if (Children[0])
		{
			Children[0]->AddCollider(collider);
			Children[1]->AddCollider(collider);
			Children[2]->AddCollider(collider);
			Children[3]->AddCollider(collider);
			Children[4]->AddCollider(collider);
			Children[5]->AddCollider(collider);
			Children[6]->AddCollider(collider);
			Children[7]->AddCollider(collider);
		}
		else
		{
			Colliders.push_back(collider);
		}
	}
}

void Octree::RunPhysics(const float deltaTime)
{
	std::thread physicsThread = {};

	if (Colliders.size() > MINIMUM_COLLIDER_COUNT_FOR_THREADING)
	{
		 physicsThread = std::thread{ [this, deltaTime] () -> void {
			for (ColliderObject* collider : Colliders)
			{
				collider->Update(&Colliders, deltaTime);
			}
		}};
	}
	else if (!Colliders.empty())
	{
		for (ColliderObject* collider : Colliders)
		{
			collider->Update(&Colliders, deltaTime);
		}
	}

	if (Children[0] == nullptr)
	{
		if (physicsThread.joinable())
			physicsThread.join();
		return;
	}

	Children[0]->RunPhysics(deltaTime);
	Children[1]->RunPhysics(deltaTime);
	Children[2]->RunPhysics(deltaTime);
	Children[3]->RunPhysics(deltaTime);
	Children[4]->RunPhysics(deltaTime);
	Children[5]->RunPhysics(deltaTime);
	Children[6]->RunPhysics(deltaTime);
	Children[7]->RunPhysics(deltaTime);

	if (physicsThread.joinable())
			physicsThread.join();
}

void Octree::ClearColliders()
{
	if (!Colliders.empty())
		Colliders.clear();

	if (!Children[0])
		return;

	Children[0]->ClearColliders();
	Children[1]->ClearColliders();
	Children[2]->ClearColliders();
	Children[3]->ClearColliders();
	Children[4]->ClearColliders();
	Children[5]->ClearColliders();
	Children[6]->ClearColliders();
	Children[7]->ClearColliders();
}

void Octree::SetHalfSize(float halfSize)
{
	_halfSize = halfSize;
}