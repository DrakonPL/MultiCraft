#ifndef MESHMATERIAL_H
#define MESHMATERIAL_H

#include <Aurora/Graphics/Image.h>

#include <string>

namespace Aurora
{
	namespace Graphics
	{
		class MeshMaterial
		{
		public:

			float ambient[4];
			float diffuse[4];
			float specular[4];
			float shininess;        // [0 = min shininess, 1 = max shininess]
			float alpha;            // [0 = fully transparent, 1 = fully opaque]

			std::string name;
			std::string textureMapFilename;

			Image *texturImage;

			bool textured;

			MeshMaterial();
		};
	}
}

#endif
