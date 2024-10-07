#pragma once
class Octree
{
	//https://iq.opengenus.org/octree/
	//bounding box
	//array of subsections[octrees still] 8 of them

	Octree* _children = new Octree[8];
};