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

#include "../include/lazarus_world_fx.h"

WorldFX::WorldFX(GLuint shaderProgram) 
    : WorldFX::MeshManager(shaderProgram, TextureLoader::StorageType::CUBEMAP)
{
    LOG_DEBUG("Constructing Lazarus::WorldFX");

    this->error        = 0;
    this->shader        = shaderProgram;
    this->imageLoader   = nullptr;
    this->skyBoxOut     = {};
    this->fogOut        = {};

    this->fogColorUniformLocation       = glGetUniformLocation(this->shader, "fogColor");
    this->fogViewpointUniformLocation   = glGetUniformLocation(this->shader, "fogViewpoint");
    this->fogMaxDistUniformLocation     = glGetUniformLocation(this->shader, "fogMaxDist");
    this->fogMinDistUniformLocation     = glGetUniformLocation(this->shader, "fogMinDist");
    this->fogDensityUniformLocation     = glGetUniformLocation(this->shader, "fogDensity");
};

lazarus_result WorldFX::createSkyBox(WorldFX::Skybox &out, std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath)
{
    this->skyBoxOut = {};
    MeshManager::CubeConfig config = {};
    config.scale = 10.0f;

    lazarus_result status = this->createCube(this->skyBoxOut.cube, config);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };
    this->skyBoxOut.paths = {rightPath, leftPath, downPath, upPath, frontPath, backPath};

    status = this->loadSkyMap();
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    };

    out = this->skyBoxOut;
    return lazarus_result::LAZARUS_OK;
};

lazarus_result WorldFX::drawSkyBox(WorldFX::Skybox skyboxIn, CameraManager::Camera camera)
{
    /* ===========================================================
        For the illusion of infinite depth of the skybox  tp work, 
        the translation transform needs to be culled from the
        viewing matrix (the result of the glm::lookAt() operation 
        called by CameraManager::create*Cam(...)). 
        
        This is done by converting the 4x4 matrix to a 3x3 and 
        back again. This truncates the row from the matrix which
        describes the vertex position from the origin and replaces
        it with 0's (essentially back at the origin). It's then
        changed back again afterwards.
    ============================================================== */
    glDisable           (GL_CULL_FACE);

    glm::mat4 viewFromOrigin = glm::mat4(glm::mat3(camera.viewMatrix)); 
    GLuint uniform = glGetUniformLocation(this->shader, "viewMatrix");
    
    this->error = glGetError();
    if(this->error != 0)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_UNIFORM_NOT_FOUND;
    };

    glUniformMatrix4fv(uniform, 1, GL_FALSE, &viewFromOrigin[0][0]);
    this->error = glGetError();
    if(this->error != 0)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };

    glDepthMask(GL_FALSE);

    MeshManager::loadMesh(skyboxIn.cube);
    MeshManager::drawMesh(skyboxIn.cube);
    
    glDepthMask(GL_TRUE);

    if(GlobalsManager::getBackFaceCulling())
    {
        glEnable            (GL_CULL_FACE);
        glCullFace          (GL_BACK);
    };

    glUniformMatrix4fv(uniform, 1, GL_FALSE, &camera.viewMatrix[0][0]);
    this->error = glGetError();
    if(this->error != 0)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result WorldFX::createFog(WorldFX::Fog &out, float minDistance, float maxDistance, float thickness, glm::vec3 color, glm::vec3 position)
{
    this->fogOut = {};
    this->fogOut.color = color;
    this->fogOut.viewpoint = position;
    this->fogOut.density = thickness;
    this->fogOut.maxDistance = maxDistance;
    this->fogOut.minDistance = minDistance;

    out = this->fogOut;

    return lazarus_result::LAZARUS_OK;
};

lazarus_result WorldFX::loadFog(WorldFX::Fog fogIn, int32_t shader)
{
    if(fogIn.density < 0.0f)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_INVALID_INTENSITY;
    };
    
    if(shader == 0)
    {
        glUniform3fv(this->fogColorUniformLocation, 1, &fogIn.color[0]);
        glUniform3fv(this->fogViewpointUniformLocation, 1, &fogIn.viewpoint[0]);
        glUniform1f(this->fogMaxDistUniformLocation, fogIn.maxDistance);
        glUniform1f(this->fogMinDistUniformLocation, fogIn.minDistance);
        glUniform1f(this->fogDensityUniformLocation, fogIn.density);
    }
    else
    {
        GLuint fogColor       = glGetUniformLocation(shader, "fogColor");
        GLuint fogViewpoint   = glGetUniformLocation(shader, "fogViewpoint");
        GLuint fogMaxDist     = glGetUniformLocation(shader, "fogMaxDist");
        GLuint fogMinDist     = glGetUniformLocation(shader, "fogMinDist");
        GLuint fogDensity     = glGetUniformLocation(shader, "fogDensity");

        glUniform3fv(fogColor, 1, &fogIn.color[0]);
        glUniform3fv(fogViewpoint, 1, &fogIn.viewpoint[0]);
        glUniform1f(fogMaxDist, fogIn.maxDistance);
        glUniform1f(fogMinDist, fogIn.minDistance);
        glUniform1f(fogDensity, fogIn.density);
    };

    this->error = glGetError();
    if(this->error != 0)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_UNIFORM_NOT_FOUND;
    };

    return lazarus_result::LAZARUS_OK;
};

lazarus_result WorldFX::loadSkyMap()
{
    lazarus_result status = lazarus_result::LAZARUS_OK;

    this->imageLoader = std::make_unique<FileLoader>();

    for(auto path: this->skyBoxOut.paths)
    {
        std::string absolutePath = "";
        FileLoader::Image image = {};

        status = imageLoader->relativePathToAbsolute(path, absolutePath);
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
        
        status = imageLoader->loadImage(image, absolutePath.c_str());
        if(status != lazarus_result::LAZARUS_OK)
        {
            return status;
        };
        
        /* =======================================================
            Validate that the image inputs for the cubemap are 
            each of the same size. 

            (OpenGL actually already does this and will surface a 
            GL_INVALID_ENUM error if the check returns false). 
            Would be good to move this check to the loadCubeMap 
            func so that this execution state isn't overwritten 
            with 301 (LAZARUS_OPENGL_ERROR) by the textureLoader's 
            checkErrors subroutine.
        ========================================================== */
        if(
            this->skyBoxOut.cubeMap.size() > 0 && 
            (image.width != image.height || image.width != this->skyBoxOut.cubeMap[0].width)
        )
        {
            LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
            
            return lazarus_result::LAZARUS_INVALID_CUBEMAP;
        };
        
        this->skyBoxOut.cubeMap.push_back(image);
    };

    /* =============================================================
        Access the same texture ID values used by the skybox's mesh.
        Do so by using the MeshManager's TextureManager inherited 
        members to perform texture operations for this skybox.
    ================================================================ */
    
    status = MeshManager::TextureLoader::storeCubeMap(this->skyBoxOut.cubeMap[0].width, this->skyBoxOut.cubeMap[0].height);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    }
    else
    {
        return MeshManager::TextureLoader::loadCubeMap(this->skyBoxOut.cubeMap);
    }
};

WorldFX::~WorldFX()
{
    LOG_DEBUG("Destroying Lazarus::WorldFX");
};