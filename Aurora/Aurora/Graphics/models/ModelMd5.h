#ifndef MODELMD5_H
#define MODELMD5_H

#include <Aurora/Math/MathLib.h>
#include <Aurora/Math/Vector3.h>
#include <Aurora/Math/Vector2.h>
#include <Aurora/Math/Quaternion.h>
#include <Aurora/Math/Matrix4.h>

#include <Aurora/Graphics/VertexBuffer.h>

using namespace Aurora::Math;
using namespace Aurora::Graphics;

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <math.h>
#include <vector>
//#include <fstream>

namespace Aurora
{
	namespace Graphics
	{	
		class Vertex 
		{
		public:
			int weightIndex;
			int weightCount;
			float pos[3];
			Vector3 binPos;
			float tc[2];
			float n[3];
		};

		class Joint 
		{
		public:
			std::string name;
			float pos[3];
			float orient[3];
			Quaternion quat;
			Matrix4 matrix;
			int parent;
			std::vector<int> children;
		};

		class Tri 
		{
		public:
			int v[3]; // vertex indices
		};

		class Weight 
		{
		public:
			int joint;
			float w;
			float pos[3];
		};

		class Mesh 
		{
		public:
			std::string name;
			std::string texture;

			std::vector<Vertex> verts;
			std::vector<Tri> tris;
			std::vector<Weight> weights;

			std::vector<Vertex> tempverts;
			std::vector<Tri> temptris;
			std::vector<Weight> tempweights;

			int textureNumber;
			bool isVisible;

			VertexBuffer _vertexBuffer;

			Mesh();
			Mesh(const Mesh *b);

			int getTempVertexNumber(Vertex vert);

			void BuildVertexBuffer();
		};

		class Frame 
		{
		public:
			std::vector<float> animatedComponents;
			std::vector<Joint> joints;
			float min[3]; // min point of bounding box
			float max[3]; // max point of bounding box
		};

		class JointInfo 
		{
		public:
			std::string name;
			int parentIndex;
			int flags;
			int startIndex;
		};

		// stores data from an anim file
		class Anim
		{
		public:
			Anim(); 
			int numFrames;
			int frameRate;
			int numAnimatedComponents;
			std::vector<Frame> frames;
			std::vector<Joint> baseJoints;
			std::vector<JointInfo> jointInfo;
		};

		class ModelMd5
		{
		private:

			Quaternion buildQuat(float x, float y, float z) const;
			Quaternion slerp(const Quaternion &q1, const Quaternion &q2, float t);

			int numJoints;
			int numMeshes;
			int currAnim;
			int currFrame;
			float animTime;
			Frame interpFrame; // used to interpolate between frames

			std::vector<Mesh*> meshes;
			std::vector<Joint> joints;
			std::vector<Anim*> anims;

			Vector3 _position;
			Vector2 _dirFacing;
			Vector3 _rotation;

		public:

			ModelMd5();
			~ModelMd5();

			void clear();
			void optimizeMesh();

			//void LoadModel(char *filename);			
			//int  LoadAnimation(const char *filename);

			Mesh* GetMesh(std::string name);

			void SetAnimation(int animIndex, int frameIndex = 0);
			void SetFrame(int frameIndex);

			void UpdateAnimation(float dt);
			void Draw();

			bool Load(std::string fileName);
			bool Save(std::string fileName);

			void SetPosition(Vector3 pos);
			void SetRotation(Vector3 rotation);
			void FaceDirection(Vector2 direction);

			void BuildVerts();
			void BuildVerts(Frame &frame);
			void buildNormals();
			void buildFrames(Anim &anim);
		};
	}
}

#endif
