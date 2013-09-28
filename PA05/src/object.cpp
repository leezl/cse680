/*
Object class to deal with Assimp later,
and hold object data.
Liesl Wigand 

*/
#include "object.h"

Object::Object(std::string path, std::string filename){
    //std::cout<<"Creating Object"<<std::endl;
    hasVert=hasTex=hasNorm=hasColor=hasFaces=false;
    loadUV=loadNorm=false;
    max[0] = max[1] = max[2] = -900;
    min[0] = min[1] = min[2] = 900;
    center[0]=center[1]=center[2] = 0;
    //loading object
    /*if (!loadObjectElementsColor(path, filename)){
        printf("Error Loading Object File.\n");
        exit(-1);
    }*/
    if ( !loadAssImp(filename) ) {
        printf("Error Loading Object File with Assimp.\n");
        exit(-1);
    }
    /*std::cout<<center[0]<<' '<<center[1]<<' '<<center[2]<<std::endl;
    std::cout<<"Number of Vertices "<<vertices.size()<<std::endl;
    for (std::vector< glm::vec3 >::iterator it = vertices.begin(); it != vertices.end(); it++) {
        //debug vertices
        std::cout<<'('<<(*it)[0]<<','<<(*it)[1]<<','<<(*it)[2]<<") ";
    }
    std::cout<<std::endl;
    std::cout<<"Number of indices "<<indices.size()<<std::endl;
    for (std::vector<unsigned short>::iterator it=indices.begin(); it!=indices.end(); it++) {
        std::cout<<(*it)<<',';
    }
    std::cout<<std::endl;*/
}

Object::~Object(){
    //had cleanUp here, but may not have initialized so...
}

void Object::cleanUp(){
    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &geometryBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &textureBuffer);
}

void Object::checkError(){
    ErrorCheckValue = glGetError();
    //std::cout<<"Checking Error"<<std::endl;
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        //std::cout<<"Error occured "<<std::endl;
        fprintf(
            stderr,
            "ERROR: Binding elementBuffer: %s \n",
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
    // aiProcess_JoinIdenticalVertices | aiProcess_Triangulate (others probably not needed)
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices );
    //check if it worked:
    if( !scene) {
        fprintf( stderr, importer.GetErrorString());
        return false;
    }

    int addedVerts=0, offset=0;//use as offset since combining meshes...
    //multiple meshes per object? weird.
    for (unsigned int j = 0; j<scene->mNumMeshes; j++) {
        const aiMesh* mesh = scene->mMeshes[j];
        addedVerts=0;
        //grab vertices
        //Reserve: better then just push_back, sizes the vector exactly instead of doubling and hoping.
        //HasPositions?
        vertices.reserve(vertices.size()+mesh->mNumVertices);
        hasVert=true;
        for(unsigned int i=0; i<mesh->mNumVertices; i++) {
            aiVector3D pos = mesh->mVertices[i];
            addedVerts++;
            vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
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

        //grab uvs
        if ( 0 < mesh->GetNumUVChannels() ) {
            //hasTex = true;
            uvs.reserve(vertices.size()+mesh->mNumVertices);
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
                uvs.push_back(glm::vec2(UVW.x, UVW.y));
            }
        }

        //grab normals
        if ( mesh->HasNormals() ) {
            //hasNorm = true;
            normals.reserve(vertices.size()+mesh->mNumVertices);
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                aiVector3D n = mesh->mNormals[i];
                normals.push_back(glm::vec3(n.x, n.y, n.z));
            }
        }

        //COLOR? is it separate or are the vertices auto paired with the diffuse color?
        //check if color exists: index value should be...?
        //mesh->HasVertexColors(0) | mesh->GetNumColorChannels()
        if ( 0 < mesh->GetNumColorChannels() ) {
            hasColor = true;
            colors.reserve(vertices.size()+mesh->mNumVertices);
            for(unsigned int i=0; i<mesh->mNumVertices; i++) {
                //only use first set...cause we have no idea why there are more yet...
                aiColor4D c = mesh->mColors[0][i];
                colors.push_back(glm::vec4(c.r, c.g, c.b, c.a));
            }
        }

        //grab indices
        //3 vertices per face: 3*numFaces
        if ( mesh->HasFaces() ) {
            hasFaces = true;
            //std::cout<<"Faces: "<<mesh->mNumFaces<<std::endl;
            indices.reserve(indices.size()+(3*mesh->mNumFaces));
            for (unsigned int i=0; i<mesh->mNumFaces; i++) {
                // The model should be all triangle since we triangulated
                indices.push_back(offset+mesh->mFaces[i].mIndices[0]);
                indices.push_back(offset+mesh->mFaces[i].mIndices[1]);
                indices.push_back(offset+mesh->mFaces[i].mIndices[2]);
            }
        }
        offset += addedVerts;
    }

    //assume all's well if we make it here
    return true;
}

/*
This version of load loads each vertex once, and uses draw_elements
*/
bool Object::loadObjectElementsColor(std::string path, std::string filename){
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;
    std::vector< std::string > temp_matName;
    std::vector< indexSet > uniqueInd;
    indexSet indexPoint;
    glm::vec4 color;
    //char materialFile[128];
    std::vector< std::string > materialFile;
    //std::string materialFile = "";
    //char matName[128];
    std::string matName = "";
    hasVert=false;
    hasTex =false;
    hasNorm = false;

    FILE * file = fopen(filename.c_str(), "r");
    if( file == NULL ){
        printf("Impossible to open the object file %s!\n", filename.c_str());
        return false;
    }

    while( 1 ){
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) {
            break; // EOF = End Of File. Quit the loop.
        }
 
        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
            hasVert = true;
            for (int i=0; i<3; i++) {
                if (vertex[i]>max[i]) {
                    max[i] = vertex[i];
                } 
                if (vertex[i]<min[i]) {
                    min[i] = vertex[i];
                } 
            }
        } else if ( strcmp( lineHeader, "vt" ) == 0 ){
            if (loadUV==true) {
                glm::vec2 uv;
                fscanf(file, "%f %f\n", &uv.x, &uv.y );
                temp_uvs.push_back(uv);
                hasTex = true;
            }
        } else if ( strcmp( lineHeader, "vn" ) == 0 ){
            if (loadNorm==true) {
                glm::vec3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
                temp_normals.push_back(normal);
                hasNorm = true;
            }
        } else if ( strcmp( lineHeader, "mtllib" ) == 0){
            char tempMaterial[128];
            // Store mtl file name for loading at end of this
            hasColor = true;
            fscanf(file, "%s\n", &tempMaterial);
            materialFile.push_back(tempMaterial);
        } else if( strcmp( lineHeader, "usemtl" ) == 0 ){
            char tempMaterial[128];
            // add material to color list
            fscanf(file, "%s\n", &tempMaterial);//if no mtl? try this later
            matName = tempMaterial;
        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            //have all vertices, grab center here
            center[0] = (max[0]+min[0])/2.0;
            center[1] = (max[1]+min[1])/2.0;
            center[2] = (max[2]+min[2])/2.0;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            std::vector < unsigned int > vertexIndTemp;
            std::vector < unsigned int > uvIndTemp;
            std::vector < unsigned int > normalIndTemp;
            if (hasVert && hasNorm && hasTex ) {
                //generalize fscanf to read in arbitrary number of points
                int matches = fscanf(file, "%d/%d/%d", &vertexIndex[0], &uvIndex[0], &normalIndex[0]);
                while ( matches > 0 ) {
                    //store all the points found
                    vertexIndTemp.push_back(vertexIndex[0]);
                    uvIndTemp.push_back(uvIndex[0]);
                    normalIndTemp.push_back(normalIndex[0]);
                    matches = fscanf(file, "%d/%d/%d", &vertexIndex[0], &uvIndex[0], &normalIndex[0]);
                }
            } else if (hasVert && hasNorm) {
                //generalize fscanf to read in arbitrary number of points
                int matches = fscanf(file, "%d//%d", &vertexIndex[0], &normalIndex[0]);
                while ( matches > 0 ) {
                    //store all the points found
                    vertexIndTemp.push_back(vertexIndex[0]);
                    uvIndTemp.push_back(-1);
                    normalIndTemp.push_back(normalIndex[0]);
                    matches = fscanf(file, "%d//%d", &vertexIndex[0], &normalIndex[0]);
                }
            } else if (hasVert && hasTex) {
                //generalize fscanf to read in arbitrary number of points
                int matches = fscanf(file, "%d/%d/", &vertexIndex[0], &uvIndex[0]);
                while ( matches > 0 ) {
                    //store all the points found
                    vertexIndTemp.push_back(vertexIndex[0]);
                    uvIndTemp.push_back(uvIndex[0]);
                    normalIndTemp.push_back(-1);
                    matches = fscanf(file, "%d/%d/", &vertexIndex[0], &uvIndex[0]);
                }
            } else {
                int matches = fscanf(file, "%d", &vertexIndex[0]);
                while ( matches > 0 ) {
                    //store all the points found
                    vertexIndTemp.push_back(vertexIndex[0]);
                    uvIndTemp.push_back(-1);
                    normalIndTemp.push_back(-1);
                    matches = fscanf(file, "%d", &vertexIndex[0]);
                }
            }
            //triangulate points into faces
            assert ( vertexIndTemp.size()>=3 );
            vertexIndex[0] = vertexIndTemp[0];
            uvIndex[0] = uvIndTemp[0];
            normalIndex[0] = normalIndTemp[0];
            for (int i=1; i<vertexIndTemp.size()-1; i++){//check off by one here...
                //create a face
                vertexIndex[1] = vertexIndTemp[i];
                uvIndex[1] = uvIndTemp[i];
                normalIndex[1] = normalIndTemp[i];
                vertexIndex[2] = vertexIndTemp[i+1];
                uvIndex[2] = uvIndTemp[i+1];
                normalIndex[2] = normalIndTemp[i+1];
                //add the face
                for (int j=0; j<3; j++) {
                    indexPoint.vertex = vertexIndex[j];
                    indexPoint.uv = uvIndex[j];
                    indexPoint.normal = normalIndex[j];
                    indexPoint.color = matName;
                    //only do this stuff if model is small otherwise takes forever...
                    if (temp_vertices.size()<100000) {
                        unsigned int first = 0;
                        while (first < uniqueInd.size()) {
                            if (uniqueInd[first].vertex==indexPoint.vertex && uniqueInd[first].normal==indexPoint.normal && 
                                uniqueInd[first].uv==indexPoint.uv && uniqueInd[first].color.compare(indexPoint.color)==0) break;
                            ++first;
                        }
                        //check if this combo is unique in indices; ie not in indices yet
                        if ( first == uniqueInd.size() ) {
                            //add new values to all arrays
                            uniqueInd.push_back(indexPoint);
                            vertices.push_back(temp_vertices[vertexIndex[j]-1]);
                            if (hasTex) {
                                uvs.push_back(temp_uvs[uvIndex[j]-1]);
                            }
                            if (hasNorm) {
                                normals.push_back(temp_normals[normalIndex[j]-1]);
                            }
                            //find correct color value later, for now store color name
                            temp_matName.push_back(matName);
                        }
                        first = 0;
                        while (first < uniqueInd.size()) {
                            if (uniqueInd[first].vertex==indexPoint.vertex && uniqueInd[first].normal==indexPoint.normal && 
                                uniqueInd[first].uv==indexPoint.uv && uniqueInd[first].color.compare(indexPoint.color)==0) break;
                            ++first;
                        }
                        // add points location to indices
                        indices.push_back(first);
                    } else {//vertex 0 1 and 2 
                        //just add vertice and indices
                        vertices.push_back(temp_vertices[vertexIndex[j]-1]);
                        if (hasTex) {
                            uvs.push_back(temp_uvs[uvIndex[j]-1]);
                        }
                        if (hasNorm) {
                            normals.push_back(temp_normals[normalIndex[j]-1]);
                        }
                        //find correct color value later, for now store color name
                        temp_matName.push_back(matName);
                        indices.push_back(vertexIndex[j]-1);
                    }
                }
            }
        }
    }
    fclose(file);

    //load materials if there was a file
    if ( materialFile.size() > 0 ) {
        for ( std::vector< std::string >::iterator it = materialFile.begin(); it!=materialFile.end(); it++) {
            //(*it).insert(0, "assets/models/");//BAD
            bool materialLoaded = loadMaterial(path+(*it));
            if ( materialLoaded == false ) {
                printf("Loading material %s failed. \n", (*it).c_str());
                return false;
            }
        }
    }
    //now we need to go through temp_matName, and load each into colors
    for (std::vector<std::string>::iterator it = temp_matName.begin(); it != temp_matName.end(); ++it){
        // find matching color in materials
        std::vector< mtl* >::iterator first = materials.begin();
        while (first != materials.end()) {
            if (((*first)->name).compare((*it))==0) break;
            ++first;
        }
        if ( first == materials.end() ) {
            std::string tempColor = *it;
            if (vertices.size()<100000) {
                printf("Failed to find color %s . Using default.\n", tempColor.c_str());
            }
            color[0] = 0.9;
            color[1] = 0.9;
            color[2] = 0.9;
            color[3] = 1.0;
        } else {
            //have
            color[0] = (*first)->Kd[0];
            color[1] = (*first)->Kd[1];
            color[2] = (*first)->Kd[2];
            color[3] = (*first)->d;
        }
        colors.push_back(color);

    }
    return true;
}

void Object::initializeObject(){
    //std::cout<<"Initializing Object"<<std::endl;
    //gen buffers
    //std::cout<<"Setting Element Buffer"<<std::endl;
    //set element buffer data
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

    checkError();

    //set vertices
    if (hasVert) {
        glEnableClientState(GL_VERTEX_ARRAY);
        //std::cout<<"Setting Vertex Buffer"<<std::endl;
        glGenBuffers(1, &geometryBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        checkError();
    }
    //check for normals
    if (hasNorm) {
        //std::cout<<"Setting Normals Buffer"<<std::endl;
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    }

    //check for uvs
    if (hasTex) {
        //std::cout<<"Setting UVs Buffer"<<std::endl;
        glGenBuffers(1, &textureBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    }

    //check colors
    if (hasColor) {
        //std::cout<<"Setting Colors Buffer"<<std::endl;
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), &colors[0], GL_STATIC_DRAW);
        checkError();
    } else {
        //glDisableClientState{GL_COLOR_ARRAY};
    }

    /*if (!hasColor && !hasTex) {
        //do somehting to make sure it is visible
        colors = std::vector< glm::vec4 > (vertices.size(), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        hasColor=true;
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), &colors[0], GL_STATIC_DRAW);
        checkError();
    }*/

    //error checker (GLU)
    checkError();
}

void Object::drawObject(GLint loc_position, GLint loc_normal, GLint loc_uv, GLint loc_color){
    //bind buffers
    //std::cout<<"Drawing Object"<<std::endl;
    //set vertices
    //if you don't have these, soemthing is seriously wrong
    glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer);
    // set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer(loc_position,  // location of attribute
                          3,  // number of elements
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,//sizeof(glm::vec3),  // stride
                          (void*)0);  // offset
    checkError();

    //check for normals
    if (hasNorm) {
        //std::cout<<"using Normals"<<std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
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
    if (hasTex) {
        //std::cout<<"using Uvs"<<std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
        glVertexAttribPointer(loc_uv,  // location of attribute
                          2,  // number of elements
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,//sizeof(glm::vec2),  // stride
                          (void*)0);  // offset
    } else {
        //std::cout<<"clearing for uvs"<<std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //check colors
    if (hasColor) {
        //std::cout<<"using colors"<<std::endl;
        glEnableVertexAttribArray(loc_color);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glVertexAttribPointer(loc_color,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          0,//sizeof(glm::vec4),
                          (void*)0);
        checkError();
    } else {
        //std::cout<<"clearing for colors"<<std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(loc_color);
    }

    //draw elements
    /*if (!hasFaces) {
        std::cout<<"HAS NO FACES"<<std::endl;
    }*/
    //std::cout<<"binding element buffer"<<std::endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    checkError();
    //std::cout<<"drawing elements"<<std::endl;
    glDrawElements(GL_TRIANGLES, indices.size(),  GL_UNSIGNED_SHORT, (void*)0);
    //std::cout<<"After Draw"<<std::endl;
    checkError();
}

bool Object::loadMaterial(std::string filename){
    mtl *currentMaterial;
    bool hasMat = false;

    FILE * file = fopen(filename.c_str(), "r");
    if( file == NULL ){
        printf("Impossible to open the %s material file !\n",filename.c_str());
        exit(-1);
        return false;
    }

    while( 1 ){
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) {
            //add final mtl to vector of materials
            if ( hasMat == true ) {
                materials.push_back(currentMaterial);
            }
            break; // EOF = End Of File. Quit the loop.
        }

        // else : parse lineHeader
        if ( strcmp( lineHeader, "newmtl" ) == 0 ) {
            //store old material (if it exists); begin new material
            if ( hasMat == true ) {
                materials.push_back(currentMaterial);
            }
            hasMat = true;
            currentMaterial = new mtl;
            char tempMaterial[128];
            fscanf(file, "%s\n", &tempMaterial);
            currentMaterial->name = tempMaterial;
        } else if ( strcmp( lineHeader, "Ns" ) == 0 ) {
            //add Ns value to current material
            fscanf(file, "%f\n", &currentMaterial->Ns);
        } else if ( strcmp( lineHeader, "Ka" ) == 0 ) {
            //add Ka value to current material
            fscanf(file, "%f %f %f\n", &currentMaterial->Ka.x, &currentMaterial->Ka.y, &currentMaterial->Ka.z);
        } else if ( strcmp( lineHeader, "Kd" ) == 0 ) {
            //add Kd value to current material
            fscanf(file, "%f %f %f\n", &currentMaterial->Kd.x, &currentMaterial->Kd.y, &currentMaterial->Kd.z);
        } else if ( strcmp( lineHeader, "Ks" ) == 0 ) {
            //add Ks value to current material
            fscanf(file, "%f %f %f\n", &currentMaterial->Ks.x, &currentMaterial->Ks.y, &currentMaterial->Ks.z);
        } else if ( strcmp( lineHeader, "Ni" ) == 0 ) {
            //add Ni value to current material
            fscanf(file, "%f\n", &currentMaterial->Ni);
        } else if ( strcmp( lineHeader, "d" ) == 0 ) {
            //add d value to current material
            fscanf(file, "%f\n", &currentMaterial->d);
        } else if ( strcmp( lineHeader, "illum" ) == 0 ) {
            //add illum value to current material
            fscanf(file, "%i\n", &currentMaterial->illum);
        }
    } // end of while
    return true;
}