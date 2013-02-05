#ifndef VERTEXBUFFEROGL_H
#define VERTEXBUFFEROGL_H

#include <Aurora/Graphics/Vertex.h>
using namespace Aurora::Graphics;

#include <string.h>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Aurora
{
	namespace Graphics
	{
		class VertexBuffer
		{
		private:

			unsigned int _vboId;
			unsigned int _indicesId;

			int _vertexCount;
			int _indicesCount;

			MultiVertex *_vertices;
			unsigned short *_indices;

			bool _built;
			bool _textured;

		public:

			VertexBuffer();

			void Generate(MultiVertex* vertices,unsigned short *indices,int vertexCount,int indicesCount,bool textured);
			void Build();
			void Reset();
			void Draw();
			void Delete();
		};
	}
}

#endif
