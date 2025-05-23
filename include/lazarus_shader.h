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
#include <memory>
#include <string>

#include "lazarus_file_reader.h"

using std::unique_ptr;

#ifndef LAZARUS_SHADER_H
#define LAZARUS_SHADER_H

extern const char *LAZARUS_DEFAULT_VERT_LAYOUT;
extern const char *LAZARUS_DEFAULT_FRAG_LAYOUT;
extern const char *LAZARUS_DEFAULT_VERT_SHADER;
extern const char *LAZARUS_DEFAULT_FRAG_SHADER;

class Shader
{
    public:
    	Shader();
        int compileShaders(std::string fragmentShader = "", std::string vertexShader = "");
        void uploadUniform(std::string identifier, void *data);
        void setActiveShader(int program);
        virtual ~Shader();

    private: 
        void reset();
        void verifyProgram(int program);

        unique_ptr<FileReader> vertReader;
        unique_ptr<FileReader> fragReader;

        std::string vertLayout; 
        std::string vertSource; 
        std::string fragLayout; 
        std::string fragSource; 

        const char *vertShaderProgram;
        const char *fragShaderProgram;

        GLuint shaderProgram;
        GLuint vertShader;
        GLuint fragShader;

        char *message;

        int errorCode; 
        int accepted;

        GlobalsManager globals;
        
};

#endif
