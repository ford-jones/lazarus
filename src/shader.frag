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

uniform vec3 textColor;

uniform int spriteAsset;
uniform int glyphAsset;
uniform int isSkyBox;

uniform float textureLayer;

uniform sampler2D textureAtlas;
uniform sampler2DArray textureArray;
uniform samplerCube textureCube;

out vec4 outFragment;

vec3 calculateLambertianDeflection (vec4 colorData, vec3 lightPosition, vec3 lightColor) 
{
    vec3 lightDirection = normalize(lightPosition - fragPosition);
    float diff = max(dot(normalCoordinate, lightDirection), 0.0);
    vec3 color = vec3(colorData.r, colorData.g, colorData.b);
    vec3 illuminatedFrag = (color * lightColor * diff);
    
    return illuminatedFrag;
}

vec4 interpretColorData ()
{
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
            
    if(isSkyBox == 1)
    {
        outFragment = fragColor;
    }
    else
    {
        vec3 illuminationResult = vec3(0.0, 0.0, 0.0);
        
        if(isUnderPerspective == 0)
        {
            illuminationResult = (vec3(fragColor.r, fragColor.g, fragColor.b) * vec3(1.0, 1.0, 1.0) * 1.0);
        }
        else
        {
            for(int i = 0; i < lightCount; i++)
            {
                illuminationResult += calculateLambertianDeflection(fragColor, lightPositions[i], lightColors[i]);
            };
        };

        outFragment = vec4(illuminationResult, 1.0);
    }

    return;
}