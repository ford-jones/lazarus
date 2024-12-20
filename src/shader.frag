#version 410 core

in vec3 fragPosition;
in vec3 diffuseColor;
in vec3 normalCoordinate;
in vec3 textureCoordinate;
in vec3 skyBoxTextureCoordinate;

flat in int isUnderPerspective;

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform vec3 textColor;

uniform int spriteAsset;
uniform int glyphAsset;
uniform int isSkyBox;

uniform float textureLayer;

uniform sampler2D textureAtlas;
uniform sampler2DArray textureArray;
uniform samplerCube textureCube;

out vec4 outFragment;

vec3 calculateLambertianDeflection (vec4 colorData) 
{
    vec3 color = vec3(colorData.r, colorData.g, colorData.b);

    vec3 lightDirection = normalize(lightPosition - fragPosition);

    float diff;
    if(isUnderPerspective == 1)
    {
        diff = max(dot(normalCoordinate, lightDirection), 0.0);
    }
    else
    {
        diff = max(dot(normalCoordinate, lightDirection), 1.0);
    }

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
        vec3 illumFrag = calculateLambertianDeflection(fragColor);

        outFragment = vec4(illumFrag, 1.0);
    }

    return;
}