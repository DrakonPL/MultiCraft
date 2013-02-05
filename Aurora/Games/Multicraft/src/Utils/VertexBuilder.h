#ifndef VERTEXBUILDER_H
#define VERTEXBUILDER_H

#include "../World/Chunk.h"

#include <Aurora/Math/Vector2.h>
#include <Aurora/Math/Vector3.h>

using namespace Aurora::Math;

#include <vector>

class VertexBuilder
{
private:

	static VertexBuilder* _instance;

	std::vector<MultiVertex> mVertices;
	std::vector<unsigned short> mTriangle;

	void triangle(int x,int y,int z);
	void vert(float x,float y,float z,float u,float v,float r,float g,float b);

	void _cleanBuffers();

	float _boundDown(float number);
	float _boundUp(float number);

	VertexBuilder();

public:

	static VertexBuilder* Instance();

	void BuildChunk(Chunk* chunk);
	void BuildTransparentChunk(Chunk* chunk);

	void BuildChunkVertexBuffer(Chunk *chunk,bool transparent);

};

#endif
