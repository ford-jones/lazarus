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
	std::cout << GREEN_TEXT << "Calling constructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
	this->shaderProgram = shader;
	
	this->finder = std::make_unique<FileReader>();
    this->texLoader = std::make_unique<TextureLoader>();

	meshLoader = nullptr;
    this->mesh = {};
    
    this->xyTextureId = 0;
    this->xyzTextureId = 0;
	
	this->errorCode = GL_NO_ERROR;
};

Mesh::TriangulatedMesh Mesh::create3DAsset(string meshPath, string materialPath, string texturePath)
{
    this->mesh = {};
    
    this->meshLoader = std::make_unique<MeshLoader>();

    mesh.is3D = 1;
    mesh.isGlyph = 0;

    this->lookupUniforms(mesh);

    glUniform1i(mesh.samplerUniformLocation, 2);
    
    glActiveTexture(GL_TEXTURE2);
    this->resolveFilepaths(mesh, texturePath, materialPath, meshPath);
    
    meshLoader->parseWavefrontObj(
        mesh.attributes,
        mesh.diffuse,
        mesh.textureId,
        mesh.textureData,
        mesh.meshFilepath.c_str(),
        mesh.materialFilepath.c_str(),
        mesh.textureFilepath.c_str()
    );

    this->setInherentProperties(mesh);

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

Mesh::TriangulatedMesh Mesh::createQuad(float width, float height, string texturePath, float uvXL, float uvXR, float uvY)
{
    this->mesh = {};

    mesh.is3D = 0;
    mesh.isGlyph = 0;

    this->lookupUniforms(mesh);

    glUniform1i(mesh.samplerUniformLocation, 3);
    
    glActiveTexture(GL_TEXTURE3);
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

    return mesh;
}

void Mesh::initialiseMesh(Mesh::TriangulatedMesh &asset)
{	
    if(asset.modelviewUniformLocation >= 0)
    {
        glGenVertexArrays(1, &this->VAO);
    	glBindVertexArray(this->VAO);

        if((asset.textureFilepath == LAZARUS_MESH_ISTEXT))
        {
            glActiveTexture(GL_TEXTURE1);
        }
        else if((asset.textureFilepath != LAZARUS_MESH_NOTEX))
        {
            if(asset.is3D == 1)
            {
                glActiveTexture(GL_TEXTURE2);
            }
            else
            {
                glActiveTexture(GL_TEXTURE3);
            }

            texLoader->loadImageToTextureStack(asset.textureData, asset.textureId);
        }

        glGenBuffers(1, &this->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

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
    }
    else
    {
        globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);
    };
	
    return;
};

void Mesh::loadMesh(Mesh::TriangulatedMesh &asset)
{
    if(asset.modelviewUniformLocation >= 0)
    {
        glUniformMatrix4fv(asset.modelviewUniformLocation, 1, GL_FALSE, &asset.modelviewMatrix[0][0]);
        glUniform1i(asset.is3DUniformLocation, asset.is3D);
        glUniform1i(asset.isGlyphUniformLocation, asset.isGlyph);
    
        if(asset.textureId != 0)
        {
            glUniform1f(asset.textureLayerUniformLocation, (asset.textureId - 1));
        }
    
        this->checkErrors(__PRETTY_FUNCTION__);
    }
    else
    {
        globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);
    };

    return;
};

void Mesh::drawMesh(Mesh::TriangulatedMesh &asset)
{
    glDrawArrays(GL_TRIANGLES, 0, asset.attributes.size());

    this->checkErrors(__PRETTY_FUNCTION__);

    this->releaseMesh();

    return;
};

void Mesh::resolveFilepaths(Mesh::TriangulatedMesh &asset, string texPath, string mtlPath, string objPath)
{
    if(objPath != "") 
    {
        asset.meshFilepath =  finder->relativePathToAbsolute(objPath);
    } 
    else
    {
        asset.meshFilepath = LAZARUS_MESH_NOOBJ;
    };

    if(mtlPath != "") 
    {
        asset.materialFilepath = finder->relativePathToAbsolute(mtlPath);
    }
    else
    {
        asset.materialFilepath = LAZARUS_MESH_NOMTL;
    }
    
    if(texPath == LAZARUS_MESH_ISTEXT)
    {
        asset.textureFilepath = LAZARUS_MESH_ISTEXT;
    }
    else if(texPath != "")
    {
	    asset.textureFilepath = finder->relativePathToAbsolute(texPath);
        asset.textureData = finder->readFromImage(asset.textureFilepath);
        texLoader->extendTextureStack(asset.textureData.width, asset.textureData.height, asset.textureId);
    }
    else
    {
        /* ========================================
            Layers of the sampler array are aren't 
            zero-indexed. Texture id's of 0 are 
            another indicator that no texture is in
            use.
        =========================================== */
        asset.textureId = 0;
    	asset.textureFilepath = LAZARUS_MESH_NOTEX;
        asset.textureData = {pixelData: NULL, height: 0, width: 0};
    };

    return;
};

void Mesh::setInherentProperties(Mesh::TriangulatedMesh &asset)
{
    asset.locationX = 0;
    asset.locationY = 0;
    asset.locationZ = 0;

    asset.modelviewMatrix = mat4(1.0f);

    asset.numOfVertices = asset.attributes.size() / 4;
    asset.numOfFaces = (asset.numOfVertices) / 3;

   return;
}

void Mesh::lookupUniforms(Mesh::TriangulatedMesh &asset)
{
    asset.modelviewUniformLocation = glGetUniformLocation(this->shaderProgram, "modelMatrix");
    asset.is3DUniformLocation = glGetUniformLocation(this->shaderProgram, "spriteAsset");
    asset.isGlyphUniformLocation = glGetUniformLocation(this->shaderProgram, "glyphAsset");

    if(asset.is3D == 1)
    {
        asset.samplerUniformLocation = glGetUniformLocation(this->shaderProgram, "xyzAssetTextures");
        asset.textureLayerUniformLocation = glGetUniformLocation(this->shaderProgram, "xyzTexLayerIndex");    
    }
    else 
    {
        asset.samplerUniformLocation = glGetUniformLocation(this->shaderProgram, "xyAssetTextures");
        asset.textureLayerUniformLocation = glGetUniformLocation(this->shaderProgram, "xyTexLayerIndex");
    }

    return;
};

void Mesh::checkErrors(const char *invoker)
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

void Mesh::releaseMesh()
{
    glDeleteVertexArrays    (1, &this->VAO);
    glDeleteBuffers         (1, &this->VBO);

    this->checkErrors(__PRETTY_FUNCTION__);

    return;
};

Mesh::~Mesh()
{
    this->releaseMesh();
    std::cout << GREEN_TEXT << "Calling destructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
};