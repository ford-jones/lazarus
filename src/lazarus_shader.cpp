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

	const int CUBEMAP = 1;
	const int ATLAS = 2;
	const int ARRAY = 3;

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

    uniform int samplerType;
    uniform int discardFrags;

    uniform sampler2D textureAtlas;
    uniform sampler2DArray textureArray;
    uniform samplerCube textureCube;

    out vec4 outFragment;

    //  Determine the rgba values of the incoming fragment
    vec4 _lazarusComputeColor ()
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
            vec4 tex = vec4(0.0, 0.0, 0.0, 0.0);
            switch(samplerType)
            {
                case ARRAY:
                    //  the array-layer number is stored in the z value of the uv

                    tex = texture(textureArray, vec3(textureCoordinate.xy, textureCoordinate.z));

                    //  I.e. MeshManager::Mesh::Material::discardAlphaZero

                    if(discardFrags == 1 && tex.a < 0.1)
                    {
                        discard;
                    }
                    break;

                case ATLAS:
                    vec4 t = texture(textureAtlas, textureCoordinate.xy);

                    //  Bitmaps are stored as a single channel (red)
                    //  Use the value to determine discarded fragments

                    vec4 sampled = vec4(1.0, 1.0, 1.0, t.r);
                    tex = vec4(textColor, 1.0) * sampled;

                    if(tex.a < 0.1)
                    {
                        discard;
                    }
                    break;

                case CUBEMAP:
                    tex = texture(textureCube, skyBoxTextureCoordinate);
                    break;

                default:
                    break;
            };

            return tex;
        };
    }

    //  Illuminate the fragment using the lambertian lighting model
    vec3 _lazarusComputeLambertianReflection (vec3 colorData) 
    {
        vec3 result = vec3(0.0, 0.0, 0.0);

        //  Calculate the fragment's diffuse lighting for each light in the scene.
        for(int i = 0; i < lightCount; i++)
        {
            vec3 displacement = lightPositions[i] - fragPosition;
            vec3 direction = normalize(displacement);
            float diffusion = max(dot(normalCoordinate, direction), 0.0);
            vec3 color = vec3(colorData.r, colorData.g, colorData.b);
            vec3 illuminatedFrag = (color * lightColors[i] * diffusion);

            //  Apply inverse square law to illumination result
            //  Note: Don't apply for directional lights when they are added
            //  This is better maybe: return illuminatedFrag * min(1.0 / pow(dot(displacement, displacement), 0.5), 1.0);
            vec3 reflection = illuminatedFrag / (dot(displacement, displacement));

            result += (reflection * lightBrightness[i]);
        };

        return result;
    }

    //  Determines the factor by which fragments should be blended with the fog color by
    float _lazarusComputeFogFactor()
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
void main ()
{
    vec4 fragColor = _lazarusComputeColor();

    //  When the fragment is part of a skybox or is observed by an orthographic camera, use color as-is.
    if(samplerType == CUBEMAP || isUnderPerspective == 0)
    {
        outFragment = fragColor;
    }
    else
    {
        vec3 lighting = 0.1 * fragColor.rgb;
        lighting += _lazarusComputeLambertianReflection(fragColor.rgb);

        outFragment = vec4(lighting, 1.0);

        if(fogDensity > 0.0)
        {
            float fogFactor = _lazarusComputeFogFactor();
            outFragment = vec4((mix(fogColor.xyz, lighting, fogFactor)), 1.0);
        }
    }

    return;
})"; 

Shader::Shader()
{
    LOG_DEBUG("Constructing Lazarus::Shader");
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

        std::string message = std::string("Shader Compilation Error: ").append(this->message);
        LOG_ERROR(message.c_str(), __FILE__, __LINE__);

        globals.setExecutionState(StatusCode::LAZARUS_VSHADER_COMPILE_FAILURE);
        return globals.getExecutionState();
    };

    glShaderSource      (this->fragShader, 1, &this->fragShaderProgram, NULL);                                                      //   Link the shader.frag file contents to the newly created OpenGL fragment shader instance
    glCompileShader     (this->fragShader);                                                                                   //   Compile the shader
    glGetShaderiv(this->fragShader, GL_COMPILE_STATUS, &this->accepted);                                                            //   Check the compilation status
    if(!accepted)                                                                                                       //   If it failed
    {
        glGetShaderInfoLog(this->fragShader, 512, NULL, this->message);                                                             //   Retrieve the OpenGL shader logs if there are any and print them to the console
        std::string message = std::string("Shader Compilation Error: ").append(this->message);
        LOG_ERROR(message.c_str(), __FILE__, __LINE__);

        globals.setExecutionState(StatusCode::LAZARUS_FSHADER_COMPILE_FAILURE);
        return globals.getExecutionState();
    };

    glAttachShader      (this->shaderProgram, this->vertShader);                                                                    //   Attatch the compiled vert shader to the shader program
    glAttachShader      (this->shaderProgram, this->fragShader);                                                                    //   Attatch the compiled frag shader to the shader program
    glLinkProgram       (this->shaderProgram);                                                                                //   Link the shader program to this OpenGL context
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &this->accepted);                                                           //   Check the link status
    if(!accepted)                                                                                                       //   If it failed
    {
        glGetProgramInfoLog(this->shaderProgram, 512, NULL, this->message);                                                         //   Retrieve the OpenGL shader logs if there are any and print them to the console

        std::string message = std::string("Shader Error: ").append(this->message);
        LOG_ERROR(message.c_str(), __FILE__, __LINE__);

        globals.setExecutionState(StatusCode::LAZARUS_SHADER_LINKING_FAILURE);
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
        std::string message = std::string("OpenGL Error: ").append(std::to_string(this->errorCode));
        LOG_ERROR(message.c_str(), __FILE__, __LINE__);

        globals.setExecutionState(StatusCode::LAZARUS_SHADER_ERROR);
    };

    return;
};

void Shader::uploadUniform(std::string identifier, void *data)
{
    //  TODO:
    //  Check openGL errors through here with glGetError

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
            LOG_ERROR("Shader Error: ", __FILE__, __LINE__);

            globals.setExecutionState(StatusCode::LAZARUS_SHADER_ERROR);
            break;
    }

    return;
};

void Shader::verifyProgram(uint32_t program)
{
    //  Validate existence of the program
    if(glIsProgram(program) != GL_TRUE)
    {
        LOG_ERROR("Shader Error: ", __FILE__, __LINE__);

        globals.setExecutionState(StatusCode::LAZARUS_SHADER_ERROR);
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
    LOG_DEBUG("Destroying Lazarus::Shader");

    for(size_t i = 0; i < this->linkedPrograms.size(); i++)
    {
        glDeleteProgram         (this->linkedPrograms[i]);
    };

    for(size_t i = 0; i < this->shaderSources.size(); i++)
    {
        glDeleteProgram         (this->shaderSources[i]);
    };
};
