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
#include <string>

#ifndef LAZARUS_LIGHT_H
#define LAZARUS_LIGHT_H

//	TODO:
//	Create a directional light

//	TODO:
//	Allow users to functionally change the light color

class LightManager
{
    public:
        struct Light
        {
            uint32_t id;
            
            float brightness;

            glm::vec3 position;                     //  The (x,y,z) location of the light source
            glm::vec3 color;                        //  The (r,g,b) color of the light
        };
        
        LightManager(GLuint shader);
        virtual ~LightManager();

        Light createLightSource(float x, float y, float z, float r, float g, float b, float brightness = 1.0f);
        void loadLightSource(Light &lightIn);

    private:
        struct LightData
        {
            /* ==========================================
                Used to traverse the point light uniform
                array.
            ============================================= */
            uint32_t uniformIndex;

            GLint lightPositionUniformLocation;
            GLint lightColorUniformLocation;
            GLint brightnessUniformLocation;
        };

        uint32_t lightCount;
        GLint lightCountLocation;
    	GLint shaderProgram;

        Light lightOut;
        LightData lightData;
        std::vector<LightData> lightStore;

        GlobalsManager globals;

};

#endif