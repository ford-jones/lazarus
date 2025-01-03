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
	std::cout << GREEN_TEXT << "Calling constructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
	this->shaderProgram = shader;
	
	this->finder = std::make_unique<FileReader>();
    this->texLoader = std::make_unique<TextureLoader>();

	meshLoader = nullptr;

    this->mesh = {};
    this->meshStore = {};
	
	this->errorCode = GL_NO_ERROR;

    this->layerCount = 0;
};

MeshManager::Mesh MeshManager::create3DAsset(string meshPath, string materialPath, string texturePath)
{
    this->mesh = {};

    this->meshLoader = std::make_unique<MeshLoader>();

    mesh.is3D = 1;
    mesh.isGlyph = 0;
    mesh.isSkybox = 0;

    this->lookupUniforms(mesh);

    glUniform1i(mesh.samplerUniformLocation, 2);

    mesh.textureUnit = GL_TEXTURE2;
    glActiveTexture(mesh.textureUnit);

    this->resolveFilepaths(mesh, texturePath, materialPath, meshPath);
    
    meshLoader->parseWavefrontObj(
        mesh.attributes,
        mesh.diffuse,
        mesh.meshFilepath.c_str(),
        mesh.materialFilepath.c_str()
    );

    this->setInherentProperties(mesh);
    this->initialiseMesh(mesh);

    return mesh;
};

/* ========================================================================================
    Note: There is some strange behaviour with this function.
    If a single instance of *this* class is responsible for creating quads as well as 3D
    assets - a texturing artifact will present itself when the user *creates* the quad 
    prior to creating the 3D assets. (i.e. createQuad is called before create3DAsset).

    At the quads origin in worldspace, a "shadow" of the quad will render using texture 
    number 1 off of the xyzTextureStack. Not sure why.

    This is *very* similar to the behaviour seen on MacOS and is possibly related. It might
    just be that the bug doesn't present itself on linux.

    Worth mentioning that this doesn't seem to happen with glyphs - which are wrapped over
    a quad under the hood.
=========================================================================================== */

MeshManager::Mesh MeshManager::createQuad(float width, float height, string texturePath, float uvXL, float uvXR, float uvY)
{
    this->mesh = {};

    mesh.is3D = 0;
    mesh.isGlyph = 0;
    mesh.isSkybox = 0;

    this->lookupUniforms(mesh);

    glUniform1i(mesh.samplerUniformLocation, 2);
    
    mesh.textureUnit = GL_TEXTURE2;
    glActiveTexture(mesh.textureUnit);

    this->resolveFilepaths(mesh, texturePath);

    /* ==========================================================
        If the UV params aren't their default values (0.0) then
        this mesh is being created for a glyph which needs to be 
        looked up in the texture atlas.

        Otherwise it's a generic sprite.
    ============================================================= */

    if((uvXL || uvXR || uvY) > 0.0 )
    {
    /* ======================================================================================================
            Vertex positions,           Diffuse colors,             Normals,                    UVs 
    ========================================================================================================= */
        mesh.attributes = {                                                                                          
            vec3(0.0f, 0.0f, 0.0f),     vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, 0.0f, 0.0f),
            vec3(width, 0.0f, 0.0f),    vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, 0.0f, 0.0f), 
            vec3(0.0f, height, 0.0f),   vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, uvY, 0.0f),

            vec3(width, height, 0.0f),  vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, uvY, 0.0f),
            vec3(width, 0.0f, 0.0f),    vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, 0.0f, 0.0f),
            vec3(0.0f, height, 0.0f),   vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, uvY, 0.0f),

            vec3(0.0f, height, 0.0f),   vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, uvY, 0.0f),
            vec3(width, 0.0f, 0.0f),    vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, 0.0f, 0.0f),
            vec3(width, height, 0.0f),  vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, uvY, 0.0f),

            vec3(0.0f, height, 0.0f),   vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, uvY, 0.0f),
            vec3(width, 0.0f, 0.0f),    vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXR, 0.0f, 0.0f), 
            vec3(0.0f, 0.0f, 0.0f),     vec3(-0.1f, -0.1f, -0.1f),     vec3(0.0f, 0.0f, 1.0f),     vec3(uvXL, 0.0f, 0.0f),
        };
    }
    else
    {
        mesh.attributes = {
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
    };

    this->setInherentProperties(mesh);
    this->initialiseMesh(mesh);

    return mesh;
}

MeshManager::Mesh MeshManager::createCube(float scale, std::string texturePath)
{
    float vertexPosition = scale / 2; 

    this->mesh = {};
    mesh.isGlyph = 0;

    this->lookupUniforms(mesh);
    this->resolveFilepaths(mesh, texturePath);

    if(mesh.textureFilepath == LAZARUS_SKYBOX_CUBE)
    {
        mesh.is3D = 0;
        mesh.isSkybox = 1;

        /* ==================================================
            Change the sampler location set by lookupUniforms
            from the texture array to the cube map.
        ===================================================== */
        mesh.samplerUniformLocation = glGetUniformLocation(this->shaderProgram, "textureCube");
        glUniform1i(mesh.samplerUniformLocation, 3);

        mesh.textureUnit = GL_TEXTURE3;
    }
    else
    {
        mesh.is3D = 1;
        mesh.isSkybox = 0;

        mesh.samplerUniformLocation = glGetUniformLocation(this->shaderProgram, "textureArray");
        glUniform1i(mesh.samplerUniformLocation, 2);

        mesh.textureUnit = GL_TEXTURE2;
    };

    glActiveTexture(mesh.textureUnit);

    mesh.attributes = {                                                                                          
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

    this->setInherentProperties(mesh);
    this->initialiseMesh(mesh);

    return this->mesh;
};

void MeshManager::initialiseMesh(MeshManager::Mesh &asset)
{	
    glGenVertexArrays(1, &asset.VAO);
   	glBindVertexArray(asset.VAO);

    if(asset.modelMatrixUniformLocation >= 0)
    {
        glGenBuffers(1, &asset.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, asset.VBO);

        glBufferData(GL_ARRAY_BUFFER, asset.attributes.size() * sizeof(vec3), &asset.attributes[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(1 * sizeof(vec3)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(2 * sizeof(vec3)));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, (4 * sizeof(vec3)), (void*)(3 * sizeof(vec3)));
        glEnableVertexAttribArray(3);

        this->checkErrors(__PRETTY_FUNCTION__);

        this->meshStore.push_back(asset);

        if(asset.isSkybox != 1)
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
    texLoader->extendTextureStack(globals.getMaxImageWidth(), globals.getMaxImageHeight(), this->layerCount);

    for(auto i: meshStore)
    {
        glActiveTexture(i.textureUnit);
    
        if((i.textureFilepath != LAZARUS_DIFFUSE_MESH))
        {
            texLoader->loadImageToTextureStack(i.textureData, i.textureLayer);
        };
    };

    return;
};

void MeshManager::loadMesh(MeshManager::Mesh &asset)
{
    if(asset.modelMatrixUniformLocation >= 0)
    {
        glUniformMatrix4fv(asset.modelMatrixUniformLocation, 1, GL_FALSE, &asset.modelMatrix[0][0]);
        glUniform1i(asset.is3DUniformLocation, asset.is3D);
        glUniform1i(asset.isGlyphUniformLocation, asset.isGlyph);
        glUniform1i(asset.isSkyBoxUniformLocation, asset.isSkybox);
    
        if(asset.textureId != 0)
        {
            glUniform1f(asset.textureLayerUniformLocation, (asset.textureLayer - 1));
        };
    
        this->checkErrors(__PRETTY_FUNCTION__);
    }
    else
    {
        globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);
    };

    return;
};

void MeshManager::drawMesh(MeshManager::Mesh &asset)
{
    glBindVertexArray(asset.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, asset.VBO);

    glActiveTexture(asset.textureUnit);

    if((asset.textureFilepath == LAZARUS_GLYPH_QUAD))
    {
        glBindTexture(GL_TEXTURE_2D, asset.textureId);
    }
    else if((asset.textureFilepath == LAZARUS_SKYBOX_CUBE))
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, asset.textureId);
    }
    else if((asset.textureFilepath != LAZARUS_DIFFUSE_MESH))
    {
        glBindTexture(GL_TEXTURE_2D_ARRAY, asset.textureId);
    };

    glDrawArrays(GL_TRIANGLES, 0, asset.attributes.size());

    this->checkErrors(__PRETTY_FUNCTION__);

    return;
};

void MeshManager::resolveFilepaths(MeshManager::Mesh &asset, string texPath, string mtlPath, string objPath)
{
    objPath != LAZARUS_PRIMITIVE_MESH
    ? asset.meshFilepath =  finder->relativePathToAbsolute(objPath)
    : asset.meshFilepath = LAZARUS_PRIMITIVE_MESH;

    mtlPath != LAZARUS_TEXTURED_MESH
    ? asset.materialFilepath =  finder->relativePathToAbsolute(mtlPath)
    : asset.materialFilepath = LAZARUS_TEXTURED_MESH;

    switch (texPath[0])
    {
        //  Glyph atlas
        case 'G':
            asset.textureFilepath = LAZARUS_GLYPH_QUAD;
            break;
        
        //  Skybox cubemap
        case 'S':
            asset.textureFilepath = LAZARUS_SKYBOX_CUBE;
            break;

        //  Diffuse color
        case 'D':
            asset.textureLayer = 0;
            asset.textureId = 0;
    	    asset.textureFilepath = LAZARUS_DIFFUSE_MESH;
            asset.textureData = {pixelData: NULL, height: 0, width: 0};
            break;
        
        //  Image array
        default:
            this->layerCount += 1;

	        asset.textureFilepath = finder->relativePathToAbsolute(texPath);
            asset.textureData = finder->readFromImage(asset.textureFilepath);
            
            asset.textureLayer = this->layerCount;
            asset.textureId = texLoader->textureStack;
            break;
    };

    return;
};

void MeshManager::setInherentProperties(MeshManager::Mesh &asset)
{
    asset.locationX = 0;
    asset.locationY = 0;
    asset.locationZ = 0;

    asset.modelMatrix = mat4(1.0f);

    asset.numOfVertices = asset.attributes.size() / 4;
    asset.numOfFaces = (asset.numOfVertices) / 3;

   return;
}

void MeshManager::lookupUniforms(MeshManager::Mesh &asset)
{
    asset.modelMatrixUniformLocation = glGetUniformLocation(this->shaderProgram, "modelMatrix");
    asset.is3DUniformLocation = glGetUniformLocation(this->shaderProgram, "spriteAsset");
    asset.isGlyphUniformLocation = glGetUniformLocation(this->shaderProgram, "glyphAsset");
    asset.isSkyBoxUniformLocation = glGetUniformLocation(this->shaderProgram, "isSkyBox");

    asset.samplerUniformLocation = glGetUniformLocation(this->shaderProgram, "textureArray");
    asset.textureLayerUniformLocation = glGetUniformLocation(this->shaderProgram, "textureLayer");    

    return;
};

void MeshManager::checkErrors(const char *invoker)
{
    this->errorCode = glGetError();
    
    if(this->errorCode != 0)
    {
        std::cerr << RED_TEXT << "ERROR::GL_ERROR::CODE " << RESET_TEXT << this->errorCode << std::endl;
        std::cerr << RED_TEXT << "INVOKED BY: " << RESET_TEXT << invoker << std::endl;

        globals.setExecutionState(LAZARUS_OPENGL_ERROR);
    }

    return;
};

MeshManager::~MeshManager()
{
    for(auto i: meshStore)
    {
        glDeleteBuffers         (1, &i.VBO);
        glDeleteVertexArrays    (1, &i.VAO);
    };

    this->checkErrors(__PRETTY_FUNCTION__);

    std::cout << GREEN_TEXT << "Calling destructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
};