//              .,                                                                                                          .(*      ,*,                
//                ((.     (.    ,                                                                          */*  ,%##%%%%%%%%%%%%%%%#(*           .      
//         .//**/#%%%%%%%%%%%%%%#*   .,                                                             ,**   .(%%%%%%#(******,***,/#%%%%%%%%###(/,         
//             #%%%#*.#%/***,,..,*(%(.    ,,                                                     *     /#%##/*****,,,,,,,,,.,...,,#%,  .#%#.            
//    .,     *%&#/   %#**,,*..,....,.*#,     ..                                               *     ,%#%#/*,,*,*,,,,,.,.,,.,...,...((     /#(//*/**.    
//           (%#    *#*...,.,,..........*/      ,                                          .      *#%(#(**,,,,,,,,..,..,..,,........(.     *#(          
//           *#     *(......,.............(#      ,                                       .     ,((, ##,,,.....,.................. ./       **  .,.     
//            *     ./........ ...........*#*,                                          ,      ,(,  ./*,,,..,,.................  .. *                   
//                   /, ........    ... ../(  *.                                              ,*     /,...,,.,,.....   ............**                   
//                    *... .............  /    ,                                             *,      ,*,,............  ,....     ...                    
//                     *.   ..... .... ..*                                                  .*        *...................   .  ...                     
//               *       ... ......... ,.                                                   ,          ... ..........  ...     ..       ,               
//                ((        .,.,.. ...                                                                   .  . .. .  .  ... .  ..      //                
//              ,/(#%#*                                                                                     .....  ... ......       .#*                 
//                 /((##%#(*                                                                                      .......        ,(#(*,                 
//               (.           .,,,,,                                                                                        .*#%%(                      
//                                                                                                      .***,.   . .,/##%###(/.  ...,,.      
/*  LAZARUS ENGINE */

#include "../include/lazarus_mesh_loader.h"

MeshLoader::MeshLoader()
{
	std::cout << GREEN_TEXT << "Calling constructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
	this->materialIdentifierIndex	=	0;
	this->triangleCount				=	0;

	this->matLoader 				=	nullptr;
};

bool MeshLoader::parseWavefrontObj(vector<vec3> &outAttributes, vector<vec3> &outDiffuse, const char* meshPath, const char* materialPath) 
{
	this->matLoader = std::make_unique<MaterialLoader>();
	
    this->materialIdentifierIndex = 0;
    this->triangleCount = 0;

    file.open(meshPath);

    if( !file.is_open() )
    {
        globals.setExecutionState(LAZARUS_FILE_UNREADABLE);
        
        return false;
    }

    while( file.getline(currentLine, 256) )
    {
        switch (currentLine[0])
        {
        case 'v':
            /* =============================================
                v = Vertex Position Coordinates (location)
            ================================================ */
            if ( currentLine[1] == ' ' )
            {
                coordinates = splitTokensFromLine(currentLine, ' ');

                this->vertex.x = stof(coordinates[1]);
                this->vertex.y = stof(coordinates[2]);
                this->vertex.z = stof(coordinates[3]);

                this->tempVertexPositions.push_back(this->vertex);
            } 
            /* =============================================
                vt = Vertex Texture Coordinates (UV / ST)
            ================================================ */
            else if ( currentLine[1] == 't' )
            {
                coordinates = splitTokensFromLine(currentLine, ' ');

                this->uv.x = stof(coordinates[1]);
                this->uv.y = stof(coordinates[2]);

                this->tempUvs.push_back(this->uv);
            }
            /* ==============================================
                vn = Vertex Normal coordinates (direction)
            ================================================= */
            else if ( currentLine[1] == 'n' )
            {
                coordinates = splitTokensFromLine(currentLine, ' ');

                this->normal.x = stof(coordinates[1]);
                this->normal.y = stof(coordinates[2]);
                this->normal.z = stof(coordinates[3]);

                this->tempNormals.push_back(this->normal);
            }
            break;
        /* ==============================================
            f = Face
        ================================================= */
        case 'f':
            this->triangleCount += 1;

            coordinates = splitTokensFromLine(currentLine, ' ');
            for(auto i: coordinates) 
            {
                stringstream ssJ(i);
                string tokenJ;

                /* ============================================
                    Unlike the other identifiers on the current
                    line which are folliowed by xyz coordinates; 
                    values following a face identifier contain 
                    the indexes describing which v, vt and vn
                    lines define the properties of *this* face.

                    Note / TODO:
                    Some editors deliminate face data with a 
                    dash character '-', others use whitespace
                    ' '. Blender uses a forward-slash '/'.
                =============================================== */
                while(getline(ssJ, tokenJ, '/')) 
                {
                    if (tokenJ != "f") 
                    {
                        attributeIndexes.push_back(tokenJ);
                    }
                }
            }            

            this->constructTriangle();

            break;
        /* ===============================================
            usemtl = Use material identifier
        ================================================== */
        case 'u':
            this->materialData = {materialIdentifierIndex, triangleCount};
			this->materialBuffer.push_back(this->materialData);
			
            this->materialIdentifierIndex += 1;
            this->triangleCount = 0;
            break;

        default:
            break;
        }
    }

    if (file.eof())
    {
        file.close();
        this->materialData = {materialIdentifierIndex, triangleCount};
		this->materialBuffer.push_back(this->materialData);
        
        matLoader->loadMaterial(outDiffuse, materialBuffer, materialPath);
    }

    this->interleaveBufferData(outAttributes, outDiffuse, this->vertexIndices.size());

    return true;
};

vector<string> MeshLoader::splitTokensFromLine(const char *wavefrontData, char delim)
{
    string token;
    string currentString = wavefrontData;
    stringstream ss(currentString);

    vector<string> tokenStore;

    while(getline(ss, token, delim)) 
    {
        tokenStore.push_back(token);
    }

    return tokenStore;
}

void MeshLoader::interleaveBufferData(vector<vec3> &outAttributes, vector<vec3> &outDiffuse, int numOfAttributes)
{
    for( int i = 0; i < numOfAttributes; i++ )
    {

        unsigned int vertexIndex    =   vertexIndices[i];
        unsigned int normalIndex    =   normalIndices[i];
        unsigned int uvIndex        =   uvIndices[i];
        
        /* =========================================
            uv is extended from its generic xy components
            to include a z value here to meet the expected
            stride range for attributes in the vertex buffer.
            
            i.e: (4 * sizeof(vec3)) = 12

            Once in the shaders it is disregarded. 
        ============================================ */
        vec3 vertex                 =   tempVertexPositions[vertexIndex - 1];
        vec3 diffuse                =   outDiffuse[i];
        vec3 normal                 =   tempNormals[normalIndex - 1];
        vec3 uv                     =   vec3(tempUvs[uvIndex - 1].x, tempUvs[uvIndex - 1].y, 0.0f);

        outAttributes.push_back(vertex);
        outAttributes.push_back(diffuse);
        outAttributes.push_back(normal);
        outAttributes.push_back(uv);
    }

    return;
}

void MeshLoader::constructTriangle()
{
    /* =======================================================
        A face should have 3 points, each with 3 
        different vertex attributes. If the face data contains
        any more than 9 vertex attribute indexes we know this 
        mesh hasn't been triangulated and can't be rendered 
        correctly.
    ========================================================== */
    if ( this->attributeIndexes.size() !=  9)
    {
        std::cout << RED_TEXT << "ERROR::MESH::MESH_LOADER " << std::endl;
        std::cout << "Status: " << LAZARUS_FILE_UNREADABLE << RESET_TEXT << std::endl;

        globals.setExecutionState(LAZARUS_FILE_UNREADABLE);

        return;
    }

    this->vertexIndices.push_back(stoi(this->attributeIndexes[0]));
    this->vertexIndices.push_back(stoi(this->attributeIndexes[3]));
    this->vertexIndices.push_back(stoi(this->attributeIndexes[6]));
    this->uvIndices    .push_back(stoi(this->attributeIndexes[1]));
    this->uvIndices    .push_back(stoi(this->attributeIndexes[4]));
    this->uvIndices    .push_back(stoi(this->attributeIndexes[7]));
    this->normalIndices.push_back(stoi(this->attributeIndexes[2]));
    this->normalIndices.push_back(stoi(this->attributeIndexes[5]));
    this->normalIndices.push_back(stoi(this->attributeIndexes[8]));

    attributeIndexes.clear();

    return;
}

MeshLoader::~MeshLoader()
{
    if( file.is_open() )
    {
        file.close();
    }
	std::cout << GREEN_TEXT << "Calling destructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
};

