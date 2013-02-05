#ifndef MODELMESH_H 
#define MODELMESH_H

#include <Aurora/Graphics/VertexBuffer.h>
#include <Aurora/Graphics/Vertex.h>
#include <Aurora/Graphics/models/MeshMaterial.h>

#include <Aurora/Math/BoundingBox.h>

using namespace Aurora::Math;

#include <string>

namespace Aurora
{
	namespace Graphics
	{
		class ModelMesh
		{
		public:

			VertexBuffer _vertexBuffer;

			MeshMaterial* _material;

			BoundingBox _aabb;
			bool _useAABB;

			std::string _meshName;

			MultiVertex * meshVertices;
			unsigned short *indices;
			int vertexCount;
			int indicesCount;

			ModelMesh();
			~ModelMesh();
		};
	}
}

#endif
