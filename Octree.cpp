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


void Octree::SetHalfSize(float halfSize)
{
	_halfSize = halfSize;
}
