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

#include "../include/lazarus_mesh.h"

MeshManager::MeshManager(GLuint shader)
{
	std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
    this->shaderProgram = shader;
    this->finder = std::make_unique<FileReader>();

    this->clearMeshStorage();

    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureAtlas"), 1);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureArray"), 2);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureCube"), 3);

    this->modelMatrixUniformLocation = glGetUniformLocation(this->shaderProgram, "modelMatrix");
    this->is3DUniformLocation = glGetUniformLocation(this->shaderProgram, "spriteAsset");
    this->isGlyphUniformLocation = glGetUniformLocation(this->shaderProgram, "glyphAsset");
    this->isSkyBoxUniformLocation = glGetUniformLocation(this->shaderProgram, "isSkyBox");

    this->textureLayerUniformLocation = glGetUniformLocation(this->shaderProgram, "textureLayer");  
};

MeshManager::Mesh MeshManager::create3DAsset(string meshPath, string materialPath, string texturePath, bool selectable)
{
    this->meshOut = {};
    this->meshData = {};

    meshOut.is3D = 1;
    meshOut.isGlyph = 0;
    meshOut.isSkybox = 0;
    
    meshData.textureUnit = GL_TEXTURE2;
    glActiveTexture(meshData.textureUnit);
    
    this->resolveFilepaths(texturePath, materialPath, meshPath);
    
    this->parseWavefrontObj(
        meshData.attributes,
        meshData.diffuse,
        meshData.indexes,
        meshOut.meshFilepath.c_str(),
        meshOut.materialFilepath.c_str()
    );
    
    this->setInherentProperties();
    this->initialiseMesh();
    this->makeSelectable(selectable);

    return meshOut;
};

/* ========================================================================================
    Note: There is some strange behaviour with this function.
    If a single instance of *this* class is responsible for creating quads as well as 3D
    assets - a texturing artifact will present itself when the user *creates* the quad 
    prior to creating the 3D assets. (i.e. createQuad is called before create3DAsset).

    At the quads origin in worldspace, a "shadow" of the quad will render using texture 
    number 1 off of the xyzTextureStack. Not sure why.

    This is *very* similar to the behaviour seen on MacOS and is possibly related. It 
    doesn't present itself on linux.

    Worth mentioning that this doesn't seem to happen with glyphs - which are wrapped over
    a quad under the hood.
=========================================================================================== */

MeshManager::Mesh MeshManager::createQuad(float width, float height, string texturePath, float uvXL, float uvXR, float uvY, bool selectable)
{
    if(width < 0.0f || height < 0.0f) globals.setExecutionState(LAZARUS_INVALID_DIMENSIONS);
    
    this->meshOut = {};
    this->meshData = {};

    meshOut.is3D = 0;
    meshOut.isGlyph = 0;
    meshOut.isSkybox = 0;
    
    meshData.textureUnit = GL_TEXTURE2;
    glActiveTexture(meshData.textureUnit);

    this->resolveFilepaths(texturePath);

    /* ======================================================
        Ensure that the origin is centered.
        (E.g. width 2.0f, height 2.0f becomes 
        width -1.0f, height +1.0f) 
    ========================================================= */
    float xMin = -(width / 2.0f);
    float xMax = width / 2.0f;
    float yMax = height / 2.0f;
    float yMin = -(height / 2.0f);
    /* ==========================================================
        If the UV params aren't their default values (0.0) then
        this mesh is being created for a glyph which needs to be 
        looked up in the texture atlas.

        Otherwise it's a generic sprite.
    ============================================================= */
    if(uvXL || uvXR || uvY > 0.0 )
    {
    /* ======================================================================================================
            Vertex positions,           Diffuse colors,             Normals,                    UVs 
    ========================================================================================================= */
        meshData.attributes = {                                                                                          
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, 0.0f, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, 0.0f, 0.0f), 
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, uvY, 0.0f),

            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXR, uvY, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXR, 0.0f, 0.0f),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXL, uvY, 0.0f),

            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, uvY, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, 0.0f, 0.0f),
            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, uvY, 0.0f),

            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXL, uvY, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXR, 0.0f, 0.0f), 
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXL, 0.0f, 0.0f),
        };
    }
    else
    {
        /* ==================================================
            For some reason Mac requires two sets of the 
            vertices for what at a glance looks like an
            unwinding / culling type of issue. Note the sign
            of the normals.
        ===================================================== */
        meshData.attributes = {
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 0.0f, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 0.0f, 0.0f),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 1.0f, 0.0f),

            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 1.0f, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 0.0f, 0.0f),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 1.0f, 0.0f),

            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 1.0f, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 0.0f, 0.0f),
            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 1.0f, 0.0f),

            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 1.0f, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 0.0f, 0.0f),
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 0.0f, 0.0f),
        };
    };

    meshData.indexes = {
        0, 1, 2, 6, 7, 8, 5, 4, 3, 11, 10, 9
    };

    this->setInherentProperties();
    this->initialiseMesh();
    this->makeSelectable(selectable);

    return meshOut;
}

MeshManager::Mesh MeshManager::createCube(float scale, std::string texturePath, bool selectable)
{
    float vertexPosition = scale / 2; 

    this->meshOut = {};
    this->meshData = {};

    meshOut.isGlyph = 0;

    this->resolveFilepaths(texturePath);

        /* ==================================================
            Default texture unit is GL_TEXTURE1, which is the
            samplerArray. Reset it appropriately here.
        ===================================================== */
    if(meshOut.textureFilepath == LAZARUS_SKYBOX_CUBE)
    {
        meshOut.is3D = 0;
        meshOut.isSkybox = 1;

        meshData.textureUnit = GL_TEXTURE3;
    }
    else
    {
        meshOut.is3D = 1;
        meshOut.isSkybox = 0;

        meshData.textureUnit = GL_TEXTURE2;
    };

    glActiveTexture(meshData.textureUnit);

    meshData.attributes = {                                                                                          
        /* ===========================================

               POS                      ST

          (5)_____(2, 6)           0:1 _______1:1
            \      /\               |          |
             \    /  \              |          |
              \  /    \             |          |
               \/______\            |__________|
            (1, 4)     (3)         0:0        1:0

        ============================================== */
        
        // Top face
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 0.0f, 0.0f),
        vec3(vertexPosition, vertexPosition,  vertexPosition),      vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 1.0f, 0.0f),
        vec3(vertexPosition,  vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 0.0f, 0.0f),
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition,  vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 1.0f, 0.0f),
        vec3(vertexPosition,    vertexPosition,  vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 1.0f, 0.0f),

        // Back face
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-vertexPosition,  -vertexPosition, -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(vertexPosition,  vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f),
        vec3(vertexPosition,  -vertexPosition, -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, 0.0f),

        // Right face
        vec3(vertexPosition, vertexPosition, -vertexPosition),      vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 0.0f, 0.0f),
        vec3(vertexPosition, -vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 1.0f, 0.0f),
        vec3(vertexPosition,  -vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 0.0f, 0.0f),
        vec3(vertexPosition, vertexPosition, -vertexPosition),      vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 0.0f, 0.0f),
        vec3(vertexPosition,  vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 1.0f, 0.0f),
        vec3(vertexPosition,  -vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 1.0f, 0.0f),

        // Front face
        vec3(vertexPosition, vertexPosition, vertexPosition),       vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition, -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 1.0f, 0.0f),
        vec3(vertexPosition,  -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 0.0f, 0.0f),
        vec3(vertexPosition, vertexPosition, vertexPosition),       vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition,  vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 1.0f, 0.0f),
        vec3(-vertexPosition,  -vertexPosition, vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 1.0f, 0.0f),

        // Left face
        vec3(-vertexPosition,  vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition,  -vertexPosition, -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-vertexPosition,  -vertexPosition,  vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-vertexPosition,  vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition,  vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
        vec3(-vertexPosition,  -vertexPosition,  -vertexPosition),  vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f),

        // Bottom face
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition, -vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-vertexPosition, -vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(vertexPosition, -vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
        vec3(-vertexPosition, -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f)
    };

    this->setInherentProperties();
    this->initialiseMesh();
    this->makeSelectable(selectable);

    return this->meshOut;
};

void MeshManager::initialiseMesh()
{	
    glGenVertexArrays(1, &meshData.VAO);
   	glBindVertexArray(meshData.VAO);

    if(this->modelMatrixUniformLocation >= 0)
    {
        glGenBuffers(1, &meshData.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.indexes.size() * sizeof(unsigned int), &meshData.indexes[0], GL_STATIC_DRAW);

        glGenBuffers(1, &meshData.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, meshData.VBO);
        glBufferData(GL_ARRAY_BUFFER, meshData.attributes.size() * sizeof(vec3), &meshData.attributes[0], GL_STATIC_DRAW);

        //  Vertex Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)0);
        glEnableVertexAttribArray(0);

        //  Diffuse Colors
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(1 * sizeof(vec3)));
        glEnableVertexAttribArray(1);

        //  Normals
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(2 * sizeof(vec3)));
        glEnableVertexAttribArray(2);

        //  UV Coordinates
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(3 * sizeof(vec3)));
        glEnableVertexAttribArray(3);

        this->checkErrors(__FILE__, __LINE__);

        this->dataStore.push_back(meshData);
        meshOut.id = dataStore.size();
        meshData.id = meshOut.id;

        /* ===============================================================
            Reload the entire texture stack / array if the mesh isn't
            being used for anything special. I.e. reallocate the space and
            upload all of the textures again. In the cases where a mesh is 
            used for some special purpose different texture loaders are 
            used. 
        ================================================================== */
        if(!meshOut.isSkybox && !meshOut.isGlyph)
        {
            this->prepareTextures();
        };
    }
    else
    {
        globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);
    };
	
    return;
};

void MeshManager::prepareTextures()
{
    this->extendTextureStack(globals.getMaxImageWidth(), globals.getMaxImageHeight(), this->layerCount);

    for(auto i: dataStore)
    {
        glActiveTexture(i.textureUnit);
    
        if((meshOut.textureFilepath != LAZARUS_DIFFUSE_MESH))
        {
            this->loadImageToTextureStack(i.textureData, i.textureLayer);
        };
    };

    return;
};

void MeshManager::clearMeshStorage()
{	
    for(auto i: dataStore)
    {
        glDeleteBuffers         (1, &i.VBO);
        glDeleteBuffers         (1, &i.EBO);
        glDeleteVertexArrays    (1, &i.VAO);
    };
    
    this->meshOut = {};
    this->meshData = {};
    this->dataStore = {};
	
	this->errorCode = GL_NO_ERROR;

    this->layerCount = 0;
};

void MeshManager::makeSelectable(bool selectable)
{
    if(selectable)
    {
        /* =============================================
            Items which can be picked from the stencil-
            depth buffer have their ID's stored in a
            global vector. The index position is then 
            used as the stencil function's reference 
            parameter. When that ID is then downloaded f
            rom the GPU after a draw call, it is used to 
            perform a lookup on the vector for the mesh 
            ID which; is then returned to userspace.
        ================================================ */
        meshOut.isClickable = true;
        globals.setPickableEntity(meshOut.id);
        dataStore[meshOut.id - 1].stencilBufferId = globals.getNumberOfPickableEntities();
    }
    else
    {
        meshOut.isClickable = false;
        dataStore[meshOut.id - 1].stencilBufferId = 0;
    };
};

void MeshManager::loadMesh(MeshManager::Mesh &meshIn)
{
    MeshManager::MeshData &data = dataStore[meshIn.id - 1];

    /* ===================================================
        Fill the stencil buffer with 0's. Wherever an 
        entity is occupying screenspace, fill the buffer
        with mesh uuid.
    ====================================================== */
    if(globals.getManageStencilBuffer())
    {
        glStencilMask(0xFF);
        glClearStencil(0x00);
        glStencilFunc(GL_ALWAYS, data.stencilBufferId, 0xFF);
    }

    if(this->modelMatrixUniformLocation >= 0)
    {
        glUniformMatrix4fv(this->modelMatrixUniformLocation, 1, GL_FALSE, &meshIn.modelMatrix[0][0]);
        glUniform1i(this->is3DUniformLocation, meshIn.is3D);
        glUniform1i(this->isGlyphUniformLocation, meshIn.isGlyph);
        glUniform1i(this->isSkyBoxUniformLocation, meshIn.isSkybox);
        
        if(data.textureId != 0)
        {
            glUniform1f(this->textureLayerUniformLocation, (data.textureLayer - 1));
        };

        this->checkErrors(__FILE__, __LINE__);
    }
    else
    {
        globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);
    };

    return;
};

void MeshManager::drawMesh(MeshManager::Mesh &meshIn)
{
    MeshManager::MeshData &data = dataStore[meshIn.id - 1];

    glBindVertexArray(data.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);

    glActiveTexture(data.textureUnit);

    if((meshIn.textureFilepath == LAZARUS_GLYPH_QUAD))
    {
        glBindTexture(GL_TEXTURE_2D, data.textureId);
    }
    else if((meshIn.textureFilepath == LAZARUS_SKYBOX_CUBE))
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, data.textureId);
    }
    else if((meshIn.textureFilepath != LAZARUS_DIFFUSE_MESH))
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, data.textureId);
    };

    // glDrawArrays(GL_TRIANGLES, 0, data.attributes.size());
    glDrawElements(GL_TRIANGLES, data.indexes.size(), GL_UNSIGNED_INT, nullptr);

    this->checkErrors(__FILE__, __LINE__);

    return;
};

void MeshManager::resolveFilepaths(string texPath, string mtlPath, string objPath)
{
    objPath != LAZARUS_PRIMITIVE_MESH
    ? meshOut.meshFilepath =  finder->relativePathToAbsolute(objPath)
    : meshOut.meshFilepath = LAZARUS_PRIMITIVE_MESH;

    mtlPath != LAZARUS_TEXTURED_MESH
    ? meshOut.materialFilepath =  finder->relativePathToAbsolute(mtlPath)
    : meshOut.materialFilepath = LAZARUS_TEXTURED_MESH;

    switch (texPath[0])
    {
        //  Glyph atlas
        case 'G':
            meshOut.textureFilepath = LAZARUS_GLYPH_QUAD;
            meshOut.isGlyph = 1;

            meshData.textureUnit = GL_TEXTURE1;
            meshData.textureId = this->bitmapTexture;;
            meshData.textureLayer = 0;
            meshData.textureData.pixelData = NULL;
            meshData.textureData.height = 0;
            meshData.textureData.width = 0;
            break;
        
        //  Skybox cubemap
        case 'S':
            meshOut.textureFilepath = LAZARUS_SKYBOX_CUBE;
            meshOut.isSkybox = 1;

            meshData.textureUnit = GL_TEXTURE3;
            meshData.textureId = this->cubeMapTexture;
            meshData.textureLayer = 1;
            meshData.textureData.pixelData = NULL;
            meshData.textureData.height = 0;
            meshData.textureData.width = 0;
            break;

        //  Diffuse color
        case 'D':
    	    meshOut.textureFilepath = LAZARUS_DIFFUSE_MESH;

            meshData.textureLayer = 0; 
            meshData.textureId = 0;
            meshData.textureData.pixelData = NULL;
            meshData.textureData.height = 0;
            meshData.textureData.width = 0;
            break;
        
        //  Image array
        default:
            this->layerCount += 1;

	        meshOut.textureFilepath = finder->relativePathToAbsolute(texPath);

            meshData.textureUnit = GL_TEXTURE2;
            meshData.textureId = this->textureStack;
            meshData.textureLayer = this->layerCount;
            meshData.textureData = finder->readFromImage(meshOut.textureFilepath);
            break;
    };

    return;
};

void MeshManager::setInherentProperties()
{
    meshOut.locationX = 0;
    meshOut.locationY = 0;
    meshOut.locationZ = 0;

    meshOut.modelMatrix = mat4(1.0f);

    meshOut.numOfVertices = meshData.attributes.size() / 4;
    meshOut.numOfFaces = (meshOut.numOfVertices) / 3;

   return;
}

void MeshManager::checkErrors(const char *file, int line)
{
    this->errorCode = glGetError();
    
    if(this->errorCode != 0)
    {
        std::cerr << RED_TEXT << file << " (" << line << ")" << RESET_TEXT << std::endl;
        std::cerr << RED_TEXT << "ERROR::GL_ERROR::CODE " << RESET_TEXT << this->errorCode << std::endl;

        globals.setExecutionState(LAZARUS_OPENGL_ERROR);
    }

    return;
};

MeshManager::~MeshManager()
{
    for(auto i: dataStore)
    {
        glDeleteBuffers         (1, &i.VBO);
        glDeleteBuffers         (1, &i.EBO);
        glDeleteVertexArrays    (1, &i.VAO);
    };

    this->checkErrors(__FILE__, __LINE__);

    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
};

MeshLoader::MeshLoader()
{
	std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
	this->materialIdentifierIndex	=	0;
	this->triangleCount				=	0;
};

bool MeshLoader::parseWavefrontObj(vector<vec3> &outAttributes, vector<vec3> &outDiffuse, vector<unsigned int> &outIndexes, const char* meshPath, const char* materialPath) 
{
    this->coordinates.clear();
    this->vertexIndices.clear();
    this->normalIndices.clear();
    this->uvIndices.clear();
    this->attributeIndexes.clear();
    this->materialBuffer.clear();
    this->materialData.clear();
	//  Temps are probably no longer needed
    this->tempVertexPositions.clear();
    this->tempNormals.clear();
    this->tempUvs.clear();
    this->tempDiffuse.clear();
	
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
        
        this->loadMaterial(outDiffuse, materialBuffer, materialPath);
    }

    this->interleaveBufferData(outAttributes, outIndexes, outDiffuse, this->vertexIndices.size());

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

void MeshLoader::interleaveBufferData(vector<vec3> &outAttributes, vector<unsigned int> &outIndexes, vector<vec3> outDiffuse, int numOfAttributes)
{
    struct Vertex
    {
        vec3 position;
        vec3 diffuseColor;
        vec3 normalCoordinates;
        vec3 uvCoordinates;
    };

    vector<Vertex> tempVertexes = {};

    int count = 0;

    for( int i = 0; i < numOfAttributes; i++ )
    {
        Vertex vertex = {};
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int normalIndex = normalIndices[i];
        unsigned int uvIndex     = uvIndices[i];
        
        /* =========================================
            uv is extended from its generic xy components
            to include a z value here to meet the expected
            stride range for attributes in the vertex buffer.
            
            i.e: (4 * sizeof(vec3)) = 12

            Once in the shaders it is disregarded. 
        ============================================ */
        vertex.position          = tempVertexPositions[vertexIndex - 1];
        vertex.diffuseColor      = outDiffuse[i];
        vertex.normalCoordinates = tempNormals[normalIndex - 1];
        vertex.uvCoordinates     = vec3(tempUvs[uvIndex - 1].x, tempUvs[uvIndex - 1].y, 0.0f);

        if(tempVertexes.size() == 0)
        {
            tempVertexes.push_back(vertex);
            outIndexes.push_back(count);
        }
        else
        {
            int beforeSize = outIndexes.size();

            for(unsigned int j = 0; j < tempVertexes.size(); j++)
            {
                Vertex previouslyValidated = tempVertexes[j];

                if(
                    (previouslyValidated.position          == vertex.position)          &&
                    (previouslyValidated.diffuseColor      == vertex.diffuseColor)      &&
                    (previouslyValidated.normalCoordinates == vertex.normalCoordinates) && 
                    (previouslyValidated.uvCoordinates     == vertex.uvCoordinates)
                )
                {
                    outIndexes.push_back(j);
                }
            };

            int currentSize = outIndexes.size(); 

            if(currentSize == beforeSize)
            {
                tempVertexes.push_back(vertex);
                count += 1;
                outIndexes.push_back(count);
            };
        }
    }

    for(auto vert : tempVertexes)
    {
        outAttributes.push_back(vert.position);
        outAttributes.push_back(vert.diffuseColor);
        outAttributes.push_back(vert.normalCoordinates);
        outAttributes.push_back(vert.uvCoordinates);
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
	std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
};

MaterialLoader::MaterialLoader()
{
	std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
	
	textureLoader = nullptr;
	diffuseCount = 0;
    texCount = 0;
};

bool MaterialLoader::loadMaterial(vector<vec3> &out, vector<vector<int>> data ,string materialPath)
{
    diffuseCount = 0;
    texCount = 0;

    file.open(materialPath.c_str());
    
    if( !file.is_open() )
    {
        globals.setExecutionState(LAZARUS_FILE_UNREADABLE);
        return false;
    }   

    while( file.getline(currentLine, 256) ) 
    {        
        /* =============================================
            Kd = diffuse colors
        ================================================ */
        if( (currentLine[0] == 'K') && (currentLine[1] == 'd') )
        {
            diffuseCount += 1;
            for(auto i: data)
            {
            	int index = i[0];
            	int faceCount = i[1];
            	
	            if(diffuseCount == index) {
                    string currentString = currentLine;
                    stringstream ss(currentString);
                    string token;

                    vector<string> tokenStore;
                    while(getline(ss, token, ' ')) 
                    {
                        tokenStore.push_back(token);
                    }

                    diffuse.r = stof(tokenStore[1]);
                    diffuse.g = stof(tokenStore[2]);
                    diffuse.b = stof(tokenStore[3]);
                    /* ====================================================
                        Push the current diffuse object into the out
                        out parameter N times.

                        N = The number of vertices which use this color.
                        (faceCount * 3)
                    ======================================================= */
    	            for(int j = 0; j < faceCount * 3; j++)
    	            {
    	                out.push_back(diffuse);
    	            };
    	        };        
            };
        }
        /* ==========================================
            map_Kd = Image texture
        ============================================= */
        if( ((currentLine[0] == 'm') && (currentLine[1] == 'a') && (currentLine[2] == 'p')))
        {
            texCount += 1;
            if( diffuseCount == 0 )
            {
                for(auto i: data)
                {
                    int faceCount = i[1];
                    for(int j = 0; j < faceCount * 3; j++)
                    {
                        /* ===========================================
                            Negative values passed here are an indicator
                            to the fragment shader that it should instead 
                            interpret the desired frag color of this face
                            from the current layer of the sampler array 
                            (an image) instead of a diffuse texture.

                            i.e: 
                                positiveDiffuseValues
                                ? fragColor(positiveDiffuseValues.xyz) 
                                : fragColor(images[layer].xyz)
                        ============================================== */
                        out.push_back(vec3(-0.1f, -0.1f, -0.1f));
                    }
                }
            }
        }
    };

    if (file.eof())
    {
        file.close();
    }

    return true;
};

MaterialLoader::~MaterialLoader()
{
    if( file.is_open() )
    {
        file.close();
    }
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
};