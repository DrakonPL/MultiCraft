#include <Aurora/Graphics/models/ModelObj2.h>

#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/TextureManager.h>
#include <Aurora/Graphics/Vertex.h>

#include <Aurora/System/FileManager.h>

#include <Aurora/Utils/Logger.h>

namespace Aurora
{
	namespace Graphics
	{

		ModelObj2::ModelObj2()
		{
			position.set(0,0,0);
		}

		bool ModelObj2::Load(std::string fileName)
		{
			System::File* file = System::FileManager::Instance()->GetFile(fileName);

			if(file == 0)
			{
				Utils::Logger::Instance()->LogMessage("Can't load file: %s \n",fileName.c_str());
				return false;
			}

			file->Open();

			//save file version
			int fileVersion = 0;
			file->Read(&fileVersion,sizeof(int),1);
			

			//number of materials
			int materialCount = 0;
			file->Read(&materialCount,sizeof(int),1);
			mMaterials.resize(materialCount);

			//number of meshes
			int meshCount = 0;
			file->Read(&meshCount,sizeof(int),1);
			mMeshes.resize(meshCount);

			//1. save materials info
			for (unsigned int i = 0;i < mMaterials.size();i++)
			{
				mMaterials[i] = new MeshMaterial();

				file->Read(mMaterials[i]->ambient,sizeof(float),4);
				file->Read(mMaterials[i]->diffuse,sizeof(float),4);
				file->Read(mMaterials[i]->specular,sizeof(float),4);

				file->Read(&mMaterials[i]->shininess,sizeof(float),1);
				file->Read(&mMaterials[i]->alpha,sizeof(float),1);

				//material name
				int nameSize = 0;
				file->Read(&nameSize,sizeof(int),1);
				char *materialName = new char[nameSize+1];
				file->Read(materialName,sizeof(char),nameSize);
				materialName[nameSize] = '\0';
				mMaterials[i]->name = materialName;
				delete [] materialName;
				

				//texture name
				int textureSize = 0;
				file->Read(&textureSize,sizeof(int),1);

				char *textureName = new char[textureSize+1];
				file->Read(textureName,sizeof(char),textureSize);
				textureName[textureSize] = '\0';
				mMaterials[i]->textureMapFilename = textureName;
				delete [] textureName;

				//TextureManager::Instance()->loadImageFromFile(mMaterials[i]->textureMapFilename);
				//mMaterials[i]->texturImage  = TextureManager::Instance()->getImage(mMaterials[i]->textureMapFilename);

				//if (mMaterials[i]->texturImage != 0)
				//	mMaterials[i]->textured = true;
			}

			//2. save all meshes
			for (unsigned int i = 0;i < mMeshes.size();i++)
			{
				mMeshes[i] = new ModelObjMesh();

				//name
				int nameSize = 0;
				file->Read(&nameSize,sizeof(int),1);
				char *meshName = new char[nameSize+1];
				file->Read(meshName,sizeof(char),nameSize);
				meshName[nameSize] = '\0';
				mMeshes[i]->_meshName = meshName;
				delete [] meshName;

				//material number
				file->Read(&mMeshes[i]->_materialNumber,sizeof(int),1);

				//vertices and indices count
				file->Read(&mMeshes[i]->vertexCount,sizeof(int),1);
				file->Read(&mMeshes[i]->indicesCount,sizeof(int),1);

				//vertices and indices
				mMeshes[i]->meshVertices = new MultiVertex[mMeshes[i]->vertexCount];
				mMeshes[i]->indices = new unsigned short[mMeshes[i]->indicesCount];

				file->Read(mMeshes[i]->meshVertices,sizeof(MultiVertex),mMeshes[i]->vertexCount);
				file->Read(mMeshes[i]->indices,sizeof(unsigned short),mMeshes[i]->indicesCount);				
			}

			file->Close();

			//load all materials?
			for (unsigned int i = 0;i < mMaterials.size();i++)
			{
				TextureManager::Instance()->loadImageFromFile(mMaterials[i]->textureMapFilename);
				mMaterials[i]->texturImage  = TextureManager::Instance()->getImage(mMaterials[i]->textureMapFilename);

				if (mMaterials[i]->texturImage != 0)
					mMaterials[i]->textured = true;
			}
			
			Utils::Logger::Instance()->LogMessage("File loaded: %s \n",fileName.c_str());

			return true;
		}

		bool ModelObj2::LoadMaterials(std::string fileName)
		{
			//file manager hack
			//TODO - change this...
			std::string _filename = System::FileManager::Instance()->GetMainDirPath();
			_filename += fileName;

			FILE *fp = fopen(_filename.c_str(),"r");

			if (!fp)
			{
				Utils::Logger::Instance()->LogMessage("Can't load file: %s \n",fileName.c_str());
				return false;
			}

			MeshMaterial *newMaterial = new MeshMaterial();

			int illum = 0;
			char buffer[255] = {0};

			bool newMat = false;

			while (fscanf(fp, "%s", buffer) != EOF)
			{
				switch(buffer[0])
				{
					case 'n': //new material
					{
						if (newMat)
						{
							mMaterials.push_back(newMaterial);
							newMaterial = new MeshMaterial();
							newMat = false;
						}

						fgets(buffer, sizeof(buffer), fp);
						sscanf(buffer, "%s %s", buffer, buffer);

						newMaterial->name = buffer;
					}
					break;

					case 'N': //Ns - shiness
					{
							fscanf(fp, "%f", &newMaterial->shininess);

							newMaterial->shininess /= 1000.0f;
					}
					break;

					case 'K':
					{
						switch(buffer[1])
						{
							case 'a'://Ka - ambient
							{
								fscanf(fp, "%f %f %f",&newMaterial->ambient[0],&newMaterial->ambient[1],&newMaterial->ambient[2]);
								newMaterial->ambient[3] = 1.0f;
							}
							break;
							case 'd'://Kd - diffuse
							{
								fscanf(fp, "%f %f %f",&newMaterial->diffuse[0],&newMaterial->diffuse[1],&newMaterial->diffuse[2]);
								newMaterial->diffuse[3] = 1.0f;
							}
							break;
							case 's'://Ks - specular
							{
								fscanf(fp, "%f %f %f",&newMaterial->specular[0],&newMaterial->specular[1],&newMaterial->specular[2]);
								newMaterial->specular[3] = 1.0f;
							}
							break;
							default:
							{
								fgets(buffer, sizeof(buffer), fp);
							}
							break;
						}
					}
					break;

					case 'T': // Tr
					{
						switch (buffer[1])
						{
							case 'r': // Tr
							{
								fscanf(fp, "%f", &newMaterial->alpha);
								newMaterial->alpha = 1.0f - newMaterial->alpha;
							}
							break;

							default:
							{
								fgets(buffer, sizeof(buffer), fp);
							}
							break;
						}
					}
					break;

					case 'd':
					{
						fscanf(fp, "%f", &newMaterial->alpha);
					}
					break;

					case 'i': // illum
					{
						fscanf(fp, "%d", &illum);

						if (illum == 1)
						{
							newMaterial->specular[0] = 0.0f;
							newMaterial->specular[1] = 0.0f;
							newMaterial->specular[2] = 0.0f;
							newMaterial->specular[3] = 1.0f;
						}
					}
					break;

					case 'm': // map_Kd, map_bump
					{
						if (strstr(buffer, "map_Kd") != 0)
						{
							fgets(buffer, sizeof(buffer), fp);
							sscanf(buffer, "%s %s", buffer, buffer);

							std::string pathName = fileName;
							size_t found = pathName.find_last_of("/");
							pathName = pathName.substr(0,found)+ "/" + buffer;

							Utils::Logger::Instance()->LogMessage("Loading texture file: %s \n",pathName.c_str());
							
							TextureManager::Instance()->loadImageFromFile(pathName);
							
							newMaterial->texturImage  = TextureManager::Instance()->getImage(pathName);
							newMaterial->textureMapFilename = pathName;
							newMaterial->textured = true;
							
							Utils::Logger::Instance()->LogMessage("Texture id: %i \n",newMaterial->texturImage->_id);

							newMat = true;

						}
						else if (strstr(buffer, "map_bump") != 0)
						{
							/*fgets(buffer, sizeof(buffer), fp);
							sscanf(buffer, "%s %s", buffer, buffer);

							newMaterial->bumpMapFilename =  "Assets/Obj/";
							newMaterial->bumpMapFilename += buffer;

							TextureManager *tManager = TextureManager::Instance();

							if (!tManager->TextureExist(newMaterial->bumpMapFilename))
							{
								tManager->LoadTexture(newMaterial->bumpMapFilename);
							}

							newMaterial->normaltextureID = tManager->GetTextureNumber(newMaterial->bumpMapFilename);
							newMat = true;*/
						}else if(strstr(buffer, "map_Ke") != 0)
						{
							/*fgets(buffer, sizeof(buffer), fp);
							sscanf(buffer, "%s %s", buffer, buffer);

							std::string pathName(Filname);
							size_t found = pathName.find_last_of("/");
							pathName = pathName.substr(0,found)+ "/" + buffer;

							newMaterial->lightMapID  = TextureManager::Instance()->loadImageFromFile(pathName)->_id;
							newMaterial->lightmapping = true;*/
						}
						else
						{
							fgets(buffer, sizeof(buffer), fp);
						}
					}
					break;

					default:
						fgets(buffer, sizeof(buffer), fp);
					break;
				}
			}

			mMaterials.push_back(newMaterial);
			fclose(fp);
			
			Utils::Logger::Instance()->LogMessage("Loaded file: %s \n",fileName.c_str());

			return true;
		}

		bool ModelObj2::LoadModel(std::string fileName)
		{
			//file manager hack
			//TODO - change this...
			std::string _filename = System::FileManager::Instance()->GetMainDirPath();
			_filename += fileName;

			FILE *fp = fopen(fileName.c_str(),"r");

			if (!fp)
			{
				Utils::Logger::Instance()->LogMessage("Can't load file: %s \n",fileName.c_str());
				return false;
			}

			pathName = fileName;

			ModelObjMesh *newMesh = new ModelObjMesh();

			char ch				= 0;
			char ch2			= 0;
			char strLine[255]	= {0};
			char buff[512];

			bool faceLoad = false;
			bool uvFace = false;
			bool normal = false;

			while(!feof(fp))
			{
				//get first character
				ch = fgetc(fp);

				switch(ch)
				{
				case 'm': //material info
					{
						char text[100];
						char materialName[100];

						//read material info
						fscanf(fp,"%s %s", &text, &materialName);

						size_t found = pathName.find_last_of("/");
						pathName = pathName.substr(0,found) + "/" + materialName;

						LoadMaterials(pathName);

						//read line to the end
						fgets(strLine, 100, fp);
					}
					break;

				case 'u'://use material
					{
						if (faceLoad)
						{
							mMeshes.push_back(newMesh);
							meshCount++;

							//new newmesh
							newMesh = new ModelObjMesh();

							faceLoad = false;
							uvFace = false;
							normal = false;
						}

						char name[50];
						char name2[50];

						//read material name
						fscanf(fp,"%s %s", &name, &name2);

						newMesh->_materialNumber = 0;

						for(unsigned int i = 0;i < mMaterials.size();i++)
						{
							if(strcmp(mMaterials[i]->name.c_str(),name2) == 0)
							{
								newMesh->_materialNumber = i;
							}
						}

						//read line to the end
						fgets(strLine, 100, fp);
					}
					break;

				case 'g': //mesh name
					{
						//read mesh name
						char name[50];
						fscanf(fp," %s", name);
						newMesh->_meshName = name;

						//read line to the end
						fgets(strLine, 100, fp);
					}
					break;



				case 'v': //vertex found
					{
						//get second character
						ch2 = fgetc(fp);

						switch(ch2)
						{
						case ' ':
							{
								if (faceLoad)
								{
									mMeshes.push_back(newMesh);
									meshCount++;

									//new newmesh
									newMesh = new ModelObjMesh();

									faceLoad = false;
									uvFace = false;
									normal = false;
								}

								//new vertex position
								Vector3 vPosition;

								//read vertex info
								fscanf(fp,"%f %f %f", &vPosition.x, &vPosition.y, &vPosition.z);

								allVertex.push_back(vPosition);

								//read line to the end
								fgets(strLine, 100, fp);
							}
							break;

						case 't': //texture info
							{
								uvFace = true;

								//new uv map
								Vector2 nObjUV;

								//read uv map
								fscanf(fp,"%f %f", &nObjUV.x, &nObjUV.y);

								allUVMap.push_back(nObjUV);

								//read line to the end
								fgets(strLine, 100, fp);
							}
							break;

						case 'n': //normal info
							{
								normal = true;

								//new normal position
								Vector3 nNormal;

								//read normal
								fscanf(fp,"%f %f %f", &nNormal.x, &nNormal.y ,&nNormal.z);

								allNormal.push_back(nNormal);

								//read line to the end
								fgets(strLine, 100, fp);
							}
							break;

						default:
							break;
						}
					}
					break;

				case 'f': //face info
					{
						faceLoad = true;

						//new face
						Vector3d nFace;
						Vector3d nUVFace;
						Vector3d nNorm;

						/* Read whole line */
						fgets (buff, sizeof (buff), fp);

						if (sscanf(buff,"%d/%d/%d %d/%d/%d %d/%d/%d",&nFace.x,&nUVFace.x,&nNorm.x,&nFace.y,&nUVFace.y,&nNorm.y,&nFace.z,&nUVFace.z,&nNorm.z) == 9)
						{
							//push UV face to mesh
							nUVFace.x -=1;nUVFace.y -=1;nUVFace.z -=1;

							newMesh->mUVFace.push_back(nUVFace);

							//we must -1 to all indicies
							nNorm.x -=1;nNorm.y -=1;nNorm.z -=1;

							//push
							newMesh->mNormalFace.push_back(nNorm);
							uvFace = true;
							normal = true;
						}else
							if (sscanf(buff,"%d/%d %d/%d %d/%d",&nFace.x,&nUVFace.x,&nFace.y,&nUVFace.y,&nFace.z,&nUVFace.z) == 6)
							{
								//push UV face to mesh
								nUVFace.x -=1;nUVFace.y -=1;nUVFace.z -=1;
								newMesh->mUVFace.push_back(nUVFace);
								uvFace = true;
							}else
							{
								sscanf(buff,"%d %d %d",&nFace.x,&nFace.y,&nFace.z);
							}

							//we must -1 to all indicies
							nFace.x -=1;nFace.y -=1;nFace.z -=1;

							//push face to mesh
							newMesh->mFace.push_back(nFace);
					}
					break;

				case '\n':

					break;

				default:
					//read line to the end
					fgets(strLine, 100, fp);
					break;
				}
			}

			//ad last submesh or if thhere is only one
			mMeshes.push_back(newMesh);

			fclose(fp);
			
			Utils::Logger::Instance()->LogMessage("Loaded file: %s \n",fileName.c_str());

			return true;
		}
		

		bool ModelObj2::Save(std::string fileName)
		{
			FILE* fp = fopen(fileName.c_str(),"wb");
			if (!fp)
			{
				return false;
			}

			//save file version
			int fileVersion = 1;
			fwrite(&fileVersion,sizeof(int),1,fp);

			//number of materials
			int materialCount = mMaterials.size();
			fwrite(&materialCount,sizeof(int),1,fp);

			//number of meshes
			int meshCount = mMeshes.size();
			fwrite(&meshCount,sizeof(int),1,fp);

			//1. save materials info
			for (unsigned int i = 0;i < mMaterials.size();i++)
			{
				fwrite(mMaterials[i]->ambient,sizeof(float),4,fp);
				fwrite(mMaterials[i]->diffuse,sizeof(float),4,fp);
				fwrite(mMaterials[i]->specular,sizeof(float),4,fp);

				fwrite(&mMaterials[i]->shininess,sizeof(float),1,fp);
				fwrite(&mMaterials[i]->alpha,sizeof(float),1,fp);

				//material name
				const char *materialName = mMaterials[i]->name.c_str();
				int nameSize = strlen(materialName);
				fwrite(&nameSize,sizeof(int),1,fp);
				fwrite(materialName,sizeof(char),strlen(materialName),fp);

				//texture name
				const char *textureName = mMaterials[i]->textureMapFilename.c_str();
				int textureSize = strlen(textureName);
				fwrite(&textureSize,sizeof(int),1,fp);
				fwrite(textureName,sizeof(char),strlen(textureName),fp);
			}

			//2. save all meshes
			for (unsigned int i = 0;i < mMeshes.size();i++)
			{
				//name
				const char *meshName = mMeshes[i]->_meshName.c_str();
				int nameSize = strlen(meshName);
				fwrite(&nameSize,sizeof(int),1,fp);
				fwrite(meshName,sizeof(char),strlen(meshName),fp);

				//material number
				fwrite(&mMeshes[i]->_materialNumber,sizeof(int),1,fp);

				//vertices and indices count
				fwrite(&mMeshes[i]->vertexCount,sizeof(int),1,fp);
				fwrite(&mMeshes[i]->indicesCount,sizeof(int),1,fp);

				//vertices and indices
				fwrite(mMeshes[i]->meshVertices,sizeof(MultiVertex),mMeshes[i]->vertexCount,fp);
				fwrite(mMeshes[i]->indices,sizeof(unsigned short),mMeshes[i]->indicesCount,fp);				
			}

			fclose(fp);

			return true;
		}

		void ModelObj2::UseFrustumCheck(bool state)
		{
			for (unsigned int i = 0; i < mMeshes.size();i++)
			{
				ModelObjMesh *mesh = mMeshes[i];
				mesh->_useAABB = state;
			}
		}

		void ModelObj2::Optimize()
		{
			for (unsigned int i = 0; i < mMeshes.size();i++)
			{
				ModelObjMesh *mesh = mMeshes[i];

				std::vector<MultiVertex> temp;
				std::vector<unsigned short> tempIndices;

				bool haveUV = allUVMap.size() > 0;

				for(unsigned int f = 0;f < mesh->mFace.size();f++)
				{
					//create temp vertex
					MultiVertex vert;

					if(haveUV)
					{
						vert.u = allUVMap[mesh->mUVFace[f].x].x;
						vert.v = allUVMap[mesh->mUVFace[f].x].y;
					}else
					{
						vert.u = 0.0f;
						vert.v = 0.0f;
					}

					vert.x = allVertex[mesh->mFace[f].x].x;
					vert.y = allVertex[mesh->mFace[f].x].y;
					vert.z = allVertex[mesh->mFace[f].x].z;
					
					vert.r = mMaterials[mesh->_materialNumber]->diffuse[0];
					vert.g = mMaterials[mesh->_materialNumber]->diffuse[1];
					vert.b = mMaterials[mesh->_materialNumber]->diffuse[2];
					vert.a = mMaterials[mesh->_materialNumber]->diffuse[3];


					//check if exist
					bool found = false;
					for(unsigned int t = 0;t < temp.size();t++)
					{
						if(temp[t].u == vert.u && temp[t].v == vert.v && temp[t].x == vert.x && temp[t].y == vert.y && temp[t].z == vert.z)
						{
							found = true;
							tempIndices.push_back(t);
							break;
						}
					}

					if (found == false)
					{
						mesh->_aabb.expandToInclude(allVertex[mesh->mFace[f].x]);
						temp.push_back(vert);
						tempIndices.push_back(temp.size()-1);
					}

					//////////////////////////////////////////////////////////////////////////
					if(haveUV)
					{
						vert.u = allUVMap[mesh->mUVFace[f].y].x;
						vert.v = allUVMap[mesh->mUVFace[f].y].y;
					}else
					{
						vert.u = 0.0f;
						vert.v = 0.0f;
					}

					vert.x = allVertex[mesh->mFace[f].y].x;
					vert.y = allVertex[mesh->mFace[f].y].y;
					vert.z = allVertex[mesh->mFace[f].y].z;
					
					vert.r = mMaterials[mesh->_materialNumber]->diffuse[0];
					vert.g = mMaterials[mesh->_materialNumber]->diffuse[1];
					vert.b = mMaterials[mesh->_materialNumber]->diffuse[2];
					vert.a = mMaterials[mesh->_materialNumber]->diffuse[3];

					//check if exist
					found = false;
					for(unsigned int t = 0;t < temp.size();t++)
					{
						if(temp[t].u == vert.u && temp[t].v == vert.v && temp[t].x == vert.x && temp[t].y == vert.y && temp[t].z == vert.z)
						{
							found = true;
							tempIndices.push_back(t);
							break;
						}
					}

					if (found == false)
					{
						mesh->_aabb.expandToInclude(allVertex[mesh->mFace[f].y]);
						temp.push_back(vert);
						tempIndices.push_back(temp.size()-1);
					}

					//////////////////////////////////////////////////////////////////////////
					if(haveUV)
					{
						vert.u = allUVMap[mesh->mUVFace[f].z].x;
						vert.v = allUVMap[mesh->mUVFace[f].z].y;
					}else
					{
						vert.u = 0.0f;
						vert.v = 0.0f;
					}

					vert.x = allVertex[mesh->mFace[f].z].x;
					vert.y = allVertex[mesh->mFace[f].z].y;
					vert.z = allVertex[mesh->mFace[f].z].z;
					
					vert.r = mMaterials[mesh->_materialNumber]->diffuse[0];
					vert.g = mMaterials[mesh->_materialNumber]->diffuse[1];
					vert.b = mMaterials[mesh->_materialNumber]->diffuse[2];
					vert.a = mMaterials[mesh->_materialNumber]->diffuse[3];

					//check if exist
					found = false;
					for(unsigned int t = 0;t < temp.size();t++)
					{
						if(temp[t].u == vert.u && temp[t].v == vert.v && temp[t].x == vert.x && temp[t].y == vert.y && temp[t].z == vert.z)
						{
							found = true;
							tempIndices.push_back(t);
							break;
						}
					}

					if (found == false)
					{
						mesh->_aabb.expandToInclude(allVertex[mesh->mFace[f].z]);
						temp.push_back(vert);
						tempIndices.push_back(temp.size()-1);
					}
				}

				//copy temp vertices and indices
				mesh->meshVertices = new MultiVertex[temp.size()];
				memcpy(mesh->meshVertices,&temp[0],sizeof(MultiVertex) * temp.size());

				mesh->indices = new unsigned short[tempIndices.size()];
				memcpy(mesh->indices,&tempIndices[0],sizeof(unsigned short) * tempIndices.size());

				mesh->vertexCount = temp.size();
				mesh->indicesCount = tempIndices.size();

				temp.clear();
				tempIndices.clear();

				mesh->mUVFace.clear();
				mesh->mFace.clear();
				mesh->mNormalFace.clear();

				//try optimize indieces for cashe misses
				/*VertexCache vertex_cache;
				VertexCacheOptimizer vco;

				int misses = vertex_cache.GetCacheMissCount(mesh->indices,mesh->triangleCount);

				VertexCacheOptimizer::Result res = vco.Optimize(mesh->indices,mesh->triangleCount);
				int misses2 = vertex_cache.GetCacheMissCount(mesh->indices,mesh->triangleCount);*/

				//create triangle strips :D
				/*STRIPERCREATE sc;

				sc.DFaces			= (unsigned int*)mesh->indices;
				sc.NbFaces			= mesh->triangleCount;
				sc.AskForWords		= true;
				sc.ConnectAllStrips	= true;
				sc.OneSided			= true;
				sc.SGIAlgorithm		= true;

				Striper Strip;
				Strip.Init(sc);

				STRIPERRESULT sr;
				Strip.Compute(sr);

				std::vector<unsigned int> testor;

				uword* Refs = (uword*)sr.StripRuns;
				for(udword i=0;i<sr.NbStrips;i++)
				{
					udword NbRefs = sr.StripLengths[i];
					for(udword j=0;j<NbRefs;j++)
					{
						//fprintf(stdout, "%d ", *Refs++);
						testor.push_back(*Refs++);
					}
				}

				delete [] mesh->indices;

				mesh->indices = new int[testor.size()];
				for(int v = 0;v < testor.size();v++)
				{
					memcpy(&mesh->indices[v],&testor[v],sizeof(int));
				}

				mesh->indicesCount = testor.size();*/
			}

			allVertex.clear();
			allNormal.clear();
			allUVMap.clear();
		}

		void ModelObj2::CreateVertexBuffers()
		{
			for (unsigned int i = 0; i < mMeshes.size();i++)
			{
				ModelObjMesh *mesh = mMeshes[i];

				mesh->_vertexBuffer.Generate(mesh->meshVertices,mesh->indices,mesh->vertexCount,mesh->indicesCount,mMaterials[mesh->_materialNumber]->textured);
				mesh->_vertexBuffer.Build();
			}
		}

		void ModelObj2::SetPosition(Vector3 pos)
		{
			position = pos;
		}

		void ModelObj2::Draw()
		{
			for (unsigned int i = 0; i < mMeshes.size();i++)
			{
				ModelObjMesh *mesh = mMeshes[i];

				bool canRender = true;

				if(mesh->_useAABB)
				{
					if(!RenderManager::Instance()->GetCurrentCam()->camFrustum->BoxInFrustum(mesh->_aabb))
						canRender = false;
				}

				if(canRender)
				{
					if(mMaterials[mesh->_materialNumber]->textured)
					RenderManager::Instance()->bindTexture(mMaterials[mesh->_materialNumber]->texturImage);

					RenderManager::Instance()->PushMatrix();
					RenderManager::Instance()->TranslateMatrix(position);

					RenderManager::Instance()->SetColor(0xFFFFFFFF);

					mesh->_vertexBuffer.Draw();
				
					RenderManager::Instance()->PopMatrix();
				}			
			}
		}

		void ModelObj2::Draw(Frustum *frustum,Vector3 headPosition)
		{
			for (unsigned int i = 0; i < mMeshes.size();i++)
			{
				ModelObjMesh *mesh = mMeshes[i];

				bool canRender = true;

				if(mesh->_useAABB)
				{
					if(!frustum->BoxInFrustum(mesh->_aabb) || mesh->_aabb.contains(headPosition))
						canRender = false;
				}

				if(canRender)
				{
					if(mMaterials[mesh->_materialNumber]->textured)
						RenderManager::Instance()->bindTexture(mMaterials[mesh->_materialNumber]->texturImage);

					RenderManager::Instance()->PushMatrix();
					RenderManager::Instance()->TranslateMatrix(position);

					RenderManager::Instance()->SetColor(0xFFFFFFFF);

					mesh->_vertexBuffer.Draw();

					RenderManager::Instance()->PopMatrix();
				}			
			}
		}
	}
}
