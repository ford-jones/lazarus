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
#ifndef LAZARUS_CONSTANTS_H
	#include "lazarus_constants.h"
#endif

#include <iostream>

#ifndef LAZARUS_GLOBALS_MANAGER_H
#define LAZARUS_GLOBALS_MANAGER_H

class GlobalsManager
{
    public:
        void setMaxImageSize(int width, int height);
        int getMaxImageWidth();
        int getMaxImageHeight();

        void setEnforceImageSanity(bool shouldEnforce);
        bool getEnforceImageSanity();

        void setCursorHidden(bool shouldHide);
        bool getCursorHidden();

        void setBackFaceCulling(bool shouldCull);
        bool getBackFaceCulling();

        void setDepthTest(bool shouldTest);
        bool getDepthTest();

        void setLaunchInFullscreen(bool shouldEnlarge);
        bool getLaunchInFullscreen();

        void setExecutionState(int state);
        int getExecutionState();

        void setVsyncDisabled(bool shouldDisable);
        bool getVsyncDisabled();

        void setNumberOfActiveLights(int count);
        int getNumberOfActiveLights();

        void setDisplaySize(int width, int height);
        int getDisplayWidth();
        int getDisplayHeight();
};

#endif