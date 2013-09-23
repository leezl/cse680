/*
Object class to deal with Assimp later,
and hold object data.
Liesl Wigand 

*/
#include "object.h"

Object::Object(const char* filename){
    hasVert=hasTex=hasNorm=hasColor=false;
    if (!loadObjectElementsColor(filename)){
        printf("Error Loading Object File.");
    }
    //temporarily remove load to test display
    hasVert=true;
    hasColor=true;
    //crazy debugging:
    std::cout<<"Vertices:"<<std::endl;
    for (std::vector<glm::vec3>::iterator it = vertices.begin() ; it != vertices.end(); ++it)
        std::cout << " (" << (*it)[0]<<','<<(*it)[1]<<','<<(*it)[2]<<") ";
    std::cout<<std::endl<<"Colors:"<<std::endl;
    for (std::vector<glm::vec4>::iterator it = colors.begin() ; it != colors.end(); ++it)
        std::cout << " (" << (*it)[0]<<','<<(*it)[1]<<','<<(*it)[2]<<','<<(*it)[3]<<") ";
    std::cout<<std::endl<<"Sizes: "<<vertices.size()<<' '<<colors.size()<<std::endl;
    std::cout<<"Indices:"<<std::endl;
    for (std::vector<unsigned int>::iterator it = indices.begin() ; it != indices.end(); ++it)
        std::cout << ' ' << (*it);
    std::cout<<std::endl;
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

/*
This version of load loads each vertex once, and uses draw_elements
*/
bool Object::loadObjectElementsColor(const char * path){
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;
    std::vector< std::string > temp_matName;
    std::vector< indexSet > uniqueInd;
    indexSet indexPoint;
    glm::vec4 color;
    //char materialFile[128];
    std::string materialFile = "";
    //char matName[128];
    std::string matName = "";
    bool hasVert=false, hasTex =false, hasNorm = false;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
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
        } else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
            hasTex = true;
        } else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
            hasNorm = true;
        } else if ( strcmp( lineHeader, "mtllib" ) == 0){
            char tempMaterial[128];
            // Store mtl file name for loading at end of this
            hasColor = true;
            fscanf(file, "%s\n", &tempMaterial);
            materialFile = tempMaterial;
        } else if( strcmp( lineHeader, "usemtl" ) == 0 ){
            char tempMaterial[128];
            // add material to color list
            fscanf(file, "%s\n", &tempMaterial);//if no mtl? try this later
            matName = tempMaterial;
        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            if (hasVert && hasNorm && hasTex ) {
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                if (matches != 9){
                    printf("Error parsing into v, vt and vn. \n");
                    return false;
                }
                for ( int i=0; i<3; ++i ) {
                    indexPoint.vertex = vertexIndex[i];
                    indexPoint.uv = uvIndex[i];
                    indexPoint.normal = normalIndex[i];
                    indexPoint.color = matName;
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
                        vertices.push_back(temp_vertices[vertexIndex[i]-1]);
                        uvs.push_back(temp_uvs[uvIndex[i]-1]);
                        normals.push_back(temp_normals[normalIndex[i]-1]);
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
                }
            } else if (hasVert && hasNorm) {
                int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                if (matches != 6){
                    printf("Error parsing into v, and vn. \n");
                    return false;
                }
                for ( int i=0; i<3; ++i ) {
                    indexPoint.vertex = vertexIndex[i];
                    indexPoint.uv = -1;
                    indexPoint.normal = normalIndex[i];
                    indexPoint.color = matName;
                    unsigned int first = 0;
                    while (first < uniqueInd.size()) {
                        if (uniqueInd[first].vertex==indexPoint.vertex && uniqueInd[first].normal==indexPoint.normal && 
                            uniqueInd[first].uv==indexPoint.uv && uniqueInd[first].color.compare(indexPoint.color)==0) break;
                        ++first;
                    }
                    //check if this combo is unique in indices; ie not in indices yet
                    if ( first == uniqueInd.size() ) {
                        uniqueInd.push_back(indexPoint);
                        vertices.push_back(temp_vertices[vertexIndex[i]-1]);
                        normals.push_back(temp_normals[normalIndex[i]-1]);
                        //store color name temporarily
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
                }
            } else {
                //sometimes no normals...
                int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
                if (matches != 3){
                    printf("Error parsing into v. \n");
                    return false;
                }
                for ( int i=0; i<3; ++i ) {
                    indexPoint.vertex = vertexIndex[i];
                    indexPoint.uv = -1;
                    indexPoint.normal = -1;
                    indexPoint.color = matName;
                    //replace find with our own find version
                    unsigned int first = 0;
                    while (first < uniqueInd.size()) {
                        if (uniqueInd[first].vertex==indexPoint.vertex && uniqueInd[first].normal==indexPoint.normal && 
                            uniqueInd[first].uv==indexPoint.uv && uniqueInd[first].color.compare(indexPoint.color)==0) break;
                        ++first;
                    }
                    //check if this combo is unique in indices; ie not in indices yet
                    if ( first == uniqueInd.size() ) {
                        uniqueInd.push_back(indexPoint);
                        std::cout<<typeid(temp_vertices[vertexIndex[i]-1]).name()<<std::endl;
                        vertices.push_back(temp_vertices[vertexIndex[i]-1]);
                        //store color name temporarily
                        temp_matName.push_back(matName);
                    }
                    // add points location to indices
                    indices.push_back(first);
                }
            }
        }
    }
    fclose(file);
    //Now: we have lists of unique vertices
    //We have list of groups of vertices that make up a collection of faces
    //We need vertex colors to be set based on diffuse (Kd)
    //We need each uniques set to have its own index: so if v is matched 
    //with three colors we'll need three of v
    
    //load materials if there was one
    if ( materialFile.length() > 0 ) {
        materialFile.insert(0, "assets/models/");
        bool materialLoaded = loadMaterial(materialFile.c_str());
        if ( materialLoaded == false ) {
            printf("Loading material failed. \n");
            return false;
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
            printf("Failed to find color: %s \n", tempColor.c_str());
            color[0] = 1.0;
            color[1] = 1.0;
            color[2] = 1.0;
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
    hasTex = false;
    hasNorm = false; //don't want to use these yet, hard coding removal for now
    hasColor = false;
    return true;
}

void Object::initializeObject(){
    std::cout<<"Initializing Object"<<std::endl;
    //gen buffers
    std::cout<<"Setting Element Buffer"<<std::endl;
    glEnableClientState(GL_VERTEX_ARRAY);
    //set element buffer data
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Binding elementBuffer: %s \n",
            gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }

    //set vertices
    if (hasVert) {
        std::cout<<"Setting Vertex Buffer"<<std::endl;
        glGenBuffers(1, &geometryBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR)
        {
            fprintf(
                stderr,
                "ERROR: Binding vertex buffer: %s \n",
                gluErrorString(ErrorCheckValue)
            );

            exit(-1);
        }
    }
    //check for normals
    if (hasNorm) {
        std::cout<<"Setting Normals Buffer"<<std::endl;
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    }

    //check for uvs
    if (hasTex) {
        std::cout<<"Setting UVs Buffer"<<std::endl;
        glGenBuffers(1, &textureBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    }

    //check colors
    if (hasColor) {
        std::cout<<"Setting Colors Buffer"<<std::endl;
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), &colors[0], GL_STATIC_DRAW);
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR)
        {
            fprintf(
                stderr,
                "ERROR: Binding color buffer: %s \n",
                gluErrorString(ErrorCheckValue)
            );

            exit(-1);
        }
    }

    //error checker (GLU)
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Could not create a VBO: %s \n",
            gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }
}

void Object::drawObject(GLint loc_position, GLint loc_normal, GLint loc_uv, GLint loc_color){
    //bind buffers
    //set vertices
    glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer);
    // set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer(loc_position,  // location of attribute
                          3,  // number of elements
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,//sizeof(glm::vec3),  // stride
                          (void*)0);  // offset
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: vertex buffer and location attribute: %s \n",
            gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }

    //glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

    //check for normals
    if (hasNorm) {
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(loc_normal,  // location of attribute
                          3,  // number of elements
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,//sizeof(glm::vec3),  // stride
                          (void*)0);  // offset
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //check for uvs
    if (hasTex) {
        glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
        glVertexAttribPointer(loc_uv,  // location of attribute
                          2,  // number of elements
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,//sizeof(glm::vec2),  // stride
                          (void*)0);  // offset
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //check colors
    if (hasColor) {
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glVertexAttribPointer(loc_color,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          0,//sizeof(glm::vec4),
                          (void*)0);
        ErrorCheckValue = glGetError();
        if (ErrorCheckValue != GL_NO_ERROR)
        {
            fprintf(
                stderr,
                "ERROR: colorBuffer and color attribute: %s \n",
                gluErrorString(ErrorCheckValue)
            );

            exit(-1);
        }
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //draw elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Bidning element buffer: %s \n",
            gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }

    glDrawElements(GL_TRIANGLES, indices.size(),  GL_UNSIGNED_INT, (void*)0);
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Drawing Elements: %s \n",
            gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
            stderr,
            "ERROR: Could not create a VBO: %s \n",
            gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }
}

bool Object::loadMaterial(const char* filename){
    mtl *currentMaterial;
    bool hasMat = false;

    FILE * file = fopen(filename, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
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