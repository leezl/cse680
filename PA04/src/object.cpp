/*
Object class to deal with Assimp later,
and hold object data.
Liesl Wigand 

*/

Object::Object(const char* filename){
    loadObject(filename, out_vertices,out_uvs,out_normals);
    //to be used thusly:
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

}

Object::~Object(){

}

/*
This version of load loads each vertex once, and uses draw_elements
*/
bool Object::loadObjectElementsColor(const char * path){
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;
    std::vector< string > temp_matName;
    std::vector< indexSet > uniqueInd;
    indexSet indexPoint;
    glm::vec3 color;
    string materialFile = "";
    string matName = "";
    bool hasv=false, hasvt=false, hasvn = false, hasGroup=false;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }

    while( 1 ){
 
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
 
        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
            hasv = true;
        } else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
            hasvt = true;
        } else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
            hasvn = true;
        } else if ( strcmp( lineHeader, "mtllib" ) == 0){
            // Store mtl file name for loading at end of this
            fscanf(file, "%s\n", &materialFile);
        } else if( strcomp( lineHeader, "usemtl" ) == 0 ){
            // add material to color list
            fscanf(file, "%s\n", &matName);//if no mtl? try this later
        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            if (hasv && hasvn && hasvt) {
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
                    //check if this combo is unique in indices; ie not in indices yet
                    if ( std::find(uniqueInd.begin(), uniqueInd.end(), indexPoint) == uniqueInd.end() ) {
                        //add new values to all arrays
                        uniqueInd.push_back(indexPoint);
                        vertices.push_back(temp_vertices[vertexIndex[i]-1]);
                        uvs.push_back(temp_uvs[uvIndex[i]-1]);
                        normals.push_back(temp_normals[normalIndex[i]-1]);
                        //find correct color value later, for now store color name
                        temp_matName.push_back(matName);
                    }
                    // add points location to indices
                    indices.push_back(distance(uniqueInd.begin(), std::find(uniqueInd.begin(), uniqueInd.end(), indexPoint)))
                }
            } else if (hasv && hasvn) {

                int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                if (matches != 6){
                    printf("Error parsing into v, and vn. \n");
                    return false;
                }
                for ( int i=0; i<3; ++i ) {
                    indexPoint.vertex = vertexIndex[i];
                    indexPoint.uv = uvIndex[i];
                    indexPoint.normal = normalIndex[i];
                    indexPoint.color = matName;
                    //check if this combo is unique in indices; ie not in indices yet
                    if ( std::find(uniqueInd.begin(), uniqueInd.end(), indexPoint) == uniqueInd.end() ) {
                        uniqueInd.push_back(indexPoint);
                        vertices.push_back(temp_vertices[vertexIndex[i]-1]);
                        uvs.push_back(temp_uvs[uvIndex[i]-1]);
                        normals.push_back(temp_normals[normalIndex[i]-1]);
                        //store color name temporarily
                        temp_matName.push_back(matName);
                    }
                    // add points location to indices
                    indices.push_back(distance(uniqueInd.begin(), std::find(uniqueInd.begin(), uniqueInd.end(), indexPoint)))
                }
            } else {
                //why would it not have normals?
                printf("Error parsing: No normals. \n");
                return false;
            }
        }
    }
    fclose(file);
    //CHANGE FOLLOWING: USING INDICES
    //Now: we have lists of unique vertices
    //We have list of groups of vertices that make up a collection of faces
    //We need vertex colors to be set based on diffuse (Kd)
    //We need each uniques set to have its own index: so if v is matched 
    //with three colors we'll need three of v
    
    //load materials if there was one
    if ( strlen(materialFile) > 0 ) {
        bool materialLoaded = loadMaterial(materialFile);
        if ( materialLoaded == false ) {
            printf("Loading material failed. \n");
            return false;
        }
    }
    //now we need to go through temp_matName, and load each into colors
    for (std::vector<string>::iterator it = temp_matName.begin() ; it != temp_matName.end(); ++it){
        //std::cout << ' ' << *it;
        // find matching color in materials
        std::vector< mtl >::iterator it2 = std::find(materials.begin(), materials.end(), *it);
        first = materials.begin();
        while (first!=materials.end()) {
            if (strcmp(*first.name,val)) break;
            ++first;
        }
        //have
        if (first!=materials.end()){
            color = *first.Kd;
            colors.push_back(color);
        } else {
            printf("The color is not in the material collection. %s\n", *it);
            return false;
        }
    }

    return true;
}

bool Object::initializeObject(){
    //gen buffers
    //set buffer data

    /*
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    */

}

bool Object::drawObject(){
    //bind buffers
    //draw elements

}

//Following loads groups, and material but does not enter as color array
bool Object::loadObjectElements(const char * path){
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;
    string materialFile = "";
    group currentGroup;
    face currentFace;
    bool hasv=false, hasvt=false, hasvn = false, hasGroup=false;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }

    while( 1 ){
 
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            //add final group to list
            if ( hasGroup == true ) {
                groups.push_back(currentGroup);
            }
            break; // EOF = End Of File. Quit the loop.
 
        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
            hasv = true;
        } else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
            hasvt = true;
        } else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
            hasvn = true;
        } else if ( strcmp( lineHeader, "mtllib" ) == 0){
            // Store mtl file name for loading at end of this
            fscanf(file, "%s\n", &materialFile);
        } else if( strcomp( lineHeader, "usemtl" ) == 0 ){
            // store old group
            if ( hasGroup == true ) {
                groups.push_back(currentGroup);
            }
            hasGroup=true;
            // add material to group...overwriting works?
            fscanf(file, "%s\n", &currentGroup.material);
        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            //check if no mtl just group
            if ( hasGroup == false ) {
                hasGroup = true;
            }
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            if (hasv && hasvn && hasvt) {
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                if (matches != 9){
                    printf("Error parsing into v, vt and vn. \n");
                    return false;
                }
                currentFace.vert.push_back(vertexIndex[0]);
                currentFace.uv  .push_back(uvIndex[0]);
                currentFace.norm.push_back(normalIndex[0]);
                currentGroup.faces.push_back(currentFace);

                currentFace.vert.push_back(vertexIndex[1]);
                currentFace.uv  .push_back(uvIndex[1]);
                currentFace.norm.push_back(normalIndex[1]);
                currentGroup.faces.push_back(currentFace);

                currentFace.vert.push_back(vertexIndex[2]);
                currentFace.uv  .push_back(uvIndex[2]);
                currentFace.norm.push_back(normalIndex[2]);
                currentGroup.faces.push_back(currentFace);
            } else if (hasv && hasvn) {

                int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                if (matches != 6){
                    printf("Error parsing into v, and vn. \n");
                    return false;
                }
                currentFace.vert.push_back(vertexIndex[0]);
                currentFace.uv  .push_back(-1);
                currentFace.norm.push_back(normalIndex[0]);
                currentGroup.faces.push_back(currentFace);

                currentFace.vert.push_back(vertexIndex[1]);
                currentFace.uv  .push_back(-1);
                currentFace.norm.push_back(normalIndex[1]);
                currentGroup.faces.push_back(currentFace);

                currentFace.vert.push_back(vertexIndex[2]);
                currentFace.uv  .push_back(-1);
                currentFace.norm.push_back(normalIndex[2]);
                currentGroup.faces.push_back(currentFace);
            } else {
                //why would it not have normals?
                printf("Error parsing: No normals. \n");
                return false;
            }
        }
    }
    //CHANGE FOLLOWING: USING INDICES
    //Now: we have lists of unique vertices
    //We have list of groups of vertices that make up a collection of faces

    // For each vertex of each triangle
    for ( unsigned int i=0; i<vertexIndices.size(); i++ ) {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        out_vertices.push_back(vertex);
    }
    // For each tecture coord (if there are any)
    for ( unsigned int i=0; i<uvIndices.size(); i++ ) {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 vertex = temp_uvs[ uvIndex-1 ];
        out_uvs.push_back(vertex);
    }
    // For each each triangle
    for ( unsigned int i=0; i<normalIndices.size(); i++ ) {
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 vertex = temp_normals[ normalIndex-1 ];
        out_normals.push_back(vertex);
    }
    
    //load material if there was one
    if ( strlen(materialFile) > 0 ) {
        bool materialLoaded = loadMaterial(materialFile);
        if ( materialLoaded == false ) {
            printf("Loading material failed. \n");
            return false;
        }
    }

    return true;
}

bool Object::loadMaterial(const char* filename){
    mtl currentMaterial;
    bool hasMat = false;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }

    while( 1 ){
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            //add final mtl to vector of materials

            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader
        if ( strcmp( lineHeader, "newmtl" ) == 0 ) {
            //store old material (if it exists); begin new material
            if ( hasMat == true ) {
                materials.push_back(currentMaterial);
            }
            hasMat = true;
            fscanf(file, "%s\n", &currentMaterial.name);
        } else if ( strcmp( lineHeader, "Ns" ) == 0 ) {
            //add Ns value to current material
            fscanf(file, "%f\n", &currentMaterial.Ns);
        } else if ( strcmp( lineHeader, "Ka" ) == 0 ) {
            //add Ka value to current material
            fscanf(file, "%f %f %f\n", &currentMaterial.Ka.x, &currentMaterial.Ka.y, &currentMaterial.Ka.z);
        } else if ( strcmp( lineHeader, "Kd" ) == 0 ) {
            //add Kd value to current material
            fscanf(file, "%f %f %f\n", &currentMaterial.Kd.x, &currentMaterial.Kd.y, &currentMaterial.Kd.z);
        } else if ( strcmp( lineHeader, "Ks" ) == 0 ) {
            //add Ks value to current material
            fscanf(file, "%f %f %f\n", &currentMaterial.Ks.x, &currentMaterial.Ks.y, &currentMaterial.Ks.z);
        } else if ( strcmp( lineHeader, "Ni" ) == 0 ) {
            //add Ni value to current material
            fscanf(file, "%f\n", &currentMaterial.Ni);
        } else if ( strcmp( lineHeader, "d" ) == 0 ) {
            //add d value to current material
            fscanf(file, "%f\n", &currentMaterial.d);
        } else if ( strcmp( lineHeader, "illum" ) == 0 ) {
            //add illum value to current material
            fscanf(file, "%i\n", &currentMaterial.illum);
        } else {
            printf("Error parsing Mtl file: unknown lineheader. \n");
            return false;
        }
    }
    //...return? anything left to do? Check later when we know where we're calling this

    return true;
}

/*
This version of load loads vertices multiple times; creating giant arrays of faces, 
and uses draw_vertex_arrays only.
Assumes no materials. 
*/
bool Object::loadObjectVertices(const char * path){

    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;
    bool hasv=false, hasvt=false, hasvn = false;

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }

    while( 1 ){
 
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
 
        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
            hasv = true;
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
            hasvt = true;
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
            hasvn = true;
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            if (hasv && hasvn && hasvt) {
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                if (matches != 9){
                    printf("Error parsing into v, vt and vn. \n");
                    return false;
                }
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                uvIndices    .push_back(uvIndex[0]);
                uvIndices    .push_back(uvIndex[1]);
                uvIndices    .push_back(uvIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
            } else if (hasv && hasvn) {
                int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                if (matches != 6){
                    printf("Error parsing into v, and vn. \n");
                    return false;
                }
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
            } else if (hasv){
                //why would it not have normals?
                int matches = fscanf(file, "%d// %d// %d//\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
                if (matches != 3){
                    printf("Error parsing into v. \n");
                    return false;
                }
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
            }
        }
    }
    // For each vertex of each triangle
    for ( unsigned int i=0; i<vertexIndices.size(); i++ ) {
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        out_vertices.push_back(vertex);
    }
    // For each tecture coord (if there are any)
    for ( unsigned int i=0; i<uvIndices.size(); i++ ) {
        unsigned int uvIndex = uvIndices[i];
        glm::vec2 vertex = temp_uvs[ uvIndex-1 ];
        out_uvs.push_back(vertex);
    }
    // For each each triangle
    for ( unsigned int i=0; i<normalIndices.size(); i++ ) {
        unsigned int normalIndex = normalIndices[i];
        glm::vec3 vertex = temp_normals[ normalIndex-1 ];
        out_normals.push_back(vertex);
    }
}


glm::mat4 Object::getModel(){

}

GLuint Object::getGeometry(){

}