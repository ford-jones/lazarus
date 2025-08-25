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

MeshManager::MeshManager(GLuint shader, TextureLoader::StorageType textureType)
    : MeshManager::TextureLoader(textureType)
{
	LOG_DEBUG("Constructing Lazarus::MeshManager");

    this->shaderProgram = shader;
    this->finder = std::make_unique<FileLoader>();

    this->clearMeshStorage();

    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureAtlas"), 1);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureArray"), 2);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureCube"), 3);

    this->modelMatrixUniformLocation = glGetUniformLocation(this->shaderProgram, "modelMatrix");
    this->is3DUniformLocation = glGetUniformLocation(this->shaderProgram, "spriteAsset");
    this->isGlyphUniformLocation = glGetUniformLocation(this->shaderProgram, "glyphAsset");
    this->isSkyBoxUniformLocation = glGetUniformLocation(this->shaderProgram, "isSkyBox");

    this->textureLayerUniformLocation = glGetUniformLocation(this->shaderProgram, "textureLayer");  

    this->maxTexWidth = 0;
    this->maxTexHeight = 0;

    this->textureStorage = textureType;
};

MeshManager::Mesh MeshManager::create3DAsset(string meshPath, string materialPath, string texturePath, bool selectable)
{
    this->meshOut = {};
    this->meshData = {};

    // meshOut.is3D = 1;
    // meshOut.isGlyph = 0;
    // meshOut.isSkybox = 0;
    
    meshData.textureUnit = GL_TEXTURE2;
    glActiveTexture(meshData.textureUnit);

    meshOut.meshFilepath = meshPath;
    meshOut.materialFilepath = materialPath;
    meshOut.textureFilepath = texturePath;
    
    /* ==========================================
        Determine whether the file is wavefront
        or gltf/glb.
    ============================================= */
    uint32_t suffixDelimiter = meshPath.find_last_of(".");
    std::string suffix = meshPath.substr(suffixDelimiter + 1);

    meshOut.material.type = MaterialType::BASE_COLOR;
    
    if(suffix.compare("obj") == 0)
    {
        this->parseWavefrontObj(
            meshData.attributes,
            meshData.diffuse,
            meshData.indexes,
            meshOut.meshFilepath.c_str(),
            meshOut.materialFilepath.c_str()
        );
        meshOut.type = MeshManager::MeshType::LOADED_WAVEFRONT;

        if(texturePath != "")
        {
            meshOut.material.type = MaterialType::IMAGE_TEXTURE;
        }
    }
    else if(suffix.compare("glb") == 0)
    {
        this->parseGlBinary(
            meshData.attributes, 
            meshData.diffuse, 
            meshData.indexes, 
            meshData.textureData,
            meshOut.meshFilepath.c_str()
        );

        meshOut.type = MeshManager::MeshType::LOADED_GLB;
        //  If this is indeed null, do these values still get set?

        if(meshData.textureData.pixelData != NULL)
        {
            this->layerCount += 1;

            meshOut.material.type = MaterialType::IMAGE_TEXTURE;
            meshOut.textureFilepath = "";

            meshData.textureUnit = GL_TEXTURE2;
            meshData.textureLayer = this->layerCount;
            meshData.textureId = this->textureStack;
        };
    };
    
    this->setMaterialProperties();
    this->setSharedProperties();
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

MeshManager::Mesh MeshManager::createQuad(float width, float height, string texturePath, float uvXL, float uvXR, float uvYU, float uvYD, bool selectable)
{
    if(width < 0.0f || height < 0.0f)
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);
        globals.setExecutionState(StatusCode::LAZARUS_INVALID_DIMENSIONS);
    };
    
    this->meshOut = {};
    this->meshData = {};

    meshOut.type = MeshManager::MeshType::PLANE;

    meshOut.meshFilepath = "";
    meshOut.materialFilepath = "";
    meshOut.textureFilepath = texturePath;
    
    meshData.textureUnit = GL_TEXTURE2;
    glActiveTexture(meshData.textureUnit);

    meshOut.material.type = MaterialType::IMAGE_TEXTURE;

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
    if(uvXL || uvXR || uvYU > 0.0f)
    {
    /* ======================================================================================================
            Vertex positions,           Diffuse colors,             Normals,                    UVs 
    ========================================================================================================= */
        meshData.attributes = {                                                                                          
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, uvYD, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, uvYD, 0.0f), 
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, uvYU, 0.0f),
            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, uvYU, 0.0f),

            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXR, uvYU, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXR, uvYD, 0.0f),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXL, uvYU, 0.0f),
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(uvXL, uvYD, 0.0f),
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
            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 1.0f, 0.0f),

            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 1.0f, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 0.0f, 0.0f),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 1.0f, 0.0f),
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 0.0f, 0.0f),
        };
    };

    meshData.indexes = {
        0, 1, 2, 
        2, 1, 3, 
        4, 5, 6, 
        6, 5, 7
    };

    this->setMaterialProperties();
    this->setSharedProperties();
    this->initialiseMesh();
    this->makeSelectable(selectable);

    return meshOut;
}

MeshManager::Mesh MeshManager::createCube(float scale, std::string texturePath, bool selectable)
{
    float vertexPosition = scale / 2.0f; 

    this->meshOut = {};
    this->meshData = {};

    meshOut.type = MeshManager::MeshType::CUBE;

    meshOut.meshFilepath = "";
    meshOut.materialFilepath = "";
    meshOut.textureFilepath = texturePath;

    // meshOut.isGlyph = 0;
    meshOut.material.type = MaterialType::IMAGE_TEXTURE;
    /* ==================================================
        Default texture unit is GL_TEXTURE1, which is the
        samplerArray. Reset it appropriately here.
    ===================================================== */
    if(this->textureStorage == TextureLoader::StorageType::CUBEMAP)
    {
        // meshOut.is3D = 0;
        // meshOut.isSkybox = 1;

        meshData.textureUnit = GL_TEXTURE3;
    }
    else
    {
        // meshOut.is3D = 1;
        // meshOut.isSkybox = 0;

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
        vec3(-vertexPosition,  vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 1.0f, 0.0f),

        // Back face
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-vertexPosition,  -vertexPosition, -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(vertexPosition,  vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f),

        // Right face
        vec3(vertexPosition, vertexPosition, -vertexPosition),      vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 0.0f, 0.0f),
        vec3(vertexPosition, -vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 1.0f, 0.0f),
        vec3(vertexPosition,  -vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 0.0f, 0.0f),
        vec3(vertexPosition,  vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 1.0f, 0.0f),

        // Front face
        vec3(vertexPosition, vertexPosition, vertexPosition),       vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition, -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 1.0f, 0.0f),
        vec3(vertexPosition,  -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 0.0f, 0.0f),
        vec3(-vertexPosition,  vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 1.0f, 0.0f),

        // Left face
        vec3(-vertexPosition,  vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition,  -vertexPosition, -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-vertexPosition,  -vertexPosition,  vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(-vertexPosition,  vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),

        // Bottom face
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-vertexPosition, -vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f),
        vec3(-vertexPosition, -vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),
        vec3(vertexPosition, -vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),
    };

    meshData.indexes = {
        0, 1, 2, 0, 3, 1,
        4, 5, 6, 4, 7, 5, 
        8, 9, 10, 8, 11, 9,
        12, 13, 14, 12, 15, 13,
        16, 17, 18, 16, 19, 17,
        20, 21, 22, 20, 23, 21
    };
    
    this->setMaterialProperties();
    this->setSharedProperties();
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
        this->clearErrors();

        glGenBuffers(1, &meshData.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, 
            meshData.indexes.size() * sizeof(uint32_t), 
            &meshData.indexes[0], 
            GL_STATIC_DRAW
        );

        glGenBuffers(1, &meshData.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, meshData.VBO);
        glBufferData(
            GL_ARRAY_BUFFER, 
            meshData.attributes.size() * sizeof(vec3), 
            &meshData.attributes[0], 
            GL_STATIC_DRAW
        );

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

        meshOut.id = dataStore.size() + 1;
        meshData.id = meshOut.id;
        this->dataStore.insert(std::pair<uint32_t, MeshManager::MeshData>(meshOut.id, meshData));

        /* ===============================================================
            Reload the entire texture stack / array if the mesh isn't
            being used for anything special. I.e. reallocate the space and
            upload all of the textures again. In the cases where a mesh is 
            used for some special purpose different texture loaders are 
            used. 
        ================================================================== */

        if(meshOut.material.type == MaterialType::IMAGE_TEXTURE && 
            meshOut.material.textureStoreVariant == TextureLoader::StorageType::ARRAY)
        {
            this->prepareTextures();
        };
    }
    else
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);
        globals.setExecutionState(StatusCode::LAZARUS_MATRIX_LOCATION_ERROR);
    };
	
    return;
};

void MeshManager::prepareTextures()
{
    uint32_t width = 0;
    uint32_t height = 0;

    if(globals.getEnforceImageSanity())
    {
        width = globals.getMaxImageWidth();
        height = globals.getMaxImageHeight();
    }
    else
    {
        width = maxTexWidth;
        height = maxTexHeight;
    }

    this->extendTextureStack(width, height, this->layerCount);

    for(auto i: dataStore)
    {
        glActiveTexture(i.second.textureUnit);
    
        if(meshOut.material.type == MaterialType::IMAGE_TEXTURE && 
           meshOut.material.textureStoreVariant == TextureLoader::StorageType::ARRAY)
        {
            this->loadImageToTextureStack(i.second.textureData, i.second.textureLayer);
        };
    };

    return;
};

void MeshManager::clearMeshStorage()
{	
    for(auto i: dataStore)
    {
        glDeleteBuffers         (1, &i.second.VBO);
        glDeleteBuffers         (1, &i.second.EBO);
        glDeleteVertexArrays    (1, &i.second.VAO);
    };
    
    this->meshOut = {};
    this->meshData = {};
    this->dataStore = {};
	
	this->errorCode = GL_NO_ERROR;

    this->layerCount = 0;

    return;
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
        dataStore.at(meshOut.id).stencilBufferId = globals.getNumberOfPickableEntities();
    }
    else
    {
        meshOut.isClickable = false;
        dataStore.at(meshOut.id).stencilBufferId = 0;
    };

    return;
};

void MeshManager::loadMesh(MeshManager::Mesh &meshIn)
{
    MeshManager::MeshData &data = dataStore.at(meshIn.id);

    /* ===================================================
        Fill the stencil buffer with 0's. 
        Wherever an entity is occupying screenspace, fill 
        the buffercwith the mesh's selection.
    ====================================================== */
    if(globals.getManageStencilBuffer())
    {
        this->clearErrors();

        glStencilMask(0xFF);
        glClearStencil(0x00);
        glStencilFunc(GL_ALWAYS, data.stencilBufferId, 0xFF);
        
        this->checkErrors(__FILE__, __LINE__);
    }

    if(this->modelMatrixUniformLocation >= 0)
    {
        this->clearErrors();

        glUniformMatrix4fv(
            this->modelMatrixUniformLocation, 
            1, 
            GL_FALSE, 
            &meshIn.modelMatrix[0][0]
        );

        //  TODO:
        //  This could be 1 upload which is check against the StorageType on the other side
        
        //  is3d
        glUniform1i(this->is3DUniformLocation, (meshIn.material.textureStoreVariant != TextureLoader::StorageType::ATLAS && meshIn.material.textureStoreVariant != TextureLoader::StorageType::CUBEMAP) ? 1 : 0);
        //  isglyph
        glUniform1i(this->isGlyphUniformLocation, meshIn.material.textureStoreVariant == TextureLoader::StorageType::ATLAS ? 1 : 0);
        //  isskybox
        glUniform1i(this->isSkyBoxUniformLocation, meshIn.material.textureStoreVariant == TextureLoader::StorageType::CUBEMAP ? 1 : 0);
        
        if(data.textureId != 0)
        {
            glUniform1f(this->textureLayerUniformLocation, (data.textureLayer - 1));
        };

        this->checkErrors(__FILE__, __LINE__);
    }
    else
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);
        globals.setExecutionState(StatusCode::LAZARUS_MATRIX_LOCATION_ERROR);
    };

    return;
};

void MeshManager::drawMesh(MeshManager::Mesh &meshIn)
{
    MeshManager::MeshData &data = dataStore.at(meshIn.id);

    this->clearErrors();

    glBindVertexArray(data.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);

    glActiveTexture(data.textureUnit);

    if(meshIn.material.type == MaterialType::IMAGE_TEXTURE)
    {
        switch (meshIn.material.textureStoreVariant)
        {
        case TextureLoader::StorageType::ATLAS:
            glBindTexture(GL_TEXTURE_2D, data.textureId);
            break;
        
        case TextureLoader::StorageType::CUBEMAP:
            glBindTexture(GL_TEXTURE_CUBE_MAP, data.textureId);
            break;
        
        case TextureLoader::StorageType::ARRAY:
            glBindTexture(GL_TEXTURE_2D_ARRAY, data.textureId);
            break;
        
        default:
            break;
        };
    }

    glDrawElements(GL_TRIANGLES, data.indexes.size(), GL_UNSIGNED_INT, nullptr);

    this->checkErrors(__FILE__, __LINE__);

    return;
};

void MeshManager::setMaterialProperties()
{
    //  TODO:
    //  Refactor out this entire function
    //  1). MeshData material stuff should be cleaned up
    //  2). These structs should be per-material and in vectors

    if(meshOut.material.type == MaterialType::IMAGE_TEXTURE)
    {
        switch(this->textureStorage)
        {
            case TextureLoader::StorageType::ATLAS:
                meshOut.textureFilepath = "";
                // meshOut.isGlyph = 1;

                meshData.textureUnit = GL_TEXTURE1;
                meshData.textureId = this->bitmapTexture;
                meshData.textureLayer = 0;
                meshData.textureData.pixelData = NULL;
                meshData.textureData.height = 0;
                meshData.textureData.width = 0;
                break;
            
            case TextureLoader::StorageType::CUBEMAP:
                meshOut.textureFilepath = "";
                // meshOut.isSkybox = 1;

                meshData.textureUnit = GL_TEXTURE3;
                meshData.textureId = this->cubeMapTexture;
                meshData.textureLayer = 1;
                meshData.textureData.pixelData = NULL;
                meshData.textureData.height = 0;
                meshData.textureData.width = 0;
                break;
            
            case TextureLoader::StorageType::ARRAY:
                /* =======================================
                    In the case of glb files, this info 
                    has already been ascertained during 
                    parsing.
                ========================================== */

                if(meshOut.type != MeshManager::MeshType::LOADED_GLB && meshOut.textureFilepath.size() > 0)
                {
                    this->layerCount += 1;

                    meshData.textureUnit = GL_TEXTURE2;
                    meshData.textureId = this->textureStack;
                    meshData.textureLayer = this->layerCount;
                    meshData.textureData = finder->loadImage(meshOut.textureFilepath.c_str());
                }
                break;
        }
    }
    else
    {
        meshOut.textureFilepath = "";

        meshData.textureLayer = 0; 
        meshData.textureId = 0;
        meshData.textureData.pixelData = NULL;
        meshData.textureData.height = 0;
        meshData.textureData.width = 0;
    };

    meshOut.material.textureStoreVariant = this->textureStorage;

    return;
};

void MeshManager::setSharedProperties()
{
    //  Placeholder
    meshOut.material.id = meshOut.id;

    /* =========================================
        Meshes are created at the origin looking
        down the z-axis at 1:1 scale to that 
        which was specified during VBO
        construction.
    ============================================ */

    meshOut.position = glm::vec3(0.0f, 0.0f, 0.0f);
    meshOut.direction = glm::vec3(0.0f, 0.0f, 1.0f);
    meshOut.scale = glm::vec3(1.0f, 1.0f, 1.0f);

    meshOut.modelMatrix = mat4(1.0f);

    meshOut.numOfVertices = meshData.attributes.size() / 4;
    meshOut.numOfFaces = (meshOut.numOfVertices) / 3;

    /* =============================================================
        In the case that image sanitisation is not enabled, we must
        track which image in the textureStack is the largest.
        Would be nice if this whole business of size checking could
        be done elsewhere.
    ================================================================ */
    this->maxTexWidth = std::max(maxTexWidth, meshData.textureData.width);
    this->maxTexHeight = std::max(maxTexHeight, meshData.textureData.height);

   return;
}

void MeshManager::checkErrors(const char *file, uint32_t line)
{
    this->errorCode = glGetError();
    
    if(this->errorCode != GL_NO_ERROR)
    {
        std::string message = std::string("OpenGL Error: ").append(std::to_string(this->errorCode));
        LOG_ERROR(message.c_str(), file, line);

        globals.setExecutionState(StatusCode::LAZARUS_OPENGL_ERROR);
    }

    return;
};

void MeshManager::clearErrors()
{
    /* ============================================================
        Reset OpenGL's error state by flushing out all of the 
        internal state flags containing the error value (which may 
        be several). This is so that persistence of an errors 
        life span is limitted to that function which caused it. 
        By doing so, subsequent glGetError calls made from a function
        other than that which actually caused the error will NOT 
        throw. 
        
        Absolutely painful, see:
        https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml#:~:text=glGetError%20should%20always%20be%20called%20in%20a%20loop
    ================================================================ */

    this->errorCode = glGetError();

    while(this->errorCode != GL_NO_ERROR)
    {
        this->errorCode = glGetError();
    };
};

MeshManager::~MeshManager()
{
    LOG_DEBUG("Destroying Lazarus::MeshManager");

    this->clearErrors();

    for(auto i: dataStore)
    {
        glDeleteBuffers         (1, &i.second.VBO);
        glDeleteBuffers         (1, &i.second.EBO);
        glDeleteVertexArrays    (1, &i.second.VAO);
    };

    this->checkErrors(__FILE__, __LINE__);
};