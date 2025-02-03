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

#include "../include/lazarus_camera.h"

CameraManager::CameraManager(GLuint shader)
{
    std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
    this->shader = shader;

    this->monitorX = globals.getDisplayWidth();
    this->monitorY = globals.getDisplayHeight();
};

CameraManager::Camera CameraManager::createPerspectiveCam(int arX, int arY)
{
    srand(time((0)));
    camera.id                   = 1 + (rand() % 2147483647);

    /* ===============================================
        If a target aspect ratio has been defined then
        use that. Otherwise use the dimensions 
        returned from the machine's primary monitor.
    ================================================== */
    if((arX + arY) > 0)
    {
        camera.aspectRatio      = static_cast<float>(arX) / static_cast<float>(arY);
    }
    else
    {
        camera.aspectRatio      = static_cast<float>(monitorX) / static_cast<float>(monitorY);
    };

    /* ===============================================
        The direction of the back of the camera, so
        the camera is actually looking down +X
    ================================================== */
    glm::vec3 inverseTarget = glm::vec3(-1.0f, 0.0f, 0.0f);

    camera.position             = vec3(0.0f, 0.0f, 0.0f);
    camera.direction            = glm::normalize(camera.position - inverseTarget);
    camera.upVector             = vec3(0.0f, 1.0f, 0.0f);
    
    camera.viewMatrix           = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
    camera.projectionMatrix     = glm::perspective(glm::radians(45.0f), camera.aspectRatio, 0.1f, 100.0f);

    camera.viewLocation         = glGetUniformLocation(shader, "viewMatrix");
    camera.projectionLocation   = glGetUniformLocation(shader, "perspectiveProjectionMatrix");

    camera.usesPerspective      = 1;

    return camera;
};

CameraManager::Camera CameraManager::createOrthoCam(int arX, int arY)
{
    srand(time((0)));
    camera.id                   = 1 + (rand() % 2147483647);

    if((arX + arY) > 0)
    {
        camera.aspectRatio      = static_cast<float>(arX) / static_cast<float>(arY);
    }
    else
    {
        camera.aspectRatio      = static_cast<float>(monitorX) / static_cast<float>(monitorY);
    };

    /* ================================================
        Negative Z so as to be "back" from the viewing
        plane.
    =================================================== */
    camera.position             = vec3(0.0f, 0.0f, -1.0f);
    camera.direction            = glm::normalize(camera.position - vec3(0.0f, 0.0f, 0.0f));
    camera.upVector             = vec3(0.0f, 1.0f, 0.0f);
    
    camera.viewMatrix           = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
    camera.projectionMatrix     = glm::ortho(0.0f, static_cast<float>(arX), 0.0f, static_cast<float>(arY));

    camera.viewLocation         = glGetUniformLocation(shader, "viewMatrix");
    camera.projectionLocation   = glGetUniformLocation(shader, "orthoProjectionMatrix");

    camera.usesPerspective      = 0;

    return camera;
};

void CameraManager::loadCamera(CameraManager::Camera &cameraData)
{
    if(cameraData.projectionLocation >= 0)
    {
        glUniformMatrix4fv     (cameraData.viewLocation, 1, GL_FALSE, &cameraData.viewMatrix[0][0]);
        glUniformMatrix4fv     (cameraData.projectionLocation, 1, GL_FALSE, &cameraData.projectionMatrix[0][0]);

        glUniform1i(glGetUniformLocation(this->shader, "usesPerspective"), cameraData.usesPerspective);
    }
    else
    {
        globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);
    };
};

CameraManager::~CameraManager()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
}