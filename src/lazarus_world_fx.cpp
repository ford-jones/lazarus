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

WorldFX::WorldFX(Shader &shader) 
    : WorldFX::ModelManager(shader, TextureLoader::StorageType::CUBEMAP)
{
    LOG_DEBUG("Constructing Lazarus::WorldFX");

    this->error        = 0;
    this->shader = &shader;
    this->updateUniformLocations();
    this->activeShaderID = shader.activeProgram;

    this->imageLoader   = nullptr;
    this->skyBoxOut     = {};
    this->fogOut        = {};
};

lazarus_result WorldFX::createSkyBox(WorldFX::Skybox &out, std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath)
{
    this->skyBoxOut = {};
    ModelManager::CubeConfig config = {};
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
    /*
        For the illusion of infinite depth of the skybox  tp work, 
        the translation transform needs to be culled from the
        viewing matrix (the result of the glm::lookAt() operation 
        called by CameraManager::create*Cam(...)). 
        
        This is done by converting the 4x4 matrix to a 3x3 and 
        back again. This truncates the row from the matrix which
        describes the vertex position from the origin and replaces
        it with 0's (essentially back at the origin). It's then
        changed back again afterwards.
    */
    glDisable           (GL_CULL_FACE);

    glm::mat4 viewFromOrigin = glm::mat4(glm::mat3(camera.viewMatrix)); 
    
    this->error = glGetError();
    if(this->error != 0)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_UNIFORM_NOT_FOUND;
    };

    glUniformMatrix4fv(this->viewMatrixLocation, 1, GL_FALSE, &viewFromOrigin[0][0]);
    this->error = glGetError();
    if(this->error != 0)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };

    glDepthMask(GL_FALSE);

    ModelManager::loadModel(skyboxIn.cube);
    ModelManager::drawModel(skyboxIn.cube);
    
    glDepthMask(GL_TRUE);

    if(GlobalsManager::getBackFaceCulling())
    {
        glEnable            (GL_CULL_FACE);
        glCullFace          (GL_BACK);
    };

    glUniformMatrix4fv(this->viewMatrixLocation, 1, GL_FALSE, &camera.viewMatrix[0][0]);
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

lazarus_result WorldFX::loadFog(WorldFX::Fog fogIn)
{
    if(fogIn.density < 0.0f)
    {
        LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);

        return lazarus_result::LAZARUS_INVALID_INTENSITY;
    };
    
    /**
     * FIXME:
     * This shouldn't take a param anymore, instead just check the member value
     * i.e. this->shader->activeProgram
     * Same goes for every other load function in this god forsaken place (mesh, cam, light, blahblah)
     */
    if(this->activeShaderID != shader->activeProgram)
    {
        this->updateUniformLocations();
        this->activeShaderID = shader->activeProgram;
    };
    
    glUniform3fv(this->fogColorUniformLocation, 1, &fogIn.color[0]);
    glUniform3fv(this->fogViewpointUniformLocation, 1, &fogIn.viewpoint[0]);
    glUniform1f(this->fogMaxDistUniformLocation, fogIn.maxDistance);
    glUniform1f(this->fogMinDistUniformLocation, fogIn.minDistance);
    glUniform1f(this->fogDensityUniformLocation, fogIn.density);

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
        
        /*
            Validate that the image inputs for the cubemap are 
            each of the same size. 

            (OpenGL actually already does this and will surface a 
            GL_INVALID_ENUM error if the check returns false). 
            Would be good to move this check to the loadCubeMap 
            func so that this execution state isn't overwritten 
            with 301 (LAZARUS_OPENGL_ERROR) by the textureLoader's 
            checkErrors subroutine.
        */
        if(
            !std::empty(this->skyBoxOut.cubeMap) && 
            (image.width != image.height || image.width != this->skyBoxOut.cubeMap[0].width)
        )
        {
            LOG_ERROR("OpenGL Error:", __FILE__, __LINE__);
            
            return lazarus_result::LAZARUS_INVALID_CUBEMAP;
        };
        
        this->skyBoxOut.cubeMap.push_back(image);
    };

    /*
        Access the same texture ID values used by the skybox's mesh.
        Do so by using the ModelManager's TextureManager inherited 
        members to perform texture operations for this skybox.
    */
    
    status = ModelManager::TextureLoader::storeCubeMap(this->skyBoxOut.cubeMap[0].width, this->skyBoxOut.cubeMap[0].height);
    if(status != lazarus_result::LAZARUS_OK)
    {
        return status;
    }
    else
    {
        return ModelManager::TextureLoader::loadCubeMap(this->skyBoxOut.cubeMap);
    }
};

lazarus_result WorldFX::updateUniformLocations()
{
    /**
     * TODO:
     * Check errors
     */
    this->viewMatrixLocation            = glGetUniformLocation(shader->activeProgram, "viewMatrix");
    this->fogColorUniformLocation       = glGetUniformLocation(shader->activeProgram, "fogColor");
    this->fogViewpointUniformLocation   = glGetUniformLocation(shader->activeProgram, "fogViewpoint");
    this->fogMaxDistUniformLocation     = glGetUniformLocation(shader->activeProgram, "fogMaxDist");
    this->fogMinDistUniformLocation     = glGetUniformLocation(shader->activeProgram, "fogMinDist");
    this->fogDensityUniformLocation     = glGetUniformLocation(shader->activeProgram, "fogDensity");

    return lazarus_result::LAZARUS_OK;
};

WorldFX::~WorldFX()
{
    LOG_DEBUG("Destroying Lazarus::WorldFX");
};