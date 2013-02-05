#include <Aurora/Graphics/models/ModelMesh.h>

namespace Aurora
{
	namespace Graphics
	{
		
		ModelMesh::ModelMesh()
		{
			_meshName = "";

			meshVertices = 0;
			indices = 0;

			vertexCount = 0;
			indicesCount = 0;

			_useAABB = true;
		}

		ModelMesh::~ModelMesh()
		{
			if (meshVertices != 0)
				delete meshVertices;

			if (indices != 0)
				delete indices;

			_vertexBuffer.Delete();
		}
	}
}

