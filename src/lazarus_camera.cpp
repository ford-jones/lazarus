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
    this->camera = {};
    this->shader                            = shader;

    this->pixelHeight                       = globals.getDisplayWidth();
    this->pixelWidth                        = globals.getDisplayHeight();

    this->viewLocation                      = glGetUniformLocation(shader, "viewMatrix");
    this->perspectiveProjectionLocation     = glGetUniformLocation(shader, "perspectiveProjectionMatrix");
    this->orthographicProjectionLocation    = glGetUniformLocation(shader, "orthoProjectionMatrix");

    this->pixel                             = 0;
    this->errorCode                         = 0;
};

CameraManager::Camera CameraManager::createPerspectiveCam(uint32_t aspectRatioX, uint32_t aspectRatioY)
{
    srand(time((0)));
    
    this->camera = {};
    
    camera.id                   = 1 + (rand() % 2147483647);

    this->setAspectRatio(aspectRatioX, aspectRatioY);

    /* ===============================================
        The direction of the back of the camera, so
        the camera is actually looking down +X
    ================================================== */
    glm::vec3 inverseTarget     = glm::vec3(-1.0f, 0.0f, 0.0f);

    camera.position             = vec3(0.0f, 0.0f, 0.0f);
    camera.direction            = glm::normalize(camera.position - inverseTarget);
    camera.upVector             = vec3(0.0f, 1.0f, 0.0f);
    
    camera.viewMatrix           = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
    /* ================================
        45° = 0.785398 radians
    =================================== */ 
    camera.projectionMatrix     = glm::perspective(0.785398f, camera.aspectRatio, 0.1f, 100.0f);

    camera.usesPerspective      = 1;

    return camera;
};

CameraManager::Camera CameraManager::createOrthoCam(uint32_t aspectRatioX, uint32_t aspectRatioY)
{
    srand(time((0)));

    this->camera = {};

    camera.id                   = 1 + (rand() % 2147483647);

    this->setAspectRatio(aspectRatioX, aspectRatioY);

    /* ================================================
        Negative Z so as to be "back" from the viewing
        plane.
    =================================================== */
    camera.position             = vec3(0.0f, 0.0f, -1.0f);
    camera.direction            = glm::normalize(camera.position);
    camera.upVector             = vec3(0.0f, 1.0f, 0.0f);
    
    camera.viewMatrix           = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
    camera.projectionMatrix     = glm::ortho(0.0f, static_cast<_Float32>(aspectRatioX), 0.0f, static_cast<_Float32>(aspectRatioY));

    camera.usesPerspective      = 0;

    return camera;
};

void CameraManager::loadCamera(CameraManager::Camera &cameraIn)
{
    if(
        this->perspectiveProjectionLocation >= 0 && 
        this->orthographicProjectionLocation >= 0
    )
    {
        glUniformMatrix4fv     (this->viewLocation, 1, GL_FALSE, &cameraIn.viewMatrix[0][0]);

        cameraIn.usesPerspective == 1
        ? glUniformMatrix4fv     (this->perspectiveProjectionLocation, 1, GL_FALSE, &cameraIn.projectionMatrix[0][0])
        : glUniformMatrix4fv     (this->orthographicProjectionLocation, 1, GL_FALSE, &cameraIn.projectionMatrix[0][0]);

        glUniform1i(glGetUniformLocation(this->shader, "usesPerspective"), cameraIn.usesPerspective);

        this->checkErrors(__FILE__, __LINE__);
    }
    else
    {
        globals.setExecutionState(LAZARUS_MATRIX_LOCATION_ERROR);
    };

    return;
};

/* ==============================================
    TODO:
    Create a function for ray-picking positions
    in worldspace out from the camera.
================================================= */

int8_t CameraManager::getPixelOccupant(uint32_t positionX, uint32_t positionY)
{
    this->pixel = 0;
    if(globals.getManageStencilBuffer())
    {
        if(positionX < pixelWidth && positionY < pixelHeight) {
            /* ============================================
                Perform an inversion of the window's y-axis
                to accomidate for FBO's measure of 
                pixel's from the top-left corner of the
                screen as opposed to bottom-left.
            =============================================== */
            uint32_t inverseY = pixelWidth - positionY;
        
            /* =================================================
                Read entity ID's if present from the back-buffer's 
                stencil-depth buffer. If none are present at the
                pixel site the result will be 0.
    
                Note: Minus positions by 1 because the pixels 
                are zero-indexed inside the framebuffer.
            ==================================================== */
            glReadBuffer(GL_BACK);
            glReadPixels(
                (positionX - 1), 
                (inverseY - 1), 
                1, 1, 
                GL_STENCIL_INDEX, 
                GL_INT, 
                &this->pixel
            );

            if(pixel)
            {
                int8_t stencilId = globals.getPickableEntity(pixel);
                pixel = stencilId;
            };
            
            this->checkErrors(__FILE__, __LINE__);
        }
        /* =================================================
            else
            {
                globals.setExecutionState(LAZARUS_INVALID_COORDINATE);
            };

            Would be good to have this here but out of frame
            cursor positions continue being recorded...
        ==================================================== */
    }
    else
    {
        globals.setExecutionState(LAZARUS_FEATURE_DISABLED);
    }
    
    return pixel;
};

void CameraManager::setAspectRatio(uint32_t x, uint32_t y)
{
    /* ===============================================
        If a target aspect ratio has been defined then
        use that. Otherwise use the dimensions 
        returned from the machine's primary monitor.
    ================================================== */
    if((x + y) > 0)
    {
        camera.aspectRatio      = static_cast<_Float32>(x) / static_cast<_Float32>(y);
    }
    else
    {
        camera.aspectRatio      = static_cast<_Float32>(this->pixelHeight) / static_cast<_Float32>(this->pixelWidth);
    };   

    return;
};

void CameraManager::checkErrors(const char *file, uint32_t line)
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

CameraManager::~CameraManager()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
}