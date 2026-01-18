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

#include "../include/lazarus_common.h"

void LOG_DEBUG(const char *DEBUG_MESSAGE)
{
    #ifdef _LAZARUS_DEBUG_BUILD
    
        std::chrono::time_point getTime = std::chrono::system_clock::now();
        const std::time_t time = std::chrono::system_clock::to_time_t(getTime);
        
        std::cout << BLUE_TEXT << "["<< time << "] " << RESET_TEXT << DEBUG_MESSAGE << std::endl;
    #endif

    return;
};

void LOG_ERROR(const char *ERR_MESSAGE, const char *ERR_FILENAME, uint32_t ERR_LINE)
{
    std::cerr << RED_TEXT << "LAZARUS::ERROR \n" << ERR_FILENAME << ":" << ERR_LINE << "\n" << ERR_MESSAGE << RESET_TEXT << std::endl;
    return;
};

uint32_t                 LAZARUS_PRIMARY_DISPLAY_WIDTH          = 0;
uint32_t                 LAZARUS_PRIMARY_DISPLAY_HEIGHT         = 0;
uint32_t                 LAZARUS_LIGHT_COUNT                    = 0;
std::vector<uint32_t>    LAZARUS_SELECTABLE_ENTITIES            = {};
const uint32_t           LAZARUS_MAX_LIGHTS                     = 150;
const uint32_t           LAZARUS_MAX_SELECTABLE_ENTITIES        = 254;

uint32_t                 LAZARUS_MAX_IMAGE_WIDTH                = 0;
uint32_t                 LAZARUS_MAX_IMAGE_HEIGHT               = 0;
bool                     LAZARUS_ENFORCE_IMAGE_SANITY           = false;
bool                     LAZARUS_DISABLE_CURSOR_VISIBILITY      = false;
bool                     LAZARUS_LAUNCH_IN_FULLSCREEN           = false;
bool                     LAZARUS_DISABLE_VSYNC                  = false;
bool                     LAZARUS_DO_STENCIL_BUFFER              = false;
bool                     LAZARUS_CULL_BACK_FACES                = true;
bool                     LAZARUS_DEPTH_TEST_FRAGS               = true;

void GlobalsManager::setEnforceImageSanity(bool shouldEnforce)
{
    LAZARUS_ENFORCE_IMAGE_SANITY = shouldEnforce;

    return;
};

void GlobalsManager::setDisplaySize(uint32_t width, uint32_t height)
{
    LAZARUS_PRIMARY_DISPLAY_WIDTH = width;
    LAZARUS_PRIMARY_DISPLAY_HEIGHT = height;
    
    return;
};

uint32_t GlobalsManager::getDisplayWidth()
{
    return LAZARUS_PRIMARY_DISPLAY_WIDTH;
};

uint32_t GlobalsManager::getDisplayHeight()
{
    return LAZARUS_PRIMARY_DISPLAY_HEIGHT;
};

bool GlobalsManager::getEnforceImageSanity()
{
    return LAZARUS_ENFORCE_IMAGE_SANITY;
};

void GlobalsManager::setMaxImageSize(uint32_t width, uint32_t height)
{
    LAZARUS_MAX_IMAGE_WIDTH = width;
    LAZARUS_MAX_IMAGE_HEIGHT = height;

    return;
};

uint32_t GlobalsManager::getMaxImageWidth() 
{
    return LAZARUS_MAX_IMAGE_WIDTH;
};

uint32_t GlobalsManager::getMaxImageHeight() 
{
    return LAZARUS_MAX_IMAGE_HEIGHT;
};

void GlobalsManager::setCursorHidden(bool shouldHide)
{
    LAZARUS_DISABLE_CURSOR_VISIBILITY = shouldHide;

    return;
};

bool GlobalsManager::getCursorHidden()
{
    return LAZARUS_DISABLE_CURSOR_VISIBILITY;
};

void GlobalsManager::setBackFaceCulling(bool shouldCull)
{
    LAZARUS_CULL_BACK_FACES = shouldCull;

    return;
};

bool GlobalsManager::getBackFaceCulling()
{
    return LAZARUS_CULL_BACK_FACES;
};

void GlobalsManager::setDepthTest(bool shouldTest)
{
    LAZARUS_DEPTH_TEST_FRAGS = shouldTest;

    return;
};

bool GlobalsManager::getDepthTest()
{
    return LAZARUS_DEPTH_TEST_FRAGS;
};

void GlobalsManager::setLaunchInFullscreen(bool shouldEnlarge)
{
    LAZARUS_LAUNCH_IN_FULLSCREEN = shouldEnlarge;

    return;
};

bool GlobalsManager::getLaunchInFullscreen()
{
    return LAZARUS_LAUNCH_IN_FULLSCREEN;
};

void GlobalsManager::setVsyncDisabled(bool shouldDisable)
{
    LAZARUS_DISABLE_VSYNC = shouldDisable;

    return;
};

bool GlobalsManager::getVsyncDisabled()
{
    return LAZARUS_DISABLE_VSYNC;
};

void GlobalsManager::setNumberOfActiveLights(uint8_t count)
{
    LAZARUS_LIGHT_COUNT = count;

    return;
};

uint8_t GlobalsManager::getNumberOfActiveLights()
{
    return LAZARUS_LIGHT_COUNT;
};

void GlobalsManager::setManageStencilBuffer(bool shouldManage)
{
    LAZARUS_DO_STENCIL_BUFFER = shouldManage;

    return;
};

bool GlobalsManager::getManageStencilBuffer()
{
    return LAZARUS_DO_STENCIL_BUFFER;
};

uint32_t GlobalsManager::getNumberOfPickableEntities()
{
    return LAZARUS_SELECTABLE_ENTITIES.size();
};

void GlobalsManager::setPickableEntity(uint32_t entityId)
{
    LAZARUS_SELECTABLE_ENTITIES.push_back(entityId);

    return;
};

uint8_t GlobalsManager::getPickableEntity(uint8_t index)
{
    return LAZARUS_SELECTABLE_ENTITIES[index - 1];
};