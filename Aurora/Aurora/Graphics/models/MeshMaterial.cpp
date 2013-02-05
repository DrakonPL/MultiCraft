#include <Aurora/Graphics/models/MeshMaterial.h>

namespace Aurora
{
	namespace Graphics
	{
		MeshMaterial::MeshMaterial()
		{
			name = "";
			textureMapFilename = "";

			ambient[0] = 0.2f;
			ambient[1] = 0.2f;
			ambient[2] = 0.2f;
			ambient[3] = 1.0f;

			diffuse[0] = 0.8f;
			diffuse[1] = 0.8f;
			diffuse[2] = 0.8f;
			diffuse[3] = 1.0f;

			specular[0] = 0.0f;
			specular[1] = 0.0f;
			specular[2] = 0.0f;
			specular[3] = 1.0f;
			shininess = 0.0f;
			alpha = 1.0f;

			textured = false;
		}
	}
}

