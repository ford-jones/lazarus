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

const char *LAZARUS_DEFAULT_VERT_LAYOUT = R"(
    #version 410 core

    layout(location = 0) in vec3 inVertex;
    layout(location = 1) in vec3 inDiffuse;
    layout(location = 2) in vec3 inNormal;
    layout(location = 3) in vec3 inTexCoord;

    uniform int usesPerspective;

    uniform mat4 modelMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 perspectiveProjectionMatrix;
    uniform mat4 orthoProjectionMatrix;

    out vec3 fragPosition;
    out vec3 diffuseColor;
    out vec3 normalCoordinate;
    out vec3 textureCoordinate;
    out vec3 skyBoxTextureCoordinate;

    flat out int isUnderPerspective;
)";

const char *LAZARUS_DEFAULT_FRAG_LAYOUT = R"(
    #version 410 core

    #define MAX_LIGHTS 150

    in vec3 fragPosition;
    in vec3 diffuseColor;
    in vec3 normalCoordinate;
    in vec3 textureCoordinate;
    in vec3 skyBoxTextureCoordinate;

    flat in int isUnderPerspective;

    uniform int lightCount;
    uniform vec3 lightPositions[MAX_LIGHTS];
    uniform vec3 lightColors[MAX_LIGHTS];
    uniform float lightBrightness[MAX_LIGHTS];

    uniform vec3 fogColor;
    uniform vec3 fogViewpoint;
    uniform float fogMaxDist;
    uniform float fogMinDist;
    uniform float fogDensity;

    uniform vec3 textColor;

    uniform int spriteAsset;
    uniform int glyphAsset;
    uniform int isSkyBox;

    uniform float textureLayer;

    uniform sampler2D textureAtlas;
    uniform sampler2DArray textureArray;
    uniform samplerCube textureCube;

    out vec4 outFragment;
)";

const char *LAZARUS_DEFAULT_VERT_SHADER =  R"(
void main ()
{
   vec4 worldPosition = modelMatrix * vec4(inVertex, 1.0);
   
   if(usesPerspective != 0)
   {
      gl_Position = perspectiveProjectionMatrix * viewMatrix * worldPosition;   
   }
   else
   {
      gl_Position = orthoProjectionMatrix * viewMatrix * worldPosition;
   }
   
   fragPosition = vec3(worldPosition);
   diffuseColor = inDiffuse;
   normalCoordinate = mat3(transpose(inverse(modelMatrix))) * inNormal;
   textureCoordinate = inTexCoord;

   isUnderPerspective = usesPerspective;

   skyBoxTextureCoordinate = -inVertex;
})"; 

const char *LAZARUS_DEFAULT_FRAG_SHADER = R"(
//  Illuminate the fragment using the lambertian lighting model
vec3 calculateLambertianDeflection (vec4 colorData, vec3 lightPosition, vec3 lightColor) 
{
    vec3 displacement = lightPosition - fragPosition;
    vec3 direction = normalize(displacement);
    float diffusion = max(dot(normalCoordinate, direction), 0.0);
    vec3 color = vec3(colorData.r, colorData.g, colorData.b);
    vec3 illuminatedFrag = (color * lightColor * diffusion);
    
    //  Apply inverse square law to illumination result
    //  Note: Don't apply for directional lights when they are added
    //  This is better maybe: return illuminatedFrag * min(1.0 / pow(dot(displacement, displacement), 0.5), 1.0);
    return illuminatedFrag / (dot(displacement, displacement));
}

float calculateFog()
{
    //  Establish distance between fragment and visibility epicenter
    float diffX         = pow(fragPosition.x - fogViewpoint.x, 2);
    float diffY         = pow(fragPosition.y - fogViewpoint.y, 2);
    float diffZ         = pow(fragPosition.z - fogViewpoint.z, 2);
    float difference    = sqrt((diffX + diffY + diffZ));

    //  Establish fog thickness: 0.0 = 100%
    float strength = 1.0 - (clamp(fogDensity, 0.1, 0.9));

    //  Establish fragment's fog-depth
    float visibilityBounds  = fogMaxDist - difference;
    float fogBounds         = fogMaxDist - fogMinDist;
    float fogFactor         = clamp((visibilityBounds / fogBounds), 0.0, strength);

    return fogFactor;
}

vec4 interpretColorData ()
{
    //  rgb with values less than 0 indicate the fragment has no texture and should use diffuse coloring
    //  Otherwise its a texture so pick out the texels from the appropriate sampler

    if((diffuseColor.r >= 0.0) &&
       (diffuseColor.g >= 0.0) && 
       (diffuseColor.b >= 0.0))
    {
        return vec4(diffuseColor, 1.0);
    }
    else 
    {
        if( spriteAsset == 1 )
        {
            vec4 tex = texture(textureArray, vec3(textureCoordinate.xy, textureLayer));
            return tex;
        } 
        else if( glyphAsset == 1)
        {
            vec4 tex = texture(textureAtlas, textureCoordinate.xy);
            
            vec4 sampled = vec4(1.0, 1.0, 1.0, tex.r);
            vec4 text = vec4(textColor, 1.0) * sampled;

            if(text.a < 0.1)
            {
                discard;
            }

            return text;           
        }
        else if( isSkyBox == 1 )
        {
            vec4 tex = texture(textureCube, skyBoxTextureCoordinate);
            return tex;
        }
        else
        {
            vec4 tex = texture(textureArray, vec3(textureCoordinate.xy, textureLayer));

            if(tex.a < 0.1)
            {
                discard;
            }

            return tex;
        } 

    }
}

void main ()
{
    vec4 fragColor = interpretColorData();

    //  When the fragment is part of a skybox or is observed by an orthographic camera, use color as-is.
    if(isSkyBox == 1 || isUnderPerspective == 0)
    {
        outFragment = fragColor;
    }
    else
    {
        vec3 illuminationResult = 0.1 * fragColor.rgb;

        //  Calculate the fragment's diffuse lighting for each light in the scene.
        for(int i = 0; i < lightCount; i++)
        {
            illuminationResult += (calculateLambertianDeflection(fragColor, lightPositions[i], lightColors[i]) * lightBrightness[i]);
        };

        outFragment = vec4(illuminationResult, 1.0);

        if(fogDensity > 0.0)
        {
            float fogFactor = calculateFog();
            outFragment = vec4((mix(fogColor.xyz, illuminationResult, fogFactor)), 1.0);
        }
    }

    return;
})"; 

Shader::Shader()
{
    std::cout << GREEN_TEXT << "Calling constructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;
    this->linkedPrograms = {};
    this->shaderSources = {};

    this->reset();
};

uint32_t Shader::compileShaders(std::string fragmentShader, std::string vertexShader)
{
    this->reset();
    this->vertReader = std::make_unique<FileLoader>();
    this->fragReader = std::make_unique<FileLoader>();

    if(fragmentShader != "")
    {
        this->fragSource = fragReader->loadText(fragmentShader.c_str());
    }
    else
    {
        this->fragSource = LAZARUS_DEFAULT_FRAG_SHADER;
    };

    if(vertexShader != "")
    {
        this->vertSource = vertReader->loadText(vertexShader.c_str());
    }
    else
    {
        this->vertSource = LAZARUS_DEFAULT_VERT_SHADER;
    };

    this->vertShaderProgram = vertLayout.append(vertSource).c_str();
    this->fragShaderProgram = fragLayout.append(fragSource).c_str();

    this->vertShader      =   glCreateShader(GL_VERTEX_SHADER);                                                               //   Create a new instance of a vertex shader program in openGL
    this->fragShader      =   glCreateShader(GL_FRAGMENT_SHADER);                                                             //   Create a new instance of a fragment shader program in openGL
    this->shaderProgram   =   glCreateProgram();                                                                              //   Instantiate a new shader program

    glShaderSource      (this->vertShader, 1, &this->vertShaderProgram, NULL);                                                      //   Link the shader.vert file contents to the newly created OpenGL vertex shader instance
    glCompileShader     (this->vertShader);                                                                                   //   Compile the shader
    glGetShaderiv(this->vertShader, GL_COMPILE_STATUS, &this->accepted);                                                            //   Check the compilation status
    if(!accepted)                                                                                                       //   If it failed
    {
        glGetShaderInfoLog(this->vertShader, 512, NULL, this->message);                                                             //   Retrieve the OpenGL shader logs if there are any and print them to the console
        std::cout << RED_TEXT << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << RESET_TEXT << this->message << std::endl;

        globals.setExecutionState(LAZARUS_VSHADER_COMPILE_FAILURE);
        return globals.getExecutionState();
    };

    glShaderSource      (this->fragShader, 1, &this->fragShaderProgram, NULL);                                                      //   Link the shader.frag file contents to the newly created OpenGL fragment shader instance
    glCompileShader     (this->fragShader);                                                                                   //   Compile the shader
    glGetShaderiv(this->fragShader, GL_COMPILE_STATUS, &this->accepted);                                                            //   Check the compilation status
    if(!accepted)                                                                                                       //   If it failed
    {
        glGetShaderInfoLog(this->fragShader, 512, NULL, this->message);                                                             //   Retrieve the OpenGL shader logs if there are any and print them to the console
        std::cout << RED_TEXT << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" << RESET_TEXT << this->message << std::endl;

        globals.setExecutionState(LAZARUS_FSHADER_COMPILE_FAILURE);
        return globals.getExecutionState();
    };

    glAttachShader      (this->shaderProgram, this->vertShader);                                                                    //   Attatch the compiled vert shader to the shader program
    glAttachShader      (this->shaderProgram, this->fragShader);                                                                    //   Attatch the compiled frag shader to the shader program
    glLinkProgram       (this->shaderProgram);                                                                                //   Link the shader program to this OpenGL context
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &this->accepted);                                                           //   Check the link status
    if(!accepted)                                                                                                       //   If it failed
    {
        glGetProgramInfoLog(this->shaderProgram, 512, NULL, this->message);                                                         //   Retrieve the OpenGL shader logs if there are any and print them to the console
        std::cout << RED_TEXT << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << RESET_TEXT << message << std::endl;

        globals.setExecutionState(LAZARUS_SHADER_LINKING_FAILURE);
        return globals.getExecutionState();
    }

    this->verifyProgram(this->shaderProgram);
    
    this->linkedPrograms.push_back(this->shaderProgram);
    this->shaderSources.push_back(this->vertShader);
    this->shaderSources.push_back(this->fragShader);

    return shaderProgram;
};

void Shader::setActiveShader(uint32_t program)
{
    this->verifyProgram(program);
    glUseProgram(this->shaderProgram);
    
    this->errorCode = glGetError(); 
    if(this->errorCode != GL_NO_ERROR)
    {
        globals.setExecutionState(LAZARUS_SHADER_ERROR);
    };

    return;
};

void Shader::uploadUniform(std::string identifier, void *data)
{
    const char *uniformName = identifier.c_str();

    const GLchar *name[1] = {
        uniformName
    };
    
    GLuint index[1] = {
        0
    };
    
    GLenum type = 0;
    GLint size = 0;
    GLchar *n = NULL;


    //  Lookup uniform location
    GLuint uniformLocation = glGetUniformLocation(this->shaderProgram, uniformName);
    
    //  Lookup uniform index by name
    glGetUniformIndices(
        this->shaderProgram,
        1,
        name,
        index
    );

    //  Lookup uniform data by index
    glGetActiveUniform(
        this->shaderProgram, 
        index[0],
        100,
        NULL,
        &size,
        &type,
        n
    );

    //  Upload uniform data
    switch (type)
    {
        case GL_INT:
        {
            GLint *integerValue = static_cast<GLint *>(data);
            glUniform1i(uniformLocation, *integerValue);

            break;
        }
        case GL_UNSIGNED_INT:
        {
            GLuint *uIntegerValue = static_cast<GLuint *>(data);
            glUniform1ui(uniformLocation, *uIntegerValue);

            break;
        }
        case GL_FLOAT:
        {
            GLfloat *floatValue = static_cast<GLfloat *>(data);
            glUniform1f(uniformLocation, *floatValue);

            break;
        }
        case GL_FLOAT_VEC4:
        {
            auto *vectorValue = reinterpret_cast<glm::vec4 *>(data);
            glm::vec4 vec = *vectorValue;
            glUniform4fv(uniformLocation, 1, &vec[0]);

            break;
        }
        case GL_FLOAT_VEC3:
        {
            auto *vectorValue = reinterpret_cast<glm::vec3 *>(data);
            glm::vec3 vec = *vectorValue;
            glUniform3fv(uniformLocation, 1, &vec[0]);

            break;
        }
        case GL_FLOAT_VEC2:
        {
            auto *vectorValue = reinterpret_cast<glm::vec2 *>(data);
            glm::vec2 vec = *vectorValue;
            glUniform2fv(uniformLocation, 1, &vec[0]);

            break;
        };

        default:
            globals.setExecutionState(LAZARUS_SHADER_ERROR);
            break;
    }

    return;
};

void Shader::verifyProgram(uint32_t program)
{
    //  Validate existence of the program
    if(glIsProgram(program) != GL_TRUE)
    {
        std::cout << RED_TEXT << "ERROR::SHADER::PROGRAM::NOT_FOUND" << RESET_TEXT << std::endl;

        globals.setExecutionState(LAZARUS_SHADER_ERROR);
    }
    else
    {
        this->shaderProgram = program;
    };

    return;
};

void Shader::reset()
{
    this->vertLayout = LAZARUS_DEFAULT_VERT_LAYOUT;
    this->fragLayout = LAZARUS_DEFAULT_FRAG_LAYOUT;

    this->vertSource = "";
    this->fragSource = "";

	this->vertReader = nullptr;
	this->fragReader = nullptr;
	this->vertShaderProgram = NULL;
	this->fragShaderProgram = NULL;
    this->message = NULL;
	
	this->accepted = 0;
	
	this->vertShader = 0;
	this->fragShader = 0;
	this->shaderProgram = 0;	

    return;
};

Shader::~Shader()
{
    std::cout << GREEN_TEXT << "Calling destructor @ file: " << __FILE__ << " line: (" << __LINE__ << ")" << RESET_TEXT << std::endl;

    for(size_t i = 0; i < this->linkedPrograms.size(); i++)
    {
        glDeleteProgram         (this->linkedPrograms[i]);
    };

    for(size_t i = 0; i < this->shaderSources.size(); i++)
    {
        glDeleteProgram         (this->shaderSources[i]);
    };
};
