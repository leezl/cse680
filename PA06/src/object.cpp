/*
Object class to deal with Assimp later,
and hold object data.
Liesl Wigand 

*/
#include "object.h"

Object::Object(std::string path, std::string filename){
    scene=NULL;
    std::cout<<"Creating Object"<<std::endl;
    max[0] = max[1] = max[2] = -900;
    min[0] = min[1] = min[2] = 900;
    center[0]=center[1]=center[2] = 0;
    //loading object
    if ( !loadAssImp(filename) ) {
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
bool Object::loadAssImp(std::string path){
    std::cout<<"Loading Object"<<std::endl;
    //need assimp importer
    Assimp::Importer importer;

    // read file into scene object
    // possible flags:
    // triangles, and normal important. others probably won't do much (vertex normals)
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals );
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
    normals.reverve(scene->mNumMeshes);
    uvs.reserve(scene->mNumMeshes);
    colors.reserve(scene->mNumMeshes);

    //iterate through each mesh (and mat) and collect
    for (unsigned int j = 0; j<scene->mNumMeshes; j++) {
        const aiMesh* mesh = scene->mMeshes[j];
        //find center, min and max for camera positioning
        std::vector<glm::vec4> localVert;
        std::vector<glm::vec3> localUV;
        aiVector3D pos;
        localVert.reserve(mesh->mNumVertices);
        for(unsigned int i=0; i<mesh->mNumVertices; i++) {
            pos = mesh->mVertices[i];
            localVert.push_back(glm::vec4(pos.x, pos.y, pos.z, 1.0));
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
        //clear to be used again or just free space
        localVert.clear();

        //grab normals
        localVert.reserve(mesh->mNumVertices);//make sure still reserved
        if (mesh->HasNormals()) {
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                pos = mesh->mNormals[i];
                localVert.push_back(glm::vec4(pos.x, pos.y, pos.z, 0.0));
            }
            normals.push_back(localVert);
            localVert.clear();
        }

        //grab colors
        localVert.reserve(mesh->mNumVertices);//make sure still reserved
        if ( 0 < mesh->GetNumColorChannels()) {
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                aiColor4D color = mesh->mColors[0][i];
                localVert.push_back(glm::vec4(color.r, color.g, color.b, color.a));
            }
            colors.push_back(localVert);
            localVert.clear();
        }

        //grab uvs
        localUV.reserve(mesh->mNumVertices);
        if (0 < mesh->GetNumUVChannels()) {
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                pos = mesh->mTextureCoords[0][i];
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


        //READ OVER
        //grab flattened indices here
        if ( mesh->HasFaces() ) {
            std::vector< unsigned short > meshIndices;//per mesh, collect all faces (every 3 drawn as face)
            //std::cout<<"Faces: "<<mesh->mNumFaces<<std::endl;
            meshIndices.reserve(3*mesh->mNumFaces);//all traingulated
            for (unsigned int i=0; i<mesh->mNumFaces; i++) {//per face
                // The model should be all triangle since we triangulated
                meshIndices.push_back(mesh->mFaces[i].mIndices[0]);
                meshIndices.push_back(mesh->mFaces[i].mIndices[1]);
                meshIndices.push_back(mesh->mFaces[i].mIndices[2]);
            }
            indices.push_back(meshIndices);//all of the faces in mesh
            meshIndices.clear();
        }
    }
    //load materials
    if (scene->HasMaterials()) {
        Material temp;
        //iterate through materials
        for (unsigned int i = 0; i<scene->mNumMaterials; i++) {
            //store material properties: specular, diffuse, ambient, shine OR Default
            //check diffuse exists
            //std::cout<<"Has materials "<<std::endl;
            aiColor4D diff(0.8f, 0.8f, 0.8f, 1.0f);//default diffuse
            aiMaterial *mtl = scene->mMaterials[i];
            aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diff);
            temp.mtlDiff[0] = diff.r;
            temp.mtlDiff[1] = diff.g;
            temp.mtlDiff[2] = diff.b;
            temp.mtlDiff[3] = diff.a;
            std::cout<<"Diffuse: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            diff.r = diff.g = diff.b = 0.2;//default specular
            diff.a = 1.0;
            aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &diff);
            temp.mtlSpec[0] = diff.r;
            temp.mtlSpec[1] = diff.g;
            temp.mtlSpec[2] = diff.b;
            temp.mtlSpec[3] = diff.a;
            std::cout<<"Specular: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            diff.r = diff.g = diff.b = 0.1;//default ambient
            diff.a = 1.0;
            aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &diff);
            temp.mtlAmb[0] = diff.r;
            temp.mtlAmb[1] = diff.g;
            temp.mtlAmb[2] = diff.b;
            temp.mtlAmb[3] = diff.a;
            std::cout<<"Ambient: "<<diff.r<<','<<diff.g<<','<<diff.b<<','<<diff.a<<std::endl;
            //shine
            float shiny = 20;
            aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shiny);
            //why is shininess being returned as color4d, when assimp
            //docs claim it should be a float...?
            temp.mtlShine = shiny;
            std::cout<<"Shine: "<<shiny<<std::endl;
            materials.push_back(temp);
        }
    }

    //assume all's well if we make it here
    return true;
}

void Object::initializeObject(){
    //std::cout<<"Initializing Object"<<std::endl;
    //gen buffers
    //have to draw each material group of faces separately: lighting
    if (scene == NULL) {
        std::cout<<"The scene points nowhere."<<std::endl;
    }
    //can we avoid all this duplication?
    //glGenBuffers (scene->mNumMeshes, &elementBuffers );?
    elementBuffers.reserve(scene->mNumMeshes);
    geometryBuffers.reserve(scene->mNumMeshes);
    normalBuffers.reserve(scene->mNumMeshes);
    colorBuffers.reserve(scene->mNumMeshes);
    textureBuffers.reserve(scene->mNumMeshes);
    GLuint spare;
    for (unsigned int j = 0; j<scene->mNumMeshes; j++) {
        const aiMesh* mesh = scene->mMeshes[j];

        if ( mesh->HasFaces() ) {
            //std::cout<<"Setting Element Buffer"<<std::endl;
            //set element buffer data
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spare);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[j].size() * sizeof(unsigned short), &indices[j][0], GL_STATIC_DRAW);
            elementBuffers.push_back(spare); 

            checkError("setting element data");
        } else {
            elementBuffers.push_back(0);
        }

        //set vertices
        if (mesh->HasPositions()) {
            glEnableClientState(GL_VERTEX_ARRAY);
            //std::cout<<"Setting Vertex Buffer"<<std::endl;
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            glBufferData(GL_ARRAY_BUFFER, vertices[j].size() * sizeof(glm::vec3), &vertices[j][0], GL_STATIC_DRAW);
            geometryBuffers.push_back(spare);
            checkError("setting vertex data");
        } else {
            geometryBuffers.push_back(0);
        }
        //check for normals
        if (mesh->HasNormals()) {
            //std::cout<<"Setting Normals Buffer"<<std::endl;
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[j][0], GL_STATIC_DRAW);
            normalBuffers.push_back(spare);
        } else {
            normalBuffers.push_back(0);
        }

        //check for uvs
        if (0 < mesh->GetNumUVChannels()) {
            //std::cout<<"Setting UVs Buffer"<<std::endl;
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //not prepared to handle several textures.
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec3), &uvs[j][0], GL_STATIC_DRAW);
            textureBuffers.push_back(spare);
        } else {
            textureBuffers.push_back(0);
        }

        //check colors
        if ( 0 < mesh->GetNumColorChannels()) {
            //std::cout<<"Setting Colors Buffer"<<std::endl;
            glGenBuffers(1, &spare);
            glBindBuffer(GL_ARRAY_BUFFER, spare);
            //not prepared to handle several colors either...
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), &colors[j][0], GL_STATIC_DRAW);
            colorBuffers.push_back(spare);
            checkError("setting color buffer");
        } else if (scene->HasMaterials()) {
            //nothing here, more in draw
            colorBuffers.push_back(0);
        } else {
            //glDisableClientState{GL_COLOR_ARRAY};
            colorBuffers.push_back(0);
        }

        //error checker (GLU)
        checkError("After Initializing a mesh");
    }
}

void Object::drawObject(GLint loc_position, GLint loc_normal, 
        GLint loc_uv, GLint loc_color, 
        Light light, LightLoc lightin){
    //bind buffers
    //std::cout<<"Drawing Object"<<std::endl;
    //iterate through all meshes (per material lighting and shading)
    for (unsigned int j = 0; j<indices.size(); j++) {
        //set vertices
        //if you don't have these, soemthing is seriously wrong
        glBindBuffer(GL_ARRAY_BUFFER, geometryBuffers[j]);
        // set pointers into the vbo for each of the attributes(position and color)
        glVertexAttribPointer(loc_position,  // location of attribute
                              3,  // number of elements
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,//sizeof(glm::vec3),  // stride
                              (void*)0);  // offset
        checkError("after geometry buffer bound");

        //check for normals
        if (normals.size()>0 && loc_normal!=-1) {
            //std::cout<<"using Normals"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, normalBuffers[j]);
            glVertexAttribPointer(loc_normal,  // location of attribute
                              3,  // number of elements
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,//sizeof(glm::vec3),  // stride
                              (void*)0);  // offset
        } else {
            //std::cout<<"clearing for normals"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //check for uvs
        if (uvs.size()>0 && loc_uv!=-1) {
            //std::cout<<"using Uvs"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, textureBuffers[j]);
            glVertexAttribPointer(loc_uv,  // location of attribute
                              3,  // number of elements
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,//sizeof(glm::vec2),  // stride
                              (void*)0);  // offset
        } else {
            //std::cout<<"clearing for uvs"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //check colors
        if (colors.size()>0 && loc_color!=-1) {
            //std::cout<<"using colors"<<std::endl;
            glEnableVertexAttribArray(loc_color);
            glBindBuffer(GL_ARRAY_BUFFER, colorBuffers[j]);
            glVertexAttribPointer(loc_color,
                              4,
                              GL_FLOAT,
                              GL_FALSE,
                              0,//sizeof(glm::vec4),
                              (void*)0);
            checkError("after color bound");
        } else {
            //std::cout<<"clearing for colors"<<std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDisableVertexAttribArray(loc_color);
        }

        //Light calculations as necessary
        //check if material properties exist, else default values
        glm::vec4 mtlAmb, mtlDiff, mtlSpec;
        float mtlShine = 0.1;
        //std::cout<<"before materials "<<std::endl;
        checkError("after all buffers bound");
        
        //glm::vec3 crap = glm::normalize(LightPosition - pos);
        /*for (unsigned int i=0; i<mesh->mNumVertices; i++) {
            //std::cout<<"Vert "<<mesh->mVertices[i][0]<<','<<mesh->mVertices[i][1]<<','<<mesh->mVertices[i][2]<<','<<std::endl;
            //std::cout<<mesh->mNormals[i][0]<<','<<mesh->mNormals[i][1]<<','<<mesh->mNormals[i][2]<<','<<std::endl;
            glm::vec3 N = glm::normalize(glm::vec3(mesh->mNormals[i][0],mesh->mNormals[i][1],mesh->mNormals[i][2]));
            glm::vec3 L = glm::vec3(light.lightPos[0], light.lightPos[1], light.lightPos[2]) - 
                glm::vec3(mesh->mVertices[i][0], mesh->mVertices[i][1], mesh->mVertices[i][2]);
            float dotter = glm::dot(N, L);
            //std::cout<<"Calc: "<<dotter<<std::endl;
            glColor3i(1.0,1.0,0.0);
            glBegin(GL_LINES);
            glVertex3f(mesh->mVertices[i][0]+0.5, mesh->mVertices[i][1]+0.5, mesh->mVertices[i][2]+0.5); // origin of the line
            glVertex3f(mesh->mNormals[i][0], mesh->mNormals[i][1], mesh->mNormals[i][2]); // ending point of the line
            glEnd( );
        }*/
        //std::cout<<"Light "<<light.lightPos[0]<<' '<<light.lightPos[1]<<' '<<light.lightPos[2]<<std::endl;
        checkError("pre-ambient Send");
        glUniform4fv(lightin.loc_AmbProd, 1, glm::value_ptr(light.lightAmb*mtlAmb));
        checkError("pre-specular send");
        glUniform4fv(lightin.loc_SpecProd, 1, glm::value_ptr(light.lightSpec*mtlSpec));
        checkError("pre-diffuse send");
        glUniform4fv(lightin.loc_DiffProd, 1, glm::value_ptr(light.lightDiff*mtlDiff));
        checkError("pre-shine send");
        glUniform1f(lightin.loc_Shin, mtlShine);//ref
        checkError("pre-element buffer bind");

        //draw elements
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffers[j]);
        checkError("pre-element draw");
        /*for (unsigned int i = 0; i<indices[j].size(); i++) {
            std::cout<<indices[j][i]<<std::endl;
        }*/
        glDrawElements(GL_TRIANGLES, indices[j].size(),  GL_UNSIGNED_SHORT, (void*)0);
        checkError("Post element draw");
    }
}