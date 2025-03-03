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

WorldFX::WorldFX(GLuint shaderProgram) : WorldFX::MeshManager(shaderProgram)
{
    std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
    this->status = 0;
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

/* =================================================================
    TODO (priority):
    For some reason when a skybox is NOT created, everything breaks
    and openGL spits out a 1282 Error.
==================================================================== */
WorldFX::SkyBox WorldFX::createSkyBox(std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath)
{
    this->skyBoxOut = {};
    this->skyBoxOut.cube = this->createCube(10.0f);
    this->skyBoxOut.paths = {rightPath, leftPath, downPath, upPath, frontPath, backPath};

    this->loadSkyMap();

    return this->skyBoxOut;
};

void WorldFX::drawSkyBox(WorldFX::SkyBox skyboxIn, CameraManager::Camera camera)
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
    this->status = glGetError();
    if(this->status != 0) globals.setExecutionState(LAZARUS_UNIFORM_NOT_FOUND);

    glUniformMatrix4fv(uniform, 1, GL_FALSE, &viewFromOrigin[0][0]);
    this->status = glGetError();
    if(this->status != 0) globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);

    glDepthMask(GL_FALSE);

    this->loadMesh(skyboxIn.cube);
    this->drawMesh(skyboxIn.cube);
    
    glDepthMask(GL_TRUE);

    if(globals.getBackFaceCulling())
    {
        glEnable            (GL_CULL_FACE);
        glCullFace          (GL_BACK);
    };

    glUniformMatrix4fv(uniform, 1, GL_FALSE, &camera.viewMatrix[0][0]);
    this->status = glGetError();
    if(this->status != 0) globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);
};

WorldFX::Fog WorldFX::createFog(float minDistance, float maxDistance, float thickness, float r, float g, float b, float x, float y, float z)
{
    this->fogOut = {};
    this->fogOut.color = glm::vec3(r, g, b);
    this->fogOut.viewpoint = glm::vec3(x, y, z);
    this->fogOut.density = thickness;
    this->fogOut.maxDistance = maxDistance;
    this->fogOut.minDistance = minDistance;

    return this->fogOut;
};

void WorldFX::loadFog(WorldFX::Fog fogIn)
{
    if(fogIn.density < 0.0) globals.setExecutionState(LAZARUS_INVALID_INTENSITY);
    
    glUniform3fv(this->fogColorUniformLocation, 1, &fogIn.color[0]);
    glUniform3fv(this->fogViewpointUniformLocation, 1, &fogIn.viewpoint[0]);
    glUniform1f(this->fogMaxDistUniformLocation, fogIn.maxDistance);
    glUniform1f(this->fogMinDistUniformLocation, fogIn.minDistance);
    glUniform1f(this->fogDensityUniformLocation, fogIn.density);

    this->status = glGetError();
    if(this->status != 0) globals.setExecutionState(LAZARUS_UNIFORM_NOT_FOUND);
};

void WorldFX::loadSkyMap()
{
    this->imageLoader = std::make_unique<FileReader>();

    for(auto path: this->skyBoxOut.paths)
    {
        std::string absolute = imageLoader->relativePathToAbsolute(path);
        FileReader::Image image = imageLoader->readFromImage(absolute);
        
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
        if(this->skyBoxOut.cubeMap.size() > 0 && (image.width != image.height || image.width != this->skyBoxOut.cubeMap[0].width))
        {
            globals.setExecutionState(LAZARUS_INVALID_CUBEMAP);
        };
        
        this->skyBoxOut.cubeMap.push_back(image);
    };

    /* =============================================================
        Access the same texture ID values used by the skybox's mesh.
        Do so by using the MeshManager's TextureManager inherited 
        members to perform texture operations for this skybox.
    ================================================================ */
    this->storeCubeMap(this->skyBoxOut.cubeMap[0].width, this->skyBoxOut.cubeMap[0].height);
    this->loadCubeMap(this->skyBoxOut.cubeMap);
};

WorldFX::~WorldFX()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
};