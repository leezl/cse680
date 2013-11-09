/*
mesh.cpp

in charge of vertices, materials, normals...
connected to physics, since physics needs these too

*/
#include "mesh.h"

Mesh::Mesh(std::string nameG) {
	name = nameG;
	meshes.push_back(SubMesh());
    totalVerts=0;
    totalTriangles=0;

	//lame cube
    meshes[0].indices.reserve(36);
    meshes[0].vertices.reserve(24);
    //meshes[0].normals.reserve(24);
    meshes[0].hasMaterials=false;
    meshes[0].hasTextures=false;

    //vertices
    meshes[0].vertices.push_back(glm::vec3(1.0f,1.0f,1.0f));//0 //right, upper, front
    meshes[0].vertices.push_back(glm::vec3(-1.0f,1.0f,1.0f));//1 //left, upper, front
    meshes[0].vertices.push_back(glm::vec3(-1.0f,-1.0f,1.0f));//2 //left, lower, front
    meshes[0].vertices.push_back(glm::vec3(-1.0f,1.0f,-1.0f));//3 //left, upper, back
    meshes[0].vertices.push_back(glm::vec3(1.0f,1.0f,-1.0f));//4 //right, upper, back
    meshes[0].vertices.push_back(glm::vec3(1.0f,-1.0f,-1.0f));//5 //right, lower, back
    meshes[0].vertices.push_back(glm::vec3(1.0f,-1.0f,1.0f));//6 //right, lower, front
    meshes[0].vertices.push_back(glm::vec3(-1.0f,-1.0f,-1.0f));//7 //left, lower, back

    //indices
    meshes[0].indices.push_back(3);//back //d
    meshes[0].indices.push_back(4);
    meshes[0].indices.push_back(5);
    meshes[0].indices.push_back(3);
    meshes[0].indices.push_back(5);
    meshes[0].indices.push_back(7);
    //
    meshes[0].indices.push_back(0);//front
    meshes[0].indices.push_back(1);
    meshes[0].indices.push_back(2);
    meshes[0].indices.push_back(0);
    meshes[0].indices.push_back(2);
    meshes[0].indices.push_back(6);
    //
    meshes[0].indices.push_back(0);//right //d
    meshes[0].indices.push_back(4);
    meshes[0].indices.push_back(5);
    meshes[0].indices.push_back(0);
    meshes[0].indices.push_back(5);
    meshes[0].indices.push_back(6);
    //
    meshes[0].indices.push_back(0);//upper //d
    meshes[0].indices.push_back(1);
    meshes[0].indices.push_back(3);
    meshes[0].indices.push_back(0);
    meshes[0].indices.push_back(3);
    meshes[0].indices.push_back(4);
    //
    meshes[0].indices.push_back(1);//left //d
    meshes[0].indices.push_back(2);
    meshes[0].indices.push_back(3);
    meshes[0].indices.push_back(2);
    meshes[0].indices.push_back(3);
    meshes[0].indices.push_back(7);
    //
    meshes[0].indices.push_back(2);//lower //d
    meshes[0].indices.push_back(5);
    meshes[0].indices.push_back(6);
    meshes[0].indices.push_back(2);
    meshes[0].indices.push_back(5);
    meshes[0].indices.push_back(7);

    //set physics //CHANGE
    //physics[0] = new ObjectPhysics();
    //physics[1] = new ObjectPhysics();
}

Mesh::Mesh(glm::vec3 maxi, glm::vec3 mini, std::string newName) {
    std::cout<<"storing box mesh"<<std::endl;
    name = newName;
    totalVerts=0;
    totalTriangles=0;
	meshes.push_back(SubMesh());

	maxVert = maxi;
	minVert = mini;
	centerVert = (maxi+mini)/glm::vec3(2.0);

	//set physics //NO PHYSICS
	//physics.push_back(NULL);
	//physics.push_back(NULL);
}

Mesh::Mesh(std::string path, std::string filename) {
    std::cout<<"Loading mesh"<<std::endl;
	name = filename;
    totalVerts=0;
    totalTriangles=0;
	//loading object
    if ( !loadAssImp(path, filename) ) {
        printf("Error Loading Object File with Assimp.\n");
        exit(-1);
    }
    //set physics //CHANGE
    //physics.push_back(new ObjectPhysics());
    //physics.push_back(new ObjectPhysics());
}

Mesh::~Mesh(){
}

bool Mesh::loadAssImp(std::string dir, std::string path){
    //std::cout<<"Loading Object "<<path<<std::endl;
    //need assimp importer
    Assimp::Importer importer;

    // read file into scene object
    // possible flags:
    // triangles, and normal important. others probably won't do much (vertex normals)
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals );//SmoothNormals?
    //aiCopyScene(scener, &scene);
    //check if it worked:
    if( !scene) {
        fprintf( stderr, importer.GetErrorString());
        return false;
    }
    //reserve space for each mesh (and material)
    meshes.reserve(scene->mNumMeshes);
    materials.reserve(scene->mNumMaterials);

    //iterate through each mesh (and mat) and collect
    for (unsigned int j = 0; j<scene->mNumMeshes; j++) {
    	//add a SubMesh for this mesh
    	SubMesh tempSubMesh;
    	meshes.push_back(tempSubMesh);
    	//add stuff straight into that SubMesh: meshes[j]
        const aiMesh* mesh = scene->mMeshes[j];
        //find center, min and max for camera positioning
        std::vector<glm::vec3> tempVec3;
        std::vector<glm::vec4> tempVec4;
        aiVector3D pos;
        //reserve vertice for this SubMesh
        meshes[j].vertices.reserve(mesh->mNumVertices);
        //add to total number for physics object
        totalVerts += mesh->mNumVertices;
        //iterate over vertice and add
        for(unsigned int i=0; i<mesh->mNumVertices; i++) {
            pos = mesh->mVertices[i];
            meshes[j].vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
            //std::cout<<"Vertices:  "<<pos.x<<','<<pos.y<<','<<pos.z<<std::endl;
            //find max and min x y z indep.
            if (pos.x>maxVert[0]) {
                maxVert[0] = pos.x;
            } else if ( pos.x<minVert[0] ) {
                minVert[0] = pos.x;
            }
            if (pos.y>maxVert[1]) {
                maxVert[1] = pos.y;
            }else if ( pos.y<minVert[1] ) {
                minVert[1] = pos.y;
            }
            if (pos.z>maxVert[2]) {
                maxVert[2] = pos.z;
            }else if ( pos.z<minVert[2] ) {
                minVert[2] = pos.z;
            }
        }

        //grab normals
        if (mesh->HasNormals()) {
        	meshes[j].normals.reserve(mesh->mNumVertices);//make sure still reserved
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                pos = mesh->mNormals[i];
                meshes[j].normals.push_back(glm::vec3(pos.x, pos.y, pos.z));
                //std::cout<<"Normals:  "<<pos.x<<','<<pos.y<<','<<pos.z<<std::endl;
            }
        }

        //grab colors
        if ( 0 < mesh->GetNumColorChannels()) {
        	meshes[j].colors.reserve(mesh->mNumVertices);//make sure still reserved
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                aiColor4D color = mesh->mColors[0][i];
                meshes[j].colors.push_back(glm::vec4(color.r, color.g, color.b, color.a));
            }
        }

        //grab uvs
        if (0 < mesh->GetNumUVChannels()) {
            meshes[j].hasTextures = true;
            meshes[j].uvs.reserve(mesh->mNumVertices);
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                pos = mesh->mTextureCoords[0][i];
                //std::cout<<"texture coords "<<pos.x<<','<<pos.y<<','<<pos.z<<std::endl;
                //folowing conversion should not be needed if wrapping textures...
                if (pos.x <0.0) {
                    pos.x = 1.0-pos.x;
                }
                if (pos.y<0.0) {
                    pos.y = 1.0-pos.y;
                }
                meshes[j].uvs.push_back(glm::vec3(pos.x, pos.y, pos.z));
            }
        }

        //grab materialIndices
        if (scene->HasMaterials()) {
            //iterate through materials
            meshes[j].materialIndex = mesh->mMaterialIndex;
            meshes[j].hasMaterials=true;
        }

        //grab flattened indices here
        if ( mesh->HasFaces() ) {
            //std::cout<<"Faces: "<<mesh->mNumFaces<<std::endl;
            meshes[j].indices.reserve(3*mesh->mNumFaces);//all traingulated
            //add to total triangles
            totalTriangles += mesh->mNumFaces;
            for (unsigned int i=0; i<mesh->mNumFaces; i++) {//per face
                // The model should be all triangle since we triangulated
                //std::cout<<"number of indices in face: "<<mesh->mFaces[i].mNumIndices<<std::endl;
                meshes[j].indices.push_back(mesh->mFaces[i].mIndices[0]);
                meshes[j].indices.push_back(mesh->mFaces[i].mIndices[1]);
                meshes[j].indices.push_back(mesh->mFaces[i].mIndices[2]);
                //std::cout<<"Indices in Face: "<<mesh->mFaces[i].mIndices[0]<<','<<mesh->mFaces[i].mIndices[1]<<','<<mesh->mFaces[i].mIndices[2]<<std::endl;
            }
            //std::cout<<"Size of indices for this mesh "<<meshIndices.size()<<std::endl;
        }
    }
    //load materials
    if (scene->HasMaterials()) {
        //iterate through materials
        for (unsigned int i = 0; i<scene->mNumMaterials; i++) {
            Material tempMater;
            materials.push_back(tempMater);
            //store material properties: specular, diffuse, ambient, shine OR Default
            //check diffuse exists
            //std::cout<<"Has materials "<<std::endl;
            aiColor4D diff(0.8f, 0.8f, 0.8f, 1.0f);//default diffuse
            aiMaterial *mtl = scene->mMaterials[i];
            /*for (unsigned int i = 0; i<mtl->mNumProperties; i++) {
                std::cout<<"Property: "<<mtl->mProperties[i]->mKey.C_Str()<<std::endl;
            }
            std::cout<<std::endl;*/
            //shading model : not being used
            int shadingMode=0;
            aiGetMaterialInteger(mtl, AI_MATKEY_SHADING_MODEL, &shadingMode);
            materials[i].shadingMode = shadingMode;
            //refraction: not useful
            float refraction=1.0;
            aiGetMaterialFloat(mtl, AI_MATKEY_SHADING_MODEL, &refraction);
            materials[i].refraction = refraction;
            // texture filename
            //not keeping track of all of them yet...probably just using diffuse right now
            std::vector< aiTextureType > texTypes { aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_AMBIENT };
            for (unsigned int texs=0; texs<texTypes.size(); texs++) {
                //std::cout<<"aiTextureType "<<texTypes[texs]<<std::endl;
                for (unsigned int texSub = 0; texSub<(mtl->GetTextureCount(texTypes[texs])); texSub++) {
                    //before creating texture
                    glActiveTexture(GL_TEXTURE0 + texSub);
                    aiString texFile("");
                    aiGetMaterialString(mtl, AI_MATKEY_TEXTURE(texTypes[texs],texSub), &texFile);
                    //adding textures rather than file names here...
                    if (materials[i].textureFiles.find(texTypes[texs]) == materials[i].textureFiles.end()) {
                        //create empty array at that location
                        std::vector < Texture > ok;
                        materials[i].textureFiles[ texTypes[texs] ] = ok;
                    }
                    std::string whatIsHappening = texFile.C_Str();
                    Texture omgEvilTexture(dir+whatIsHappening); /////////
                    //std::cout<<"adding texture "<<dir+whatIsHappening<<' '<<name<<std::endl;
                    (materials[i].textureFiles[ texTypes[texs] ]).push_back( omgEvilTexture );
                    checkError("after texture creation");
                }
            }
            //loadTextures(temp);
            //opacity
            float opacity=1.0;
            aiGetMaterialFloat(mtl, AI_MATKEY_OPACITY, &opacity);
            diff.a = opacity;
            //using c interface instead of c++ here, because c++ wasn't working...not sure why
            aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diff);
            materials[i].diff[0] = diff.r;
            materials[i].diff[1] = diff.g;
            materials[i].diff[2] = diff.b;
            materials[i].diff[3] = diff.a;
            //std::cout<<"Diffuse: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            diff.r = diff.g = diff.b = 0.2;//default specular
            aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &diff);
            materials[i].spec[0] = diff.r;
            materials[i].spec[1] = diff.g;
            materials[i].spec[2] = diff.b;
            materials[i].spec[3] = diff.a;
            //std::cout<<"Specular: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            diff.r = diff.g = diff.b = 0.1;//default ambient
            aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &diff);
            materials[i].amb[0] = diff.r;
            materials[i].amb[1] = diff.g;
            materials[i].amb[2] = diff.b;
            materials[i].amb[3] = diff.a;
            //std::cout<<"Ambient: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            //shine
            float shiny = 20.0;
            aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shiny);
            //if file was obj: shiniess 0-1000, in opengl 0-128 assume obj for now:
            shiny = (shiny*128.0)/1000.0;
            //why is shininess being returned as color4d, when assimp
            //docs claim it should be a float...?
            materials[i].shine = shiny;
            //std::cout<<"Shine: "<<shiny<<std::endl;
        }
    }
    centerVert = (maxVert + minVert) / glm::vec3(2.0);

    //assume all's well if we make it here
    return true;
}

void Mesh::cleanUp() {    //soooo tedious...must be a better way
    for (unsigned int i=0; i<meshes.size(); i++) {
        if(meshes[i].elementBuffer!=0) {glDeleteBuffers(1, &meshes[i].elementBuffer);}
        if(meshes[i].geometryBuffer!=0) {glDeleteBuffers(1, &meshes[i].geometryBuffer);}
        if(meshes[i].normalBuffer!=0) {glDeleteBuffers(1, &meshes[i].normalBuffer);}
        if(meshes[i].colorBuffer!=0) {glDeleteBuffers(1, &meshes[i].colorBuffer);}
        if(meshes[i].textureBuffer!=0) {glDeleteBuffers(1, &meshes[i].textureBuffer);}
    }
	for (unsigned int i=0; i<materials.size(); i++) {
        for(std::map< aiTextureType, std::vector< Texture> >::iterator iter = materials[i].textureFiles.begin(); iter != materials[i].textureFiles.end(); ++iter) {
            for(std::vector< Texture >::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
                iter2->cleanUp();//I need to fix this deallocation
            }
        }
    }
    physics[0]->cleanUp();
    physics[1]->cleanUp();
}

void Mesh::flipNormals() {
	for (unsigned int i = 0; i<meshes.size(); i++) {
        for (unsigned int j =0; j<meshes[i].normals.size(); j++) {
            meshes[i].normals[j] = -meshes[i].normals[j];
        }
    }
}

bool Mesh::setProgram(std::vector< Program* > programs) {
	//select the program that suits each SubMesh
	for (unsigned int i = 0; i<meshes.size(); i++) {
		//check SubMesh parameters and program Parameters
		if (meshes[i].normals.size()>0 && meshes[i].hasTextures) {
			//check each program
			for (unsigned int j=0; j<programs.size(); j++) {
				if (programs[j]->needsNormals && programs[j]->needsTextures) {
				    meshes[i].program = programs[j]; 
				}
			}
		} else if (meshes[i].normals.size()>0 && !meshes[i].hasTextures) {
			//check each program
			for (unsigned int j=0; j<programs.size(); j++) {
				if (programs[j]->needsNormals && !programs[j]->needsTextures) {
				    meshes[i].program = programs[j]; 
				}
			}
		} else {
			//first program default: cheap, does not check if parameters met
			std::cout<<"ERROR: No matching program for SubMesh. Using default. "<<name<<std::endl;
			meshes[i].program = programs[0];
		}
	}
}

void Mesh::addPhysics(std::string shapeType, std::string motionType, PhysicsWorld* world, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale) {
    std::cout<<"Setting Mesh Physics "<<std::endl;
	physics.push_back( new ObjectPhysics());
	physics[physics.size()-1]->world = world;
	//set COLLISION SHAPE
	//physics[physics.size()-1]->setCollisionShape(shapeType, scale, totalVerts, totalTriangles);
	btCollisionShape* shape;
	if (shapeType == "mesh") {
        //std::cout<<"MESH PHYSICS"<<std::endl;
        int indexStride = 3*sizeof(int);
        int vertStride = sizeof(btVector3);
        std::cout<<totalVerts<<std::endl;
        btVector3* gVertices = new btVector3[totalVerts];
        int* gIndices = new int[totalTriangles*3];
        int indicesSoFar = 0;
        int verticesSoFar = 0;
        //this is so slow...
        for (unsigned int i=0; i<meshes.size(); i++) {
            //appends the indices to the end of our giant index array
            //add offset
            for (unsigned int j = 0; j<meshes[i].indices.size(); j++) {
                if (indicesSoFar+j >= totalTriangles*3) {
                    std::cout<<"Index range mesh error"<<std::endl;
                }
                gIndices[(indicesSoFar+j)] = meshes[i].indices[j]+(verticesSoFar);
            }
            for (unsigned int j = 0; j<meshes[i].vertices.size(); j++) {
                if (verticesSoFar+j >= totalVerts) {
                    std::cout<<"Vertex range mesh error"<<std::endl;
                }
                gVertices[(verticesSoFar+j)].setValue(meshes[i].vertices[j].x, meshes[i].vertices[j].y, meshes[i].vertices[j].z);
            }
            //add offset into gIndices
            indicesSoFar += meshes[i].indices.size();
            verticesSoFar += meshes[i].vertices.size();
        }
        //create the btTrianglemesh
        btTriangleIndexVertexArray* m_indexVertexArrays = new btTriangleIndexVertexArray(
            totalTriangles, gIndices, indexStride,
            totalVerts, (btScalar*) &gVertices[0].x(), vertStride);
        shape = new btBvhTriangleMeshShape(m_indexVertexArrays, true);
    } else if (shapeType == "convex") {
        //Create a COnvex SHape
        btConvexHullShape * m_indexVertexArrays = new btConvexHullShape();
        //create bt arrays of vertices
        //this is so slow...
        for (unsigned int i = 0; i<meshes.size(); i++) {
            for (unsigned int j = 0; j<meshes[i].vertices.size(); j++) {
                m_indexVertexArrays->addPoint(btVector3(meshes[i].vertices[j].x, meshes[i].vertices[j].y, meshes[i].vertices[j].z));
            }
        }
        //btConvexShape* originalConvexShape; is the original convexHullShape
        //create a hull approximation
        btShapeHull* hull = new btShapeHull(m_indexVertexArrays);
        btScalar margin = m_indexVertexArrays->getMargin();
        hull->buildHull(margin);
        shape = new btConvexHullShape(hull->getVertexPointer()[0],hull->numVertices());
    } else if (shapeType == "cylinderY") {
        //Assume height in y direction
        //make cylinder: need radius, height (and center needed)
        float height = maxVert[1]-minVert[1]; //glm::distance(glm::vec3(center[0],center[1],center[2]) , glm::vec3(max[0], max[1], max[2]));
        float radius = glm::distance(glm::vec3(centerVert[0], centerVert[1], centerVert[2]), glm::vec3(minVert[0], centerVert[1], minVert[2]));
        //set cylinder: why 2 radii? No answer
        shape = new btCylinderShape(btVector3(height, radius, radius));
    } else if (shapeType == "cylinderZ") {
        //Assume height in y direction
        //make cylinder: need radius, height (and center needed)
        float height = maxVert[2]-minVert[2]; //glm::distance(glm::vec3(center[0],center[1],center[2]) , glm::vec3(max[0], max[1], max[2]));
        float radius = glm::distance(glm::vec3(centerVert[0], centerVert[1], centerVert[2]), glm::vec3(minVert[0], minVert[1], centerVert[2]));
        //set cylinder: why 2 radii? No answer
        shape = new btCylinderShapeZ(btVector3(height, radius, radius));
    } else if (shapeType == "cylinderX") {
        //Assume height in y direction
        //make cylinder: need radius, height (and center needed)
        float height = maxVert[0]-minVert[0]; //glm::distance(glm::vec3(center[0],center[1],center[2]) , glm::vec3(max[0], max[1], max[2]));
        float radius = glm::distance(glm::vec3(centerVert[0], centerVert[1], centerVert[2]), glm::vec3(centerVert[0], minVert[1], minVert[2]));
        //set cylinder: why 2 radii? No answer
        shape = new btCylinderShapeX(btVector3(height, radius, radius));
    } else if (shapeType == "sphere") {
        //Create a SPHERE
        //match sphere type to shape (find center and any other vertex to get radius)
        float radius = glm::distance(glm::vec3(centerVert[0],centerVert[1],centerVert[2]) , meshes[0].vertices[0]); 
        shape = new btSphereShape(radius);
    } else {
        //Create a radius 1 Sphere by default
        //what other types do we want? default sphere? cube?
        std::cout<<"ERROR: Unrecognized Collision Shape. "<<std::endl;
        shape = new btSphereShape(1);
    }
     //SET nextScale OF OBJECTS
    physics[physics.size()-1]->setCollisionShape(shape, scale);

    //set MOTION STATE (now done in rigidBody setter)
    //physics[physics.size()-1].setMotionState(nextTranslate, nextRotate);

    //set RIGIDBODY
    physics[physics.size()-1]->setRigidBody(translate, rotate, motionType);
}

void Mesh::resetPhysics(glm::mat4 model) {
	//for each physics object reset
	for (unsigned int i=0; i<physics.size(); i++) {
		physics[i]->resetMotion(model);
	}
}

void Mesh::disablePhysics() {
	//for each physics object reset
	for (unsigned int i=0; i<physics.size(); i++) {
		physics[i]->removeFromWorld();
	}
}

void Mesh::enablePhysics() {
	//for each physics object reset
	for (unsigned int i=0; i<physics.size(); i++) {
		physics[i]->addToWorld();
	}
}

void Mesh::initializeBuffers() {
	GLuint spare;
	for (unsigned int i=0; i<meshes.size(); i++) {
		if ( meshes[i].indices.size()>0 ) {
            //std::cout<<"Setting Element Buffer "<<std::endl;
            //set element buffer data
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spare);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[i].indices.size() * sizeof(unsigned int), &meshes[i].indices[0], GL_STATIC_DRAW);
            meshes[i].elementBuffer=spare; 
            //checkError("setting element data");
        } else {
            meshes[i].elementBuffer=0;
        }
        //set vertices
        if (meshes[i].vertices.size()>0) {
            glEnableClientState(GL_VERTEX_ARRAY);
            //std::cout<<"Setting Vertex Buffer"<<std::endl;
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //std::cout<<"number of vertices "<<vertices[j].size()<<std::endl;
            glBufferData(GL_ARRAY_BUFFER, meshes[i].vertices.size() * sizeof(glm::vec3), &meshes[i].vertices[0], GL_STATIC_DRAW);
            meshes[i].geometryBuffer=spare;
            //checkError("setting vertex data");
        } else {
            meshes[i].geometryBuffer=0;
        }
        //check for normals
        if (meshes[i].normals.size()>0) {
            //std::cout<<"Setting Normals Buffer "<<std::endl;
            glGenBuffers(1, &spare);
            //std::cout<<"normal buffer "<<spare<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //std::cout<<"number of normals "<<normals[j].size()<<std::endl;
            glBufferData(GL_ARRAY_BUFFER, meshes[i].normals.size() * sizeof(glm::vec3), &meshes[i].normals[0], GL_STATIC_DRAW);
            meshes[i].normalBuffer=spare;
        } else {
            meshes[i].normalBuffer=0;
        }
                //check for uvs
        if (meshes[i].uvs.size()>0) {
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //not prepared to handle several textures.
            glBufferData(GL_ARRAY_BUFFER, meshes[i].uvs.size() * sizeof(glm::vec3), &meshes[i].uvs[0], GL_STATIC_DRAW);
            meshes[i].textureBuffer=spare;
        } else {
            meshes[i].textureBuffer=0;
        }
        /*
		if ( meshes[i].colors.size()>0) {
            //std::cout<<"Setting Colors Buffer"<<std::endl;
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //not prepared to handle several colors either...
            glBufferData(GL_ARRAY_BUFFER, meshes[i].colors.size() * sizeof(glm::vec4), &meshes[i].colors[0], GL_STATIC_DRAW);
            meshes[i].colorBuffer=spare;
            checkError("setting color buffer");
        } else if (materials.size()>0) {
            //nothing here, more in draw
            meshes[i].colorBuffer=0;
        } else {
            //glDisableClientState{GL_COLOR_ARRAY};
            meshes[i].colorBuffer=0;
        }
        */
	}
}

void Mesh::drawMeshes(glm::mat4 model) {
	for (unsigned int i=0; i<meshes.size(); i++) {
		if (meshes[i].program->view==NULL || meshes[i].program->projection==NULL) {
	        std::cerr<<"No view, projection or light set for program."<<std::endl;
	    }
        //std::cout<<"Drawing Meshes "<<name<<std::endl;
		meshes[i].program->startProgram();
        checkError("starting program");
        //std::cout<<"Setting Model"<<std::endl;
        meshes[i].program->setModelView(&model, NULL);
		//meshes[i].program->setModel(&model);
        //std::cout<<"Setting View"<<std::endl;
		//meshes[i].program->setView(NULL);
        checkError("after setting view");
        //std::cout<<"Setting Projection"<<std::endl;
	    meshes[i].program->setProjection(NULL);
        //std::cout<<"Setting Lights"<<std::endl;
	    meshes[i].program->setLights();
        //std::cout<<"Done with lights"<<std::endl;
        checkError("after lighting updated");

	    //prepare materials
	    if (meshes[i].materialIndex>=0 && materials.size()>meshes[i].materialIndex) {
            Material currentMat = materials[meshes[i].materialIndex];
            //Light calculations as necessary
            //std::cout<<"before materials "<<name <<std::endl;
            //switched here to upload material not products
            glUniform4fv(meshes[i].program->materialPosition.locs[0], 1, glm::value_ptr(currentMat.amb));
            glUniform4fv(meshes[i].program->materialPosition.locs[1], 1, glm::value_ptr(currentMat.diff));
            glUniform4fv(meshes[i].program->materialPosition.locs[2], 1, glm::value_ptr(currentMat.spec));
            glUniform1f(meshes[i].program->materialPosition.locs[3], currentMat.shine);
            checkError("after material updated");
            if ( !currentMat.textureFiles.empty() ) {
                //add loop for this if mult textures later
                glActiveTexture( GL_TEXTURE0 + 0);
                checkError("set active texture");
                meshes[i].program->setTextureSampler( NULL );
                //just diffuse for now
                if (currentMat.textureFiles[aiTextureType_DIFFUSE].size() > 0) {
                    //std::cout<<"setting up textures "<<name<<' '<<currentMat.textureFiles[aiTextureType_DIFFUSE].size()<<std::endl;
                    checkError("after image updated");
                    currentMat.textureFiles[aiTextureType_DIFFUSE][0].bindTexture();//binds the texture image location
                    checkError("after texture updated");
                }
            } else {
                //std::cout<<"NO TEXTURES "<<name<<std::endl;
                //glBindTexture(GL_TEXTURE_2D,0);
            }
        } else {
        	//no materials, do what? Nothing, if no mats then program probably doens't need them.
        }


        //if you don't have these, something is seriously wrong
        glBindBuffer(GL_ARRAY_BUFFER, meshes[i].geometryBuffer);
        // set pointers into the vbo for each of the attributes(position and color)
        glVertexAttribPointer(meshes[i].program->loc_position,  // location of attribute
                              3,  // number of elements
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,//sizeof(glm::vec3),  // stride
                              (void*)0);  // offset
        checkError("after geometry buffer bound");

        //check for normals
        if (meshes[i].normals.size()>0) {
            //std::cout<<"using Normals "<<loc_normal<<' '<<normalBuffers[j]<<std::endl;
            //std::cout<<"how many? "<<normals.size()<<' '<<normals[j].size()<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].normalBuffer);
            glVertexAttribPointer(meshes[i].program->loc_normal,  // location of attribute
                              3,  // number of elements
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,//sizeof(glm::vec3),  // stride
                              (void*)0);  // offset
            checkError("after normal buffer bound");
        } else {
            //std::cout<<"clearing for normals"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }


        //check for uvs
        if (meshes[i].uvs.size()>0) {
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].textureBuffer);
            glVertexAttribPointer(meshes[i].program->loc_uv,  // location of attribute
                              3,  // number of elements
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,//sizeof(glm::vec3),  // stride
                              (void*)0);  // offset
        } else {
            //std::cout<<"clearing for uvs"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //check colors
        /*if (meshes[i].colors.size()>0) {
            //std::cout<<"using colors"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].colorBuffer);
            glVertexAttribPointer(meshes[i].program->loc_color,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              0,//sizeof(glm::vec4),
                              (void*)0);
            checkError("after color bound");
        } else {
            //std::cout<<"clearing for colors"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }*/

        //draw elements
        //std::cout<<"binding buffers "<<j<<' '<<elementBuffers.size()<<' '<<elementBuffers[j]<<std::endl;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i].elementBuffer);
        checkError("pre-element draw");
        //std::cout<<"drawing elements "<<name<<std::endl;
        glDrawElements(GL_TRIANGLES, meshes[i].indices.size(),  GL_UNSIGNED_INT, (void*)0);
        checkError("Post element draw");

        meshes[i].program->stopProgram();
	}
}

void Mesh::checkError(std::string where=" "){
	GLenum ErrorCheckValue;
    ErrorCheckValue = glGetError();
    //std::cout<<"Checking Error"<<std::endl;
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        //std::cout<<"Error occured "<<std::endl;
        fprintf(
            stderr,
            "ERROR: %s: %s \n", where.c_str(),
            gluErrorString(ErrorCheckValue)
        );
        exit(-1);
    }
}

void Mesh::setCCD(int dist) {
    for (int i =0; i<physics.size(); i++) {
        //->setCcdMotionThreshold(1);
        physics[i]->setCCD(dist);
    }
}