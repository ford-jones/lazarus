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
    this->texLoader = std::make_unique<TextureLoader>();

	// meshLoader = nullptr;

    this->mesh = {};
    this->meshStore = {};
	
	this->errorCode = GL_NO_ERROR;

    this->layerCount = 0;

    //  TODO: 
    //  Remove locations from mesh struct
    //  List here so they are all garaunteed a value
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureAtlas"), 1);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureArray"), 2);
    glUniform1i(glGetUniformLocation(this->shaderProgram, "textureCube"), 3);

    this->modelMatrixUniformLocation = glGetUniformLocation(this->shaderProgram, "modelMatrix");
    this->is3DUniformLocation = glGetUniformLocation(this->shaderProgram, "spriteAsset");
    this->isGlyphUniformLocation = glGetUniformLocation(this->shaderProgram, "glyphAsset");
    this->isSkyBoxUniformLocation = glGetUniformLocation(this->shaderProgram, "isSkyBox");

    this->textureLayerUniformLocation = glGetUniformLocation(this->shaderProgram, "textureLayer");  
};

MeshManager::Mesh MeshManager::create3DAsset(string meshPath, string materialPath, string texturePath)
{
    this->mesh = {};

    // this->meshLoader = std::make_unique<MeshLoader>();

    mesh.is3D = 1;
    mesh.isGlyph = 0;
    mesh.isSkybox = 0;



    mesh.textureUnit = GL_TEXTURE2;
    glActiveTexture(mesh.textureUnit);

    this->resolveFilepaths(mesh, texturePath, materialPath, meshPath);
    
    this->parseWavefrontObj(
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
    if(width < 0.0f || height < 0.0f)
    {
        globals.setExecutionState(LAZARUS_INVALID_DIMENSIONS);
    };
    
    this->mesh = {};

    mesh.is3D = 0;
    mesh.isGlyph = 0;
    mesh.isSkybox = 0;


    
    mesh.textureUnit = GL_TEXTURE2;
    glActiveTexture(mesh.textureUnit);

    this->resolveFilepaths(mesh, texturePath);

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
        mesh.attributes = {                                                                                          
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
        mesh.attributes = {
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

    this->setInherentProperties(mesh);
    this->initialiseMesh(mesh);

    return mesh;
}

MeshManager::Mesh MeshManager::createCube(float scale, std::string texturePath)
{
    float vertexPosition = scale / 2; 

    this->mesh = {};
    mesh.isGlyph = 0;

    this->resolveFilepaths(mesh, texturePath);

        /* ==================================================
            Default texture unit is GL_TEXTURE1, which is the
            samplerArray. Reset it appropriately here.
        ===================================================== */
    if(mesh.textureFilepath == LAZARUS_SKYBOX_CUBE)
    {
        mesh.is3D = 0;
        mesh.isSkybox = 1;

        mesh.textureUnit = GL_TEXTURE3;
    }
    else
    {
        mesh.is3D = 1;
        mesh.isSkybox = 0;

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

    if(this->modelMatrixUniformLocation >= 0)
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

        this->checkErrors(__FILE__, __LINE__);

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
    if(this->modelMatrixUniformLocation >= 0)
    {
        glUniformMatrix4fv(this->modelMatrixUniformLocation, 1, GL_FALSE, &asset.modelMatrix[0][0]);
        glUniform1i(this->is3DUniformLocation, asset.is3D);
        glUniform1i(this->isGlyphUniformLocation, asset.isGlyph);
        glUniform1i(this->isSkyBoxUniformLocation, asset.isSkybox);
    
        if(asset.textureId != 0)
        {
            glUniform1f(this->textureLayerUniformLocation, (asset.textureLayer - 1));
        };

        this->checkErrors(__FILE__, __LINE__);
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

    this->checkErrors(__FILE__, __LINE__);

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
          
            asset.textureData.pixelData = NULL;
            asset.textureData.height = 0;
            asset.textureData.width = 0;
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
    for(auto i: meshStore)
    {
        glDeleteBuffers         (1, &i.VBO);
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

	// this->matLoader 				=	nullptr;
};

bool MeshLoader::parseWavefrontObj(vector<vec3> &outAttributes, vector<vec3> &outDiffuse, const char* meshPath, const char* materialPath) 
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