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

#include "../include/mesh.h"

Mesh::Mesh(GLuint shader)
{
	std::cout << GREEN_TEXT << "Constructing class 'Mesh'." << RESET_TEXT << std::endl;
	this->shaderProgram = shader;
	
	this->finder = nullptr;
	this->meshLoader = nullptr;
    this->texLoader = std::make_unique<TextureLoader>();
	this->triangulatedMesh = nullptr;
    this->quad = nullptr;

    this->vertexAttributes = {};
    this->diffuseColors = {};
    
    this->textureId = 0;
	
	this->errorCode = GL_NO_ERROR;
};

std::shared_ptr<Mesh::TriangulatedMesh> Mesh::createTriangulatedMesh(string meshPath, string materialPath, string texturePath)
{
    this->finder = std::make_unique<FileReader>();
    this->meshLoader = std::make_unique<MeshLoader>();
    this->triangulatedMesh = std::make_shared<Mesh::TriangulatedMesh>();

    srand(time((0)));
    triangulatedMesh->id = 1 + (rand() % 2147483647);

    triangulatedMesh->is3D = 1;

    triangulatedMesh->meshFilepath =  finder->relativePathToAbsolute(meshPath);
    triangulatedMesh->materialFilepath = finder->relativePathToAbsolute(materialPath);
    
    if(texturePath != "")
    {
	    triangulatedMesh->textureFilepath = finder->relativePathToAbsolute(texturePath);
    }
    else
    {
    	triangulatedMesh->textureFilepath = LAZARUS_MESH_NOTEX;
    };
    
    this->meshLoader->loadMesh(
        this->vertexAttributes,
        this->diffuseColors,
        this->textureId,
        this->texStore,
        triangulatedMesh->meshFilepath.c_str(),
        triangulatedMesh->materialFilepath.c_str(),
        triangulatedMesh->textureFilepath.c_str()
    );
    
    triangulatedMesh->locationX = 0;
    triangulatedMesh->locationY = 0;
    triangulatedMesh->locationZ = 0;

    triangulatedMesh->attributes = this->vertexAttributes;

    triangulatedMesh->textureId = this->textureId;
    triangulatedMesh->textureData.width = this->texStore.width;
    triangulatedMesh->textureData.height = this->texStore.height;
    triangulatedMesh->textureData.pixelData = this->texStore.pixelData;

    triangulatedMesh->modelviewMatrix = mat4(1.0f);                                                                                          //  Define the model-view matrix to default 4x4

    triangulatedMesh->modelviewUniformLocation = glGetUniformLocation(shaderProgram, "modelMatrix");                                                //  Retrieve the locations of where vert and frag shaders uniforms should be stored
    triangulatedMesh->samplerUniformLocation = glGetUniformLocation(shaderProgram, "threeDimensionalMeshTextures");
    triangulatedMesh->textureLayerUniformLocation = glGetUniformLocation(shaderProgram, "threeDimensionalTexLayerIndex");
    triangulatedMesh->is3DUniformLocation = glGetUniformLocation(shaderProgram, "meshHasThreeDimensions");

    triangulatedMesh->numOfVertices = vertexAttributes.size() / 4;
    triangulatedMesh->numOfFaces = (triangulatedMesh->numOfVertices) / 3;

    return triangulatedMesh;
};

std::shared_ptr<Mesh::TriangulatedMesh> Mesh::createQuad(float width, float height, string texturePath)
{
    this->quad = std::make_shared<TriangulatedMesh>();
    this->finder = std::make_unique<FileReader>();

    srand(time((0)));
    quad->id = 1 + (rand() % 2147483647);

    quad->is3D = 0;

    quad->meshFilepath = "";
    quad->materialFilepath = "";

    if(texturePath != "")
    {
	    quad->textureFilepath = finder->relativePathToAbsolute(texturePath);
    }
    else
    {
    	quad->textureFilepath = LAZARUS_MESH_NOTEX;
    };

    /* ======================================================================================================
            Vertex positions,           Diffuse colors,             Normals,                    UVs 
    ========================================================================================================= */
        quad->attributes = {
        vec3(0.0f, 0.0f, 0.0f),     vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 0.0f, 0.0f),
        vec3(width, 0.0f, 0.0f),    vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, height, 0.0f),   vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 1.0f, 0.0f),

        vec3(width, height, 0.0f),  vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 1.0f, 0.0f),
        vec3(width, 0.0f, 0.0f),    vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, height, 0.0f),   vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 1.0f, 0.0f),
    
        vec3(0.0f, height, 0.0f),   vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 1.0f, 0.0f),
        vec3(width, 0.0f, 0.0f),    vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 0.0f, 0.0f),
        vec3(width, height, 0.0f),  vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 1.0f, 0.0f),

        vec3(0.0f, height, 0.0f),   vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 1.0f, 0.0f),
        vec3(width, 0.0f, 0.0f),    vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, 0.0f, 0.0f),     vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 0.0f, 0.0f),
    };

    if(quad->textureFilepath != LAZARUS_MESH_NOTEX)
    {
        texLoader->storeTexture(quad->textureFilepath, this->textureId, this->texStore);
    }

    quad->textureId = this->textureId;
    quad->textureData.width = this->texStore.width;
    quad->textureData.height = this->texStore.height;
    quad->textureData.pixelData = this->texStore.pixelData;

    quad->locationX = 0;
    quad->locationY = 0;
    quad->locationZ = 0;

    quad->modelviewMatrix = mat4(1.0f);

    quad->modelviewUniformLocation = glGetUniformLocation(shaderProgram, "modelMatrix");                                                //  Retrieve the locations of where vert and frag shaders uniforms should be stored
    quad->samplerUniformLocation = glGetUniformLocation(shaderProgram, "twoDimensionalMeshTextures");
    quad->textureLayerUniformLocation = glGetUniformLocation(shaderProgram, "twoDimensionalTexLayerIndex");
    quad->is3DUniformLocation = glGetUniformLocation(shaderProgram, "meshHasThreeDimensions");

    quad->numOfVertices = vertexAttributes.size() / 4;
    quad->numOfFaces = (quad->numOfVertices) / 3;

    return quad;
}

std::shared_ptr<Mesh::TriangulatedMesh> Mesh::initialiseMesh(std::shared_ptr<TriangulatedMesh> meshData)
{
	if(triangulatedMesh != nullptr)
	{
		triangulatedMesh.reset();
	};

	triangulatedMesh = std::move(meshData);
	
    glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

    glGenBuffers(1, &this->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

    glBufferData(GL_ARRAY_BUFFER, triangulatedMesh->attributes.size() * sizeof(vec3), &triangulatedMesh->attributes[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(1 * sizeof(vec3)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(3 * sizeof(vec3)));
    glEnableVertexAttribArray(3);

    this->checkErrors(__PRETTY_FUNCTION__);

    if(triangulatedMesh->textureFilepath != LAZARUS_MESH_NOTEX)
    {
        texLoader->loadTexture(triangulatedMesh->textureData, triangulatedMesh->textureId);
    }
	
    return triangulatedMesh;
};

std::shared_ptr<Mesh::TriangulatedMesh> Mesh::loadMesh(shared_ptr<TriangulatedMesh> meshData)
{
	if(triangulatedMesh != nullptr)
	{
		triangulatedMesh.reset();
	};
	
	triangulatedMesh = std::move(meshData);

    glUniformMatrix4fv(triangulatedMesh->modelviewUniformLocation, 1, GL_FALSE, &triangulatedMesh->modelviewMatrix[0][0]);                                    //  Pass the values for each uniform into the shader program
    glUniform1i(triangulatedMesh->is3DUniformLocation, triangulatedMesh->is3D);

    if(triangulatedMesh->textureId != 0)
    {
        glUniform1i(triangulatedMesh->samplerUniformLocation, 0);
        glUniform1f(triangulatedMesh->textureLayerUniformLocation, (triangulatedMesh->textureId - 1));
    }

    this->checkErrors(__PRETTY_FUNCTION__);

    return triangulatedMesh;
};

std::shared_ptr<Mesh::TriangulatedMesh> Mesh::drawMesh(shared_ptr<TriangulatedMesh> meshData)
{
	if(triangulatedMesh != nullptr)
	{
		triangulatedMesh.reset();
	};
	
	triangulatedMesh = std::move(meshData);
	
    glDrawArrays(GL_TRIANGLES, 0, triangulatedMesh->attributes.size());

    this->checkErrors(__PRETTY_FUNCTION__);

    this->releaseMesh();

    return triangulatedMesh;
};

void Mesh::checkErrors(const char *invoker)
{
    this->errorCode = glGetError();                                                                                       //  Check for errors
    
    if(this->errorCode != 0)                                                                                                  //  If a valid error code is returned from OpenGL
    {
        std::cerr << RED_TEXT << "ERROR::GL_ERROR::CODE " << RESET_TEXT << this->errorCode << std::endl;                      //  Print it to the console
        std::cerr << RED_TEXT << "INVOKED BY: " << RESET_TEXT << invoker << std::endl;                      //  Print it to the console
    };

};

void Mesh::releaseMesh()
{
    glDeleteVertexArrays    (1, &this->VAO);
    glDeleteBuffers         (1, &this->VBO);

    this->checkErrors(__PRETTY_FUNCTION__);
};

Mesh::~Mesh()
{
    this->releaseMesh();
    std::cout << GREEN_TEXT << "Destroying 'Mesh' class." << RESET_TEXT << std::endl;
};