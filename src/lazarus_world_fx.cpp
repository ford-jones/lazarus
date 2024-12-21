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
{
    std::cout << GREEN_TEXT << "Calling constructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
    this->shader = shaderProgram;

    this->meshLoader = std::make_unique<MeshManager>(this->shader);
    this->textureLoader = std::make_unique<TextureLoader>();
    this->imageLoader = nullptr;
    
    this->skyBox = {};
};

WorldFX::SkyBox WorldFX::createSkyBox(std::string rightPath, std::string leftPath, std::string downPath, std::string upPath, std::string frontPath, std::string backPath)
{
    this->skyBox.cube = meshLoader->createCube(10.0f);
    this->skyBox.paths = {rightPath, leftPath, downPath, upPath, frontPath, backPath};

    this->loadSkyMap();

    this->skyBox.cube.textureId = textureLoader->cubeMapTexture;
    this->skyBox.cube.textureFilepath = LAZARUS_MESH_ISSKYBOX;

    /* =================================================
        TODO:
            Do something about this.
    ==================================================== */
    this->skyBox.cube.textureLayer = 1;    
    this->skyBox.cube.textureData = {pixelData: NULL, height: 0, width: 0};

    return this->skyBox;
};

void WorldFX::drawSkyBox(WorldFX::SkyBox sky, CameraManager::Camera camera)
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

    glUniformMatrix4fv(uniform, 1, GL_FALSE, &viewFromOrigin[0][0]);

    glDepthMask(GL_FALSE);

    meshLoader->loadMesh(sky.cube);
    meshLoader->drawMesh(sky.cube);
    
    glDepthMask(GL_TRUE);

    if(globals.getBackFaceCulling())
    {
        glEnable            (GL_CULL_FACE);
        glCullFace          (GL_BACK);
    };

    glUniformMatrix4fv(uniform, 1, GL_FALSE, &camera.viewMatrix[0][0]);
};

void WorldFX::loadSkyMap()
{
    this->imageLoader = std::make_unique<FileReader>();

    for(auto path: this->skyBox.paths)
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
        if(this->skyBox.cubeMap.size() > 0 && (image.width != image.height || image.width != this->skyBox.cubeMap[0].width))
        {
            globals.setExecutionState(LAZARUS_INVALID_CUBEMAP);
        };
        
        this->skyBox.cubeMap.push_back(image);
    };

    textureLoader->storeCubeMap(this->skyBox.cubeMap[0].width, this->skyBox.cubeMap[0].height);
    textureLoader->loadCubeMap(this->skyBox.cubeMap);
};

WorldFX::~WorldFX()
{
    std::cout << GREEN_TEXT << "Calling destructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
};