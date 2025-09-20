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
#include <vector>

#include "lazarus_file_loader.h"

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
        uint32_t compileShaders(std::string fragmentShader = "", std::string vertexShader = "");
        void uploadUniform(std::string identifier, void *data);
        void setActiveShader(uint32_t program);
        virtual ~Shader();

    private: 
        void verifyProgram(uint32_t program);
        void reset();
        void checkErrors(const char *file, uint32_t line);
        void clearErrors();

        unique_ptr<FileLoader> vertReader;
        unique_ptr<FileLoader> fragReader;

        std::string vertLayout; 
        std::string vertSource; 
        std::string fragLayout; 
        std::string fragSource; 

        const char *vertShaderProgram;
        const char *fragShaderProgram;

        GLuint shaderProgram;
        GLuint vertShader;
        GLuint fragShader;

        GLint accepted;
        
        int32_t errorCode; 
        char *message;

        std::vector<GLuint> linkedPrograms;
        std::vector<GLuint> shaderSources;

        GlobalsManager globals;
        
};

#endif
