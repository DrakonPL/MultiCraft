#ifndef VBOPSP_H
#define VBOPSP_H

#include <Aurora/Graphics/Vertex.h>

namespace Aurora
{
	namespace Graphics
	{
		typedef struct
		{
			float u,v;
			unsigned int color;
			float x,y,z;
		}__attribute__((packed)) CraftPSPVertex;
			
		
		class VertexBuffer
		{
		private:

			unsigned int _vboId;
			unsigned int _indicesId;

			int _vertexCount;
			int _indicesCount;

			CraftPSPVertex* _pspVertices;
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
