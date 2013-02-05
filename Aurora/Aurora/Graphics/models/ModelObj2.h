#ifndef OBJMODEL2_H
#define OBJMODEL2_H

#include <Aurora/Graphics/models/ModelMesh.h>
#include <Aurora/Graphics/Camera.h>

#include <Aurora/Math/Vector3.h>
#include <Aurora/Math/Vector2.h>

using namespace Aurora::Math;

#include <vector>
#include <string>

namespace Aurora
{
	namespace Graphics
	{
		class ModelObjMesh : public ModelMesh
		{
		public:

			std::vector<Vector3d> mFace;
			std::vector<Vector3d> mUVFace;
			std::vector<Vector3d> mNormalFace;

			int _materialNumber;

			bool uvMapping;
			bool isNormals;
			int listCreated;
		};

		class ModelObj2
		{
		private:

			std::vector<ModelObjMesh*>	mMeshes;

			std::vector<Vector3>	allVertex;
			std::vector<Vector3>	allNormal;
			std::vector<Vector2>	allUVMap;

			//materials
			std::vector<MeshMaterial*>	mMaterials;

			//meshes count
			int meshCount;
			std::string				pathName;

			Vector3 position;

		public:

			ModelObj2();

			void UseFrustumCheck(bool state);

			bool Load(std::string fileName);
			bool LoadMaterials(std::string fileName);
			bool LoadModel(std::string fileName);

			bool Save(std::string fileName);

			void Optimize();
			void CreateVertexBuffers();

			void SetPosition(Vector3 pos);

			void Draw();
			void Draw(Frustum *frustum,Vector3 headPosition);
		};
	}
}

#endif
