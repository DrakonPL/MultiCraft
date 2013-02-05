#include <Aurora/Graphics/models/ModelMd5.h>

#include <Aurora/System/FileManager.h>
#include <Aurora/Graphics/TextureManager.h>
#include <Aurora/Graphics/RenderManager.h>

namespace Aurora
{
	namespace Graphics
	{
		Mesh::Mesh()
		{
			isVisible = true;
		}

		Mesh::Mesh(const Mesh *b)
		{
			name.assign(b->name.c_str());
			texture.assign(b->texture.c_str());

			//verts
			for( size_t i=0; i < b->verts.size(); i++ )
			{
				verts.push_back(b->verts[i]);
			}
			//tris
			for( size_t i=0; i < b->tris.size(); i++ )
			{
				tris.push_back(b->tris[i]);
			}
			//weights
			for( size_t i=0; i < b->weights.size(); i++ )
			{
				weights.push_back(b->weights[i]);
			}
		}

		int Mesh::getTempVertexNumber(Vertex vert)
		{
			for (unsigned int i = 0; i < tempverts.size();i++)
			{
				if ((vert.pos[0] == tempverts[i].pos[0]) && (vert.pos[1] == tempverts[i].pos[1]) && (vert.pos[2] == tempverts[i].pos[2]) && (vert.tc[0] == tempverts[i].tc[0]) && (vert.tc[1] == tempverts[i].tc[1]))
				{
					return i;
				}
			}
			return -1;
		}

		void Mesh::BuildVertexBuffer()
		{
			MultiVertex* vertices = new MultiVertex[verts.size()];
			unsigned short* indices = new unsigned short[tris.size()*3];

			//copy all data
			for (unsigned int i = 0; i < verts.size(); i++)
			{
				vertices[i].a = vertices[i].r = vertices[i].g = vertices[i].b = 1.0f;
				
				vertices[i].u = verts[i].tc[0];
				vertices[i].v = verts[i].tc[1];

				vertices[i].x = verts[i].pos[0];
				vertices[i].y = verts[i].pos[1];
				vertices[i].z = verts[i].pos[2];
			}

			int indi = 0;
			for (unsigned int i = 0;i < tris.size();i++)
			{
				indices[indi] = tris[i].v[0];indi++;
				indices[indi] = tris[i].v[1];indi++;
				indices[indi] = tris[i].v[2];indi++;
			}

			_vertexBuffer.Delete();
			_vertexBuffer.Generate(vertices,indices,verts.size(),tris.size()*3,true);
			_vertexBuffer.Build();
		}

		ModelMd5::ModelMd5() : numJoints(0),numMeshes(0),currAnim(-1),currFrame(0),animTime(0.0f)
		{
			_position.set(0,0,0);
			_dirFacing.set(0,1);
			_rotation.set(-90,0,0);
		}

		ModelMd5::~ModelMd5()
		{
			clear();
		}

		void ModelMd5::clear()
		{
			// delete meshes
			for ( size_t i=0; i < meshes.size(); i++ )
				if ( meshes[i] != NULL )
					delete meshes[i];

			// delete animations
			for ( size_t i=0; i < anims.size(); i++ )
				if ( anims[i] )
					delete anims[i];

			meshes.clear();
			anims.clear();
			joints.clear();
		}

		bool ModelMd5::Load(std::string fileName)
		{
			System::File* file = System::FileManager::Instance()->GetFile(fileName);

			if(file == 0)
				return false;

			file->Open();
			
			//meshes count
			file->Read(&numMeshes,sizeof(int),1);

			//joints count
			file->Read(&numJoints,sizeof(int),1);

			//animation count
			int animCount = anims.size();
			file->Read(&animCount,sizeof(int),1);

			//load all joints
			joints.resize(numJoints);
			for (unsigned int i = 0; i < numJoints; i++)
			{
				//joint name
				
				int nameSize = 0;
				file->Read(&nameSize,sizeof(int),1);

				char *name = new char[nameSize+1];
				file->Read(name,sizeof(char),nameSize);
				name[nameSize] = '\0';
				joints[i].name = name;
				delete [] name;

				//orientation
				file->Read(joints[i].orient,sizeof(float),3);

				joints[i].quat = buildQuat(joints[i].orient[0], joints[i].orient[1], joints[i].orient[2]);

				//position
				file->Read(joints[i].pos,sizeof(float),3);

				//parent
				file->Read(&joints[i].parent,sizeof(int),1);

				//children
				int chindlrenSize = 0;
				file->Read(&chindlrenSize,sizeof(int),1);
				joints[i].children.resize(chindlrenSize);

				for (int c = 0; c < chindlrenSize;c++)
				{
					file->Read(&joints[i].children[c],sizeof(int),1);
				}
			}

			//load all meshes
			meshes.resize(numMeshes);
			for (unsigned int i = 0;i < numMeshes;i++)
			{
				meshes[i] = new Mesh();
				//mesh name				
				int nameSize = 0;
				file->Read(&nameSize,sizeof(int),1);

				char *name = new char[nameSize+1];
				file->Read(name,sizeof(char),nameSize);
				name[nameSize] = '\0';
				meshes[i]->name = name;
				delete [] name;

				//texture name				
				int shaderSize = 0;
				file->Read(&shaderSize,sizeof(int),1);
				char *shader = new char[shaderSize+1];
				file->Read(shader,sizeof(char),shaderSize);
				shader[shaderSize] = '\0';
				meshes[i]->texture = shader;
				delete [] shader;

				//load texture
				std::string pathName(fileName);
				size_t found = pathName.find_last_of("/");
				pathName = pathName.substr(0,found) + "/" + meshes[i]->texture;

				TextureManager::Instance()->loadImageFromFile(pathName);
				meshes[i]->texture  = pathName;

				//verts count
				int vertsCount = 0;
				file->Read(&vertsCount,sizeof(int),1);
				meshes[i]->verts.resize(vertsCount);

				//tris count
				int trisCount = 0;
				file->Read(&trisCount,sizeof(int),1);
				meshes[i]->tris.resize(trisCount);

				//weights count
				int weightCount = 0;
				file->Read(&weightCount,sizeof(int),1);
				meshes[i]->weights.resize(weightCount);

				//save all verts
				for (int v = 0; v < vertsCount;v++)
				{
					file->Read(&meshes[i]->verts[v],sizeof(Vertex),1);
				}

				//save all triangles
				for (int t = 0; t < trisCount;t++)
				{
					file->Read(&meshes[i]->tris[t],sizeof(Tri),1);
				}

				//save all weights
				for (int w = 0; w < weightCount;w++)
				{
					file->Read(&meshes[i]->weights[w],sizeof(Weight),1);
				}

				//build vertex buffer
				meshes[i]->BuildVertexBuffer();
			}

			//load all anim anims
			anims.resize(animCount);
			for (int i = 0;i < animCount;i++)
			{
				anims[i] = new Anim();

				file->Read(&anims[i]->numFrames,sizeof(int),1);
				file->Read(&anims[i]->frameRate,sizeof(int),1);
				file->Read(&anims[i]->numAnimatedComponents,sizeof(int),1);

				//joints info
				int jointsInfoCount = 0;//anims[i]->jointInfo.size();
				file->Read(&jointsInfoCount,sizeof(int),1);
				anims[i]->jointInfo.resize(jointsInfoCount);

				for (int ji = 0; ji <jointsInfoCount; ji++)
				{
					//name
					int nameSize = 0;
					file->Read(&nameSize,sizeof(int),1);

					char *name = new char[nameSize+1];
					file->Read(name,sizeof(char),nameSize);
					name[nameSize] = '\0';
					anims[i]->jointInfo[ji].name = name;
					delete [] name;
		
					file->Read(&anims[i]->jointInfo[ji].parentIndex,sizeof(int),1);
					file->Read(&anims[i]->jointInfo[ji].flags,sizeof(int),1);
					file->Read(&anims[i]->jointInfo[ji].startIndex,sizeof(int),1);
				}				
				
				//frame bounds
				anims[i]->frames.resize(anims[i]->numFrames);
				for (int b = 0; b < anims[i]->numFrames; b++)
				{
					file->Read(&anims[i]->frames[b].min,sizeof(int),3);
					file->Read(&anims[i]->frames[b].max,sizeof(int),3);
				}

				//base joints
				anims[i]->baseJoints.resize(numJoints);
				for (int b = 0; b < anims[i]->baseJoints.size(); b++)
				{
					file->Read(&anims[i]->baseJoints[b].pos,sizeof(int),3);
					file->Read(&anims[i]->baseJoints[b].orient,sizeof(int),3);
					anims[i]->baseJoints[b].quat = buildQuat(anims[i]->baseJoints[b].orient[0], anims[i]->baseJoints[b].orient[1], anims[i]->baseJoints[b].orient[2]);
				}

				//frames
				for (int b = 0; b < anims[i]->frames.size(); b++)
				{
					anims[i]->frames[b].animatedComponents.resize(anims[i]->numAnimatedComponents);

					for (int ac = 0; ac < anims[i]->numAnimatedComponents; ac++)
						file->Read(&anims[i]->frames[b].animatedComponents[ac],sizeof(float),1);
				}

				// build frames for this animation
				buildFrames(*anims[i]);

				// make this the current animation
				SetAnimation(i, 0);
			}

			file->Close();

			return true;
		}

		bool ModelMd5::Save(std::string fileName)
		{
			FILE* fp = fopen(fileName.c_str(),"wb");
			if (!fp)
			{
				return false;
			}

			//meshes count
			fwrite(&numMeshes,sizeof(int),1,fp);

			//joints count
			fwrite(&numJoints,sizeof(int),1,fp);

			//animation count
			int animCount = anims.size();
			fwrite(&animCount,sizeof(int),1,fp);

			//save all joints
			for (unsigned int i = 0; i < joints.size(); i++)
			{
				//joint name
				const char *name = joints[i].name.c_str();
				int nameSize = strlen(name);
				fwrite(&nameSize,sizeof(int),1,fp);
				fwrite(name,sizeof(char),strlen(name),fp);

				//orientation
				fwrite(joints[i].orient,sizeof(float),3,fp);

				//position
				fwrite(joints[i].pos,sizeof(float),3,fp);

				//parent
				fwrite(&joints[i].parent,sizeof(int),1,fp);

				//children
				int chindlrenSize = joints[i].children.size();
				fwrite(&chindlrenSize,sizeof(int),1,fp);

				for (int c = 0; c < chindlrenSize;c++)
				{
					fwrite(&joints[i].children[c],sizeof(int),1,fp);
				}
			}

			//save all meshes
			for (unsigned int i = 0;i < meshes.size();i++)
			{
				//mesh name
				const char *name = meshes[i]->name.c_str();
				int nameSize = strlen(name);
				fwrite(&nameSize,sizeof(int),1,fp);
				fwrite(name,sizeof(char),strlen(name),fp);

				//texture name
				const char *shader = meshes[i]->texture.c_str();
				int shaderSize = strlen(shader);
				fwrite(&shaderSize,sizeof(int),1,fp);
				fwrite(shader,sizeof(char),strlen(shader),fp);

				//verts count
				int vertsCount = meshes[i]->verts.size();
				fwrite(&vertsCount,sizeof(int),1,fp);

				//tris count
				int trisCount = meshes[i]->tris.size();
				fwrite(&trisCount,sizeof(int),1,fp);

				//weights count
				int weightCount = meshes[i]->weights.size();
				fwrite(&weightCount,sizeof(int),1,fp);

				//save all verts
				for (unsigned int v = 0; v < meshes[i]->verts.size();v++)
				{
					fwrite(&meshes[i]->verts[v],sizeof(Vertex),1,fp);
				}
				
				//save all triangles
				for (unsigned int t = 0; t < meshes[i]->tris.size();t++)
				{
					fwrite(&meshes[i]->tris[t],sizeof(Tri),1,fp);
				}
				
				//save all weights
				for (unsigned int w = 0; w < meshes[i]->weights.size();w++)
				{
					fwrite(&meshes[i]->weights[w],sizeof(Weight),1,fp);
				}
			}

			//save all anims
			for (unsigned int i = 0;i < anims.size();i++)
			{
				fwrite(&anims[i]->numFrames,sizeof(int),1,fp);
				fwrite(&anims[i]->frameRate,sizeof(int),1,fp);
				fwrite(&anims[i]->numAnimatedComponents,sizeof(int),1,fp);

				//joints info
				int jointsInfoCount = anims[i]->jointInfo.size();
				fwrite(&jointsInfoCount,sizeof(int),1,fp);
				for (unsigned int ji = 0; ji < anims[i]->jointInfo.size(); ji++)
				{
					//name
					const char *name = anims[i]->jointInfo[ji].name.c_str();
					int nameSize = strlen(name);
					fwrite(&nameSize,sizeof(int),1,fp);
					fwrite(name,sizeof(char),strlen(name),fp);
		
					fwrite(&anims[i]->jointInfo[ji].parentIndex,sizeof(int),1,fp);
					fwrite(&anims[i]->jointInfo[ji].flags,sizeof(int),1,fp);
					fwrite(&anims[i]->jointInfo[ji].startIndex,sizeof(int),1,fp);
				}				
				
				//frame bounds
				for (int b = 0; b < anims[i]->numFrames; b++)
				{
					fwrite(&anims[i]->frames[b].min,sizeof(int),3,fp);
					fwrite(&anims[i]->frames[b].max,sizeof(int),3,fp);
				}

				//base joints
				for (int b = 0; b < anims[i]->baseJoints.size(); b++)
				{
					fwrite(&anims[i]->baseJoints[b].pos,sizeof(int),3,fp);
					fwrite(&anims[i]->baseJoints[b].orient,sizeof(int),3,fp);
				}

				//frames
				for (int b = 0; b < anims[i]->frames.size(); b++)
				{
					for (int ac = 0; ac < anims[i]->numAnimatedComponents; ac++)
						fwrite(&anims[i]->frames[b].animatedComponents[ac],sizeof(float),1,fp);
				}

			}


			fclose(fp);

			return true;
		}

		/*void ModelMd5::LoadModel(char *filename)
		{
			FILE *fp;
			char buff[512];
			int version;
			int curr_mesh = 0;
			int i;

			fp = fopen (filename, "rb");
			if (!fp)
			{
				fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
				return ;
			}

			while (!feof (fp))
			{
				// Read whole line 
				fgets (buff, sizeof (buff), fp);

				//get file version
				if (sscanf (buff, " MD5Version %d", &version) == 1)
				{
					if (version != 10)
					{
						// Bad version 
						fprintf (stderr, "Error: bad model version\n");
						fclose (fp);
						return ;
					}
				}
				//number of joints
				else if (sscanf (buff, " numJoints %d", &numJoints) == 1)
				{
					if (numJoints > 0)
					{
						// Allocate memory for base skeleton joints 
						printf("joints number: %d\n",numJoints);
					}
				}
				//number of meshes
				else if (sscanf (buff, " numMeshes %d", &numMeshes) == 1)
				{
					if (numMeshes > 0)
					{
						printf("meshes number: %d\n",numMeshes);
					}
				}
				//load joints
				else if (strncmp (buff, "joints {", 8) == 0)
				{
					// Read each joint 
					for (i = 0; i < numJoints; ++i)
					{
						Joint joint;

						//Read whole line 
						fgets (buff, sizeof (buff), fp);

						float quat[3];
						char name[64];

						if (sscanf (buff, "%s %d ( %f %f %f ) ( %f %f %f )",
							name, &joint.parent, &joint.pos[0],
							&joint.pos[1], &joint.pos[2], &quat[0],
							&quat[1], &quat[2]) == 8)
						{
							//* Compute the w component 
							//printf("cos..\n");
						}

						joint.name = name;

						// read in index of parent joint
						int parentIndex = joint.parent;
						joint.orient[0] = quat[0];
						joint.orient[1] = quat[1];
						joint.orient[2] = quat[2];

						joint.quat = buildQuat(quat[0], quat[1], quat[2]);

						// add index to parent's list of child joints (if it has a parent,
						// root joints have parent index -1)
						if ( parentIndex >= 0 )
						{
							if ( size_t(parentIndex) >= joints.size() )
								return;

							joints[parentIndex].children.push_back( int(joints.size()) );
						}

						// add joint to vector of joints
						joints.push_back(joint);
					}
				}
				//load meshes
				else if (strncmp (buff, "mesh {", 6) == 0)
				{
					char nameBuff[50];

					int numverts = 0;
					int numtris = 0;

					int vertIndex = 0;
					float vertPos[2];
					int vertsIndex[2];

					int triIndex = 0;
					int triangles[3];

					int num_weights = 0;

					int weightIndex = 0;
					int weightjoint = 0;
					float weightw = 0;
					float weightpos[3];

					if ( numMeshes <= 0 )
					{
						printf("numMeshes needs to be set before 'mesh' block\n");
						return;
					}

					Mesh *mesh = new Mesh;

					while ((buff[0] != '}') && !feof (fp))
					{
						// Read whole line
						fgets (buff, sizeof (buff), fp);

						//shader
						if (strstr (buff, "shader "))
						{
							int quote = 0, j = 0;
							std::string shader;

							// Copy the shader name whithout the quote marks 
							for (i = 0; i < sizeof (buff) && (quote < 2); ++i)
							{
								if (buff[i] == '\"')
									quote++;

								if ((quote == 1) && (buff[i] != '\"'))
								{
									shader += buff[i];
									j++;
								}
							}

							mesh->texture = shader;

							//texture loading
							if (strcmp(shader.c_str(),"")!=0)
							{
								std::string pathName(filename);
								size_t found = pathName.find_last_of("/");
								pathName = pathName.substr(0,found) + "/" + shader;

								TextureManager::Instance()->loadImageFromFile(pathName);
								mesh->textureNumber  = TextureManager::Instance()->getImage(pathName)->_id;
							}
						}

						//name
						else if(strstr (buff, "name "))
						{
							int quote = 0, j = 0;
							std::string name;

							//Copy  name whithout the quote marks 
							for (i = 0; i < sizeof (buff) && (quote < 2); ++i)
							{
								if (buff[i] == '\"')
									quote++;

								if ((quote == 1) && (buff[i] != '\"'))
								{
									name += buff[i];
									j++;
								}
							}

							mesh->name = name;
						}
						//numverts
						else if (sscanf (buff, " numverts %d", &numverts) == 1)
						{
							if (numverts <= 0)
							{
								printf("Zla ilosc wertexow!\n");
								return;
							}

							mesh->verts.resize(numverts);

						}
						//numtris
						else if (sscanf (buff, " numtris %d", &numtris) == 1)
						{
							if (numtris <= 0)
							{
								printf("Zla ilosc trisow!\n");
								return;
							}

							mesh->tris.resize(numtris);
						}
						//numweight
						else if (sscanf (buff, " numweights %d", &num_weights) == 1)
						{
							if (num_weights <= 0)
							{
								printf("Zla ilosc weightow!\n");
								return;
							}

							mesh->weights.resize(num_weights);
						}
						//vertex
						else if (sscanf (buff, " vert %d ( %f %f ) %d %d", &vertIndex,
							&vertPos[0], &vertPos[1], &vertsIndex[0], &vertsIndex[1]) == 5)
						{
							// Copy vertex data 
							Vertex vert;

							vert.tc[0] = vertPos[0];
							vert.tc[1] = vertPos[1];
							vert.weightIndex = vertsIndex[0];
							vert.weightCount = vertsIndex[1];

							mesh->verts[vertIndex] = vert;
						}
						//triangles
						else if (sscanf (buff, " tri %d %d %d %d", &triIndex,
							&triangles[0], &triangles[1], &triangles[2]) == 4)
						{
							Tri tri;

							tri.v[0] = triangles[0];
							tri.v[1] = triangles[1];
							tri.v[2] = triangles[2];
							mesh->tris[triIndex] = tri;
						}
						//weighta
						else if (sscanf (buff, " weight %d %d %f ( %f %f %f )",
							&weightIndex, &weightjoint, &weightw,
							&weightpos[0], &weightpos[1], &weightpos[2]) == 6)
						{
							Weight weight;

							weight.joint = weightjoint;
							weight.w = weightw;
							weight.pos[0] = weightpos[0];
							weight.pos[1] = weightpos[1];
							weight.pos[2] = weightpos[2];

							mesh->weights[weightIndex] = weight;
						}


					}
					meshes.push_back(mesh);
				}

			}
			fclose (fp);
		}*/

		void ModelMd5::optimizeMesh()
		{
			for ( size_t i=0; i < meshes.size(); i++ )
			{
				//optimize vertex number
				for ( size_t j=0; j < meshes[i]->verts.size(); j++ )
				{
					Vertex &v = meshes[i]->verts[j];
					int test = meshes[i]->getTempVertexNumber(v);
					if (test == -1)
					{
						meshes[i]->tempverts.push_back(v);
					}

				}

				//rewrite triangles info
				for ( size_t t=0; t < meshes[i]->tris.size(); t++ )
				{
					Tri newTri;
					Tri &tr = meshes[i]->tris[t];
					Vertex &oldOne1 = meshes[i]->verts[tr.v[0]];

					int test1 = meshes[i]->getTempVertexNumber(oldOne1);
					if (test1 != -1)
					{
						newTri.v[0] = test1;
					}

					Vertex &oldOne2 = meshes[i]->verts[tr.v[1]];
					int test2 = meshes[i]->getTempVertexNumber(oldOne2);
					if (test2 != -1)
					{
						newTri.v[1] = test2;
					}

					Vertex &oldOne3 = meshes[i]->verts[tr.v[2]];
					int test3 = meshes[i]->getTempVertexNumber(oldOne3);
					if (test3 != -1)
					{
						newTri.v[2] = test3;
					}

					meshes[i]->temptris.push_back(newTri);

				}

				//rewrite vertex and triangles from temptooriginal ones containers....
				meshes[i]->verts.clear();
				meshes[i]->verts.resize(meshes[i]->tempverts.size());
				for ( size_t t=0; t < meshes[i]->tempverts.size(); t++ )
				{
					meshes[i]->verts[t] = meshes[i]->tempverts[t];
				}



				meshes[i]->tris.clear();
				meshes[i]->tris.resize(meshes[i]->temptris.size());
				for ( size_t t=0; t < meshes[i]->temptris.size(); t++ )
				{
					meshes[i]->tris[t] = meshes[i]->temptris[t];
				}

				//optimieze weights number
				int numer = 0;
				for (unsigned int w = 0;w < meshes[i]->verts.size();w++)
				{
					numer = meshes[i]->verts[w].weightIndex;
					int tempsize = meshes[i]->tempweights.size();
					for (int wagi = 0;wagi < meshes[i]->verts[w].weightCount;wagi++)
					{
						meshes[i]->tempweights.push_back(meshes[i]->weights[numer + wagi]);
					}
					meshes[i]->verts[w].weightIndex = tempsize;
				}

				meshes[i]->weights.clear();
				meshes[i]->weights.resize(meshes[i]->tempweights.size());
				for ( size_t t=0; t < meshes[i]->tempweights.size(); t++ )
				{
					meshes[i]->weights[t] = meshes[i]->tempweights[t];
				}


			}
		}

		/*int ModelMd5::LoadAnimation(const char *filename)
		{
			FILE *fp;
			char buff[512];
			int version;
			int curr_mesh = 0;
			int n;
			int frameIndex = 0;

			fp = fopen (filename, "rb");
			if (!fp)
			{
				fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
				return -1;
			}

			Anim *anim = new Anim;

			while (!feof (fp))
			{
				//Read whole line 
				fgets (buff, sizeof (buff), fp);

				//modelversion
				if (sscanf (buff, " MD5Version %d", &version) == 1)
				{
					if (version != 10)
					{
						// Bad version
						fprintf (stderr, "Error: bad animation version\n");
						fclose (fp);
						return -1;
					}
				}

				//numFrames
				else if (sscanf (buff, " numFrames %d", &anim->numFrames) == 1)
				{
					if (anim->numFrames <= 0)
					{
						//throw Exception("ModelMd5::readNumFramesEl(): numFrames must be greater than 0");
						return -1;
					}
					anim->frames.resize(anim->numFrames);
				}
				//numjoints
				else if (sscanf (buff, " numJoints %d", &n) == 1)
				{
					if (n != numJoints)
					{
						//throw Exception("ModelMd5::readAnimElements(): anim file does not match mesh");
						return -1;
					}
				}

				//framerate
				else if (sscanf (buff, " frameRate %d", &n) == 1)
				{
					//if ( anim->frameRate != 0 )
						//throw Exception("ModelMd5::readFrameRateEl(): frameRate has already been set");

					//if ( n <= 0 )
						//throw Exception("ModelMd5::readFrameRateEl(): frameRate must be a positive integer");

					anim->frameRate =  n;
				}
				//animated components
				else if (sscanf (buff, " numAnimatedComponents %d", &n) == 1)
				{
					//if ( anim->numAnimatedComponents != 0 )
						//throw Exception("ModelMd5::readNumAnimatedComponentsEl(): numAnimatedComponents has already been set");

					//if ( n <= 0 )
						//throw Exception("ModelMd5::readNumAnimatedComponentsEl(): numAnimatedComponents must be a positive integer");

					anim->numAnimatedComponents = n;
				}
				//hierarchy
				else if (strncmp (buff, "hierarchy {", 11) == 0)
				{
					anim->jointInfo.clear();
					for (int i = 0; i < numJoints; ++i)
					{
						//Read whole line 
						fgets (buff, sizeof (buff), fp);

						JointInfo info;
						char name[40];

						//Read joint info 
						sscanf (buff, " %s %d %d %d", &name, &info.parentIndex,&info.flags, &info.startIndex);
						int quote = 0, j = 0;
						std::string shader;

						//Copy the shader name whithout the quote marks
						for (j = 0; i < sizeof (buff) && (quote < 2); ++j)
						{
							if (buff[j] == '\"')
								quote++;

							if ((quote == 1) && (buff[j] != '\"'))
							{
								info.name += buff[j];
							}
						}

						anim->jointInfo.push_back(info);
					}

					//if ( int(anim->jointInfo.size()) != numJoints )
						//throw Exception("ModelMd5::readHierarchyEl(): number of entires in hierarchy block does not match numJoints");

				}

				//bounds
				else if (strncmp (buff, "bounds {", 8) == 0)
				{
					for (int i = 0; i < anim->numFrames; ++i)
					{
						//Read whole line
						fgets (buff, sizeof (buff), fp);

						//Read bounding box 
						sscanf (buff, " ( %f %f %f ) ( %f %f %f )",
							&anim->frames[i].min[0], &anim->frames[i].min[1],
							&anim->frames[i].min[2], &anim->frames[i].max[0],
							&anim->frames[i].max[1], &anim->frames[i].max[2]);
					}
				}

				//baseframe
				else if (strncmp (buff, "baseframe {", 10) == 0)
				{
					anim->baseJoints.resize(numJoints);

					for (int i = 0; i < numJoints; ++i)
					{
						//Read whole line
						fgets (buff, sizeof (buff), fp);
						float quat[3];

						//Read base frame joint
						if (sscanf (buff, " ( %f %f %f ) ( %f %f %f )",
							&anim->baseJoints[i].pos[0], &anim->baseJoints[i].pos[1],
							&anim->baseJoints[i].pos[2], &quat[0],
							&quat[1], &quat[2]) == 6)
						{
							///Compute the w component 
							anim->baseJoints[i].orient[0] = quat[0];
							anim->baseJoints[i].orient[1] = quat[1];
							anim->baseJoints[i].orient[2] = quat[2];
							anim->baseJoints[i].quat = buildQuat(quat[0], quat[1], quat[2]);
						}
					}
				}

				//frames
				else if (sscanf (buff, " frame %d", &frameIndex) == 1)
				{
					//if ( frameIndex < 0 || frameIndex >= anim->numFrames )
						//throw Exception("ModelMd5::readFrameEl(): invalid frame index");

					// get reference to frame and set number of animated components
					Frame &frame = anim->frames[frameIndex];
					frame.animatedComponents.resize(anim->numAnimatedComponents);

					//Read frame data 
					for (int i = 0; i < anim->numAnimatedComponents; ++i)
						fscanf (fp, "%f", &frame.animatedComponents[i]);

				}


			}

			fclose(fp);


			// add to array of animations
			int animIndex = (int)anims.size();
			anims.push_back(anim);

			// build frames for this animation
			buildFrames(*anim);

			// make this the current animation
			SetAnimation(animIndex, 0);

			return animIndex;
		}*/

		void ModelMd5::SetAnimation(int animIndex, int frameIndex)
		{
			if ( currAnim != animIndex )
			{
				currAnim = animIndex;
				SetFrame(frameIndex);
			}
		}

		void ModelMd5::SetFrame(int frameIndex)
		{
			BuildVerts(anims[currAnim]->frames[frameIndex]);
			buildNormals();
			currFrame = frameIndex;
			animTime = 0.0f;
		}

		void ModelMd5::UpdateAnimation(float dt)
		{ 
			Anim *anim = anims[currAnim];

			// compute index of next frame
			int nextFrameIndex = currFrame >= anim->numFrames - 1 ? 0 : currFrame + 1;

			// update animation time
			animTime += dt*float(anim->frameRate);
			if ( animTime > 1.0f )
			{
				while ( animTime > 1.0f )
					animTime -= 1.0f;
				//setFrame(nextFrameIndex);
				currFrame = nextFrameIndex;
				nextFrameIndex = currFrame >= anim->numFrames - 1 ? 0 : currFrame + 1;
			}

			// make sure size of storage for interpolated frame is correct
			if ( int(interpFrame.joints.size()) != numJoints )
				interpFrame.joints.resize(numJoints);

			///// now interpolate between the two frames /////

			Frame &frame     = anim->frames[currFrame],
				&nextFrame = anim->frames[nextFrameIndex];

			// interpolate between the joints of the current frame and those of the next
			// frame and store them in interpFrame
			for ( int i=0; i < numJoints; i++ )
			{
				Joint &interpJoint = interpFrame.joints[i];

				// linearly interpolate between joint positions
				float *pos1 = frame.joints[i].pos,
					*pos2 = nextFrame.joints[i].pos;
				interpJoint.pos[0] = pos1[0] + animTime*(pos2[0] - pos1[0]);
				interpJoint.pos[1] = pos1[1] + animTime*(pos2[1] - pos1[1]);
				interpJoint.pos[2] = pos1[2] + animTime*(pos2[2] - pos1[2]);

				interpJoint.quat = slerp(frame.joints[i].quat, nextFrame.joints[i].quat, animTime);

				interpJoint.matrix = interpJoint.quat.toMatrix4() * Matrix4::createTranslate(interpJoint.pos[0],interpJoint.pos[1],interpJoint.pos[2]);
				interpJoint.matrix *=joints[i].matrix;
			}

			BuildVerts(interpFrame);
			buildNormals();
		}

		Mesh* ModelMd5::GetMesh(std::string name)
		{
			for( size_t i=0; i < meshes.size(); i++ )
			{
				if(meshes[i]->name == name)
					return meshes[i];
			}

			return 0;
		}

		void ModelMd5::SetPosition(Vector3 pos)
		{
			_position = pos;
		}

		void ModelMd5::SetRotation(Vector3 rotation)
		{
			_rotation = rotation;
		}

		void ModelMd5::FaceDirection(Vector2 direction)
		{
			float perpDot = _dirFacing.x * direction.y - _dirFacing.y * direction.x;
			_rotation.z = atan2(perpDot,Vector2::dot(_dirFacing,direction));

			_rotation.z *= (-180.0f / 3.14159f);			
		}

		void ModelMd5::Draw()
		{
			for( size_t i=0; i < meshes.size(); i++ )
			{
				Mesh *mesh = meshes[i];

				if(mesh->isVisible)
				{
					RenderManager::Instance()->bindTexture(mesh->texture);
					mesh->_vertexBuffer.Draw();
				}
			}
			/*glColor3f(1,1,1);
			glPushAttrib(GL_CURRENT_BIT);
			glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glEnable(GL_TEXTURE_2D);

			glPushMatrix();
			glTranslatef(_position.x,_position.y,_position.z);

			glRotatef(_rotation.x,1,0,0);
			glRotatef(_rotation.y,0,1,0);
			glRotatef(_rotation.z,0,0,1);

			for( size_t i=0; i < meshes.size(); i++ )
			{
				const Mesh *mesh = meshes[i];

				if(mesh->isVisible)
				{
					//~drakon
					glBindTexture(GL_TEXTURE_2D, mesh->textureNumber);

					glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(Vertex)), mesh->verts[0].pos);
					glNormalPointer(GL_FLOAT, GLsizei(sizeof(Vertex)), mesh->verts[0].n);
					glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(Vertex)), mesh->verts[0].tc);

					glDrawElements(GL_TRIANGLES, GLsizei(mesh->tris.size()*3), GL_UNSIGNED_INT, &mesh->tris[0]);
				}				
			}

			glDisable(GL_TEXTURE_2D);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glPopMatrix();

			glPopClientAttrib();
			glPopAttrib();*/
		}

		void ModelMd5::BuildVerts()
		{
			for ( size_t i=0; i < meshes.size(); i++ )
			{
				for ( size_t j=0; j < meshes[i]->verts.size(); j++ )
				{
					if(!meshes[i]->isVisible)
						continue;

					Vertex &v = meshes[i]->verts[j];
					v.pos[0] = v.pos[1] = v.pos[2] = 0.0;

					for ( int k=0; k < v.weightCount; k++ )
					{
						Weight &w = meshes[i]->weights[v.weightIndex + k];
						Joint &joint = joints[w.joint];

						Quaternion q(0.0f,w.pos[0], w.pos[1], w.pos[2]);
						Quaternion result = joint.quat.conjugate() * q *joint.quat;

						v.pos[0] += (joint.pos[0] + result.x)*w.w;
						v.pos[1] += (joint.pos[1] + result.y)*w.w;
						v.pos[2] += (joint.pos[2] + result.z)*w.w;

					} // for (weights)

					//revrite vertex to bind pos
					v.binPos.x = v.pos[0];
					v.binPos.y = v.pos[1];
					v.binPos.z = v.pos[2];

				} // for (mesh vertices)
			} // for (meshes)
		}

		void ModelMd5::BuildVerts(Frame &frame)
		{
			for ( size_t i=0; i < meshes.size(); i++ )
			{
				for ( size_t j=0; j < meshes[i]->verts.size(); j++ )
				{
					if(!meshes[i]->isVisible)
						continue;

					Vertex &v = meshes[i]->verts[j];
					v.pos[0] = v.pos[1] = v.pos[2] = 0.0;

					for ( int k=0; k < v.weightCount; k++ )
					{
						Weight &w = meshes[i]->weights[v.weightIndex + k];
						Joint &joint = frame.joints[w.joint];

						//test += joint.matrix * w.w;

						Quaternion q(0.0f,w.pos[0], w.pos[1], w.pos[2]);
						Quaternion result = joint.quat.conjugate() * q *joint.quat;

						v.pos[0] += (joint.pos[0] + result.x)*w.w;
						v.pos[1] += (joint.pos[1] + result.y)*w.w;
						v.pos[2] += (joint.pos[2] + result.z)*w.w;

					} // for (weights)
				} // for (mesh vertices)
			} // for (meshes)
		}

		void ModelMd5::buildNormals()
		{
			// zero out the normals
			for ( size_t i=0; i < meshes.size(); i++) 
			{
				if(!meshes[i]->isVisible)
					continue;

				for ( size_t j=0; j < meshes[i]->verts.size(); j++ ) 
				{
					meshes[i]->verts[j].n[0] = 0.0;
					meshes[i]->verts[j].n[1] = 0.0;
					meshes[i]->verts[j].n[2] = 0.0;
				}

				// for each normal, add contribution to normal from every face that vertex
				// is part of
				for ( size_t j=0; j < meshes[i]->tris.size(); j++)
				{
					Vertex &v0 = meshes[i]->verts[ meshes[i]->tris[j].v[0] ];
					Vertex &v1 = meshes[i]->verts[ meshes[i]->tris[j].v[1] ];
					Vertex &v2 = meshes[i]->verts[ meshes[i]->tris[j].v[2] ];

					float Ax = v1.pos[0] - v0.pos[0];
					float Ay = v1.pos[1] - v0.pos[1];
					float Az = v1.pos[2] - v0.pos[2];

					float Bx = v2.pos[0] - v0.pos[0];
					float By = v2.pos[1] - v0.pos[1];
					float Bz = v2.pos[2] - v0.pos[2];

					float nx = Ay * Bz - By * Az;
					float ny = -(Ax * Bz - Bx * Az);
					float nz = Ax * By - Bx * Ay;

					v0.n[0] += nx;
					v0.n[1] += ny;
					v0.n[2] += nz;

					v1.n[0] += nx;
					v1.n[1] += ny;
					v1.n[2] += nz;

					v2.n[0] += nx;
					v2.n[1] += ny;
					v2.n[2] += nz;
				}

				// normalize each normal
				for ( size_t j=0; j < meshes[i]->verts.size(); j++ ) 
				{
					Vertex &v = meshes[i]->verts[j];

					float mag = (float)sqrt( float(v.n[0]*v.n[0] + v.n[1]*v.n[1] + v.n[2]*v.n[2]) );

					// avoid dividing by zero
					if ( mag > 0.0001f ) 
					{
						v.n[0] /= mag;
						v.n[1] /= mag;
						v.n[2] /= mag;
					}
				}
			} // for (meshes)
		}

		void ModelMd5::buildFrames(Anim &anim)
		{ 
			for ( int i=0; i < anim.numFrames; i++ ) 
			{
				// allocate storage for joints for this frame
				anim.frames[i].joints.resize(numJoints);

				for ( int j=0; j < numJoints; j++ ) 
				{
					const Joint &joint = anim.baseJoints[j];

					float pos[3]    = { joint.pos[0],  joint.pos[1],  joint.pos[2]  };
					float orient[3] = { joint.quat.x, joint.quat.y, joint.quat.z };

					int n = 0;
					for ( int k=0; k < 3; k++ )
					{
						if ( anim.jointInfo[j].flags & (1 << k)  )
						{
							pos[k] = anim.frames[i].animatedComponents[anim.jointInfo[j].startIndex + n];
							n++;
						}
					}

					for ( int k=0; k < 3; k++ )
					{
						if ( anim.jointInfo[j].flags & (8 << k)  ) 
						{
							orient[k] = anim.frames[i].animatedComponents[anim.jointInfo[j].startIndex + n];
							n++;
						}
					}

					Quaternion q = buildQuat(orient[0], orient[1], orient[2]);

					Joint &frameJoint = anim.frames[i].joints[j];
					frameJoint.name   = anim.jointInfo[j].name;
					frameJoint.parent = anim.jointInfo[j].parentIndex;

					// root joint?
					if ( anim.jointInfo[j].parentIndex < 0 )
					{
						frameJoint.pos[0] = pos[0];
						frameJoint.pos[1] = pos[1];
						frameJoint.pos[2] = pos[2];
						frameJoint.quat = q;
					}
					else 
					{
						Joint &parent = anim.frames[i].joints[anim.jointInfo[j].parentIndex];

						// rotate position (qp is quaternion representation of position)
						Quaternion qp(0.0f,pos[0], pos[1], pos[2]);
						Quaternion result = parent.quat.conjugate()*qp * parent.quat;

						frameJoint.pos[0] = result.x + parent.pos[0];
						frameJoint.pos[1] = result.y + parent.pos[1];
						frameJoint.pos[2] = result.z + parent.pos[2];

						// store orientation of this joint
						frameJoint.quat = q *parent.quat;
						frameJoint.quat.normalize();
					} // else
				} // for
			} // for
		}

		Quaternion ModelMd5::buildQuat(float x, float y, float z) const
		{
			// compute the 4th component of the quaternion
			float w = 1.0f - x*x - y*y - z*z;
			w = w < 0.0 ? 0.0f : (float)-sqrt( double(w) );

			Quaternion q(w,x, y, z);
			q.normalize();

			return q;
		}

		Quaternion ModelMd5::slerp(const Quaternion &q1, const Quaternion &q2, float t)
		{
			Quaternion result, _q2 = q2;

			float cosOmega = q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;

			if ( cosOmega < 0.0f )
			{
				_q2.x = -_q2.x;
				_q2.y = -_q2.y;
				_q2.z = -_q2.z;
				_q2.w = -_q2.w;
				cosOmega = -cosOmega;
			}

			float k0, k1;
			if ( cosOmega > 0.99999f )
			{
				k0 = 1.0f - t;
				k1 = t;
			}
			else
			{
				float sinOmega = (float)sqrt( double(1.0f - cosOmega*cosOmega) );
				float omega = (float)atan2( double(sinOmega), double(cosOmega) );

				float invSinOmega = 1.0f/sinOmega;

				k0 = float( sin(double((1.0f - t)*omega)) )*invSinOmega;
				k1 = float( sin(double(t*omega)) )*invSinOmega;
			}

			result.x = q1.x*k0 + _q2.x*k1;
			result.y = q1.y*k0 + _q2.y*k1;
			result.z = q1.z*k0 + _q2.z*k1;
			result.w = q1.w*k0 + _q2.w*k1;

			return result;
		}

		Anim::Anim() : numFrames(0),frameRate(0),numAnimatedComponents(0)
		{

		}
	}
}