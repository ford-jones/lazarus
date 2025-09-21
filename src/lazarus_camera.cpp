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
    LOG_DEBUG("Constructing Lazarus::CameraManager");
    this->camera = {};
    this->shader                            = shader;

    this->pixelHeight                       = GlobalsManager::getDisplayWidth();
    this->pixelWidth                        = GlobalsManager::getDisplayHeight();

    this->viewLocation                      = glGetUniformLocation(shader, "viewMatrix");
    this->perspectiveProjectionLocation     = glGetUniformLocation(shader, "perspectiveProjectionMatrix");
    this->orthographicProjectionLocation    = glGetUniformLocation(shader, "orthoProjectionMatrix");

    this->pixel                             = 0;
    this->errorCode                         = 0;
    this->aspectRatio = 0.0f;
};

lazarus_result CameraManager::createPerspectiveCam(CameraManager::Camera &out, CameraManager::CameraConfig options)
{
    srand(time((0)));
    
    this->camera = {};
    
    camera.id                   = 1 + (rand() % 2147483647);
    camera.config               = options;

    this->setAspectRatio(camera.config.aspectRatioX, camera.config.aspectRatioY);

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
    camera.projectionMatrix     = glm::perspective(0.785398f, this->aspectRatio, 0.1f, camera.config.clippingDistance);

    camera.usesPerspective      = 1;

    out = camera;
    return lazarus_result::LAZARUS_OK;
};

lazarus_result CameraManager::createOrthoCam(CameraManager::Camera &out, CameraManager::CameraConfig options)
{
    srand(time((0)));

    this->camera = {};

    camera.id                       = 1 + (rand() % 2147483647);
    camera.config.aspectRatioX      = options.aspectRatioX;
    camera.config.aspectRatioY      = options.aspectRatioY;
    camera.config.clippingDistance  = 0.0f;

    /* ================================================
        Negative Z so as to be "back" from the viewing
        plane.
    =================================================== */
    camera.position             = vec3(0.0f, 0.0f, -1.0f);
    camera.direction            = glm::normalize(camera.position);
    camera.upVector             = vec3(0.0f, 1.0f, 0.0f);
    
    camera.viewMatrix           = glm::lookAt(camera.position, (camera.position + camera.direction), camera.upVector);
    camera.projectionMatrix     = glm::ortho(0.0f, static_cast<float>(camera.config.aspectRatioX), 0.0f, static_cast<float>(camera.config.aspectRatioY));

    camera.usesPerspective      = 0;

    out = camera;
    return lazarus_result::LAZARUS_OK;
};

lazarus_result CameraManager::loadCamera(CameraManager::Camera &cameraIn)
{
    this->clearErrors();
    
    if(
        this->perspectiveProjectionLocation >= 0 && 
        this->orthographicProjectionLocation >= 0
    )
    {
        glUniformMatrix4fv     (this->viewLocation, 1, GL_FALSE, &cameraIn.viewMatrix[0][0]);

        cameraIn.usesPerspective == 1
        ? glUniformMatrix4fv     (this->perspectiveProjectionLocation, 1, GL_FALSE, &cameraIn.projectionMatrix[0][0])
        : glUniformMatrix4fv     (this->orthographicProjectionLocation, 1, GL_FALSE, &cameraIn.projectionMatrix[0][0]);

        GLuint location = glGetUniformLocation(this->shader, "usesPerspective");
        glUniform1i(location, cameraIn.usesPerspective);

        return this->checkErrors(__FILE__, __LINE__);
    }
    else
    {
        LOG_ERROR("Camera Error:", __FILE__, __LINE__);
        
        return lazarus_result::LAZARUS_MATRIX_LOCATION_ERROR;
    };

    return lazarus_result::LAZARUS_OK;
};

/* ==============================================
    TODO:
    Create a function for ray-picking positions
    in worldspace out from the camera.
================================================= */

lazarus_result CameraManager::getPixelOccupant(uint32_t positionX, uint32_t positionY, uint8_t &out)
{
    if(GlobalsManager::getManageStencilBuffer())
    {
        this->pixel = 0;
        if(positionX < pixelWidth && positionY < pixelHeight) {
            this->clearErrors();
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

            lazarus_result status = this->checkErrors(__FILE__, __LINE__);
            if(status != lazarus_result::LAZARUS_OK)
            {
                out = 0;
                return status;
            };

            if(pixel)
            {
                uint8_t stencilId = GlobalsManager::getPickableEntity(pixel);
                out = stencilId;
            };
        }
        /* =================================================
            else
            {
                globals.setExecutionState(lazarus_result::LAZARUS_INVALID_COORDINATE);
            };

            Would be good to have this here but out of frame
            cursor positions continue being recorded...
        ==================================================== */
    }
    else
    {
        out = 0;

        LOG_ERROR("Camera Error:", __FILE__, __LINE__);
        return lazarus_result::LAZARUS_FEATURE_DISABLED;
    }
    
    return lazarus_result::LAZARUS_OK;
};

lazarus_result CameraManager::setAspectRatio(uint32_t x, uint32_t y)
{
    /* ===============================================
        If a target aspect ratio has been defined then
        use that. Otherwise use the dimensions 
        returned from the machine's primary monitor.
    ================================================== */
    if((x + y) > 0)
    {
        this->aspectRatio      = static_cast<float>(x) / static_cast<float>(y);
    }
    else
    {
        this->aspectRatio      = static_cast<float>(this->pixelHeight) / static_cast<float>(this->pixelWidth);
    };   

    return lazarus_result::LAZARUS_OK;
};

lazarus_result CameraManager::checkErrors(const char *file, uint32_t line)
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

lazarus_result CameraManager::clearErrors()
{
    this->errorCode = glGetError();
    
    while(this->errorCode != GL_NO_ERROR)
    {
        this->errorCode = glGetError();
    };

    return lazarus_result::LAZARUS_OK;
};

CameraManager::~CameraManager()
{
    LOG_DEBUG("Destroying Lazarus::CameraManager");
}