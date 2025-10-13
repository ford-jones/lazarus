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
#ifndef LAZARUS_GL_INCLUDES_H
    #include "lazarus_gl_includes.h"
#endif

#ifndef LAZARUS_COMMON_H
    #include "lazarus_common.h"
#endif

#include <iostream>
#include <vector>
#include <map>
#include <string>

#ifndef LAZARUS_LIGHT_H
#define LAZARUS_LIGHT_H

//	TODO:
//	functional settings updates

class LightManager
{
    public:
        enum LightType 
        {
            DIRECTIONAL = 1,
            POINT = 2
        };
        struct LightConfig
        {
            std::string name = "LIGHT_";
            LightType type = LightType::DIRECTIONAL;
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); 
            glm::vec3 direction = glm::vec3(1.0f, 0.0, 0.0);
            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
            float brightness = 1.0f;
        };
        struct Light
        {
            uint32_t id;
            LightConfig config;
        };
        
        LightManager(GLuint shader);
        virtual ~LightManager();

        lazarus_result createLightSource(Light &out, LightConfig options);
        lazarus_result loadLightSource(Light &lightIn, int32_t shader = 0);

    private:
        lazarus_result checkErrors(const char *file, uint32_t line);
        void clearErrors();
        struct LightData
        {
            /* ==========================================
                Used to traverse the point light uniform
                array.
            ============================================= */
            uint32_t uniformIndex;

            GLint lightTypeUniformLocation;
            GLint lightPositionUniformLocation;
            GLint lightDirectionUniformLocation;
            GLint lightColorUniformLocation;
            GLint brightnessUniformLocation;
        };

        uint32_t lightCount;
        GLint lightCountLocation;
    	GLint shaderProgram;

        int32_t errorCode;

        Light lightOut;
        LightData lightData;
        std::map<uint32_t, LightData> lightStore;
};

#endif