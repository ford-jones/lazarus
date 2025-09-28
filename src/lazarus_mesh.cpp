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
    : MeshManager::AssetLoader(), 
      MeshManager::TextureLoader(textureType)
{
	LOG_DEBUG("Constructing Lazarus::MeshManager");

    this->shaderProgram = shader;
    this->finder = std::make_unique<FileLoader>();

    this->clearMeshStorage();

    /* ===================================================
        Bind samplers to texture units and load locations.
        https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#:~:text=The%20value%20you%20provide%20to%20a%20sampler%20uniform%20is%20the%20texture%20image%20unit%20to%20which%20you%20will%20bind%20the%20texture%20that%20the%20sampler%20will%20access
    ====================================================== */

    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureAtlas"), 1);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureArray"), 2);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureCube"), 3);

    this->modelMatrixUniformLocation    = glGetUniformLocation(this->shaderProgram, "modelMatrix");
    this->meshVariantLocation           = glGetUniformLocation(this->shaderProgram, "samplerType");
    this->discardFragsLocation          = glGetUniformLocation(this->shaderProgram, "discardFrags");

    this->maxTexWidth = 0;
    this->maxTexHeight = 0;

    this->textureStorage = textureType;
};

lazarus_result MeshManager::create3DAsset(MeshManager::Mesh &out, MeshManager::AssetConfig options)
{
    this->meshOut = {};
    this->meshData = {};
    std::vector<glm::vec3> diffuseColors;
    std::vector<FileLoader::Image> images;
    
    /* ======================================================
        Default to unit 2 (TextureLoader::StorageType::ARRAY)
        as this will be set appropriately when material 
        properties are assigned.
    ========================================================= */

    meshData.texture.unitId = GL_TEXTURE2;
    glActiveTexture(meshData.texture.unitId);

    meshOut.meshFilepath = options.meshPath;
    meshOut.materialFilepath = options.materialPath;
    
    //  TODO:
    //  If not present populate from file contents instead of filename
    this->meshOut.name = options.name.size()
    ? options.name
    : meshOut.meshFilepath + "_" + std::to_string(this->dataStore.size());

    /* ==========================================
        Determine whether the file is wavefront
        or gltf/glb.
    ============================================= */
    uint32_t suffixDelimiter = meshOut.meshFilepath.find_last_of(".");
    std::string suffix = meshOut.meshFilepath.substr(suffixDelimiter + 1);

    lazarus_result status = lazarus_result::LAZARUS_OK;
    if(suffix.compare("obj") == 0)
    {
        status = AssetLoader::parseWavefrontObj(
            meshData.attributes,
            meshData.indexes,
            diffuseColors,
            images,
            meshOut.meshFilepath.c_str(),
            meshOut.materialFilepath.c_str()
        );
        meshOut.type = MeshManager::MeshType::LOADED_WAVEFRONT;
    }
    else if(suffix.compare("glb") == 0)
    {
        status = AssetLoader::parseGlBinary(
            meshData.attributes, 
            meshData.indexes, 
            diffuseColors, 
            images,
            meshOut.meshFilepath.c_str()
        );    
        meshOut.type = MeshManager::MeshType::LOADED_GLB;
    };
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    status = this->setMaterialProperties(diffuseColors, images);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    this->setSharedProperties();

    status = this->initialiseMesh();
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    this->makeSelectable(options.selectable);

    out = this->meshOut;
    return lazarus_result::LAZARUS_OK;
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

lazarus_result MeshManager::createQuad(MeshManager::Mesh &out, MeshManager::QuadConfig options)
{
    if(options.width < 0.0f || options.height < 0.0f)
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_INVALID_DIMENSIONS;
    };

    std::vector<glm::vec3> diffuseColors = {};
    std::vector<FileLoader::Image> images = {};
    
    this->meshOut = {};
    this->meshData = {};

    meshOut.type = MeshManager::MeshType::PLANE;

    meshOut.meshFilepath = "";
    meshOut.materialFilepath = "";

    this->meshOut.name = options.name.size() 
    ? options.name 
    : this->meshOut.name.append(std::to_string(this->dataStore.size()));
    
    meshData.texture.unitId = this->textureStorage == TextureLoader::StorageType::ATLAS
    ? GL_TEXTURE1
    : GL_TEXTURE2;

    glActiveTexture(meshData.texture.unitId);

    lazarus_result status = lazarus_result::LAZARUS_OK;

    if(options.texturePath.size() > 0)
    {
        FileLoader::Image image = {};
        status = finder->loadImage(image, options.texturePath.c_str());
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };

        images.push_back(image);   
        diffuseColors.push_back(vec3(-0.1f, -0.1f, -0.1f));

        /* ============================================
            Increment the loaders texture array slice 
            count to stay in sync with future loads of
            other assets.
        =============================================== */

        AssetLoader::layerCount += 1;
    };

    float layer = static_cast<float>(AssetLoader::layerCount);

    /* ======================================================
        Ensure that the origin is centered.
        (E.g. width 2.0f, height 2.0f becomes 
        width -1.0f, height +1.0f) 
    ========================================================= */
    float xMin = -(options.width / 2.0f);
    float xMax = options.width / 2.0f;
    float yMax = options.height / 2.0f;
    float yMin = -(options.height / 2.0f);
    /* ==========================================================
        If the UV params aren't their default values (0.0) then
        this mesh is being created for a glyph which needs to be 
        looked up in the texture atlas.

        Otherwise it's a generic sprite.
    ============================================================= */
    if(options.uvXL || options.uvXR || options.uvYU > 0.0f)
    {
    /* ======================================================================================================
            Vertex positions,           Diffuse colors,             Normals,                    UVs 
    ========================================================================================================= */
        meshData.attributes = {                                                                                          
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(options.uvXL, options.uvYD, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(options.uvXR, options.uvYD, 0.0f), 
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(options.uvXL, options.uvYU, 0.0f),
            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(options.uvXR, options.uvYU, 0.0f),

            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(options.uvXR, options.uvYU, 0.0f),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(options.uvXR, options.uvYD, 0.0f),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(options.uvXL, options.uvYU, 0.0f),
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(options.uvXL, options.uvYD, 0.0f),
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
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 0.0f, layer),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 0.0f, layer),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(0.0f, 1.0f, layer),
            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(1.0f, 1.0f, layer),

            vec3(xMax, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 1.0f, layer),
            vec3(xMax, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(1.0f, 0.0f, layer),
            vec3(xMin, yMax, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 1.0f, layer),
            vec3(xMin, yMin, 0.0f), vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, -1.0f),     vec3(0.0f, 0.0f, layer),
        };
    };

    meshData.indexes = {
        0, 1, 2, 
        2, 1, 3, 
        4, 5, 6, 
        6, 5, 7
    };

    status = this->setMaterialProperties(diffuseColors, images);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    this->setSharedProperties();

    status = this->initialiseMesh();
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    this->makeSelectable(options.selectable);

    out = this->meshOut;
    return status;
}

lazarus_result MeshManager::createCube(MeshManager::Mesh &out, MeshManager::CubeConfig options)
{
    std::vector<glm::vec3> diffuseColors = {};
    std::vector<FileLoader::Image> images = {};

    float vertexPosition = options.scale / 2.0f; 

    this->meshOut = {};
    this->meshData = {};

    meshOut.type = MeshManager::MeshType::CUBE;

    meshOut.meshFilepath = "";
    meshOut.materialFilepath = "";

    this->meshOut.name = options.name.size() 
    ? options.name 
    : this->meshOut.name.append(std::to_string(this->dataStore.size()));
    
    meshData.texture.unitId = this->textureStorage == TextureLoader::StorageType::CUBEMAP
    ? GL_TEXTURE3
    : GL_TEXTURE2;

    float layer = 0.0;
    
    lazarus_result status = lazarus_result::LAZARUS_OK;

    if(this->textureStorage == TextureLoader::StorageType::CUBEMAP)
    {
        FileLoader::Image image = {};
        image.height = 0;
        image.width = 0;
        image.pixelData = NULL;

        images.push_back(image);   
    }
    else
    {
        if(options.texturePath.size() > 0)
        {
            FileLoader::Image image = {};
            status = finder->loadImage(image, options.texturePath.c_str());
            if(status != lazarus_result::LAZARUS_OK)
            {
                return status;
            };

            images.push_back(image);   

            /* ============================================
                Increment the loaders texture array slice 
                count to stay in sync with future loads of
                other assets.
            =============================================== */
            
            AssetLoader::layerCount += 1;
        };
        layer = static_cast<float>(AssetLoader::layerCount);
    };
    
    diffuseColors.push_back(vec3(-0.1f, -0.1f, -0.1f));

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
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 0.0f, layer),
        vec3(vertexPosition, vertexPosition,  vertexPosition),      vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 1.0f, layer),
        vec3(vertexPosition,  vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(1.0f, 0.0f, layer),
        vec3(-vertexPosition,  vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 1.0f, 0.0f),  vec3(0.0f, 1.0f, layer),

        // Back face
        vec3(-vertexPosition, vertexPosition,  -vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, layer),
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 1.0f, layer),
        vec3(-vertexPosition,  -vertexPosition, -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, layer),
        vec3(vertexPosition,  vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, layer),

        // Right face
        vec3(vertexPosition, vertexPosition, -vertexPosition),      vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 0.0f, layer),
        vec3(vertexPosition, -vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 1.0f, layer),
        vec3(vertexPosition,  -vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(1.0f, 0.0f, layer),
        vec3(vertexPosition,  vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(1.0f, 0.0f, 0.0f),  vec3(0.0f, 1.0f, layer),

        // Front face
        vec3(vertexPosition, vertexPosition, vertexPosition),       vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 0.0f, layer),
        vec3(-vertexPosition, -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 1.0f, layer),
        vec3(vertexPosition,  -vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(1.0f, 0.0f, layer),
        vec3(-vertexPosition,  vertexPosition,  vertexPosition),    vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, 0.0f, 1.0f),  vec3(0.0f, 1.0f, layer),

        // Left face
        vec3(-vertexPosition,  vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, layer),
        vec3(-vertexPosition,  -vertexPosition, -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, layer),
        vec3(-vertexPosition,  -vertexPosition,  vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, layer),
        vec3(-vertexPosition,  vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, layer),

        // Bottom face
        vec3(vertexPosition, -vertexPosition, -vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, layer),
        vec3(-vertexPosition, -vertexPosition, vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 1.0f, layer),
        vec3(-vertexPosition, -vertexPosition,  -vertexPosition),   vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 0.0f, layer),
        vec3(vertexPosition, -vertexPosition,  vertexPosition),     vec3(-0.1f, -0.1f, -0.1f),  vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, layer),
    };

    meshData.indexes = {
        0, 1, 2, 0, 3, 1,
        4, 5, 6, 4, 7, 5, 
        8, 9, 10, 8, 11, 9,
        12, 13, 14, 12, 15, 13,
        16, 17, 18, 16, 19, 17,
        20, 21, 22, 20, 23, 21
    };

    status = this->setMaterialProperties(diffuseColors, images);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    this->setSharedProperties();

    status = this->initialiseMesh();
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    this->makeSelectable(options.selectable);

    out = this->meshOut;
    return status;
};

lazarus_result MeshManager::initialiseMesh()
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

        lazarus_result status = this->checkErrors(__FILE__, __LINE__);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };

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

        if(this->textureStorage == TextureLoader::StorageType::ARRAY)
        {
            for(size_t i = 0; i < meshOut.materials.size(); i++)
            {
                if(meshOut.materials[i].type == MaterialType::IMAGE_TEXTURE)
                {
                    status = this->prepareTextures();
                    if(status != lazarus_result::LAZARUS_OK)
                    {
                        return status;
                    };
                };
            };
        };
    }
    else
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };
	
    return LAZARUS_OK;
};

lazarus_result MeshManager::prepareTextures()
{
    uint32_t width = 0;
    uint32_t height = 0;

    /* =========================================
        Determine the height and width of the 
        texture array and allocate the space.
    ============================================ */

    if(GlobalsManager::getEnforceImageSanity())
    {
        width = GlobalsManager::getMaxImageWidth();
        height = GlobalsManager::getMaxImageHeight();
    }
    else
    {
        width = this->maxTexWidth;
        height = this->maxTexHeight;
    }

    lazarus_result status = TextureLoader::extendTextureStack(width, height, AssetLoader::layerCount);
    if (status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    /* ===============================================
        Since memory size has been redefined, the
        previous contents now need to be rewritten to
        the new location.

        TODO:
        Is there some way to invoke a memcpy of the
        VRAM contents from here?
    ================================================== */

    uint32_t textureCount = 0;
    for(auto i: this->dataStore)
    {
        glActiveTexture(i.second.texture.unitId);

        if(this->textureStorage == TextureLoader::StorageType::ARRAY)
        {
            for(size_t j = 0; j < i.second.images.size(); j++)
            {
                if(i.second.images[j].pixelData != NULL)
                {
                    status = TextureLoader::loadImageToTextureStack(i.second.images[j], textureCount + j + 1);
                    if (status != lazarus_result::LAZARUS_OK)
                    {
                        return status;
                    };
                };
            };
            textureCount += i.second.images.size();
        };
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result MeshManager::clearMeshStorage()
{	
    this->clearErrors();
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
    
    AssetLoader::layerCount = 0;
    
    return this->checkErrors(__FILE__, __LINE__);
};

lazarus_result MeshManager::makeSelectable(bool selectable)
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
        GlobalsManager::setPickableEntity(meshOut.id);
        dataStore.at(meshOut.id).stencilBufferId = GlobalsManager::getNumberOfPickableEntities();
    }
    else
    {
        meshOut.isClickable = false;
        dataStore.at(meshOut.id).stencilBufferId = 0;
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result MeshManager::loadMesh(MeshManager::Mesh &meshIn)
{
    MeshManager::MeshData &data = dataStore.at(meshIn.id);
    lazarus_result status = lazarus_result::LAZARUS_OK;
    /* ===================================================
        Fill the stencil buffer with 0's. 
        Wherever an entity is occupying screenspace, fill 
        the buffercwith the mesh's selection.
    ====================================================== */
    if(GlobalsManager::getManageStencilBuffer())
    {
        this->clearErrors();

        glStencilMask(0xFF);
        glClearStencil(0x00);
        glStencilFunc(GL_ALWAYS, data.stencilBufferId, 0xFF);
        
        status = this->checkErrors(__FILE__, __LINE__);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
    };

    if(this->modelMatrixUniformLocation >= 0)
    {
        this->clearErrors();

        glUniformMatrix4fv(
            this->modelMatrixUniformLocation, 
            1, 
            GL_FALSE, 
            &meshIn.modelMatrix[0][0]
        );
        
        glUniform1i(this->meshVariantLocation, this->textureStorage);
        glUniform1i(this->discardFragsLocation, data.texture.discardAlphaZero);

        status = this->checkErrors(__FILE__, __LINE__);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
    }
    else
    {
        LOG_ERROR("Asset Error:", __FILE__, __LINE__);

        status = lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };

    return status;
};

lazarus_result MeshManager::drawMesh(MeshManager::Mesh &meshIn)
{
    MeshManager::MeshData &data = dataStore.at(meshIn.id);

    this->clearErrors();

    glBindVertexArray(data.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.EBO);

    glActiveTexture(data.texture.unitId);
    /* =========================================
        When a texture is present bind the id 
        of it's sampler to the texture target, 
        overwriting what was previously bound.

        Note the textureStoreVariant will be the 
        same for all materials.
    ============================================ */

    switch (this->textureStorage)
    {
        case TextureLoader::StorageType::ATLAS:
            glBindTexture(GL_TEXTURE_2D, data.texture.samplerId);
            break;
    
        case TextureLoader::StorageType::CUBEMAP:
            glBindTexture(GL_TEXTURE_CUBE_MAP, data.texture.samplerId);
            break;
        
        case TextureLoader::StorageType::ARRAY:
            glBindTexture(GL_TEXTURE_2D_ARRAY, data.texture.samplerId);
            break;
    
        default:
            break;
    };

    glDrawElements(GL_TRIANGLES, data.indexes.size(), GL_UNSIGNED_INT, nullptr);

    return this->checkErrors(__FILE__, __LINE__);
};

lazarus_result MeshManager::setDiscardFragments(MeshManager::Mesh &meshIn, bool shouldDiscard)
{
    MeshManager::MeshData &data = dataStore.at(meshIn.id);
    data.texture.discardAlphaZero = shouldDiscard;

    return lazarus_result::LAZARUS_OK;
};

lazarus_result MeshManager::setMaterialProperties(std::vector<glm::vec3> diffuse, std::vector<FileLoader::Image> images)
{
    if(diffuse.size() != images.size())
    {
        LOG_ERROR("Load Error: Invalid materials", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_ASSET_LOAD_ERROR;
    };

    for(size_t i = 0; i < diffuse.size(); i++)
    {
        Material material = {};
        material.diffuse = diffuse[i];
        material.texture = images[i];

        material.id = i;

        if((material.diffuse.r + material.diffuse.b + material.diffuse.g) < -0.1f)
        {
            material.type = MaterialType::IMAGE_TEXTURE;

            /* ===============================================
                The data struct must keep it's own copy of the 
                non-null images, used when time to upload / 
                re-upload.
            ================================================== */

            meshData.images.push_back(material.texture);
        }
        else
        {
            material.type = MaterialType::BASE_COLOR;
    
            meshData.texture.samplerId = 0;
            meshData.texture.unitId = GL_TEXTURE2;
        };

        meshOut.materials.push_back(material);

        /* =============================================================
            In the case that image sanitisation is not enabled, we must
            track which image in the textureStack is the largest.
        ================================================================ */

        this->maxTexWidth = std::max(maxTexWidth, material.texture.width);
        this->maxTexHeight = std::max(maxTexHeight, material.texture.height);
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result MeshManager::setSharedProperties()
{
    meshData.texture.discardAlphaZero = false;
    meshData.texture.samplerId = TextureLoader::textureId;

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

   return lazarus_result::LAZARUS_OK;
}

lazarus_result MeshManager::checkErrors(const char *file, uint32_t line)
{
    this->errorCode = glGetError();
    
    if(this->errorCode != GL_NO_ERROR)
    {
        std::string message = std::string("OpenGL Error: ").append(std::to_string(this->errorCode));
        LOG_ERROR(message.c_str(), file, line);

        return lazarus_result::LAZARUS_OPENGL_ERROR;
    }

    return lazarus_result::LAZARUS_OK;
};

lazarus_result MeshManager::clearErrors()
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

    return lazarus_result::LAZARUS_OK;
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