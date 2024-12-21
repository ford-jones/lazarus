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

#include "../include/lazarus_shader.h"

Shader::Shader()
{
    std::cout << GREEN_TEXT << "Calling constructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
	this->vertReader = nullptr;
	this->fragReader = nullptr;
	this->vertShaderProgram = NULL;
	this->fragShaderProgram = NULL;
	
	this->accepted = 0;
	
	this->vertShader = 0;
	this->fragShader = 0;
	this->shaderProgram = 0;	
};

GLuint Shader::initialiseShader()
{
    this->vertReader = std::make_unique<FileReader>();
    this->fragReader = std::make_unique<FileReader>();

    this->vertShaderProgram   =   vertReader->readFromText("shader.vert");                                                      //  Retrieve the vertex shader file contents through stringstream
    this->fragShaderProgram   =   fragReader->readFromText("shader.frag");                                                      //  Retrieve the fragment shader file contents through stringstream


    this->vertShader      =   glCreateShader(GL_VERTEX_SHADER);                                                               //   Create a new instance of a vertex shader program in openGL
    this->fragShader      =   glCreateShader(GL_FRAGMENT_SHADER);                                                             //   Create a new instance of a fragment shader program in openGL
    this->shaderProgram   =   glCreateProgram();                                                                              //   Instantiate a new shader program

    glShaderSource      (this->vertShader, 1, &this->vertShaderProgram, NULL);                                                      //   Link the shader.vert file contents to the newly created OpenGL vertex shader instance
    glCompileShader     (this->vertShader);                                                                                   //   Compile the shader
    glGetShaderiv(this->vertShader, GL_COMPILE_STATUS, &this->accepted);                                                            //   Check the compilation status
    if(!accepted)                                                                                                       //   If it failed
    {
        glGetShaderInfoLog(this->vertShader, 512, NULL, this->infoLog);                                                             //   Retrieve the OpenGL shader logs if there are any and print them to the console
        std::cout << RED_TEXT << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << RESET_TEXT << infoLog << std::endl;

        globals.setExecutionState(LAZARUS_VSHADER_COMPILE_FAILURE);
        return globals.getExecutionState();
    };

    glShaderSource      (this->fragShader, 1, &this->fragShaderProgram, NULL);                                                      //   Link the shader.frag file contents to the newly created OpenGL fragment shader instance
    glCompileShader     (this->fragShader);                                                                                   //   Compile the shader
    glGetShaderiv(this->fragShader, GL_COMPILE_STATUS, &this->accepted);                                                            //   Check the compilation status
    if(!accepted)                                                                                                       //   If it failed
    {
        glGetShaderInfoLog(this->fragShader, 512, NULL, this->infoLog);                                                             //   Retrieve the OpenGL shader logs if there are any and print them to the console
        std::cout << RED_TEXT << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" << RESET_TEXT << this->infoLog << std::endl;

        globals.setExecutionState(LAZARUS_FSHADER_COMPILE_FAILURE);
        return globals.getExecutionState();
    };

    glAttachShader      (this->shaderProgram, this->vertShader);                                                                    //   Attatch the compiled vert shader to the shader program
    glAttachShader      (this->shaderProgram, this->fragShader);                                                                    //   Attatch the compiled frag shader to the shader program
    glLinkProgram       (this->shaderProgram);                                                                                //   Link the shader program to this OpenGL context
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &this->accepted);                                                           //   Check the link status
    if(!accepted)                                                                                                       //   If it failed
    {
        glGetProgramInfoLog(this->shaderProgram, 512, NULL, this->infoLog);                                                         //   Retrieve the OpenGL shader logs if there are any and print them to the console
        std::cout << RED_TEXT << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << RESET_TEXT << infoLog << std::endl;

        globals.setExecutionState(LAZARUS_SHADER_LINKING_FAILURE);
        return globals.getExecutionState();
    }

    if(glIsProgram(this->shaderProgram) != GL_TRUE)                                                                           //   Check that the shader program now exists
    {
        std::cout << RED_TEXT << "ERROR::SHADER::PROGRAM::NOT_FOUND" << RESET_TEXT << std::endl;

        globals.setExecutionState(LAZARUS_SHADER_ERROR);
        return globals.getExecutionState();
    }

    return shaderProgram;
};

Shader::~Shader()
{
    std::cout << GREEN_TEXT << "Calling destructor @: " << __PRETTY_FUNCTION__ << RESET_TEXT << std::endl;
    glDeleteShader          (this->vertShader);
    glDeleteShader          (this->fragShader);
    glDeleteProgram         (this->shaderProgram);
};
