/*
Object class to deal with Assimp later,
and hold object data.
Liesl Wigand 

*/
#include "object.h"

Object::Object() {
    hasMaterials=false;
    hasTextures=false;
    max[0] = max[1] = max[2] = -1;
    min[0] = min[1] = min[2] = 1;
    center[0]=center[1]=center[2] = 0;
    name = "DefaultCube";
    //lame cube
    indices.reserve(36);
    vertices.reserve(24);
    normals.reserve(24);
    std::vector< glm::vec3 > temp;
    //vertices
    temp.push_back(glm::vec3(1.0f,1.0f,1.0f));//0 //right, upper, front
    temp.push_back(glm::vec3(-1.0f,1.0f,1.0f));//1 //left, upper, front
    temp.push_back(glm::vec3(-1.0f,-1.0f,1.0f));//2 //left, lower, front
    temp.push_back(glm::vec3(-1.0f,1.0f,-1.0f));//3 //left, upper, back
    temp.push_back(glm::vec3(1.0f,1.0f,-1.0f));//4 //right, upper, back
    temp.push_back(glm::vec3(1.0f,-1.0f,-1.0f));//5 //right, lower, back
    temp.push_back(glm::vec3(1.0f,-1.0f,1.0f));//6 //right, lower, front
    temp.push_back(glm::vec3(-1.0f,-1.0f,-1.0f));//7 //left, lower, back
    vertices.push_back(temp);
    //indices
    std::vector < unsigned int > temp2;
    temp2.push_back(3);//back //d
    temp2.push_back(4);
    temp2.push_back(5);
    temp2.push_back(3);
    temp2.push_back(5);
    temp2.push_back(7);
    //
    temp2.push_back(0);//front
    temp2.push_back(1);
    temp2.push_back(2);
    temp2.push_back(0);
    temp2.push_back(2);
    temp2.push_back(6);
    //
    temp2.push_back(0);//right //d
    temp2.push_back(4);
    temp2.push_back(5);
    temp2.push_back(0);
    temp2.push_back(5);
    temp2.push_back(6);
    //
    temp2.push_back(0);//upper //d
    temp2.push_back(1);
    temp2.push_back(3);
    temp2.push_back(0);
    temp2.push_back(3);
    temp2.push_back(4);
    //
    temp2.push_back(1);//left //d
    temp2.push_back(2);
    temp2.push_back(3);
    temp2.push_back(2);
    temp2.push_back(3);
    temp2.push_back(7);
    //
    temp2.push_back(2);//lower //d
    temp2.push_back(5);
    temp2.push_back(6);
    temp2.push_back(2);
    temp2.push_back(5);
    temp2.push_back(7);
    indices.push_back(temp2);
}

Object::Object(std::string path, std::string filename){
    hasTextures=false;
    hasMaterials=false;
    totalTriangles = 0;
    totalVerts = 0;
    scale = glm::vec3(1.0,1.0,1.0);
    translate = glm::vec3(0.0,0.0,0.0);
    rotate = glm::vec3(0.0,0.0,0.0);
    std::cout<<"Creating Object "<<path+filename<<std::endl;
    max[0] = max[1] = max[2] = -900;
    min[0] = min[1] = min[2] = 900;
    center[0]=center[1]=center[2] = 0;
    name = filename;
    //loading object
    if ( !loadAssImp(path, filename) ) {
        printf("Error Loading Object File with Assimp.\n");
        exit(-1);
    }
}

Object::~Object(){
    //had cleanUp here, but may not have initialized so...
}

void Object::cleanUp(){
    //soooo tedious...must be a better way
    for (unsigned int i=0; i<elementBuffers.size(); i++) {
        glDeleteBuffers(1, &elementBuffers[i]);
    }
    for (unsigned int i=0; i<geometryBuffers.size(); i++) {
        glDeleteBuffers(1, &geometryBuffers[i]);
    }
    for (unsigned int i=0; i<normalBuffers.size(); i++) {
        glDeleteBuffers(1, &normalBuffers[i]);
    }
    for (unsigned int i=0; i<colorBuffers.size(); i++) {
        glDeleteBuffers(1, &colorBuffers[i]);
    }
    for (unsigned int i=0; i<textureBuffers.size(); i++) {
        glDeleteBuffers(1, &textureBuffers[i]);
    }
    for (unsigned int i=0; i<materials.size(); i++) {
        for(std::map< aiTextureType, std::vector< Texture> >::iterator iter = materials[i].textureFiles.begin(); iter != materials[i].textureFiles.end(); ++iter) {
            for(std::vector< Texture >::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
                iter2->cleanUp();//good god I need to fix this deallocation
            }
        }
    }
}

void Object::setProgram(Program * program) {
    myProgram = program;
    /*std::cout<<"object program ";
    std::cout<<myProgram;
    std::cout<<"\n";*/
}

void Object::setPhysics(std::string collisionType, std::string motionType, PhysicsWorld* world) {
    //m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles,
    //    gIndices,
    //    indexStride,
    //    totalVerts,(btScalar*) &gVertices[0].x(),vertStride);
    //trimeshShape  = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression,aabbMin,aabbMax);
    if (collisionType == "mesh") {
        int indexStride = 3*sizeof(int);
        int vertStride = sizeof(btVector3);
        btVector3* gVertices = new btVector3[totalVerts];
        int* gIndices = new int[totalTriangles*3];

        int indicesSoFar = 0;
        int verticesSoFar = 0;
        if (indices.size()!=vertices.size()) {
            std::cerr<<"Indices and vertices have different lengths. "<<std::endl;
            exit(EXIT_FAILURE);
        }
        //this is so slow...
        for (unsigned int i=0; i<indices.size(); i++) {
            //appends the indices to the end of our giant index array
            //add offset
            for (unsigned int j = 0; j<indices[i].size(); j++) {
                gIndices[(indicesSoFar+j)] = indices[i][j]+(verticesSoFar);
            }
            for (unsigned int j = 0; j<vertices[i].size(); j++) {
                gVertices[(verticesSoFar+j)].setValue(vertices[i][j].x, vertices[i][j].y, vertices[i][j].z);
            }
            //add offset into gIndices
            indicesSoFar += indices[i].size();
            verticesSoFar += vertices[i].size();
        }

        btTriangleIndexVertexArray* m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles,
            gIndices,
            indexStride,
            totalVerts, (btScalar*) &gVertices[0].x(), vertStride);
        btBvhTriangleMeshShape* trimeshShape  = new btBvhTriangleMeshShape(m_indexVertexArrays, true);
        physics.objectShape = trimeshShape;
    } else {
        //what other types do we want? default sphere? cube?
        btSphereShape* trimeshShape = new btSphereShape(1);
        physics.objectShape = trimeshShape;
    }

    physics.objectShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));

    //set motion
    if (motionType == "ground") {
        btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(rotate.x, rotate.y, rotate.z,1),btVector3(translate.x,translate.y,translate.z)));
        physics.objectMotionState = motionState;
    } else {
        //default is, non ground? Same for now, cuz why not?
        btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(rotate.x, rotate.y, rotate.z,1),btVector3(translate.x,translate.y,translate.z)));
        physics.objectMotionState = motionState;
    }

    //set rigidbody
    if (motionType == "ground") {
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0,physics.objectMotionState,physics.objectShape,btVector3(0,0,0));
        btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);
        physics.objectRigidBody = rigidBody;
    } else {
        btScalar mass = 1;
        btVector3 fallInertia(0,0,0);
        physics.objectShape->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, physics.objectMotionState, physics.objectShape, fallInertia);
        btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);
        physics.objectRigidBody = rigidBody;
    }

    //add to dynamic World
    world->addRigidBodyObject(physics.objectRigidBody);

}

void Object::updateModel() {
    //btTransform trans;
    //fallRigidBody->getMotionState()->getWorldTransform(trans);
    if (physics.objectRigidBody !=NULL) {
        btTransform transform;
        physics.objectRigidBody->getMotionState()->getWorldTransform(transform);
        btQuaternion rots = transform.getRotation();
        btVector3 trans = transform.getOrigin();
        btVector3 rotAxis = rots.getAxis();
        //this may be in the wrong order
        model = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));
        model = glm::rotate(model, rots.getAngle(), glm::vec3(rotAxis.getX(), rotAxis.getY(), rotAxis.getZ()));
        model = glm::translate(model, glm::vec3(trans.getX(), trans.getY(), trans.getZ()));
    }
}

void Object::checkError(std::string where=" "){
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

/*
Function for loading using Assimp. WIP
*/
bool Object::loadAssImp(std::string dir, std::string path){
    std::cout<<"Loading Object "<<path<<std::endl;
    //need assimp importer
    Assimp::Importer importer;

    // read file into scene object
    // possible flags:
    // triangles, and normal important. others probably won't do much (vertex normals)
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals );//SmoothNormals?
    //aiCopyScene(scener, &scene);
    //check if it worked:
    if( !scene) {
        fprintf( stderr, importer.GetErrorString());
        return false;
    }
    //reserve space for each mesh (and mat)
    indices.reserve(scene->mNumMeshes);
    vertices.reserve(scene->mNumMeshes);
    materials.reserve(scene->mNumMaterials);
    materialIndices.reserve(scene->mNumMeshes);
    normals.reserve(scene->mNumMeshes);
    uvs.reserve(scene->mNumMeshes);
    colors.reserve(scene->mNumMeshes);

    //iterate through each mesh (and mat) and collect
    for (unsigned int j = 0; j<scene->mNumMeshes; j++) {
        const aiMesh* mesh = scene->mMeshes[j];
        //find center, min and max for camera positioning
        std::vector<glm::vec3> localVert;
        std::vector<glm::vec3> localNorm;
        std::vector<glm::vec3> localUV;
        std::vector<glm::vec4> localColor;
        aiVector3D pos;
        localVert.reserve(mesh->mNumVertices);
        totalVerts += mesh->mNumVertices;
        for(unsigned int i=0; i<mesh->mNumVertices; i++) {
            pos = mesh->mVertices[i];
            localVert.push_back(glm::vec3(pos.x, pos.y, pos.z));
            //std::cout<<"Vertices:  "<<pos.x<<','<<pos.y<<','<<pos.z<<std::endl;
            if (pos.x>max[0]) {
                max[0] = pos.x;
            } else if ( pos.x<min[0] ) {
                min[0] = pos.x;
            }
            if (pos.y>max[1]) {
                max[1] = pos.y;
            }else if ( pos.y<min[1] ) {
                min[1] = pos.y;
            }
            if (pos.z>max[2]) {
                max[2] = pos.z;
            }else if ( pos.z<min[2] ) {
                min[2] = pos.z;
            }
        }
        //add this mesh
        vertices.push_back(localVert);
        //std::cout<<"Number of vertice sin this mesh "<<localVert.size()<<std::endl;
        //clear to be used again or just free space
        localVert.clear();

        //grab normals
        localNorm.reserve(mesh->mNumVertices);//make sure still reserved
        if (mesh->HasNormals()) {
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                pos = mesh->mNormals[i];
                localNorm.push_back(glm::vec3(pos.x, pos.y, pos.z));
                //std::cout<<"Normals:  "<<pos.x<<','<<pos.y<<','<<pos.z<<std::endl;
            }
            normals.push_back(localNorm);
            localNorm.clear();
        }

        //grab colors
        localColor.reserve(mesh->mNumVertices);//make sure still reserved
        if ( 0 < mesh->GetNumColorChannels()) {
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                aiColor4D color = mesh->mColors[0][i];
                localColor.push_back(glm::vec4(color.r, color.g, color.b, color.a));
            }
            colors.push_back(localColor);
            localColor.clear();
        }

        //grab uvs
        localUV.reserve(mesh->mNumVertices);
        if (0 < mesh->GetNumUVChannels()) {
            hasTextures = true;
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
                localUV.push_back(glm::vec3(pos.x, pos.y, pos.z));
            }
            uvs.push_back(localUV);
            localUV.clear();
        }

        //grab materialIndices
        if (scene->HasMaterials()) {
            //iterate through materials
            materialIndices.push_back(mesh->mMaterialIndex);
        }

        //grab flattened indices here
        if ( mesh->HasFaces() ) {
            std::vector< unsigned int > meshIndices;//per mesh, collect all faces (every 3 drawn as face)
            //std::cout<<"Faces: "<<mesh->mNumFaces<<std::endl;
            meshIndices.reserve(3*mesh->mNumFaces);//all traingulated
            totalTriangles += mesh->mNumFaces;
            for (unsigned int i=0; i<mesh->mNumFaces; i++) {//per face
                // The model should be all triangle since we triangulated
                //std::cout<<"number of indices in face: "<<mesh->mFaces[i].mNumIndices<<std::endl;
                meshIndices.push_back(mesh->mFaces[i].mIndices[0]);
                meshIndices.push_back(mesh->mFaces[i].mIndices[1]);
                meshIndices.push_back(mesh->mFaces[i].mIndices[2]);
                //std::cout<<"Indices in Face: "<<mesh->mFaces[i].mIndices[0]<<','<<mesh->mFaces[i].mIndices[1]<<','<<mesh->mFaces[i].mIndices[2]<<std::endl;
            }
            //std::cout<<"Size of indices for this mesh "<<meshIndices.size()<<std::endl;
            indices.push_back(meshIndices);//all of the faces in mesh
            meshIndices.clear();
        }
    }
    //load materials
    if (scene->HasMaterials()) {
        hasMaterials = true;
        Material temp;
        //iterate through materials
        for (unsigned int i = 0; i<scene->mNumMaterials; i++) {
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
            temp.shadingMode = shadingMode;
            //refraction: not useful
            float refraction=1.0;
            aiGetMaterialFloat(mtl, AI_MATKEY_SHADING_MODEL, &refraction);
            temp.refraction = refraction;
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
                    if (temp.textureFiles.find(texTypes[texs]) == temp.textureFiles.end()) {
                        //create empty array at that location
                        std::vector < Texture > ok;
                        temp.textureFiles[ texTypes[texs] ] = ok;
                    }
                    std::string whatIsHappening = texFile.C_Str();
                    Texture omgEvilTexture(dir+whatIsHappening); /////////
                    //std::cout<<"adding texture "<<dir+whatIsHappening<<' '<<name<<std::endl;
                    (temp.textureFiles[ texTypes[texs] ]).push_back( omgEvilTexture );
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
            temp.diff[0] = diff.r;
            temp.diff[1] = diff.g;
            temp.diff[2] = diff.b;
            temp.diff[3] = diff.a;
            //std::cout<<"Diffuse: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            diff.r = diff.g = diff.b = 0.2;//default specular
            aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &diff);
            temp.spec[0] = diff.r;
            temp.spec[1] = diff.g;
            temp.spec[2] = diff.b;
            temp.spec[3] = diff.a;
            //std::cout<<"Specular: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            diff.r = diff.g = diff.b = 0.1;//default ambient
            aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &diff);
            temp.amb[0] = diff.r;
            temp.amb[1] = diff.g;
            temp.amb[2] = diff.b;
            temp.amb[3] = diff.a;
            //std::cout<<"Ambient: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            //shine
            float shiny = 20.0;
            aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shiny);
            //if file was obj: shiniess 0-1000, in opengl 0-128 assume obj for now:
            shiny = (shiny*128.0)/1000.0;
            //why is shininess being returned as color4d, when assimp
            //docs claim it should be a float...?
            temp.shine = shiny;
            //std::cout<<"Shine: "<<shiny<<std::endl;
            materials.push_back(temp);
        }
    }

    //assume all's well if we make it here
    return true;
}

void Object::setTransforms(glm::vec3 trans, glm::vec3 rot, glm::vec3 sca, PhysicsWorld* world, std::string moves) {
    translate = trans;
    rotate =rot;
    scale = sca;
    if (world != NULL) {
        setPhysics(std::string("mesh"), moves, world);
    } else {
        physics.objectRigidBody=NULL;
        physics.objectShape=NULL;
        physics.objectMotionState=NULL;
    }
}

void Object::initializeObject(){
    std::cout<<"Initializing Object "<<name<<std::endl;
    //gen buffers
    //have to draw each material group of faces separately: lighting
    //can we avoid all this duplication?
    //glGenBuffers (scene->mNumMeshes, &elementBuffers );?
    elementBuffers.reserve(indices.size());
    geometryBuffers.reserve(indices.size());
    normalBuffers.reserve(indices.size());
    colorBuffers.reserve(indices.size());
    textureBuffers.reserve(indices.size());
    GLuint spare;
    for (unsigned int j = 0; j<indices.size(); j++) {
        if ( indices.size()>j ) {
            //std::cout<<"Setting Element Buffer "<<std::endl;
            //set element buffer data
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spare);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[j].size() * sizeof(unsigned int), &indices[j][0], GL_STATIC_DRAW);
            elementBuffers.push_back(spare); 
            checkError("setting element data");
        } else {
            elementBuffers.push_back(0);
        }

        //set vertices
        if (vertices.size()>j) {
            glEnableClientState(GL_VERTEX_ARRAY);
            //std::cout<<"Setting Vertex Buffer"<<std::endl;
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //std::cout<<"number of vertices "<<vertices[j].size()<<std::endl;
            glBufferData(GL_ARRAY_BUFFER, vertices[j].size() * sizeof(glm::vec3), &vertices[j][0], GL_STATIC_DRAW);
            geometryBuffers.push_back(spare);
            checkError("setting vertex data");
        } else {
            geometryBuffers.push_back(0);
        }
        //check for normals
        if (normals.size()>j) {
            //std::cout<<"Setting Normals Buffer "<<std::endl;
            glGenBuffers(1, &spare);
            //std::cout<<"normal buffer "<<spare<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //std::cout<<"number of normals "<<normals[j].size()<<std::endl;
            glBufferData(GL_ARRAY_BUFFER, normals[j].size() * sizeof(glm::vec3), &normals[j][0], GL_STATIC_DRAW);
            normalBuffers.push_back(spare);
        } else {
            normalBuffers.push_back(0);
        }

        //check for uvs
        if (uvs.size()>j) {
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //not prepared to handle several textures.
            glBufferData(GL_ARRAY_BUFFER, uvs[j].size() * sizeof(glm::vec3), &uvs[j][0], GL_STATIC_DRAW);
            textureBuffers.push_back(spare);
        } else {
            textureBuffers.push_back(0);
        }

        //check colors
        /*if ( colors.size()>j) {
            //std::cout<<"Setting Colors Buffer"<<std::endl;
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //not prepared to handle several colors either...
            glBufferData(GL_ARRAY_BUFFER, colors[j].size() * sizeof(glm::vec4), &colors[j][0], GL_STATIC_DRAW);
            colorBuffers.push_back(spare);
            checkError("setting color buffer");
        } else if (materials.size()>0) {
            //nothing here, more in draw
            colorBuffers.push_back(0);
        } else {
            //glDisableClientState{GL_COLOR_ARRAY};
            colorBuffers.push_back(0);
        }*/
        //std::cout<<elementBuffers.size()<<','<<indices.size()<<','<<geometryBuffers.size()<<','<<normalBuffers.size()<<std::endl;
        //error checker (GLU)
        checkError("After Initializing a mesh");
    }
}

void Object::drawObject(){//may recieve view, projection, light if needed
    //bind buffers
    //std::cout<<"Drawing Object "<<name<<std::endl;
    //std::cout<<elementBuffers.size()<<','<<indices.size()<<','<<geometryBuffers.size()<<','<<normalBuffers.size()<<std::endl;
    myProgram->startProgram();
    // upload the matrix to the shader
    myProgram->setModel(&model);
    //only do following if they haven't been set in init
    myProgram->setView(NULL);
    myProgram->setProjection(NULL);
    myProgram->setLightPosition(NULL, NULL);
    if (myProgram->view==NULL || myProgram->projection==NULL || myProgram->light==NULL) {
        std::cerr<<"No view, projection or light set for program."<<std::endl;
    }

    //iterate through all meshes (per material lighting and shading)
    for (unsigned int j = 0; j<indices.size(); j++) {
        if (materialIndices.size()>j && materials.size()>materialIndices[j]) {
            Material currentMat = materials[materialIndices[j]];
            //Light calculations as necessary
            //std::cout<<"before materials "<<name <<std::endl;
            glUniform4fv(myProgram->lightin.loc_AmbProd, 1, glm::value_ptr(myProgram->light->amb*currentMat.amb));
            glUniform4fv(myProgram->lightin.loc_SpecProd, 1, glm::value_ptr(myProgram->light->spec*currentMat.spec));
            glUniform4fv(myProgram->lightin.loc_DiffProd, 1, glm::value_ptr(myProgram->light->diff*currentMat.diff));
            glUniform1f(myProgram->lightin.loc_Shin, currentMat.shine);//ref
            checkError("after lighting updated");
            if ( !currentMat.textureFiles.empty() ) {
                //add loop for this if mult textures later
                glActiveTexture( GL_TEXTURE0 + 0);
                checkError("set active texture");
                myProgram->setTextureSampler( NULL );
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
          //std::cout<<"Error with material Ranges."<<std::endl;
        }
        //set vertices
        //if you don't have these, soemthing is seriously wrong
        glBindBuffer(GL_ARRAY_BUFFER, geometryBuffers[j]);
        // set pointers into the vbo for each of the attributes(position and color)
        glVertexAttribPointer(myProgram->loc_position,  // location of attribute
                              3,  // number of elements
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,//sizeof(glm::vec3),  // stride
                              (void*)0);  // offset
        checkError("after geometry buffer bound");

        //check for normals
        if (normals.size()>0) {
            //std::cout<<"using Normals "<<loc_normal<<' '<<normalBuffers[j]<<std::endl;
            //std::cout<<"how many? "<<normals.size()<<' '<<normals[j].size()<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[j]);
            glVertexAttribPointer(myProgram->loc_normal,  // location of attribute
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
        if (uvs.size()>0) {
            glBindBuffer(GL_ARRAY_BUFFER, textureBuffers[j]);
            glVertexAttribPointer(myProgram->loc_uv,  // location of attribute
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
        /*if (colors.size()>0) {
            //std::cout<<"using colors"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, colorBuffers[j]);
            glVertexAttribPointer(myProgram->loc_color,
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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffers[j]);
        checkError("pre-element draw");
        //std::cout<<"drawing elements "<<name<<std::endl;
        glDrawElements(GL_TRIANGLES, indices[j].size(),  GL_UNSIGNED_INT, (void*)0);
        checkError("Post element draw");
    }

    myProgram->stopProgram();
}

void Object::flipNormals(){
    for (unsigned int j = 0; j<normals.size(); j++) {
        for (unsigned int i =0; i<normals[j].size(); i++) {
            normals[j][i] = -normals[j][i];
        }
    }
}
